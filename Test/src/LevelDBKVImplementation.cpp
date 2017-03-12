/*
g++ --std=c++0x -g -I /home/jashdave/RAMCloud/install/include -I. RamCloudKVImplementation.cpp  -o RamCloudKVImplementation.out -L/home/jashdave/RAMCloud/install/bin -lramcloud -Wl,-rpath=/home/jashdave/RAMCloud/install/bin -pthread
*/

#include "KVImplementation.h"
#include <iostream>
using namespace std;


#include "../Implementation/LevelDB_Disk/client/src/KVStore.h"
using namespace kvstore;


#define c__kvsclient ((KVStore<string,string> *)kvsclient)


bool KVImplementation::bind(string ip, string port){
  kvsclient = (void *) new KVStore<string,string>();
  return c__kvsclient->bind(ip+":"+port,"TableName12");
}

bool KVImplementation::put(string key, string val){
    return (c__kvsclient->put(key,val).ierr==0);
}

string KVImplementation::get(string key){
  return c__kvsclient->get(key).value;
}
