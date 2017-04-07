/*
g++ -std=c++11 CommonNoReqPerSecTest.cpp libMemcachedKVImpl.cpp -lmemcached -pthread
*/

#include "KVImplementation.h"
#include <iostream>
#include <vector>
#include <libmemcached/memcached.h>
using namespace std;



#define c_kvsclient ((KVSClientData *)kvsclient)

class KVSClientData {
public:
  string conn;
  memcached_st *memc;
  string tablename;
};

bool KVImplementation::bind(string ip, string port){
  kvsclient = (void *) new KVSClientData();
  c_kvsclient->tablename = "TableName12";
  c_kvsclient->conn = "--SERVER="+ip+":"+port;
  c_kvsclient->memc = memcached(c_kvsclient->conn.c_str(), c_kvsclient->conn.size());
  if(c_kvsclient->memc == NULL){
    /* cerr<<"Error creating memcached connection."<<endl; */
    return false;
  }
  return true;
}

bool KVImplementation::put(string key, string val){//cout<<"PUT"<<endl;
  key = c_kvsclient->tablename + key;
  memcached_return_t rc = memcached_set(c_kvsclient->memc, key.c_str(), key.size(), val.c_str(), val.size(), (time_t)0, (uint32_t)0);
  if (rc != MEMCACHED_SUCCESS)
  {
    cerr<<"set failure"<<endl;
    return false;
  }
  return true;
}

string KVImplementation::get(string key){
  key = c_kvsclient->tablename + key;
  char *op;
  size_t value_length;
  uint32_t flags;
  memcached_return_t error;
  op = memcached_get(c_kvsclient->memc, key.c_str(), key.size(), &value_length, &flags, &error);
  if(error != MEMCACHED_SUCCESS){
    cerr<<"GET Error"<<endl;
    return "Error__..";
    // return string(libmemcached_strerror(error));
                  // libmemcached_strerror
  }
  string ret(op,value_length);
  free(op);
  return ret;
}
