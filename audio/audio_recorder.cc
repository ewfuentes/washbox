#include <atomic>
#include <chrono>
#include <csignal>
#include <functional>
#include <iostream>
#include <memory>
#include <thread>

#include "audio/proto/audio_samples_to_proto.hh"
#include "common/logging/log_writer.hh"
#include "gflags/gflags.h"
#include "zmq.hpp"

using namespace std::chrono_literals;

namespace washbox::audio {
namespace {
std::atomic<bool> should_exit = false;
using WriteFunction = std::function<bool(const proto::AudioSamples &)>;
} // namespace

void record_stream(const std::string &server_address, const WriteFunction &writer) {
    std::signal(SIGINT, [](int sig) { should_exit = true; });
    zmq::context_t context;
    zmq::socket_t socket(context, zmq::socket_type::sub);

    socket.setsockopt(ZMQ_SUBSCRIBE, nullptr, 0);
    std::cout << "connecting to: " << server_address << std::endl;
    socket.connect(server_address);
    std::cout << "connected? " << socket.connected() << std::endl;
    zmq::message_t message{};
    proto::AudioSamples samples_proto;

    while (!should_exit) {
        zmq::recv_result_t result = socket.recv(message, zmq::recv_flags::dontwait);
        if (result) {
            samples_proto.ParseFromArray(message.data(), message.size());
            const auto audio_samples = unpack_from(samples_proto);
            std::cout << "ToV: " << audio_samples.time_of_validity
                      << " sample rate: " << audio_samples.sample_rate
                      << " num_samples: " << audio_samples.channel_data.front().size() << std::endl;
            writer(samples_proto);
        }
        std::this_thread::sleep_for(50ms);
    }
}
} // namespace washbox::audio

DEFINE_string(server_address, "tcp://127.0.0.1:12345", "0MQ port to connect to");
DEFINE_string(write_directory, "", "directory to write data samples");
DEFINE_string(topic_name, "microphone_samples",
              "topic name under which the samples should be written to");

int main(int argc, char **argv) {
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    const std::unique_ptr<washbox::logging::LogWriter> writer =
        std::make_unique<washbox::logging::LogWriter>(FLAGS_write_directory);

    auto write_function = [log_writer = writer.get(),
                           topic = FLAGS_topic_name](const auto &proto_samples) {
        return log_writer->write_topic(topic, proto_samples);
    };

    washbox::audio::record_stream(FLAGS_server_address, write_function);

    return 0;
}
