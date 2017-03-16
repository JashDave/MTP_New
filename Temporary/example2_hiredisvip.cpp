#include<stdio.h>
#include<cstdlib>
#include<hiredis-vip/hircluster.h>
#include<hiredis-vip/adapters/libevent.h>
#include<unistd.h>
#include <thread>
#include<event2/thread.h>
#include <atomic>
#include <mutex>

using namespace std;

int all_count=0;
atomic<long> cnt(0);

typedef struct calldata
{
    redisClusterAsyncContext *acc;
    int count;
}calldata;

void getCallback(redisClusterAsyncContext *acc, void *r, void *privdata)
{
    redisReply *reply = r;
    int count =  *(int*)privdata;
    all_count ++;
    printf("%d ",all_count);
    // if(all_count >= count)
    // {
    //     redisClusterAsyncDisconnect(acc);
    // }
    cnt--;
}

void connectCallback(const redisAsyncContext *c, int status)
{
    if (status != REDIS_OK) {
        printf("Error: %s\n", c->errstr);
        return;
    }
    printf("Connected...\n");
}

void disconnectCallback(const redisAsyncContext *c, int status)
{
    if (status != REDIS_OK) {
        printf("Error: %s\n", c->errstr);
        return;
    }

    printf("\nDisconnected...\n");
}

void myCallback(redisClusterAsyncContext *acc, void *r, void *privdata){
  printf("*** my callback ***\n");
}

int main(int argc, char **argv)
{
    int status;
    printf("Use evthread :%d",evthread_use_pthreads());
    struct event_base *base = event_base_new();
    mutex mtx;

    event *evjd = event_new(base,-1,EV_TIMEOUT,myCallback,NULL);
    timeval tv = {10,0};
    event_add(evjd,&tv);

    std::thread td;

    redisClusterAsyncContext *acc = redisClusterAsyncConnect("10.129.28.101:7003", HIRCLUSTER_FLAG_NULL);
    if (acc->err)
    {
        printf("Error: %s\n", acc->errstr);
        return 1;
    }
    redisClusterLibeventAttach(acc,base);
    redisClusterAsyncSetConnectCallback(acc,connectCallback);
    redisClusterAsyncSetDisconnectCallback(acc,disconnectCallback);


    td = std::thread([&]{
      int ret;
      while(1){
        while(cnt==0){printf("sleeping\n"); usleep(10000); }
        mtx.lock();
        ret = event_base_loop(base,EVLOOP_NONBLOCK);
        mtx.unlock();
        if(ret==1){
          printf("Wainting...\n");
          usleep(1000);
        } else if(ret==-1){
          printf("Error ret -1\n");
        }
      }
    });

    int count = 10000;
    while(1)
    for(int i = 0; i < count; i ++)
    {
      // if(i%100==1){
      //   printf("dispatching\n");
      //   event_base_dispatch(base);
      // }
        mtx.lock();
        status = redisClusterAsyncCommand(acc, getCallback, &count, "set %d %d", i, i);
        mtx.unlock();
        if(status != REDIS_OK)
        {
            printf("error: %d %s\n", acc->err, acc->errstr);
        } else {
          cnt++;
        }
    }
  // });
  // td.detach();
    // sleep(5);
    printf("End.\n");

    if(td.joinable()) { printf("Joined\n"); td.join();}
    sleep(5);
    return 0;
}
