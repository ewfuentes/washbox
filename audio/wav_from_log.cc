
#include <fstream>
#include <memory>

#include "gflags/gflags.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"

#include "audio/proto/audio_samples_to_proto.hh"
#include "audio/wav_file.hh"
#include "common/logging/proto/log.pb.h"

namespace washbox::audio {
bool write_wav_to_file(const std::string &log_directory, const std::string &topic,
                       const std::string &output_file) {

    // Read the index file
    logging::proto::Index index;
    {
        std::string index_file = log_directory + "/" + topic + ".index";
        std::ifstream index_stream(index_file, std::ios::binary);
        index.ParseFromIstream(&index_stream);
    }

    int current_file_idx = -1;
    std::ifstream input_stream;
    std::unique_ptr<google::protobuf::io::IstreamInputStream> proto_stream;
    proto::AudioSamples proto_samples;
    int num_samples = 0;
    WavFile wav_file;
    for (int i = 0; i < index.entries_size(); i++) {
        const auto &entry = index.entries(i);
        if (current_file_idx != entry.data_file_idx()) {
            input_stream = std::ifstream(index.data_files(entry.data_file_idx()), std::ios::binary);
            current_file_idx = entry.data_file_idx();
            proto_stream =
                std::make_unique<google::protobuf::io::IstreamInputStream>(&input_stream);
        }

        // Read in the next message
        proto_samples.Clear();
        if (i != (index.entries_size() - 1)) {
          // We have a next entry
          const auto &next_entry = index.entries(i+1);
          if (entry.data_file_idx() == next_entry.data_file_idx()) {
            // And it's in the current file
            const int bytes_to_read = next_entry.data_idx() - entry.data_idx();
            proto_samples.MergePartialFromBoundedZeroCopyStream(proto_stream.get(), bytes_to_read);
          } else {
            // and it's in the next file, so read to the end of the current file
            proto_samples.ParseFromZeroCopyStream(proto_stream.get());
          }
        } else {
          // This is the last entry, read to the end of the file
          proto_samples.ParseFromZeroCopyStream(proto_stream.get());
        }

        AudioSamples samples = unpack_from(proto_samples);
        num_samples += samples.channel_data.front().size();
        wav_file.insert_samples(samples);
    }
    std::cout << "read " << num_samples << " samples" << std::endl;

    std::ofstream file_out(output_file , std::ios::binary);
    wav_file.write_to_ostream(file_out);

    return true;
}
} // namespace washbox::audio

DEFINE_string(read_directory, "", "directory to read from");
DEFINE_string(topic, "", "topic name");
DEFINE_string(output, "", "output file");

int main(int argc, char **argv) {
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    washbox::audio::write_wav_to_file(FLAGS_read_directory, FLAGS_topic, FLAGS_output);
}
