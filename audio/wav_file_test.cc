
#include "audio/wav_file.hh"

#include <cmath>
#include <cstdint>
#include <fstream>
#include <sstream>

#include "gtest/gtest.h"

namespace washbox::audio {
namespace {
#pragma pack(push, 1)
struct WavFormat {
    struct FormatChunk {
        char header[4] = {'f', 'm', 't', ' '};
        std::uint32_t fmt_chunk_size;
        std::uint16_t audio_format;
        std::uint16_t num_channels;
        std::uint32_t sample_rate;
        std::uint32_t byte_rate;
        std::uint16_t block_align;
        std::uint16_t bits_per_sample;
        std::uint16_t cb_size;
    };

    struct DataChunk {
        char header[4] = {'d', 'a', 't', 'a'};
        std::uint32_t chunk_size;
        char samples[];
    };

    char riff_header[4] = {'R', 'I', 'F', 'F'};
    std::uint32_t remaining_size;
    char wav_header[4] = {'W', 'A', 'V', 'E'};
    FormatChunk format_chunk;
    DataChunk data_chunk;
};
#pragma pack(pop)

AudioSamples make_dummy_samples(const std::vector<double> freqs) {
    AudioSamples out;
    constexpr double SAMPLE_RATE_HZ = 44100;
    constexpr double SAMPLE_LENGTH_S = 1.0;
    constexpr int NUM_SAMPLES = SAMPLE_RATE_HZ * SAMPLE_LENGTH_S;
    out.time_of_validity = 123.456;
    out.sample_rate = SAMPLE_RATE_HZ;

    out.channel_data.reserve(freqs.size());
    for (int i = 0; i < static_cast<int>(freqs.size()); i++) {
        out.channel_data.push_back({});
        out.channel_data.back().reserve(NUM_SAMPLES);
        for (int sample_count = 0; sample_count < NUM_SAMPLES; sample_count++) {
            out.channel_data.back().push_back(
                std::sin(2 * M_PI * freqs[i] * sample_count / SAMPLE_RATE_HZ));
        }
    }

    return out;
}

bool is_string_equal(const char *a, const char *b, const int length) {
    for (int i = 0; i < length; i++) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

void check_is_valid_wav(const std::string &data, const int num_channels, const int num_samples,
                        const int sample_rate) {
    // We should at least have enough data to read the header and the size
    ASSERT_GE(data.size(), 8);
    const WavFormat *wav = reinterpret_cast<const WavFormat *>(data.data());
    EXPECT_TRUE(is_string_equal(wav->riff_header, "RIFF", 4));

    // Check that the size of the data buffer is the same as what is reported
    ASSERT_EQ(wav->remaining_size + 8, data.size());

    // Check for the WAVE header
    EXPECT_TRUE(is_string_equal(wav->wav_header, "WAVE", 4));

    // Check for the fmt header
    EXPECT_TRUE(is_string_equal(wav->format_chunk.header, "fmt ", 4));

    // Check for the expected size of the format section
    EXPECT_EQ(wav->format_chunk.fmt_chunk_size, 18);

    // Check that the audio format is set to float
    constexpr int FLOAT_SAMPLES = 3;
    EXPECT_EQ(wav->format_chunk.audio_format, FLOAT_SAMPLES);

    // Check that the number of channels is expected
    EXPECT_EQ(wav->format_chunk.num_channels, num_channels);

    // Check that the sample rate is correct
    EXPECT_EQ(wav->format_chunk.sample_rate, sample_rate);

    // Check the the byte rate is correct
    const int byte_rate = num_channels * sample_rate * sizeof(float);
    EXPECT_EQ(wav->format_chunk.byte_rate, byte_rate);

    // Check the block alignment
    const int block_align = num_channels * sizeof(float);
    EXPECT_EQ(wav->format_chunk.block_align, block_align);

    // Check the number of bits per sample
    EXPECT_EQ(wav->format_chunk.bits_per_sample, 8 * sizeof(float));

    // Expect there to be no extra config information
    EXPECT_EQ(wav->format_chunk.cb_size, 0);

    // Look for the data header
    EXPECT_TRUE(is_string_equal(wav->data_chunk.header, "data", 4));

    // Check for the data size
    const int data_size = num_channels * num_samples * sizeof(float);
    EXPECT_EQ(wav->data_chunk.chunk_size, data_size);
}

} // namespace
TEST(WavFileTest, SingleChannelSingleBuffer) {
    // SETUP
    const AudioSamples samples = make_dummy_samples({440.0});
    WavFile wav;
    std::ostringstream data;

    // ACTION
    wav.insert_samples(samples);
    wav.write_to_ostream(data);

    // VERIFICATION
    check_is_valid_wav(data.str(), samples.channel_data.size(), samples.channel_data.front().size(),
                       samples.sample_rate);

    std::ofstream out("test1.wav", std::ios::binary);
    const std::string data_str = data.str();
    out.write(data_str.c_str(), data_str.size());
}

TEST(WavFileTest, DualChannelSingleBuffer) {
    // SETUP
    const AudioSamples samples = make_dummy_samples({440.0, 660.0});
    WavFile wav;
    std::ostringstream data;

    // ACTION
    wav.insert_samples(samples);
    wav.write_to_ostream(data);

    // VERIFICATION
    check_is_valid_wav(data.str(), samples.channel_data.size(), samples.channel_data.front().size(),
                       samples.sample_rate);

    std::ofstream out("test2.wav", std::ios::binary);
    const std::string data_str = data.str();
    out.write(data_str.c_str(), data_str.size());
}

TEST(WavFileTest, DualChannelMultiBuffer) {
    // SETUP
    const AudioSamples samples = make_dummy_samples({440.0, 660.0, 880.0});
    WavFile wav;
    std::ostringstream data;

    // ACTION
    wav.insert_samples(samples);
    wav.insert_samples(samples);
    wav.write_to_ostream(data);

    // VERIFICATION
    check_is_valid_wav(data.str(), samples.channel_data.size(),
                       samples.channel_data.front().size() * 2, samples.sample_rate);

    std::ofstream out("test3.wav", std::ios::binary);
    const std::string data_str = data.str();
    out.write(data_str.c_str(), data_str.size());
}
} // namespace washbox::audio
