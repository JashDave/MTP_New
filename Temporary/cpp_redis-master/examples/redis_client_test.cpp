#include <cpp_redis/cpp_redis>

#include <iostream>

using namespace std;
using namespace std::chrono;

int main(void) {
  //! Enable logging
  cpp_redis::active_logger = std::unique_ptr<cpp_redis::logger>(new cpp_redis::logger);

  cpp_redis::redis_client client;

  client.connect("127.0.0.1", 6379, [](cpp_redis::redis_client&) {
    std::cout << "client disconnected (disconnection handler)" << std::endl;
  });

  high_resolution_clock::time_point t1 = high_resolution_clock::now();

double loopcount  = 1e5;

for(double i=0;i<loopcount;i++){
  // same as client.send({ "SET", "hello", "42" }, ...)
  // client.set("hello", "42");
  client.set("hello"+to_string(i), to_string(i));
  // client.set("hello"+to_string(i), to_string(i), [](cpp_redis::reply& reply) {
    // std::cout << "set hello 42: " << reply << std::endl;
    // if (reply.is_string())
    //   do_something_with_string(reply.as_string())
  // });

  // // same as client.send({ "DECRBY", "hello", 12 }, ...)
  // client.decrby("hello", 12, [](cpp_redis::reply& reply) {
  //   std::cout << "decrby hello 12: " << reply << std::endl;
  //   // if (reply.is_integer())
  //   //   do_something_with_integer(reply.as_integer())
  // });

  // // same as client.send({ "GET", "hello" }, ...)
  // client.get("hello"+to_string(i), [](cpp_redis::reply& reply) {
  //   // std::cout << "get hello: " << reply << std::endl;
  //   // if (reply.is_string())
  //   //   do_something_with_string(reply.as_string())
  // });

  // commands are pipelined and only sent when client.commit() is called
  client.commit();

  // synchronous commit, no timeout
  // client.sync_commit();

  // synchronous commit, timeout
  // client.sync_commit(std::chrono::milliseconds(100));
}
client.sync_commit();
high_resolution_clock::time_point t2 = high_resolution_clock::now();

  double time_span = duration_cast<microseconds>(t2 - t1).count();

  cout << "It took me " << time_span << " microseconds for " << loopcount << " operations." << endl;
  cout << time_span/loopcount << " microseconds per operation." << endl;
  cout << loopcount/(time_span/1e6) << " operations per second." << endl;
  return 0;
}
