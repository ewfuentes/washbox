
#include "wav_file.hh"

#include <algorithm>
#include <iostream>

namespace washbox::audio {

bool WavFile::insert_samples(const AudioSamples &samples) {
    if (samples_.channel_data.empty()) {
        // This is the first set of samples, perform a straight copy
        samples_ = samples;
        return true;
    }

    // Otherwise append to the existing buffers
    if (samples_.sample_rate != samples.sample_rate ||
        samples_.channel_data.size() != samples.channel_data.size()) {
        return false;
    }

    for (int channel_idx = 0; channel_idx < static_cast<int>(samples_.channel_data.size());
         channel_idx++) {
        const auto &channel = samples.channel_data[channel_idx];
        std::copy(channel.begin(), channel.end(),
                  std::back_inserter(samples_.channel_data[channel_idx]));
    }

    return true;
}

std::ostream &WavFile::write_to_ostream(std::ostream &stream) const {
    // Write the header
    stream.write("RIFF", 4);

    // Write the remaining data size
    constexpr int WAV_HEADER_SIZE = 4;
    constexpr int FORMAT_CHUNK_SIZE = 26;
    constexpr int DATA_HEADER_SIZE = 8;
    const int num_channels = samples_.channel_data.size();
    const int num_samples = samples_.channel_data.front().size();
    const int sample_rate = samples_.sample_rate;
    const std::uint32_t remaining_size = WAV_HEADER_SIZE + FORMAT_CHUNK_SIZE + DATA_HEADER_SIZE +
                                         num_channels * num_samples * sizeof(float);
    stream.write(reinterpret_cast<const char *>(&remaining_size), 4);

    // Write the WAV chunk
    stream.write("WAVE", 4);

    // Write the fmt chunk
    stream.write("fmt ", 4);

    // Write the fmt chunk size
    std::uint32_t fmt_chunk_size = 18;
    stream.write(reinterpret_cast<const char *>(&fmt_chunk_size), 4);

    // Write the sample type
    constexpr std::uint16_t FLOAT_SAMPLES = 3;
    stream.write(reinterpret_cast<const char *>(&FLOAT_SAMPLES), 2);

    // Num Channels
    stream.write(reinterpret_cast<const char *>(&num_channels), 2);

    // Sample Rate
    stream.write(reinterpret_cast<const char *>(&sample_rate), 4);

    // Byte rate
    const std::uint32_t byte_rate = num_channels * sample_rate * 4;
    stream.write(reinterpret_cast<const char *>(&byte_rate), 4);

    // Block Align
    const std::uint16_t block_align = num_channels * 4;
    stream.write(reinterpret_cast<const char *>(&block_align), 2);

    // bits per sample
    const std::uint16_t bits_per_sample = 32;
    stream.write(reinterpret_cast<const char *>(&bits_per_sample), 2);

    // Extra config size
    const std::uint16_t extra_config_size = 0;
    stream.write(reinterpret_cast<const char *>(&extra_config_size), 2);

    // Write the data chunk
    stream.write("data", 4);

    // Data chunk size
    const std::uint32_t data_size = num_channels * num_samples * sizeof(float);
    stream.write(reinterpret_cast<const char *>(&data_size), 4);

    for (int i = 0; i < num_samples; i++) {
        for (int j = 0; j < num_channels; j++) {
            stream.write(reinterpret_cast<const char *>(&samples_.channel_data[j][i]),
                         sizeof(float));
        }
    }
    return stream;
}

} // namespace washbox::audio
