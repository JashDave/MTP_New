/*
  g++ -std=c++11 TestKVStoreImpl_Basic_Async.cpp -lkvstore_v2 -lboost_serialization -pthread -lkvs_redis_v2
*/

#define CONF string("10.129.28.44:8091")
// #define CONF string("10.129.28.141:7003")
#define TABLE string("TestTable123")

#define JDEBUG

#include "jutils.h"
#include <iostream>
#include <cassert>
#include <kvstore/KVStoreHeader_v2.h>

using namespace std;
using namespace kvstore;

void successfullGetCallBack(std::shared_ptr<KVData<string>> r,void *data){
  jAssert(r->ierr!=0, cout<<" Error in get(1):"<<r->serr<<" called from line "<<*((int*)data)<<endl;)
  jAssert(r->value!="One", cout<<"Incorrect value from get(1) got:"<<r->value<<" called from line "<<*((int*)data)<<endl;)
  TRACE(cout<<"successfull get :"<<r->value<<endl;)
}

void unsuccessfullGetCallBack(std::shared_ptr<KVData<string>> r,void *data){
  jAssert(r->ierr==0, cout<<" Error in unsuccessfull get(1) got:"<<r->value<<" called from line "<<*((int*)data)<<endl;)
  TRACE(cout<<"unsuccessfull get."<<endl;)
}

void successfullCallBack(std::shared_ptr<KVData<string>> r,void *data){
  jAssert(r->ierr!=0, cout<<"Serr:"<<r->serr<<" called from line "<<*((int*)data)<<endl;)
  TRACE(cout<<"successfull callback."<<endl;)
}

void unsuccessfullCallBack(std::shared_ptr<KVData<string>> r,void *data){
  jAssert(r->ierr==0, cout<<"Error in unsuccessfull del called from line "<<*((int*)data)<<endl;)
  TRACE(cout<<"unsuccessfull callback."<<endl;)
}


int main(){
  shared_ptr<KVData<string>> r;

  /* Create connection */
  KVStore<int,string> ks;
  IS_REACHABLE
  bool succ = ks.bind(CONF,TABLE);
  jAssert(!succ,cout<<"Connection error"<<endl;);

  /* Check successfull put */
  IS_REACHABLE
  int line_no1 = __LINE__; ks.async_put(1,"One",successfullCallBack,&line_no1);

  /* Check successfull get */
  IS_REACHABLE
  int line_no2 = __LINE__; ks.async_get(1,successfullGetCallBack,&line_no2);

  /* Check successfull del */
  IS_REACHABLE
  int line_no3 = __LINE__; ks.async_del(1,successfullCallBack,&line_no3);

  /* Check unsuccessfull get */
  IS_REACHABLE
  int line_no4 = __LINE__; ks.async_get(1,unsuccessfullGetCallBack,&line_no4);

  /* Check unsuccessfull del */
  IS_REACHABLE
  int line_no5 = __LINE__; ks.async_del(1,unsuccessfullCallBack,&line_no5);

  sleep(5); /*wait for callbacks to complete*/
  cout<<"All testcases passed successfully for "<<__FILE__<<"."<<endl;
  return 0;
}
