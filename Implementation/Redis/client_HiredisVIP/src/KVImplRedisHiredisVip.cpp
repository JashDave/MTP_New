#include "KVStoreHeader_v2.h"
#include <hiredis-vip/hircluster.h>

namespace kvstore {

  #define c_kvsclient ((KVStoreClient*)dataholder)

  class KVStoreClient{
  public:
    redisClusterContext* rc;
    string tablename;
    ~KVStoreClient(){
      redisClusterFree(rc);
    }
  };

  KVImplHelper::KVImplHelper(){
    dataholder = (void*) new KVStoreClient();
  }

  KVImplHelper::~KVImplHelper(){
    delete(dataholder);
  }

  bool KVImplHelper::bind(string conn, string tablename){
    c_kvsclient->tablename = tablename;
    c_kvsclient->rc = redisClusterConnect(conn.c_str(), HIRCLUSTER_FLAG_NULL);
    if(c_kvsclient->rc == NULL || c_kvsclient->rc->err)
    {
      //printf("connect error : %s\n", rc == NULL ? "NULL" : rc->errstr);
      return false;
    }
    return true;
  }

  std::shared_ptr<KVData<string>> KVImplHelper::get(string const& key){
    std::shared_ptr<KVData<string>> ret = std::make_shared<KVData<string>>();
    redisReply *reply = redisClusterCommand(c_kvsclient->rc, "get %s", (c_kvsclient->tablename+key).c_str());
    if(reply == NULL)
    {
      ret->ierr = -1;
      ret->serr = "Unknown error.";
      // printf("reply is null[%s]\n", c_kvsclient->rc->errstr);
      //redisClusterFree(c_kvsclient->rc); //??
    } else if(reply->str == NULL){
      ret->ierr = -1;
      ret->serr = "Value doesn't exists.";
      freeReplyObject(reply);
    } else {
      ret->ierr = 0;
      ret->value = string(reply->str);
      freeReplyObject(reply);
    }
    return ret;
  }

  std::shared_ptr<KVData<string>> KVImplHelper::put(string const& key,string const& val){
    std::shared_ptr<KVData<string>> ret = std::make_shared<KVData<string>>();
    redisReply *reply = redisClusterCommand(c_kvsclient->rc, "set %s %s", (c_kvsclient->tablename+key).c_str(), val.c_str());
    if(reply == NULL)
    {
      ret->ierr = -1;
      ret->serr = "Unknown error.";
      // printf("reply is null[%s]\n", c_kvsclient->rc->errstr);
      //redisClusterFree(c_kvsclient->rc); //??
    } else {
      ret->ierr = 0;
      freeReplyObject(reply);
    }
    return ret;
  }

  std::shared_ptr<KVData<string>> KVImplHelper::del(string const& key){
    std::shared_ptr<KVData<string>> ret = std::make_shared<KVData<string>>();
    redisReply *reply = redisClusterCommand(c_kvsclient->rc, "del %s", (c_kvsclient->tablename+key).c_str());
    if(reply == NULL)
    {
      ret->ierr = -1;
      ret->serr = "Unknown error.";
      // printf("reply is null[%s]\n", c_kvsclient->rc->errstr);
      //redisClusterFree(c_kvsclient->rc); //??
    } else {
      ret->ierr = reply->integer; // 0;
      freeReplyObject(reply);
    }
    return ret;
  }

  bool KVImplHelper::clear(){
    /* Not yet implemented */
    return false;
  };

  int KVImplHelper::mget(vector<string>& key, vector<string>& tablename, vector<std::shared_ptr<KVData<string>>>& ret){
    int sz = key.size();
    for(int i=0;i<sz;i++){
      c_kvsclient->tablename = tablename[i];
      ret.push_back(get(key[i]));
    }
    return 0;
  }
  int KVImplHelper::mput(vector<string>& key, vector<string>& val, vector<string>& tablename, vector<std::shared_ptr<KVData<string>>>& ret){
    int sz = key.size();
    for(int i=0;i<sz;i++){
      c_kvsclient->tablename = tablename[i];
      ret.push_back(put(key[i],val[i]));
    }
    return 0;
  }
  int KVImplHelper::mdel(vector<string>& key, vector<string>& tablename, vector<std::shared_ptr<KVData<string>>>& ret){
    int sz = key.size();
    for(int i=0;i<sz;i++){
      c_kvsclient->tablename = tablename[i];
      ret.push_back(del(key[i]));
    }
    return 0;
  }
}
