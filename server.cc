// http://mattn.kaoriya.net/software/grpc-benchmark.htm
// export DYLD_LIBRARY_PATH="/path/to/linear-cpp/src/.libs:/path/to/linear-cpp/deps/libtv/src/.libs:/path/to/linear-cpp/deps/libtv/deps/libuv/.libs"

#include <iostream>

#include "linear/ws_server.h"

struct Person {
  std::string name;
  int age;
  LINEAR_PACK(name, age);
};

class CustomerServiceHandler : public linear::Handler {
 private:
  std::vector<Person> customers;
  void AddPerson(const linear::Socket& socket, const linear::Request& request) {
    std::cout << "AddPerson" << std::endl;
    customers.push_back(request.params.as<Person>());
    linear::Response response(request.msgid, linear::type::nil());
    response.Send(socket);
    std::cout << "Done" << std::endl;
  }
  void ListPerson(const linear::Socket& socket, const linear::Request& request) {
    std::cout << "ListPerson" << std::endl;
    linear::Response response(request.msgid, customers);
    response.Send(socket);
    std::cout << "Done" << std::endl;
  }
 public:
  void OnMessage(const linear::Socket& socket, const linear::Message& message) {
    linear::Request request = message.as<linear::Request>();
    if (request.method == "AddPerson") {
      AddPerson(socket, request);
    } else if (request.method == "ListPerson") {
      ListPerson(socket, request);
    }
  }
};

int
main(int argc, char* argv[]) {
  linear::shared_ptr<CustomerServiceHandler> handler = std::make_shared<CustomerServiceHandler>();
  linear::WSServer server(handler);
  server.Start("0.0.0.0", 10000);
  std::cout << "press enter to exit" << std::endl;
  std::string p;
  std::getline(std::cin, p);
  return 0;
}
