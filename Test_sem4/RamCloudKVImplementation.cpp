/*
g++ --std=c++0x -g -I /home/jashdave/RAMCloud/install/include -I. RamCloudKVImplementation.cpp  -o RamCloudKVImplementation.out -L/home/jashdave/RAMCloud/install/bin -lramcloud -Wl,-rpath=/home/jashdave/RAMCloud/install/bin -pthread
*/

#include "KVImplementation.h"
#include <iostream>
using namespace std;

#include "ramcloud/RamCloud.h"
using namespace RAMCloud;


#define c_kvsclient ((KVSClientData *)kvsclient)

class KVSClientData {
public:
  string tablename;
  uint64_t table;
  RamCloud *cluster;
};

bool KVImplementation::bind(string ip, string port){
  kvsclient = (void *) new KVSClientData();
  c_kvsclient->tablename = "TableName12";
  c_kvsclient->cluster = new RamCloud(("tcp:host="+ip+",port="+port).c_str(),"test_cluster");
  c_kvsclient->table = c_kvsclient->cluster->createTable(c_kvsclient->tablename.c_str());
  return true;
}

bool KVImplementation::put(string skey, string sval){//cout<<"PUT"<<endl;
  try{
    c_kvsclient->cluster->write(c_kvsclient->table, skey.c_str(), skey.size(), sval.c_str(), sval.size()+1);
  }
  catch (RAMCloud::Exception& e) {
    return false;
  }
  catch (std::exception const& e) {
    return false;
  }
  catch (...) {
    return false;
  }
  return true;
}

string KVImplementation::get(string key){
  //val,serr,ierr
  string value="Err";
  try{
    Buffer buffer;
    c_kvsclient->cluster->read(c_kvsclient->table, key.c_str(), key.size(), &buffer);
    char *buf = new char[buffer.size()+1];
    buffer.copy(0,buffer.size(),buf);
    value = string(buf);
    delete buf;
  }
  catch (RAMCloud::Exception& e) {
    value = e.str();
  }
  catch (std::exception const& e) {
    value = e.what();
  }
  catch (...) {
    value = "Unknown Exception!!!";
  }
  return value;
}
