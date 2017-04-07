#include <kvstore/KVStoreHeader_v2.h>
#include <libmemcached/memcached.h>
#include <mutex>
#include <atomic>
#include <queue>
#include <chrono>
#include <iostream>
#include <thread>
using namespace std;
namespace kvstore {

  void replaceStrChar(string &str, char old, char new_) {
    for (int i = 0; i < str.length(); ++i) {
      if (str[i] == old)
        str[i] = new_;
    }
  }

  #define c_kvsclient ((KVStoreClient*)dataholder)

  struct async_data{
    void (*fn)(KVData<string>,void *,void *);
    void *data;
    void *vfn;
    int type;
  };

  class KVStoreClient{
  public:
    string conn;
    memcached_st *memc;
    string tablename;
//
//
//     memc = memcached(config_string.c_str(), config_string.size());
//     return true;
//     redisClusterContext* rc=NULL;
//     string tablename;
//     string conn;
//     std::mutex mtx;
//     std::queue<struct async_data> q;
//     bool keeprunning = true;
//     thread td;
//     // std::atomic<long long> count;
//     KVStoreClient(){
//       // count=0;
//     }
//     ~KVStoreClient(){
//       keeprunning = false;
//       if(td.joinable()){
//         td.join();
//       }
//       // std::terminate(td);
//       if(rc!=NULL)
//       redisClusterFree(rc);
//     }
//     void eventLoop(){
//       redisReply *reply;
//       int rep;
//       std::chrono::milliseconds waittime(500);
//
//       while(keeprunning){
//         while(true){mtx.lock();if(!q.empty()){mtx.unlock(); break;}; mtx.unlock();std::this_thread::sleep_for(waittime);if(!keeprunning)return;}
//         mtx.lock();
//         rep = redisClusterGetReply(rc, (void**)&reply);
//         mtx.unlock();
//
// // #define REDIS_REPLY_STRING 1
// // #define REDIS_REPLY_ARRAY 2
// // #define REDIS_REPLY_INTEGER 3
// // #define REDIS_REPLY_NIL 4
// // #define REDIS_REPLY_STATUS 5
// // #define REDIS_REPLY_ERROR 6
// 				if(rep == REDIS_OK){
//           if(reply == NULL){
//             cerr<<"Reply Null"<<endl;
//           } else {
//             KVData<string> ret = KVData<string>();
//             mtx.lock();
//             // if(q.empty()){
//             //   cout<<"Queue empty :"<<__FILE__<<endl;
//             // }
//             struct async_data ad = q.front(); q.pop();  //? lock required?
//             mtx.unlock();
//             // count--;
//             if(reply->type == REDIS_REPLY_STRING){
//                 ret.ierr = 0;
//                 // if(reply->str == NULL){
//                 //   cout<<"NULL in str :"<<__FILE__<<endl;
//                 // }
//                 ret.value = string(reply->str);
//                 // cout<<"Got:"<<ret.value<<endl;
//             } else if (reply->type == REDIS_REPLY_STATUS){
//                 ret.ierr = 0;
//               //str == OK
//             } else if (reply->type == REDIS_REPLY_NIL){
//                 ret.ierr = -1;
//                 ret.serr = "Value doesn't exists.";
//               //value doesnt exists
//             } else if (reply->type == REDIS_REPLY_INTEGER){
//                 if(reply->integer <= 0){
//                   ret.ierr = -1; /*reply->integer;*/
//                   ret.serr = "Value doesn't exists.";
//                 } else {
//                   ret.ierr = 0;
//                 }
//             } else {
//               cerr<<"Reply type:"<<reply->type<<endl;
//             }
//             ad.fn(ret,ad.data,ad.vfn);
//             freeReplyObject(reply);
//           }
// 				} else {
//           cerr<<"Error in return file:"<<__FILE__<<" line:"<<__LINE__<<endl;
// 					// redisClusterReset(cc);
// 				}
//       }
//     }
//     void startEventLoop(){
//       td = thread([&]{eventLoop();});
//       // td = thread(KVStoreClient::eventLoop,this);
//     }
  };

  KVImplHelper::KVImplHelper(){
    dataholder = (void*) new KVStoreClient();
  }


  KVImplHelper::KVImplHelper(const KVImplHelper& kh){
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
    c_kvsclient->memc = memcached(conn.c_str(), conn.size());
    if(c_kvsclient->memc == NULL){
      /* cerr<<"Error creating memcached connection."<<endl; */
      return false;
    }
    // c_kvsclient->startEventLoop();
    return true;
  }

  KVData<string> KVImplHelper::get(string const& skey){
    KVData<string> ret = KVData<string>();
    string key = c_kvsclient->tablename + skey;
    replaceStrChar(key, ' ', '_');
    replaceStrChar(key, '\t', '_');
    replaceStrChar(key, '\n', '_');
    replaceStrChar(key, '\r', '_');

    char *op;
    size_t value_length;
    uint32_t flags;
    memcached_return_t error;
    op = memcached_get(c_kvsclient->memc, key.c_str(), key.size(), &value_length, &flags, &error);
    if(error != MEMCACHED_SUCCESS){
      /* cerr<<"GET Error"<<endl; */
      /* http://docs.libmemcached.org/libmemcached/memcached_return_t.html#memcached_return_t */
      ret.ierr = -1;
      ret.serr = string(memcached_strerror(NULL,error));
      return ret;
    }
    ret.ierr = 0;
    ret.value = string(op,value_length);
    free(op);
    return ret;
  }

  KVData<string> KVImplHelper::put(string const& skey,string const& val){
    KVData<string> ret = KVData<string>();
    string key = c_kvsclient->tablename + skey;
    replaceStrChar(key, ' ', '_');
    replaceStrChar(key, '\t', '_');
    replaceStrChar(key, '\n', '_');
    replaceStrChar(key, '\r', '_');
    // cout<<"Key:"<<key<<endl;
    memcached_return_t error = memcached_set(c_kvsclient->memc, key.c_str(), key.size(), val.c_str(), val.size(), (time_t)0, (uint32_t)0);
    if (error != MEMCACHED_SUCCESS)
    {
      /* http://docs.libmemcached.org/libmemcached/memcached_return_t.html#memcached_return_t */
      ret.ierr = -1;
      ret.serr = string(memcached_strerror(NULL,error));
      // cout<<"Error in memcached put. Err:"<<ret.serr<<endl;
      return ret;
    }
    ret.ierr = 0;
    return ret;
  }

  KVData<string> KVImplHelper::del(string const& skey){
    KVData<string> ret = KVData<string>();
    string key = c_kvsclient->tablename + skey;
    replaceStrChar(key, ' ', '_');
    replaceStrChar(key, '\t', '_');
    replaceStrChar(key, '\n', '_');
    replaceStrChar(key, '\r', '_');
    memcached_return_t error = memcached_delete(c_kvsclient->memc, key.c_str(), key.size(), (time_t)0);
    if (error != MEMCACHED_SUCCESS)
    {
      /* http://docs.libmemcached.org/libmemcached/memcached_return_t.html#memcached_return_t */
      ret.ierr = -1;
      ret.serr = string(memcached_strerror(NULL,error));
      return ret;
    }
    ret.ierr = 0;
    return ret;
  }

  bool KVImplHelper::clear(){
    /* Not yet implemented */
    return false;
  };

  int KVImplHelper::mget(vector<string>& key, vector<string>& tablename, vector<KVData<string>>& vret){
    KVData<string> ret = KVData<string>();
    int sz = key.size();
    size_t key_length[sz];
    const char *keys[sz];
    string tbkey;
    for(int i=0;i<sz;i++){
      tbkey =  tablename[i] + key[i];
      replaceStrChar(tbkey, ' ', '_');
      replaceStrChar(tbkey, '\t', '_');
      replaceStrChar(tbkey, '\n', '_');
      replaceStrChar(tbkey, '\r', '_');
      key_length[i] = tbkey.size();
      keys[i] = tbkey.c_str();
    }
    uint32_t flags;
    char return_key[MEMCACHED_MAX_KEY];
    size_t return_key_length;
    char *return_value;
    size_t return_value_length;
    memcached_return_t error;


    error = memcached_mget(c_kvsclient->memc, keys, key_length, sz);
    if(error != MEMCACHED_SUCCESS){
      cerr<<""<<__FILE__<<" :"<<__LINE__<<" Error in memcached_mget :"<<string(memcached_strerror(NULL,error))<<endl;
      return -1;
    }
    int itr=0;
    while ((return_value = memcached_fetch(c_kvsclient->memc, return_key, &return_key_length, &return_value_length, &flags, &error)))
    {
      string rkey = string(return_key,return_key_length);
      while(itr<sz){
        if(key[itr] == rkey){
          ret.ierr = 0;
          ret.serr = "";
          ret.value = string(return_value,return_value_length);
          vret.push_back(ret);
          break;
        } else {
          ret.ierr = -1;
          ret.serr = "Unknown error [error type cannot be identified].";
          ret.value = "";
          vret.push_back(ret);
          itr++;
        }
      }
      free(return_value);
    }
    return 0;
  }

  int KVImplHelper::mput(vector<string>& skey, vector<string>& val, vector<string>& tablename, vector<KVData<string>>& vret){
    KVData<string> ret = KVData<string>();
    memcached_return_t error;
    int sz = skey.size();
    for(int i=0; i<sz; i++){
      string key = tablename[i] + skey[i];
      replaceStrChar(key, ' ', '_');
      replaceStrChar(key, '\t', '_');
      replaceStrChar(key, '\n', '_');
      replaceStrChar(key, '\r', '_');
      error = memcached_set(c_kvsclient->memc, key.c_str(), key.size(), val[i].c_str(), val[i].size(), (time_t)0, (uint32_t)0);
      if (error != MEMCACHED_SUCCESS)
      {
        /* http://docs.libmemcached.org/libmemcached/memcached_return_t.html#memcached_return_t */
        ret.ierr = -1;
        ret.serr = string(memcached_strerror(NULL,error));
      } else {
        ret.ierr = 0;
        ret.serr = "";
      }
      vret.push_back(ret);
    }
    return 0;
  }

  int KVImplHelper::mdel(vector<string>& skey, vector<string>& tablename, vector<KVData<string>>& vret){
    KVData<string> ret = KVData<string>();
    memcached_return_t error;
    int sz = skey.size();
    for(int i=0; i<sz; i++){
      string key = tablename[i] + skey[i];
      replaceStrChar(key, ' ', '_');
      replaceStrChar(key, '\t', '_');
      replaceStrChar(key, '\n', '_');
      replaceStrChar(key, '\r', '_');
      error = memcached_delete(c_kvsclient->memc, key.c_str(), key.size(), (time_t)0);
      if (error != MEMCACHED_SUCCESS)
      {
        /* http://docs.libmemcached.org/libmemcached/memcached_return_t.html#memcached_return_t */
        ret.ierr = -1;
        ret.serr = string(memcached_strerror(NULL,error));
      } else {
        ret.ierr = 0;
        ret.serr = "";
      }
      vret.push_back(ret);
    }
    return 0;
  }

  void KVImplHelper::async_get(string key, void (*fn)(KVData<string>,void *, void *),void *data, void *vfn){
    cerr<<"Asyn not yet implemented."<<endl;
  }

  void KVImplHelper::async_put(string key,string val, void (*fn)(KVData<string>,void *, void *),void *data, void *vfn){
    cerr<<"Asyn not yet implemented."<<endl;
  }
  void KVImplHelper::async_del(string key, void (*fn)(KVData<string>,void *, void *),void *data, void *vfn){
    cerr<<"Asyn not yet implemented."<<endl;
  }


  void KVImplHelper::async_get(string key, string tablename, void (*fn)(KVData<string>,void *, void *),void *data, void *vfn){
    cerr<<"Asyn not yet implemented."<<endl;
  }

  void KVImplHelper::async_put(string key,string val, string tablename, void (*fn)(KVData<string>,void *, void *),void *data, void *vfn){
    cerr<<"Asyn not yet implemented."<<endl;
  }
  void KVImplHelper::async_del(string key, string tablename, void (*fn)(KVData<string>,void *, void *),void *data, void *vfn){
    cerr<<"Asyn not yet implemented."<<endl;
  }
}
