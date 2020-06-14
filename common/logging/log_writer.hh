#pragma once

#include <fstream>
#include <string>
#include <utility>

#include "common/logging/proto/log.pb.h"

namespace washbox::logging {
class LogWriter {
  public:
    LogWriter(const std::string &directory, const int max_block_size = 1024 * 1024 * 128);

    ~LogWriter();

    template <typename MessageType>
    bool write_topic(const std::string &topic, const MessageType &message) {
        auto stream_iter = streams_by_topic_.find(topic);
        if (stream_iter == streams_by_topic_.end()) {
            std::tie(stream_iter, std::ignore) =
              streams_by_topic_.insert({topic, make_new_stream(topic, message.GetTypeName())});
        }

        auto &index = stream_iter->second.first;
        auto &out = stream_iter->second.second;

        // Write out the record
        auto &entry = *index.add_entries();
        entry.set_data_file_idx(index.data_files_size() - 1);
        entry.set_data_idx(out.tellp());

        message.SerializeToOstream(&out);

        // If we're over the max block size, open a new block
        if (out.tellp() > max_block_size_) {
            streams_by_topic_.at(topic).second = open_new_block(topic, index);
        }
        return true;
    }

  const proto::Index &index_for_topic(const std::string &topic) { return streams_by_topic_.at(topic).first; }

  private:
    using Stream = std::pair<proto::Index, std::ofstream>;

    Stream make_new_stream(const std::string &topic, const std::string &message);
    std::ofstream open_new_block(const std::string &topic, proto::Index &index);

    std::string directory_;
    int max_block_size_;
    std::unordered_map<std::string, Stream> streams_by_topic_;
};
} // namespace washbox::logging
