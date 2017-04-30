#include <kvstore/KVStoreHeader_v3.h>
#include <hiredis-vip/hircluster.h>
#include <mutex>
#include <atomic>
#include <queue>
#include <chrono>
#include <iostream>
#include <thread>

using namespace std;

namespace kvstore {

  #define c_kvsclient ((KVStoreClient*)dataholder)

  // struct async_data{
  //   void (*fn)(KVData<string>,void *,void *);
  //   void *data;
  //   void *vfn;
  //   int type;
  // };

  class KVStoreClient{
  public:
    redisClusterContext* rc=NULL;
    string tablename;
    string conn;
    // std::mutex mtx;
    // std::queue<struct async_data> q;
    // bool keeprunning = true;
    // thread td;
    // std::atomic<long long> count;
    KVStoreClient(){
      // count=0;
    }
    ~KVStoreClient(){
      // keeprunning = false;
      // if(td.joinable()){
      //   td.join();
      // }
      // std::terminate(td);
      if(rc!=NULL)
      redisClusterFree(rc);
    }


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
//             KVERR<<"Reply Null"<<endl;
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
//               KVERR<<"Reply type:"<<reply->type<<endl;
//             }
//             ad.fn(ret,ad.data,ad.vfn);
//             freeReplyObject(reply);
//           }
// 				} else {
//           KVERR<<"Error in return file:"<<__FILE__<<" line:"<<__LINE__<<endl;
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
      KVERR << "Error copying KVImplHelper object" << std::endl;
    }
  }

  KVImplHelper::~KVImplHelper(){
    delete(c_kvsclient);
  }

  bool KVImplHelper::bind(string& conn, string& tablename){
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
    // c_kvsclient->startEventLoop();
    return true;
  }

  KVData<string> KVImplHelper::get(string const& key){
    KVData<string> ret = KVData<string>();
    redisReply *reply = (redisReply *)redisClusterCommand(c_kvsclient->rc, "get %s", (c_kvsclient->tablename+key).c_str());
    if(reply == NULL)
    {
      ret.ierr = -1;
      ret.serr = "Unknown error.";
      // printf("reply is null[%s]\n", c_kvsclient->rc->errstr);
      //redisClusterFree(c_kvsclient->rc); //??
    } else if(reply->str == NULL){
      ret.ierr = -1;
      ret.serr = "Value doesn't exists.";
      freeReplyObject(reply);
    } else {
      ret.ierr = 0;
      ret.value = string(reply->str);
      freeReplyObject(reply);
    }
    return ret;
  }

  KVData<string> KVImplHelper::put(string const& key,string const& val){
    KVData<string> ret = KVData<string>();
    redisReply *reply = (redisReply *)redisClusterCommand(c_kvsclient->rc, "set %s %s", (c_kvsclient->tablename+key).c_str(), val.c_str());
    if(reply == NULL)
    {
      ret.ierr = -1;
      ret.serr = "Unknown error.";
      // printf("reply is null[%s]\n", c_kvsclient->rc->errstr);
      //redisClusterFree(c_kvsclient->rc); //??
    } else {
      ret.ierr = 0;
      freeReplyObject(reply);
    }
    return ret;
  }

  KVData<string> KVImplHelper::del(string const& key){
    KVData<string> ret = KVData<string>();
    redisReply *reply = (redisReply *)redisClusterCommand(c_kvsclient->rc, "del %s", (c_kvsclient->tablename+key).c_str());
    if(reply == NULL)
    {
      ret.ierr = -1;
      ret.serr = "Unknown error.";
      // printf("reply is null[%s]\n", c_kvsclient->rc->errstr);
      //redisClusterFree(c_kvsclient->rc); //??
    } else {
      if(reply->integer <= 0){
        ret.ierr = -1;
        ret.serr = "Value doesn't exists.";
      } else {
        ret.ierr = 0;
      }
      freeReplyObject(reply);
    }
    return ret;
  }

  bool KVImplHelper::clear(){
    /* Not yet implemented */
    return false;
  };

  vector<KVData<string>> KVImplHelper::mget(vector<string> const& key){
    int sz = key.size();
    vector<KVData<string>> ret(sz);
    int rep;
    redisReply *reply;

    for(int i=0;i<sz;i++){
      rep = redisClusterAppendCommand(c_kvsclient->rc, "get %s",(c_kvsclient->tablename+key[i]).c_str()) ;
      if(rep == REDIS_ERR){
        KVERR<<"Get Append error"<<endl;
      }
    }
    for(int i=0;i<sz;i++){
      rep = redisClusterGetReply(c_kvsclient->rc, (void**)&reply);
      if(rep == REDIS_OK){
        if(reply == NULL){
          KVERR<<"Reply Null"<<endl;
        } else {
          if(reply->type == REDIS_REPLY_STRING){
              ret[i].ierr = 0;
              ret[i].value = string(reply->str);
          } else if (reply->type == REDIS_REPLY_NIL){
              ret[i].ierr = -1;
              ret[i].serr = "Value doesn't exists.";
          } else {
            KVERR<<"Reply type:"<<reply->type<<endl;
          }
          freeReplyObject(reply);
        }
      } else {
        KVERR<<"Error in return file:"<<__FILE__<<" line:"<<__LINE__<<endl;
        // redisClusterReset(cc);
      }
    }
    return ret;
  }

  vector<KVData<string>> KVImplHelper::mput(vector<string> const& key, vector<string> const& val){
    int sz = key.size();
    vector<KVData<string>> ret(sz);
    int rep;
    redisReply *reply;

    for(int i=0;i<sz;i++){
      rep = redisClusterAppendCommand(c_kvsclient->rc, "set %s %s",(c_kvsclient->tablename+key[i]).c_str(), val[i].c_str()) ;
      if(rep == REDIS_ERR){
        KVERR<<"Set Append error"<<endl;
      }
    }
    for(int i=0;i<sz;i++){
      rep = redisClusterGetReply(c_kvsclient->rc, (void**)&reply);
      if(rep == REDIS_OK){
        if(reply == NULL){
          KVERR<<"Reply Null"<<endl;
        } else {
          if (reply->type == REDIS_REPLY_STATUS){
              ret[i].ierr = 0;
              // cout<<reply->str<<endl;
          } else {
            KVERR<<"Reply type:"<<reply->type<<endl;
          }
          freeReplyObject(reply);
        }
      }
    }
    return ret;
  }

  vector<KVData<string>> KVImplHelper::mdel(vector<string> const& key){
    int sz = key.size();
    vector<KVData<string>> ret(sz);
    int rep;
    redisReply *reply;

    for(int i=0;i<sz;i++){
      rep = redisClusterAppendCommand(c_kvsclient->rc, "del %s",(c_kvsclient->tablename+key[i]).c_str());
      if(rep == REDIS_ERR){
        KVERR<<"Del Append error"<<endl;
      }
    }
    for(int i=0;i<sz;i++){
      rep = redisClusterGetReply(c_kvsclient->rc, (void**)&reply);
      if(rep == REDIS_OK){
        if(reply == NULL){
          KVERR<<"Reply Null"<<endl;
        } else {
          if (reply->type == REDIS_REPLY_INTEGER){
              if(reply->integer <= 0){
                ret[i].ierr = -1; /*reply->integer;*/
                ret[i].serr = "Value doesn't exists.";
              } else {
                ret[i].ierr = 0;
              }
          } else {
            KVERR<<"Reply type:"<<reply->type<<endl;
          }
          freeReplyObject(reply);
        }
      }
    }
    return ret;
  }
/*
  void KVImplHelper::async_get(string key, void (*fn)(KVData<string>,void *, void *),void *data, void *vfn){
    // cout<<"Get key:"<<key<<endl;
      c_kvsclient->mtx.lock();
      int ret = redisClusterAppendCommand(c_kvsclient->rc, "get %s",(c_kvsclient->tablename+key).c_str()) ;
      c_kvsclient->mtx.unlock();
			if(ret!= REDIS_ERR){
				// c_kvsclient->count++;
        struct async_data ad{fn,data,vfn,1};
        c_kvsclient->mtx.lock();
        c_kvsclient->q.push(ad);
        c_kvsclient->mtx.unlock();
			}	else {
				KVERR<<"\n\n\nget Append error\n\n\n"<<endl;
			}
  }

  void KVImplHelper::async_put(string key,string val, void (*fn)(KVData<string>,void *, void *),void *data, void *vfn){
  // cout<<"Put key:"<<key<<endl;
  // cout<<"Put val:"<<val<<endl;
    c_kvsclient->mtx.lock();
    int ret = redisClusterAppendCommand(c_kvsclient->rc, "set %s %s",(c_kvsclient->tablename+key).c_str(),val.c_str()) ;
    c_kvsclient->mtx.unlock();
		if(ret!= REDIS_ERR){
			// c_kvsclient->count++;
      struct async_data ad{fn,data,vfn,2};
      c_kvsclient->mtx.lock();
      c_kvsclient->q.push(ad);
      c_kvsclient->mtx.unlock();
		}	else {
			KVERR<<"\n\n\nput Append error\n\n\n"<<endl;
		}
  }
  void KVImplHelper::async_del(string key, void (*fn)(KVData<string>,void *, void *),void *data, void *vfn){
    // cout<<"del key:"<<key<<endl;
    c_kvsclient->mtx.lock();
    int ret = redisClusterAppendCommand(c_kvsclient->rc, "del %s",(c_kvsclient->tablename+key).c_str()) ;
    c_kvsclient->mtx.unlock();
		if(ret!= REDIS_ERR){
      // c_kvsclient->count++;
      struct async_data ad{fn,data,vfn,3};
      c_kvsclient->mtx.lock();
      c_kvsclient->q.push(ad);
      c_kvsclient->mtx.unlock();
		}	else {
			KVERR<<"\n\n\ndel Append error\n\n\n"<<endl;
		}
  }


  void KVImplHelper::async_get(string key, string tablename, void (*fn)(KVData<string>,void *, void *),void *data, void *vfn){
    // cout<<"Get key:"<<key<<endl;
      c_kvsclient->mtx.lock();
      int ret = redisClusterAppendCommand(c_kvsclient->rc, "get %s",(tablename+key).c_str()) ;
      c_kvsclient->mtx.unlock();
			if(ret!= REDIS_ERR){
				// c_kvsclient->count++;
        struct async_data ad{fn,data,vfn,1};
        c_kvsclient->mtx.lock();
        c_kvsclient->q.push(ad);
        c_kvsclient->mtx.unlock();
			}	else {
				KVERR<<"\n\n\nget Append error\n\n\n"<<endl;
			}
  }

  void KVImplHelper::async_put(string key,string val, string tablename, void (*fn)(KVData<string>,void *, void *),void *data, void *vfn){
  // cout<<"Put key:"<<key<<endl;
  // cout<<"Put val:"<<val<<endl;
    c_kvsclient->mtx.lock();
    int ret = redisClusterAppendCommand(c_kvsclient->rc, "set %s %s",(tablename+key).c_str(),val.c_str()) ;
    c_kvsclient->mtx.unlock();
		if(ret!= REDIS_ERR){
			// c_kvsclient->count++;
      struct async_data ad{fn,data,vfn,2};
      c_kvsclient->mtx.lock();
      c_kvsclient->q.push(ad);
      c_kvsclient->mtx.unlock();
		}	else {
			KVERR<<"\n\n\nput Append error\n\n\n"<<endl;
		}
  }
  void KVImplHelper::async_del(string key, string tablename, void (*fn)(KVData<string>,void *, void *),void *data, void *vfn){
    // cout<<"del key:"<<key<<endl;
    c_kvsclient->mtx.lock();
    int ret = redisClusterAppendCommand(c_kvsclient->rc, "del %s",(tablename+key).c_str()) ;
    c_kvsclient->mtx.unlock();
		if(ret!= REDIS_ERR){
      // c_kvsclient->count++;
      struct async_data ad{fn,data,vfn,3};
      c_kvsclient->mtx.lock();
      c_kvsclient->q.push(ad);
      c_kvsclient->mtx.unlock();
		}	else {
			KVERR<<"\n\n\ndel Append error\n\n\n"<<endl;
		}
  }
  */
}
