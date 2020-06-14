
#include "common/logging/log_writer.hh"

#include <iomanip>
#include <sstream>

namespace washbox::logging {

LogWriter::LogWriter(const std::string &directory, const int max_block_size)
    : directory_(directory), max_block_size_(max_block_size){};

  LogWriter::~LogWriter() {
    for (auto &topic_and_stream : streams_by_topic_) {
      const std::string &topic = topic_and_stream.first;
      const auto &index = topic_and_stream.second.first;

      const std::string index_file = directory_ + "/" + topic + ".index";
      std::ofstream out(index_file, std::ios::binary);
      index.SerializeToOstream(&out);
    }
  }

LogWriter::Stream LogWriter::make_new_stream(const std::string &topic,
                                             const std::string &type_name) {
    proto::Index index;
    index.set_message_name(type_name);
    std::ofstream fstream = open_new_block(topic, index);
    return std::make_pair(std::move(index), std::move(fstream));
}

std::ofstream LogWriter::open_new_block(const std::string &topic, proto::Index &index) {
    std::ostringstream filename;
    filename << directory_ << "/" << topic + ".data." << std::setw(4) << std::setfill('0')
             << index.data_files_size();

    index.add_data_files(filename.str());
    return std::ofstream(filename.str(), std::ios::binary);
}

} // namespace washbox::logging
