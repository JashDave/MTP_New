/*
  g++ -std=c++11 TestAsyncVsBlocking_SingleUserThread.cpp -lkvstore_v2 -lboost_serialization -pthread -lkvs_redis_v2
  NOTE :
  Note that here we are using single user thread, but async implementation may
  (may not) use multiple threads inside.
*/


#define CONF string("10.129.28.44:8091")
// #define CONF string("10.129.28.141:7003")
#define TABLE string("TestTable123")
#define OPERATION_COUNT 2e4
#define READ_PROBABILITY 0.5
#define DATA_SET_SIZE 1000
#define KEY_SIZE 30
#define VALUE_SIZE 2000

#define JDEBUG

#include "jutils.h"
#include "TestUtils.h"
#include "./AutomatedBenchmarking/DataSetGenerator.cpp"
#include "./AutomatedBenchmarking/RandomNumberGenerator.cpp"
#include <iostream>
#include <vector>
#include <cassert>
#include <kvstore/KVStoreHeader_v2.h>

using namespace std;
using namespace kvstore;

struct data_holder {
  long long opr_count;
  long long failure_count;
};

void callback_handler(shared_ptr<KVData<string>> kd, void *data){
  struct data_holder *dh = (struct data_holder *)data;
  dh->opr_count++;
  if(kd->ierr != 0){
    dh->failure_count++;
  }
  TRACE(if(dh->opr_count%1000==0){cout<<"Done:"<<dh->opr_count<<endl;})
}


int main(){
  shared_ptr<KVData<string>> kd;
  vector<string> keys = DataSetGenerator::getRandomStrings(DATA_SET_SIZE,KEY_SIZE);
  vector<string> vals = DataSetGenerator::getRandomStrings(DATA_SET_SIZE,VALUE_SIZE);
  TRACE(cout<<"Initialized"<<endl);


  /* Create connection */
  KVStore<string,string> ks;
  bool succ = ks.bind(CONF,TABLE);
  jAssert(!succ,cout<<"Connection error"<<endl;);
  TRACE(cout<<"Connection successfull"<<endl);

  /* Load Data */
  for(long long i = 0; i<DATA_SET_SIZE; i++){
    kd = ks.put(keys[i],vals[i]);
    if(kd->ierr != 0){
      cerr<<"Error loading data"<<endl;
    }
  }
  TRACE(cout<<"Loading successfull"<<endl);

  /* Test Blocking call */
  // {
  //   long long opr_count = OPERATION_COUNT;
  //   int r1;
  //   int r2;
  //   long long failure_count = 0;
  //   double read_prob = READ_PROBABILITY * RAND_MAX;
  //   long long start_time = currentMicros();
  //   while(opr_count){
  //     r1 = RandomNumberGenerator::uniform(0,RAND_MAX-1);
  //     r2 = RandomNumberGenerator::zipf(0,DATA_SET_SIZE-1);
  //     if(r1<read_prob){
  //       kd = ks.get(keys[r2]);
  //     } else {
  //       kd = ks.put(keys[r2],vals[r2]);
  //     }
  //     if(kd->ierr != 0){
  //       failure_count++;
  //     }
  //     opr_count--;
  //     TRACE(if(opr_count%500 == 0){
  //       cout<<opr_count<<endl;
  //     })
  //   }
  //   long long end_time = currentMicros();
  //   long long total_time = end_time - start_time;
  //   cout<<"Blocking Call Stats:"<<endl;
  //   cout<<"Total time taken :"<<total_time/(1e6)<<" seconds"<<endl;
  //   cout<<"Throughput :"<< OPERATION_COUNT / (total_time/(1e6)) << " ops"<<endl;
  //   cout<<"Operation count:"<<(long long) (OPERATION_COUNT)<<endl;
  //   cout<<"Failures :"<<failure_count<<endl;
  // }


  /* Test Async call */
  {
    struct data_holder dh = {0,0};
    long long opr_count = OPERATION_COUNT;
    int r1;
    int r2;
    double read_prob = READ_PROBABILITY * RAND_MAX;
    long long start_time = currentMicros();
    while(opr_count){
      r1 = RandomNumberGenerator::uniform(0,RAND_MAX-1);
      r2 = RandomNumberGenerator::zipf(0,DATA_SET_SIZE-1);
      if(r1<read_prob){
        ks.async_get(keys[r2],callback_handler,&dh);
      } else {
        ks.async_put(keys[r2],vals[r2],callback_handler,&dh);
      }
      opr_count--;
      TRACE(if(opr_count%500 == 0){
        cout<<opr_count<<endl;
      })
    }
    /*wait while all callbacks are completed*/
    while(dh.opr_count!=OPERATION_COUNT){usleep(1000);}
    long long end_time = currentMicros();
    long long total_time = end_time - start_time;
    cout<<"Async Call Stats:"<<endl;
    cout<<"Total time taken :"<<total_time/(1e6)<<" seconds"<<endl;
    cout<<"Throughput :"<< OPERATION_COUNT / (total_time/(1e6)) << " ops"<<endl;
    cout<<"Operation count:"<<(long long) (OPERATION_COUNT)<<endl;
    cout<<"Failures :"<<dh.failure_count<<endl;
  }

  return 0;
}
