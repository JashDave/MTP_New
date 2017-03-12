#include "KVImplementation.h"
#include "redis/RedisClient.hpp"
#include <mutex>

CRedisClient redisCli;
bool flagnotdone = true; //Race
mutex mtx; //Race resolved

bool KVImplementation::bind(string ip, string port){
  mtx.lock();
  if(flagnotdone){
    flagnotdone=false;
    if (!redisCli.Initialize(ip, atoi(port.c_str()), 20, 300))
    {
        cout << "Connection to redis failed" << endl;
        mtx.unlock();
        return false;
    }
  }
  mtx.unlock();
  return true;
}

bool KVImplementation::put(string key, string value){
  if (redisCli.Set(key, value) == RC_SUCCESS)
  {
    return true;
  }
  return false;
}

string KVImplementation::get(string key){
  string strVal;
  if (redisCli.Get(key, &strVal) == RC_SUCCESS && !strVal.empty())
  {
      return strVal;
  }
  return "Err";
}
