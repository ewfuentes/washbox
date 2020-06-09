#include <atomic>
#include <csignal>
#include <iostream>
#include <chrono>
#include <thread>

#include "gflags/gflags.h"
#include "zmq.hpp"

using namespace std::chrono_literals;

namespace washbox::audio {
  namespace {
    std::atomic<bool> should_exit = false;
  }
  void record_stream(const std::string &server_address) {

    std::signal(SIGINT, [](int sig){ should_exit = true; });
    zmq::context_t context;
    zmq::socket_t socket(context, zmq::socket_type::sub);

    socket.setsockopt(ZMQ_SUBSCRIBE, nullptr, 0);
    std::cout << "connecting to: " << server_address << std::endl;
    socket.connect(server_address);
    std::cout << "connected? " << socket.connected() << std::endl;
    zmq::message_t message{};
    while (!should_exit) {
      zmq::recv_result_t result = socket.recv(message);
      if (result) {
        std::cout << "received message of size: " << *result << std::endl;
      }
      std::this_thread::sleep_for(50ms);
    }
  }
}

DEFINE_string(server_address, "tcp://127.0.0.1:12345" , "0MQ port to connect to");

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  washbox::audio::record_stream(FLAGS_server_address);
}
