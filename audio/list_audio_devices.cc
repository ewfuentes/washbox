
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
  int num_frames = 0;
  int num_callbacks = 0;
  int max_channels = 0;
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

  Pa_Terminate();
}
