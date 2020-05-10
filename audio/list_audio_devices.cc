
#include <array>
#include <ios>
#include <iostream>
#include <limits>
#include <cmath>

#include "portaudio.h"

std::ostream &operator<<(std::ostream &out, const PaHostApiInfo &info) {
  out << "API Info: " << info.name;
  out << " device count: " << info.deviceCount;
  out << " default input: " << info.defaultInputDevice;
  out << " default output: " << info.defaultOutputDevice;
  return out;
}

std::ostream &operator<<(std::ostream &out, const PaDeviceInfo &info) {
  out << "Device Info: " << info.name
      << " max ch(in/out): " << info.maxInputChannels << " "
      << info.maxOutputChannels
      << " default low latency(in/out): " << info.defaultLowInputLatency << " "
      << info.defaultLowOutputLatency
      << " default high latency(in/out): " << info.defaultHighInputLatency
      << " " << info.defaultHighOutputLatency
      << " sample rate: " << info.defaultSampleRate;
  return out;
}

std::ostream &operator<<(std::ostream &out, const PaStreamInfo &info) {
  out << "Stream info Input Latency: " << info.inputLatency
      << " Output Latency: " << info.outputLatency
      << " sample rate: " << info.sampleRate;
    return out;
}

constexpr int SAMPLE_RATE_HZ = 44100;
struct CallbackData {
  // Buffers with two seconds worth of data
  std::array<std::array<float,SAMPLE_RATE_HZ * 2>, 2> samples;
  double start_time = std::numeric_limits<double>::max();
  double time_to_generate_s = 1.0;
  int num_frames = 0.0;
};

int main() {
  std::cout << "Hello World!" << std::endl;
  std::cout << std::hex << Pa_GetVersion() << std::dec << std::endl;

  const auto error = Pa_Initialize();
  if (error) {
    std::cout << "Error Initializing PortAudio: " << Pa_GetErrorText(error)
              << std::endl;
    return 1;
  }

  const int count = Pa_GetHostApiCount();
  std::cout << "Num API's: " << count << std::endl;

  const PaHostApiIndex host_api_idx = Pa_GetDefaultHostApi();
  // The memory is owned by the library and is valid until Pa_Terminate() is
  // called
  const PaHostApiInfo &api_info = *Pa_GetHostApiInfo(host_api_idx);
  std::cout << api_info << std::endl;

  const int num_devices = Pa_GetDeviceCount();

  for (int i = 0; i < num_devices; i++) {
    std::cout << i << " " << *Pa_GetDeviceInfo(i) << std::endl;
  }

  const PaDeviceIndex output_device_idx = Pa_GetDefaultOutputDevice();
  if (output_device_idx == paNoDevice) {
    std::cout << "Error getting default output device" << std::endl;
    Pa_Terminate();
    return 1;
  }
  const PaDeviceInfo &dev_info = *Pa_GetDeviceInfo(output_device_idx);

  PaStreamParameters stream_params{
      .device = output_device_idx,
      .channelCount = 2,
      .sampleFormat = paFloat32 | paNonInterleaved,
      .suggestedLatency = dev_info.defaultHighOutputLatency};

  CallbackData data;
  PaStream *stream;
  Pa_OpenStream(
      &stream,
      nullptr,         // input params
      &stream_params,  // output params
      44100.0,         // sample rate
      0,               // frames per buffer
      0,               // Stream flags
      [](const void *, void *output, long unsigned int frame_count,
         const PaStreamCallbackTimeInfo *time_info, PaStreamCallbackFlags,
         void *data) -> int {
        CallbackData &callback_data = *static_cast<CallbackData *>(data);
        if (callback_data.start_time > time_info->currentTime) {
          callback_data.start_time = time_info->currentTime;
        }

        constexpr float dt = 1 / 44100.0;
        constexpr float volume = 0.1;
        float **out_arrays = static_cast<float**>(output);
        bool should_exit = false;
        for (int i = 0; i < static_cast<int>(frame_count); i++) {
          const float t = callback_data.num_frames * dt;

          const float value = volume * std::sin(2 * M_PI * 440 * t);
          if (t < callback_data.start_time + 5.0) {
            out_arrays[0][i] = value;
            out_arrays[1][i] = value;
          } else if (t < callback_data.start_time + 5.1){
            const float gain = std::min(std::max(0.0, 5.250 - t), 1.0);
            out_arrays[0][i] = gain * value;
            out_arrays[1][i] = gain * value;
          } else {
            out_arrays[0][i] = 0.0;
            out_arrays[1][i] = 0.0;
            should_exit = true;
          }
          callback_data.num_frames += 1;
        }

        if (should_exit) {
          return paComplete;
        } else {
          return paContinue;
        }
      },
      &data);

  const PaStreamInfo &stream_info = *Pa_GetStreamInfo(stream);
  std::cout << stream_info << std::endl;

  Pa_StartStream(stream);
  while (Pa_IsStreamActive(stream)) {
    Pa_Sleep(100);
  }
  Pa_CloseStream(stream);

  Pa_Terminate();
}
