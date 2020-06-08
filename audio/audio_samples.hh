
#pragma once

#include <vector>

namespace washbox::audio {
struct AudioSamples {
    double time_of_validity;
    double sample_rate;
    std::vector<std::vector<float>> channel_data;
};
} // namespace washbox::audio
