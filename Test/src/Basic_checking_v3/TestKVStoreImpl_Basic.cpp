/*
  g++ -std=c++11 TestKVStoreImpl_Basic.cpp -lkvstore_v2 -lboost_serialization -pthread -lkvs_redis_v2
*/

// #define CONF string("10.129.28.44:8091")
// #define CONF string("10.129.28.141:7003")
// #define CONF string("127.1.1.1:8090")
#define TABLE string("TestTable123")

// #define JDEBUG

#include "../jutils.h"
#include <iostream>
#include <cassert>
#include <kvstore/KVStoreHeader_v3.h>

using namespace std;
using namespace kvstore;

int main(){
  KVData<string> r;

  /* Create connection */
  KVStore<int,string> ks;
  IS_REACHABLE
  bool succ = ks.bind(CONF,TABLE);
  jAssert(!succ,cout<<"Connection error"<<endl;);

  /* Check successfull put */
  IS_REACHABLE
  r = ks.put(1,"One");
  jAssert(r.ierr!=0, cout<<"Serr:"<<r.serr<<endl;)

  /* Check successfull get */
  IS_REACHABLE
  r = ks.get(1);
  jAssert(r.ierr!=0, cout<<" Error in get(1):"<<r.serr<<endl;)
  jAssert(r.value!="One", cout<<"Incorrect value from get(1) got:"<<r.value<<endl;)

  /* Check successfull del */
  IS_REACHABLE
  r = ks.del(1);
  jAssert(r.ierr!=0, cout<<"Error in del serr:"<<r.serr<<endl;)

  /* Check unsuccessfull get */
  IS_REACHABLE
  r = ks.get(1);
  jAssert(r.ierr==0, cout<<" Error in unsuccessfull get(1) got:"<<r.value<<endl;)

  /* Check unsuccessfull del */
  IS_REACHABLE
  r = ks.del(1);
  jAssert(r.ierr==0, cout<<"Error in unsuccessfull del"<<r.serr<<endl;)



  /* Check successfull mput */
  std::vector<int> keys = {1,2,3};
  std::vector<string> vals = {"one","two","three"};
  int sz = keys.size();
  IS_REACHABLE
  std::vector<KVData<string>> rr = ks.mput(keys,vals);
  for(int i=0;i<sz;i++){
    jAssert(rr[i].ierr!=0, cout<<"i:"<<i<<" mput err:"<<rr[i].serr<<endl;)
  }

  // r = ks.put(1,"One");
  /* Check successfull mget */
  IS_REACHABLE
  rr = ks.mget(keys);
  for(int i=0;i<sz;i++){
    jAssert(rr[i].ierr!=0, cout<<"i:"<<i<<" mget err:"<<rr[i].serr<<endl;)
    jAssert(rr[i].value!=vals[i], cout<<"i:"<<i<<" mget value mismatch got:"<<rr[i].value<<" expeted:"<<vals[i]<<endl;)
  }

  /* Check successfull mdel */
  IS_REACHABLE
  rr = ks.mdel(keys);
  for(int i=0;i<sz;i++){
    jAssert(rr[i].ierr!=0, cout<<"i:"<<i<<" mdel err:"<<rr[i].serr<<endl;)
  }

  /* Check unsuccessfull mget */
  IS_REACHABLE
  rr = ks.mget(keys);
  for(int i=0;i<sz;i++){
    jAssert(rr[i].ierr==0, cout<<"i:"<<i<<" mget unsuccesfull got:"<<rr[i].value<<endl;)
  }

  cout<<"All testcases passed successfully for "<<__FILE__<<"."<<endl;
  return 0;
}
