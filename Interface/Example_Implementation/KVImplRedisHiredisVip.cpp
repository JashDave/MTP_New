#include <kvstore/KVStoreHeader_v2.h>
#include <hiredis-vip/hircluster.h>

namespace kvstore {

  #define c_kvsclient ((KVStoreClient*)dataholder)

  class KVStoreClient{
  public:
    redisClusterContext* rc;
    string tablename;
    string conn;
    ~KVStoreClient(){
      redisClusterFree(rc);
    }
  };

  KVImplHelper::KVImplHelper(){
    dataholder = (void*) new KVStoreClient();
  }

  KVImplHelper::KVImplHelper(KVImplHelper& kh){
    dataholder = (void*) new KVStoreClient();
    bool succ = bind(((KVStoreClient*)kh.dataholder)->conn,((KVStoreClient*)kh.dataholder)->tablename);
    if(!succ){
      std::cerr << "Error copying KVImplHelper object" << std::endl;
    }
  }

  KVImplHelper::~KVImplHelper(){
    delete(c_kvsclient);
  }

  bool KVImplHelper::bind(string conn, string tablename){
    c_kvsclient->tablename = tablename;
    c_kvsclient->conn = conn;

    bool retry = true;
    int attempts = 0;
    int MAX_TRIES = 10;
    while(retry){
      c_kvsclient->rc = redisClusterConnect(conn.c_str(), HIRCLUSTER_FLAG_NULL);
      if(c_kvsclient->rc == NULL || c_kvsclient->rc->err)
      {
        attempts++;
        if(attempts == MAX_TRIES){
          //printf("connect error : %s\n", rc == NULL ? "NULL" : rc->errstr);
          return false;
        }
      } else {
        retry = false; /* break; */
      }
    }
    return true;
  }

  std::shared_ptr<KVData<string>> KVImplHelper::get(string const& key){
    std::shared_ptr<KVData<string>> ret = std::make_shared<KVData<string>>();
    redisReply *reply = (redisReply *)redisClusterCommand(c_kvsclient->rc, "get %s", (c_kvsclient->tablename+key).c_str());
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
    redisReply *reply = (redisReply *)redisClusterCommand(c_kvsclient->rc, "set %s %s", (c_kvsclient->tablename+key).c_str(), val.c_str());
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
    redisReply *reply = (redisReply *)redisClusterCommand(c_kvsclient->rc, "del %s", (c_kvsclient->tablename+key).c_str());
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

  void KVImplHelper::async_get(string key, void (*fn)(std::shared_ptr<KVData<string>>,void *),void *data){
    std::shared_ptr<KVData<string>> ret = std::make_shared<KVData<string>>();
    /* Do async get and update 'ret' */
    fn(ret,data);
  }

  void KVImplHelper::async_put(string key,string val, void (*fn)(std::shared_ptr<KVData<string>>,void *),void *data){
    std::shared_ptr<KVData<string>> ret = std::make_shared<KVData<string>>();
    /* Do async put and update 'ret' */
    fn(ret,data);
  }

  void KVImplHelper::async_del(string key, void (*fn)(std::shared_ptr<KVData<string>>,void *),void *data){
    std::shared_ptr<KVData<string>> ret = std::make_shared<KVData<string>>();
    /* Do async del and update 'ret' */
    fn(ret,data);
  }
}
