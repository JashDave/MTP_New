#ifndef __KVIMPL__
#define __KVIMPL__

#include <iostream>
#include <string>
using namespace std;

class KVImplementation {
  void *kvsclient;
public:
  bool bind(string ip, string port);
  bool put(string key, string value);
  string get(string key);
};

#endif
