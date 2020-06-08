#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <csignal>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <thread>

#include "audio/audio_samples.hh"
#include "audio/proto/audio_samples_to_proto.hh"
#include "gflags/gflags.h"
#include "portaudio.h"
#include "zmq.hpp"

namespace washbox::audio {
namespace {
using namespace std::chrono_literals;
std::atomic<bool> should_exit = false;

struct Buffer {
    enum class BufferState { UNUSED, LOADING, READY };
    std::atomic<BufferState> state = BufferState::UNUSED;
    AudioSamples sample_data;

    Buffer &reset() {
        for (auto &channel : sample_data.channel_data) {
            channel.clear();
        }
        state = BufferState::UNUSED;
        return *this;
    }
};

struct MicrophoneStream {
    // Config Parameters
    const PaDeviceInfo *device_info;
    PaStream *stream;
    PaStreamParameters stream_params;
    double sample_rate;
    PaStreamFlags stream_flags;

    // Stream Callback State
    std::array<Buffer, 8> sample_data_buffers;
};
} // namespace

std::shared_ptr<int> initialize_portaudio() {
    const PaError init_error = Pa_Initialize();
    if (init_error) {
        std::cerr << "Failed to init portaudio. " << Pa_GetErrorText(init_error) << std::endl;
        return nullptr;
    }

    return std::shared_ptr<int>(new int, [](int *p) {
        Pa_Terminate();
        delete p;
    });
}

std::unique_ptr<MicrophoneStream> get_microphone_stream(const int device_index) {
    const PaDeviceInfo *device_info = Pa_GetDeviceInfo(device_index);
    // Get the device info
    if (!device_info) {
        std::cerr << "Unable to get device info for device index: " << device_index << std::endl;
        return nullptr;
    }

    // Make the stream parameters
    const PaStreamParameters stream_params{
        .device = device_index,
        .channelCount = device_info->maxInputChannels,
        .sampleFormat = paFloat32 | paNonInterleaved,
        .suggestedLatency = device_info->defaultHighInputLatency,
    };

    const double sample_rate = device_info->defaultSampleRate;
    const auto format_error = Pa_IsFormatSupported(&stream_params, nullptr, sample_rate);
    if (format_error) {
        std::cerr << "Unable to make valid stream parameters. " << Pa_GetErrorText(format_error)
                  << std::endl;
        return nullptr;
    }

    // Open a stream
    std::unique_ptr<MicrophoneStream> out = std::make_unique<MicrophoneStream>();
    out->device_info = device_info;
    out->stream_params = stream_params;
    out->sample_rate = sample_rate;
    out->stream_flags = 0;
    for (Buffer &buffer : out->sample_data_buffers) {
        for (int i = 0; i < device_info->maxInputChannels; i++) {
            buffer.sample_data.channel_data.emplace_back();
            buffer.sample_data.channel_data.back().reserve(8192);
        }
    }

    const auto open_stream_error = Pa_OpenStream(
        &out->stream, &stream_params, nullptr, sample_rate, 0, 0,
        [](const void *input, void *, unsigned long num_samples,
           const PaStreamCallbackTimeInfo *time_info, PaStreamCallbackFlags status_flags,
           void *user_data) -> int {
            MicrophoneStream &stream = *static_cast<MicrophoneStream *>(user_data);

            // Find the first buffer that is large enough
            auto maybe_buffer_iter = std::find_if(
                stream.sample_data_buffers.begin(), stream.sample_data_buffers.end(),
                [](const auto &buffer) { return buffer.state == Buffer::BufferState::LOADING; });

            if (maybe_buffer_iter == stream.sample_data_buffers.end()) {
                // We weren't in progress on any buffer, so let's grab the first one
                maybe_buffer_iter = std::find_if(
                    stream.sample_data_buffers.begin(), stream.sample_data_buffers.end(),
                    [](const auto &buffer) { return buffer.state == Buffer::BufferState::UNUSED; });
                if (maybe_buffer_iter == stream.sample_data_buffers.end()) {
                    std::cout << "Abort 1" << std::endl;
                    return paAbort;
                }
            } else {
                // We found a buffer that is loading, let's check that we can
                // fit all of the data in this buffer.
                if (num_samples + maybe_buffer_iter->sample_data.channel_data.front().size() >
                    maybe_buffer_iter->sample_data.channel_data.front().capacity()) {
                    maybe_buffer_iter->state = Buffer::BufferState::READY;
                    maybe_buffer_iter = ++maybe_buffer_iter == stream.sample_data_buffers.end()
                                            ? stream.sample_data_buffers.begin()
                                            : maybe_buffer_iter;
                    if (maybe_buffer_iter->state != Buffer::BufferState::UNUSED) {
                        return paAbort;
                    }
                }
            }

            if (maybe_buffer_iter->state == Buffer::BufferState::UNUSED) {
                maybe_buffer_iter->sample_data.time_of_validity = time_info->inputBufferAdcTime;
                maybe_buffer_iter->sample_data.sample_rate = stream.sample_rate;
                maybe_buffer_iter->state = Buffer::BufferState::LOADING;
            }

            const float *const *input_samples = static_cast<const float *const *>(input);
            Buffer &buffer = *maybe_buffer_iter;
            for (int channel_idx = 0; channel_idx < stream.device_info->maxInputChannels;
                 channel_idx++) {
                for (int sample_idx = 0; sample_idx < static_cast<int>(num_samples); sample_idx++) {
                    buffer.sample_data.channel_data[channel_idx].push_back(
                        input_samples[channel_idx][sample_idx]);
                }
            }

            return paContinue;
        },
        &*out);

    if (open_stream_error) {
        std::cerr << "Unable to open microphone stream. " << Pa_GetErrorText(open_stream_error)
                  << std::endl;
        return nullptr;
    }
    return out;
}

void microphone_publisher(const int device_index) {
    // Initialize PortAudio
    const auto token = initialize_portaudio();
    if (token == nullptr) {
        return;
    }

    // Create a stream
    auto maybe_stream = get_microphone_stream(device_index);
    if (!maybe_stream) {
        std::cerr << "Unable to get microphone stream" << std::endl;
        return;
    }

    std::signal(SIGINT, [](int sig) { should_exit = true; });

    zmq::context_t context;
    zmq::socket_t socket(context, zmq::socket_type::pub);

    socket.bind("tcp://*:12345");

    Pa_StartStream(maybe_stream->stream);
    // Publish Samples
    std::cout << std::setprecision(10);
    proto::AudioSamples audio_samples_proto;
    std::string proto_string;
    proto_string.reserve(1024 * 1024);
    while (!should_exit && Pa_IsStreamActive(maybe_stream->stream)) {
        // Scan through the data buffers and find any that are ready
        int count = 0;
        const auto start_time = std::chrono::steady_clock::now();

        for (auto &buffer : maybe_stream->sample_data_buffers) {
            if (buffer.state != Buffer::BufferState::READY) {
                continue;
            }
            const auto start_time = std::chrono::steady_clock::now();
            pack_into(buffer.sample_data, &audio_samples_proto);
            const auto pack_time = std::chrono::steady_clock::now();
            proto_string.clear();
            audio_samples_proto.SerializeToString(&proto_string);
            const auto serialize_time = std::chrono::steady_clock::now();

            const zmq::send_result_t result =
                socket.send(zmq::const_buffer(proto_string.c_str(), proto_string.size()),
                            zmq::send_flags::dontwait);
            const auto send_time = std::chrono::steady_clock::now();
            std::cout << "   pack time: "
                      << std::chrono::duration<double>(pack_time - start_time).count()
                      << " serial time: "
                      << std::chrono::duration<double>(serialize_time - pack_time).count()
                      << " send time: "
                      << std::chrono::duration<double>(send_time - serialize_time).count()
                      << " size: " << proto_string.size() << std::endl;
            if (!result) {
                std::cout << "Unable to send" << std::endl;
            }
            buffer.reset();
            count++;
        }
        const auto end_time = std::chrono::steady_clock::now();
        const auto delta = end_time - start_time;
        std::cout << "Num Full Buffers " << count
                  << " time delta: " << std::chrono::duration<double>(delta).count() << std::endl;
        std::this_thread::sleep_for(250ms);
    }

    Pa_StopStream(maybe_stream->stream);

    std::cout << "Exiting" << std::endl;
}

} // namespace washbox::audio
} // namespace washbox

DEFINE_int32(device_index, -1,
             "PortAudio Device Index. "
             "Use `list_audio_devices` to get a valid index");

int main(int argc, char **argv) {
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    std::cout << "Hello World!" << std::endl;

    washbox::audio::microphone_publisher(FLAGS_device_index);
}
