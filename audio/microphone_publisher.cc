#include <algorithm>
#include <atomic>
#include <array>
#include <csignal>
#include <chrono>
#include <limits>
#include <iostream>
#include <iomanip>
#include <memory>
#include <thread>

#include "gflags/gflags.h"
#include "portaudio.h"

namespace washbox {
  namespace audio {
    namespace {
      using namespace std::chrono_literals;
      std::atomic<bool> should_exit = false;
    }
  struct SampleData {
    enum class BufferState {UNUSED, LOADING, READY};
    std::atomic<BufferState> state = BufferState::UNUSED;
    std::atomic<int> num_samples = 0;
    std::vector<std::array<float, 8192>> channel_data;
  };

  struct MicrophoneStream {
    // Config Parameters
    const PaDeviceInfo *device_info;
    PaStream *stream;
    PaStreamParameters stream_params;
    double sample_rate;
    PaStreamFlags stream_flags;

    // Stream Callback State
    std::array<SampleData, 10> sample_data_buffers;
    int num_buffers_processed = 0;
  };

  std::shared_ptr<int> initialize_portaudio() {
    const PaError init_error = Pa_Initialize();
    if (init_error) {
      std::cerr << "Failed to init portaudio. " << Pa_GetErrorText(init_error) << std::endl;
      return nullptr;
    }

    return std::shared_ptr<int>(new int, [](int *p){
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
      std::cerr << "Unable to make valid stream parameters. "
                << Pa_GetErrorText(format_error) << std::endl;
      return nullptr;
    }


    // Open a stream
    std::unique_ptr<MicrophoneStream> out = std::make_unique<MicrophoneStream>();
    out->device_info = device_info;
    out->stream_params = stream_params;
    out->sample_rate = sample_rate;
    out->stream_flags = 0;
    for (SampleData &sample_data : out->sample_data_buffers) {
      sample_data.channel_data.reserve(device_info->maxInputChannels);
    }

    const auto open_stream_error = Pa_OpenStream(
                  &out->stream,
                  &stream_params,
                  nullptr,
                  sample_rate,
                  0,
                  0,
                  [](const void *input, void *, unsigned long frame_count,
                     const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags status_flags,
                     void *user_data) -> int {
                    MicrophoneStream &stream = *static_cast<MicrophoneStream *>(user_data);

                    // Find the first buffer that is large enough
                    auto maybe_buffer_iter = std::find_if(stream.sample_data_buffers.begin(),
                                 stream.sample_data_buffers.end(),
                                 [](const auto &sample_data){
                        return sample_data.state == SampleData::BufferState::LOADING;
                    });

                    if (maybe_buffer_iter == stream.sample_data_buffers.end()) {
                      // We weren't in progress on any buffer, so let's grab the first one
                      maybe_buffer_iter = stream.sample_data_buffers.begin();
                      if (maybe_buffer_iter->state != SampleData::BufferState::UNUSED) {
                        return paAbort;
                      }
                      maybe_buffer_iter->state = SampleData::BufferState::LOADING;
                    } else {
                      // We found a buffer that is loading, let's check that we can fit all of the
                      // data in this buffer.
                      if (frame_count + maybe_buffer_iter->num_samples >
                          maybe_buffer_iter->channel_data[0].size()) {
                        maybe_buffer_iter->state = SampleData::BufferState::READY;
                        maybe_buffer_iter = ++maybe_buffer_iter == stream.sample_data_buffers.end() ?
                          stream.sample_data_buffers.begin() : maybe_buffer_iter;
                        if (maybe_buffer_iter->state != SampleData::BufferState::UNUSED) {
                          return paAbort;
                        }
                        maybe_buffer_iter->state = SampleData::BufferState::LOADING;
                      }
                    }

                    const float * const *input_samples = static_cast<const float *const *>(input);
                    SampleData &sample_data = *maybe_buffer_iter;
                    for (int channel_idx = 0; channel_idx < stream.device_info->maxInputChannels; channel_idx++) {
                      for (int sample_idx = 0; sample_idx < static_cast<int>(frame_count); sample_idx++) {
                        sample_data.channel_data[channel_idx][sample_idx] = input_samples[channel_idx][sample_idx];
                      }
                    }
                    sample_data.num_samples += frame_count;
                    std::cout << "Processing samples: " << frame_count
                              << " " << timeInfo->inputBufferAdcTime
                              << " " << std::distance(stream.sample_data_buffers.begin(), maybe_buffer_iter)<< std::endl;
                    return paContinue;
                  },
                  &*out);

    if (open_stream_error) {
      std::cerr << "Unable to open microphone stream. "
                << Pa_GetErrorText(open_stream_error) << std::endl;
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

    std::signal(SIGINT, [](int sig){should_exit = true;});

    Pa_StartStream(maybe_stream->stream);
    // Publish Samples
    std::cout << std::setprecision(10);
    while (!should_exit && Pa_IsStreamActive(maybe_stream->stream)) {
      // Scan through the data buffers and find any that are ready
      for (auto &sample_data : maybe_stream->sample_data_buffers) {
        if (sample_data.state != SampleData::BufferState::READY) {
          continue;
        }

        sample_data.num_samples = 0;
        sample_data.state = SampleData::BufferState::UNUSED;
      }

      std::this_thread::sleep_for(100ms);
    }

    Pa_StopStream(maybe_stream->stream);

    std::cout << "Exiting" << std::endl;
  }

}
}

DEFINE_int32(device_index, -1, "PortAudio Device Index. "
             "Use `list_audio_devices` to get a valid index");

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  std::cout << "Hello World!" << std::endl;

  washbox::audio::microphone_publisher(FLAGS_device_index);
}
