// Why both implementation and declaration are in same file
// Refer : https://isocpp.org/wiki/faq/templates
//        http://stackoverflow.com/questions/3040480/c-template-function-compiles-in-header-but-not-implementation
//It only includes templates implementation

#ifndef __KVSTORE_DEF__
#define __KVSTORE_DEF__

// #include "redis/RedisClient.hpp"
#include <hiredis-vip/hircluster.h>
#include "kvscommon/KVStoreHeader.h"
#include <mutex>


#define c_kvsclient ((KVStoreClient *)kvsclient)

namespace kvstore {
  //? move to NonTemplateImpl
  class KVStoreClient;
  class KVStoreClient{
  public:
    redisClusterContext* rc;
    string tbname;
    bool init(string ip,string port);
  };

  // class KVMgmt{
  // public:
  //  int count; //can be used for pool
  //  mutex mtx;
  //  CRedisClient* rc=NULL;
  // };


    	template<typename ValType>
    	KVData<ValType> KVResultSet::get(int idx){
        KVData<ValType> ret;
        if(idx>=0 && idx<count){
          ret.serr = res[idx*3+1];
          ret.ierr = stoi(res[idx*3]);
          if(ret.ierr==0){
            ret.value = toBoostObject<ValType>(res[idx*3+2]);
          }
        } else {
          ret.ierr=-1;
          if(count==0)
          ret.serr="Empty KVResultSet due to empty request queue, please ensure that your request queue is not empty.";
          else
          ret.serr="KVResultSet index out of bound. Valid range is 0 to "+to_string(count-1)+" found "+to_string(idx);
        }
        return ret;
      }


    template<typename KeyType, typename ValType>
    void KVRequest::get(KeyType const& key,string tablename){
      v.push_back("g");
      vget.push_back(tablename+toBoostString(key));
    }

    template<typename KeyType, typename ValType>
    void KVRequest::put(KeyType const& key,ValType const& val,string tablename){
      v.push_back("p");
      vputk.push_back(tablename+toBoostString(key));
      vputv.push_back(toBoostString(val));
    }

    template<typename KeyType, typename ValType>
    void KVRequest::del(KeyType const& key,string tablename){
      v.push_back("d");
      vdel.push_back(tablename+toBoostString(key));
    }

  template<typename KeyType, typename ValType>
  KVStore<KeyType,ValType>::KVStore(){
  }

  template<typename KeyType, typename ValType>
  bool KVStore<KeyType,ValType>::bind(string connection,string tablename){
    int colon = connection.find(":");
    string ip = connection.substr(0,colon);
    string port = connection.substr(colon+1);
    kvsclient = (void *) new KVStoreClient();
    c_kvsclient->tbname = tablename;
    return c_kvsclient->init(ip,port);
  }

  template<typename KeyType, typename ValType>
  KVStore<KeyType,ValType>::~KVStore(){
    delete(c_kvsclient);
  }

  template<typename KeyType, typename ValType>
  KVData<ValType>  KVStore<KeyType,ValType>::get(KeyType const& key){
    string skey=c_kvsclient->tbname+toBoostString(key);
    string sval;
    KVData<ValType> kvd;

    redisReply *reply = redisClusterCommand(c_kvsclient->rc, "get %s", skey.c_str());
    if(reply == NULL)
    {
      // printf("reply is null[%s]\n", c_kvsclient->rc->errstr);
      //redisClusterFree(c_kvsclient->rc); //??
     kvd.serr = string(c_kvsclient->rc->errstr);
     kvd.ierr = -1;
   } else if(reply->str == NULL){
    kvd.serr = string("Data doesn't exists");
    kvd.ierr = -1;
    freeReplyObject(reply);
   } else {
      // printf("get %s\n",reply->str );
      kvd.ierr=0;
      kvd.value = toBoostObject<ValType>(string(reply->str));
      freeReplyObject(reply); // Double free?
    }
    return kvd;
  }

  template<typename KeyType, typename ValType>
  KVData<ValType> KVStore<KeyType,ValType>::put(KeyType const& key,ValType const& val){
    //cout<<"PUT"<<endl;
    string skey=c_kvsclient->tbname+toBoostString(key);
    string sval=toBoostString(val);
    KVData<ValType> kvd;

    redisReply *reply = redisClusterCommand(c_kvsclient->rc, "set %s %s", skey.c_str(),sval.c_str());
    if(reply == NULL)
    {
      // printf("reply is null[%s]\n", c_kvsclient->rc->errstr);
      //redisClusterFree(c_kvsclient->rc); //??
      kvd.serr = string(c_kvsclient->rc->errstr);
      kvd.ierr = -1;
    }else{
    // printf("get %s\n",reply->str );
      kvd.ierr=0;
      freeReplyObject(reply); // Double free?
    }
    return kvd;
  }

  template<typename KeyType, typename ValType>
  KVData<ValType> KVStore<KeyType,ValType>::del(KeyType const& key){
    string skey=c_kvsclient->tbname+toBoostString(key);
    KVData<ValType> kvd;

    redisReply *reply = redisClusterCommand(c_kvsclient->rc, "del %s", skey.c_str());
    if(reply == NULL)
    {
      // printf("reply is null[%s]\n", c_kvsclient->rc->errstr);
      //redisClusterFree(c_kvsclient->rc); //??
        kvd.serr = string(c_kvsclient->rc->errstr);
        kvd.ierr = -1;
    } else {
      kvd.ierr=0;
      freeReplyObject(reply);
    }
    return kvd;
  }

  template<typename KeyType, typename ValType>
  bool KVStore<KeyType,ValType>::clear() {
    // Not yet implemented
    // FLUSHDB can be used
    // Current problem can do FLUSHALL but all tables will be deleted (tablename+key)
    return false;
  }

}
#endif
