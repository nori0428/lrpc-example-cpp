// http://mattn.kaoriya.net/software/grpc-benchmark.htm
// export DYLD_LIBRARY_PATH="/path/to/linear-cpp/src/.libs:/path/to/linear-cpp/deps/libtv/src/.libs:/path/to/linear-cpp/deps/libtv/deps/libuv/.libs"

#include <iostream>

#include "linear/condition_variable.h"
#include "linear/ws_client.h"

struct Person {
  Person() {}
  Person(const std::string& n, int a) : name(n), age(a) {}
  std::string name;
  int age;
  LINEAR_PACK(name, age);
};

class CustomerServiceHandler : public linear::Handler {
 public:
  CustomerServiceHandler() : finished_(false) {}
  ~CustomerServiceHandler() {}
  void OnMessage(const linear::Socket& socket, const linear::Message& message) {
    linear::Response response = message.as<linear::Response>();
    if (response.request.method == "AddPerson") {
      finished_ = true;
    } else if (response.request.method == "ListPerson") {
      customers_ = response.result.as<std::vector<Person> >();
    }
    linear::unique_lock<linear::mutex> lock(mutex_);
    cv_.notify_one();
  }
  bool AddPerson(const linear::Socket& socket, const Person& person) {
    linear::Request request("AddPerson", person);
    request.Send(socket);
    linear::unique_lock<linear::mutex> lock(mutex_);
    cv_.wait(lock);
    return finished_;
  }
  const std::vector<Person>& ListPerson(const linear::Socket& socket) {
    linear::Request request("ListPerson", linear::type::nil());
    request.Send(socket);
    linear::unique_lock<linear::mutex> lock(mutex_);
    cv_.wait(lock);
    return customers_;
  }
 private:
  bool finished_;
  std::vector<Person> customers_;
  linear::mutex mutex_;
  linear::condition_variable cv_;
};

int
main(int argc, char** argv) {
  linear::shared_ptr<CustomerServiceHandler> handler = std::make_shared<CustomerServiceHandler>();
  linear::WSClient client(handler);
  linear::WSSocket socket = client.CreateSocket("127.0.0.1", 10000);
  socket.Connect();
  if (argc == 3) {
    Person person(argv[1], atoi(argv[2]));
    handler->AddPerson(socket, person);
  } else {
    std::vector<Person> customers = handler->ListPerson(socket);
    for (const Person& p : customers) {
      std::cout << "name=" << p.name << ", age=" << p.age << std::endl;
    }
  }
  return 0;
}
