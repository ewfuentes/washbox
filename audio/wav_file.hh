
#pragma once

#include <ostream>

#include "audio/audio_samples.hh"

namespace washbox::audio {

// Class that collects audio samples and write out to a stream on demand
class WavFile {
  public:
    WavFile() = default;

    // Can be called multiple times, but the number of channels and sample rate
    // must be consistent across function calls. Additionally, there should be
    // an equal number of samples in each channel
    bool insert_samples(const AudioSamples &samples);

    std::ostream &write_to_ostream(std::ostream &stream) const;

  private:
    AudioSamples samples_;
};
} // namespace washbox::audio
