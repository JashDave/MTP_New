#include "KVImplementation.h"
#include <stdio.h>

#include "redis/KVStore.h"
using namespace kvstore;

#define k ((KVStore<string,string>*)kvsclient)

bool KVImplementation::bind(string ip, string port){
  kvsclient = new KVStore<string,string>();
  return k->bind(ip+":"+port,"TB123");
}

bool KVImplementation::put(string key, string value){
  KVData<string> kvd;
  kvd = k->put(key,value);
  return kvd.ierr==0;
}

string KVImplementation::get(string key){
  KVData<string> kvd;
  kvd = k->get(key);
  return kvd.value;
}

// KVImplementation::~KVImplementation(){
//   // redisClusterFree(cc);
// }
