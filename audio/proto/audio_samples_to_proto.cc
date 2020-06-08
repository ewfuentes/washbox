#include "audio/proto/audio_samples_to_proto.hh"

namespace washbox::audio::proto {
void pack_into(const audio::AudioSamples &in, AudioSamples *out) {
    out->Clear();
    out->set_time_of_validity(in.time_of_validity);
    out->set_sample_rate(in.sample_rate);

    for (const auto &channel : in.channel_data) {
        AudioSamples::Samples &channel_data = *out->add_channel_data();
        for (const double sample : channel) {
            channel_data.add_samples(sample);
        }
    }
}

audio::AudioSamples unpack_from(AudioSamples &in) {
    audio::AudioSamples out;
    out.time_of_validity = in.time_of_validity();
    out.sample_rate = in.sample_rate();
    out.channel_data.reserve(in.channel_data_size());
    for (const auto &channel : in.channel_data()) {
        out.channel_data.emplace_back();
        out.channel_data.back().reserve(in.channel_data_size());
        for (const double sample : channel.samples()) {
            out.channel_data.back().push_back(sample);
        }
    }
    return out;
}
} // namespace washbox::audio::proto
