#include <cpp_redis/cpp_redis>
#include <iostream>

using namespace std;
using namespace std::chrono;

//#define THREAD_COUNT 1

string IP = "127.0.0.1";
//string IP = "10.129.28.101";
int PORT = 6379;

double loopcount  = 1e5;
cpp_redis::redis_client client;
string data="ABCDEFGHIJ";

double doPut(){
  static int counter=0;
  // counter++; //? Race condition "BAD USE"

  cpp_redis::redis_client client;
  client.connect(IP, PORT, [](cpp_redis::redis_client&) {
    std::cout << "client disconnected (disconnection handler)" << std::endl;
  });
  high_resolution_clock::time_point t1 = high_resolution_clock::now();

  for(double i=0;i<loopcount;i++){
    client.set("hello"+to_string(counter)+"_"+to_string(i), data+to_string(i));
    // client.set("hello"+to_string(i), to_string(i), [](cpp_redis::reply& reply) {
      // std::cout << "set hello 42: " << reply << std::endl;
      // if (reply.is_string())
      //   do_something_with_string(reply.as_string())
    // });
    // commands are pipelined and only sent when client.commit() is called
    // client.commit();
    // synchronous commit, no timeout
    client.sync_commit();
    // synchronous commit, timeout
    // client.sync_commit(std::chrono::milliseconds(100));
  }
  client.sync_commit();
  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  double time_span = duration_cast<microseconds>(t2 - t1).count();
  return time_span;
}



double doGet(){
  static int counter=0;
  // counter++; //? Race condition "BAD USE"

  cpp_redis::redis_client client;

  client.connect(IP, PORT, [](cpp_redis::redis_client&) {
    std::cout << "client disconnected (disconnection handler)" << std::endl;
  });
  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  for(double i=0;i<loopcount;i++){
    // // same as client.send({ "GET", "hello" }, ...)
    client.get("hello"+to_string(counter)+"_"+to_string(i), [](cpp_redis::reply& reply) {
      // std::cout << "get hello: " << reply << std::endl;
    //   // if (reply.is_string())
    //   //   do_something_with_string(reply.as_string())
    });

    // commands are pipelined and only sent when client.commit() is called
    // client.commit();
    // synchronous commit, no timeout
    client.sync_commit();
    // synchronous commit, timeout
    // client.sync_commit(std::chrono::milliseconds(100));
  }
  client.sync_commit();
  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  double time_span = duration_cast<microseconds>(t2 - t1).count();
  return time_span;
}

/*
double doGet(){

  cpp_redis::redis_client client;

  client.connect(IP, PORT, [](cpp_redis::redis_client&) {
    std::cout << "client disconnected (disconnection handler)" << std::endl;
  });
  high_resolution_clock::time_point t1 = high_resolution_clock::now();
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
  return time_span;
}
*/


void doFunc(double (*func)(),string desc){
  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  thread t[THREAD_COUNT];
  for(int i=0;i<THREAD_COUNT;i++){
    t[i] = thread(func);
  }
  for (int i = 0; i < THREAD_COUNT; i++) {
    if (t[i].joinable()) {
      t[i].join();
    }
  }
  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  double time_span = duration_cast<microseconds>(t2 - t1).count();
  cout << desc << endl;
  cout << "It took me " << time_span << " microseconds for " << loopcount*THREAD_COUNT << " operations." << endl;
  cout << time_span/(loopcount*THREAD_COUNT) << " microseconds per operation." << endl;
  cout << (loopcount*THREAD_COUNT)/(time_span/1e6) << " operations per second." << endl;
}

int main(void) {
  for(int i=0;i<1e2;i++){
   data+="1234567890";
  }

  client.connect(IP, PORT, [](cpp_redis::redis_client&) {
    std::cout << "client disconnected (disconnection handler)" << std::endl;
  });

  //! Enable logging
  // cpp_redis::active_logger = std::unique_ptr<cpp_redis::logger>(new cpp_redis::logger);
  doFunc(doPut,"Put function");
  doFunc(doGet,"Get function");

  return 0;
}
