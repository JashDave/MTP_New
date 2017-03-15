#include<stdio.h>
#include<cstdlib>
#include<hiredis-vip/hircluster.h>
#include<hiredis-vip/adapters/libevent.h>
#include<unistd.h>
#include <thread>

int all_count=0;

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
    printf("%d\n",all_count);
    if(all_count >= count)
    {
        redisClusterAsyncDisconnect(acc);
    }
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

int main(int argc, char **argv)
{
    int status;
    struct event_base *base = event_base_new();
    redisClusterAsyncContext *acc = redisClusterAsyncConnect("10.129.28.101:7003", HIRCLUSTER_FLAG_NULL);
    if (acc->err)
    {
        printf("Error: %s\n", acc->errstr);
        return 1;
    }
    redisClusterLibeventAttach(acc,base);
    redisClusterAsyncSetConnectCallback(acc,connectCallback);
    redisClusterAsyncSetDisconnectCallback(acc,disconnectCallback);

// std::thread td([&]{
    int count = 10000;
    for(int i = 0; i < count; i ++)
    {
      // if(i%100==1){
      //   printf("dispatching\n");
      //   event_base_dispatch(base);
      // }
        status = redisClusterAsyncCommand(acc, getCallback, &count, "set %d %d", i, i);
        if(status != REDIS_OK)
        {
            printf("error: %d %s\n", acc->err, acc->errstr);
        }
    }
  // });
  // td.detach();
    // sleep(5);
    std::thread td[5];
    for(int i=0; i<5;i++) {td[i] = std::thread(event_base_dispatch, base);}
    printf("End.\n");

    // if(td.joinable()) { printf("Joined\n"); td.join();}
    sleep(5);
    return 0;
}
