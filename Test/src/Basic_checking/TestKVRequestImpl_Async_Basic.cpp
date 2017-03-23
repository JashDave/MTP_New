/*
  g++ -std=c++11 TestKVRequestImpl_Async_Basic.cpp -lkvstore_v2 -lboost_serialization -pthread -lkvs_redis_v2
*/

// #define CONF string("10.129.28.44:8091")
// #define CONF string("10.129.28.141:7003")
#define TABLE string("TestTable123")

// #define JDEBUG

#include "../jutils.h"
#include <iostream>
#include <vector>
#include <cassert>
#include <kvstore/KVStoreHeader_v2.h>

using namespace std;
using namespace kvstore;

#define KEYS vector<int> keys = {1,2,3};
#define VALUES vector<string> vals = {"One","Two","Three"};

int main(){
  KEYS
  VALUES
  // int sz = keys.size();

  /* Create connection */
  KVRequest kr;
  IS_REACHABLE
  bool succ = kr.bind(CONF);
  jAssert(!succ,cout<<"Connection error"<<endl;);

  /* Check successfull put */
  IS_REACHABLE
  for(int i=0;i<keys.size();i++){
    kr.put<int,string>(keys[i],vals[i],TABLE);
  }
  IS_REACHABLE
  auto lambda_fn = [](shared_ptr<KVResultSet> rs,void *p){
    KEYS
    VALUES
    // cout<<"Lambda"<<endl;
    shared_ptr<KVData<string>> kd;
    jAssert(rs->size()!=keys.size(),cout<<"Put size mismatch expected:"<<keys.size()<<" got:"<<rs->size()<<endl;)
    for(int i=0;i<rs->size();i++){
      kd = rs->get<string>(i);
      jAssert(rs->oprType(i)!=OPR_TYPE_PUT, cout<<"Incorrect operation type for index="<<i<<" expected:"<<OPR_TYPE_GET<<" got:"<<rs->oprType(i)<<endl;)
      jAssert(kd->ierr!=0, cout<<"Error in put serr:"<<kd->serr<<" for index="<<i<<endl;)
    }
  };
  IS_REACHABLE
  kr.async_execute(lambda_fn,NULL);
  IS_REACHABLE
  kr.reset();
  IS_REACHABLE


  /* Check successfull get */
  IS_REACHABLE
  for(int i=0;i<keys.size();i++){
    kr.get<int,string>(keys[i],TABLE);
  }
  kr.async_execute([](shared_ptr<KVResultSet> rs,void *p){
    KEYS
    VALUES
    shared_ptr<KVData<string>> kd;
    jAssert(rs->size()!=keys.size(),cout<<"Get size mismatch expected:"<<keys.size()<<" got:"<<rs->size()<<endl;)
    for(int i=0;i<rs->size();i++){
      kd = rs->get<string>(i);
      jAssert(rs->oprType(i)!=OPR_TYPE_GET, cout<<"Incorrect operation type for index="<<i<<" expected:"<<OPR_TYPE_GET<<" got:"<<rs->oprType(i)<<endl;)
      jAssert(kd->ierr!=0, cout<<" Error in get serr:"<<kd->serr<<" for index="<<i<<endl;)
      jAssert(kd->value!=vals[i], cout<<"Incorrect value for get("<<keys[i]<<") expected:"<<vals[i]<<" got:"<<kd->value<<endl;)
    }
  },NULL);
  kr.reset();

  /* Check successfull del */
  IS_REACHABLE
  for(int i=0;i<keys.size();i++){
    kr.del<int,string>(keys[i],TABLE);
  }
  kr.async_execute([&](shared_ptr<KVResultSet> rs,void *p){
    KEYS
    VALUES
    shared_ptr<KVData<string>> kd;
    jAssert(rs->size()!=keys.size(),cout<<"Del size mismatch expected:"<<keys.size()<<" got:"<<rs->size()<<endl;)
    for(int i=0;i<rs->size();i++){
      kd = rs->get<string>(i);
      jAssert(rs->oprType(i)!=OPR_TYPE_DEL, cout<<"Incorrect operation type for index="<<i<<" expected:"<<OPR_TYPE_DEL<<" got:"<<rs->oprType(i)<<endl;)
      jAssert(kd->ierr!=0, cout<<"Error in del serr:"<<kd->serr<<" for index="<<i<<endl;)
    }
  },NULL);
  kr.reset();

  /* Check unsuccessfull get */
  IS_REACHABLE
  for(int i=0;i<keys.size();i++){
    kr.get<int,string>(keys[i],TABLE);
  }
  kr.async_execute([&](shared_ptr<KVResultSet> rs,void *p){
    KEYS
    VALUES
    shared_ptr<KVData<string>> kd;
    jAssert(rs->size()!=keys.size(),cout<<"Unsuccessfull get size mismatch expected:"<<keys.size()<<" got:"<<rs->size()<<endl;)
    for(int i=0;i<rs->size();i++){
      kd = rs->get<string>(i);
      jAssert(rs->oprType(i)!=OPR_TYPE_GET, cout<<"Incorrect operation type for index="<<i<<" expected:"<<OPR_TYPE_GET<<" got:"<<rs->oprType(i)<<endl;)
      jAssert(kd->ierr==0, cout<<"Error in unsuccessfull get got:"<<kd->value<<" for index="<<i<<endl;)
    }
  },NULL);
  kr.reset();

  /* Check unsuccessfull del */
  IS_REACHABLE
  for(int i=0;i<keys.size();i++){
    kr.del<int,string>(keys[i],TABLE);
  }
  kr.async_execute([&](shared_ptr<KVResultSet> rs,void *p){
    KEYS
    VALUES
    shared_ptr<KVData<string>> kd;
    jAssert(rs->size()!=keys.size(),cout<<"Unsuccessfull del size mismatch expected:"<<keys.size()<<" got:"<<rs->size()<<endl;)
    for(int i=0;i<rs->size();i++){
      kd = rs->get<string>(i);
      jAssert(rs->oprType(i)!=OPR_TYPE_DEL, cout<<"Incorrect operation type for index="<<i<<" expected:"<<OPR_TYPE_DEL<<" got:"<<rs->oprType(i)<<endl;)
      jAssert(kd->ierr==0, cout<<"Error in unsuccessfull del for index="<<i<<endl;)
    }
  },NULL);
  kr.reset();

  sleep(5); /*Wait for async to complete;*/
  cout<<"All testcases passed successfully for "<<__FILE__<<"."<<endl;
  return 0;
}
