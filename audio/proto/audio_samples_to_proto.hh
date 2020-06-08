
#pragma once

#include "audio/audio_samples.hh"
#include "audio/proto/audio_samples.pb.h"

namespace washbox::audio::proto {
void pack_into(const audio::AudioSamples &in, AudioSamples *out);
audio::AudioSamples unpack_from(AudioSamples &in);
} // namespace washbox::audio::proto
