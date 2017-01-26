/*
//Uses https://github.com/shawn246/redis_client/
g++ -std=c++11 simple_use.cpp src/RedisClient.cpp -I./test -lhiredis -pthread

g++ -MMD -pg -g -ggdb -O0 -D_DEBUG -D_PRINT_HANDLE_TIME_ -Wno-deprecated  -Wall -m64 -pipe -std=c++11 -I. -I./common -I/usr/local/mysql/include -c ../src/RedisClient.cpp -o ../src/RedisClientd.o
g++ -pg -g -ggdb -O0 -D_DEBUG -D_PRINT_HANDLE_TIME_ -Wno-deprecated  -Wall -m64 -pipe -std=c++11   -Wno-deprecated -o client_debug  maind.o TestBased.o TestClientd.o TestConcurd.o TestGenericd.o TestHashd.o TestListd.o TestSetd.o TestStringd.o TestZsetd.o ../src/RedisClientd.o  -L./lib/linux -lhiredis -lpthread
*/

#include "redis/RedisClient.hpp"
#include <vector>
#include <unistd.h>
using namespace std;

#define RUN_TIME 30
#define THREAD_COUNT 50
//#define MOD 3+4
bool run;
int scounter[THREAD_COUNT];
int counter[THREAD_COUNT];
thread thd[THREAD_COUNT];
CRedisClient redisCli;


void doGet(int tid){
  string strKey="Key"+to_string(tid%MOD);
  string strVal;
  while(run){
        if (redisCli.Get(strKey, &strVal) == RC_SUCCESS && !strVal.empty())
        {
	    scounter[tid]++;
            //cout << "key_1 has value " << strVal << endl;
            //return 0;
        }
        else
        {
            //cout << "GET failed" << endl;
            //return -1;
        }
//cout<<strVal<<endl;
        counter[tid]++;
  }
}


void doPut(int tid){
  string strKey="Key"+to_string(tid%MOD);
  string strVal;
  while(run){
        if (redisCli.Set(strKey, "ABCDaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa") == RC_SUCCESS)
        {
	    scounter[tid]++;
            //cout << "key_1 has value " << strVal << endl;
            //return 0;
        }
        else
        {
            //cout << "SET failed" << endl;
            //return -1;
        }
        counter[tid]++;
  }
}

int main()
{
    if (!redisCli.Initialize("10.129.28.101", 7001, 20, THREAD_COUNT))
    {
        cout << "connect to redis failed" << endl;
        return -1;
    }
    int i,count,scount;

    run = true;
    count =0;
    scount =0;
    for (i = 0; i < THREAD_COUNT; i++) {
      counter[i]=0;
      scounter[i]=0;
      thd[i] = thread(doPut,i);
    }
    sleep(RUN_TIME);
    run=false;
    for (i = 0; i < THREAD_COUNT; i++) {
      if (thd[i].joinable()) {
        thd[i].join();
      }
      count+=counter[i];
      scount+=scounter[i];
    }
    cout<<"Put per second : "<<count/RUN_TIME<<endl;
    cout<<"Put per second succ: "<<scount/RUN_TIME<<endl;



    run = true;
    count =0;
    scount =0;
    for (i = 0; i < THREAD_COUNT; i++) {
      counter[i]=0;
      scounter[i]=0;
      thd[i] = thread(doGet,i);
    }
    sleep(RUN_TIME);
    run=false;
    for (i = 0; i < THREAD_COUNT; i++) {
      if (thd[i].joinable()) {
        thd[i].join();
      }
      scount+=scounter[i];
      count+=counter[i];
    }
    cout<<"Get per second : "<<count/RUN_TIME<<endl;
    cout<<"Get per second succ: "<<scount/RUN_TIME<<endl;


    // string strKey = "key_1";
    // string strVal;
    // redisCli.Set(strKey, "ABCD");
    // // redisCli.Del(strKey);
    //
    //
    // if (redisCli.Get(strKey, &strVal) == RC_SUCCESS && !strVal.empty())
    // {
    //     cout << "key_1 has value " << strVal << endl;
    //     //return 0;
    // }
    // else
    // {
    //     cout << "request failed" << endl;
    //     //return -1;
    // }
    //
    // vector<string> key_vec={"key_1","key_1_abc","key_2"};
    // vector<string> vvec={"V1","V2",""};
    // redisCli.Mset(key_vec,vvec);
    // vector<string> val_vec(3);
    // redisCli.Mget(key_vec,&val_vec);
    // for(auto v:val_vec){
    //   cout<<v<<endl;
    // }
    cout<<"End"<<endl;
}
