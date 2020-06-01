#include <iostream>
#include <chrono>
#include <thread>
#include "zmq.hpp"


int main() {
  using namespace std::chrono;
  zmq::context_t context{1};

  zmq::socket_t socket(context, zmq::socket_type::rep);

  socket.bind("tcp://*:5555");

  const std::string data = "World!";

  for (;;) {
    zmq::message_t request;
    socket.recv(request, zmq::recv_flags::none);
    std::cout << "Received: " << request.to_string() << std::endl;

    std::this_thread::sleep_for(1s);

    socket.send(zmq::buffer(data), zmq::send_flags::none);
  }

  std::cout << "Hello, World!" << std::endl;
}

