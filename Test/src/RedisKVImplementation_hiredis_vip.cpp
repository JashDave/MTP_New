#include "KVImplementation.h"
#include <stdio.h>
#include <hircluster.h>


#define cc ((redisClusterContext*)kvsclient)

bool KVImplementation::bind(string ip, string port){
  kvsclient = redisClusterConnect(string(ip+":"+port).c_str(), HIRCLUSTER_FLAG_NULL);
  if(cc == NULL || cc->err)
  {
    printf("connect error : %s\n", cc == NULL ? "NULL" : cc->errstr);
    return false;
  }
  return true;
}

bool KVImplementation::put(string key, string value){
  redisReply *reply = redisClusterCommand(cc, "set %s %s", key.c_str(), value.c_str());
  if(reply == NULL)
  {
    printf("reply is null[%s]\n", cc->errstr);
    //redisClusterFree(cc); //??
    return false;
  }
  freeReplyObject(reply); //Double free?
  return true;
}

string KVImplementation::get(string key){
  string strVal;
  redisReply *reply = redisClusterCommand(cc, "get %s", key.c_str());
  if(reply == NULL)
  {
    printf("reply is null[%s]\n", cc->errstr);
    //redisClusterFree(cc); //??
    return string(cc->errstr);
  }
  // printf("get %s\n",reply->str );
  freeReplyObject(reply); // Double free?
  if(reply->str == NULL){
    printf("reply->str is NULL\n");
  }
  return string(reply->str);
}

// KVImplementation::~KVImplementation(){
//   // redisClusterFree(cc);
// }
