#include <kvstore/KVStoreHeader_v2.h>

/* #include <YourIncludes> */
#include "../../../../Helper/src/kvstore_client.cpp"
#include <vector>
#include <string>
#define c_kvsclient (((KVManager *)dataholder)->kc)

namespace kvstore {
  class KVManager{
  public:
    string conn;
    string tablename;
    KVStoreClient *kc=NULL;
    ~KVManager(){
      if(kc!=NULL){
        delete(kc);
      }
    }
  };


  KVImplHelper::KVImplHelper(){
  }

  KVImplHelper::~KVImplHelper(){
    if(dataholder != NULL){
      delete((KVManager *)dataholder);
    }
  }

  KVImplHelper::KVImplHelper(KVImplHelper& kh){
    if(dataholder != NULL){
      string conn = ((KVManager *)(kh.dataholder))->conn;
      string tablename = ((KVManager *)(kh.dataholder))->tablename;
      bind(conn,tablename);
    }
  }

  bool KVImplHelper::bind(string connection, string tablename){
    dataholder = (void *) new KVManager();
    ((KVManager *)dataholder)->conn = connection;
    ((KVManager *)dataholder)->tablename = tablename;

    int colon = connection.find(":");
    string ip = connection.substr(0,colon);
    string port = connection.substr(colon+1);
    dataholder = (void *) new KVStoreClient(ip,stoi(port));

    std::vector<string> v;
    v.push_back("CreateTable");
    v.push_back(tablename);
    c_kvsclient->send(v);
    v=c_kvsclient->receive();
    if(v[0].compare("true")==0)
    {
      return true;
    }
    return false;
  }

  std::shared_ptr<KVData<string>> KVImplHelper::get(string const& key){
    std::shared_ptr<KVData<string>> ret = std::make_shared<KVData<string>>();
    std::vector<string> v;
    v.push_back("Get");
    v.push_back(key);
    c_kvsclient->send(v);

    v=c_kvsclient->receive();
    ret->serr = v[1];
    ret->ierr = stoi(v[2]);
    if(ret->ierr==0){
      ret->value = v[0];
    }
    return ret;
  }

  std::shared_ptr<KVData<string>> KVImplHelper::put(string const& key,string const& val){
    std::shared_ptr<KVData<string>> ret = std::make_shared<KVData<string>>();
    std::vector<string> v;
    v.push_back("Put");
    v.push_back(key);
    v.push_back(val);
    c_kvsclient->send(v);

    v=c_kvsclient->receive();
    ret->serr = v[1];
    ret->ierr = stoi(v[2]);
    return ret;
  }

  std::shared_ptr<KVData<string>> KVImplHelper::del(string const& key){
    std::shared_ptr<KVData<string>> ret = std::make_shared<KVData<string>>();
    std::vector<string> v;
    v.push_back("Del");
    v.push_back(key);
    c_kvsclient->send(v);

    v=c_kvsclient->receive();
    ret->serr = v[1];
    ret->ierr = stoi(v[2]);
    return ret;
  }

  void KVImplHelper::async_get(string key, void (*fn)(std::shared_ptr<KVData<string>>,void *, void *),void *data, void *vfn){
    std::shared_ptr<KVData<string>> ret = std::make_shared<KVData<string>>();
    /* Do async get and update 'ret' */
    // fn(ret,data);
  }

  void KVImplHelper::async_put(string key,string val, void (*fn)(std::shared_ptr<KVData<string>>,void *, void *),void *data, void *vfn){
    std::shared_ptr<KVData<string>> ret = std::make_shared<KVData<string>>();
    /* Do async put and update 'ret' */
    // fn(ret,data);
  }

  void KVImplHelper::async_del(string key, void (*fn)(std::shared_ptr<KVData<string>>,void *, void *),void *data, void *vfn){
    std::shared_ptr<KVData<string>> ret = std::make_shared<KVData<string>>();
    /* Do async del and update 'ret' */
    // fn(ret,data);
  }

  bool KVImplHelper::clear(){
    std::vector<string> v;
    v.push_back("Clear");
    c_kvsclient->send(v);
    v=c_kvsclient->receive();
    if(v[0].compare("true")==0)
    {
      return true;
    }
    return false;
  }

  std::vector<std::shared_ptr<KVData<string>>> parseResults(vector<string> &res){
    int count = res.size()/4;
    std::vector<std::shared_ptr<KVData<string>>> fret(count);
    std::shared_ptr<KVData<string>> ret = std::make_shared<KVData<string>>();
    for(int idx=0;idx<count;idx++){
      ret->serr = res[idx*4+2];
      ret->ierr = stoi(res[idx*4+3]);
      if(ret->ierr==0){
        ret->value = res[idx*4+1];
      }
      fret[idx]=ret;
    }
    return fret;
  }

  int KVImplHelper::mget(vector<string>& key, vector<string>& tablename, vector<std::shared_ptr<KVData<string>>>& ret){
    /* Do multiget and send the response via 'ret' vector */
    int sz = key.size();
    std::vector<string> v;
    for(int i=0;i<sz;i++){
      v.push_back("CreateTable");
      v.push_back(tablename[i]);
      v.push_back("Get");
      v.push_back(key[i]);
    }
    c_kvsclient->send(v);
    std::vector<string> rcv=c_kvsclient->receive();
    std::vector<std::shared_ptr<KVData<string>>> res = parseResults(rcv);
    for(std::shared_ptr<KVData<string>>& kd: res){
      ret.push_back(kd);
    }
    return 0;
  }

  int KVImplHelper::mput(vector<string>& key, vector<string>& val, vector<string>& tablename, vector<std::shared_ptr<KVData<string>>>& ret){
    /* Do multiput and send the response via 'ret' vector */
    int sz = key.size();
    std::vector<string> v;
    for(int i=0;i<sz;i++){
      v.push_back("CreateTable");
      v.push_back(tablename[i]);
      v.push_back("Put");
      v.push_back(key[i]);
      v.push_back(val[i]);
    }
    c_kvsclient->send(v);
    std::vector<string> rcv=c_kvsclient->receive();
    std::vector<std::shared_ptr<KVData<string>>> res = parseResults(rcv);
    for(std::shared_ptr<KVData<string>>& kd: res){
      ret.push_back(kd);
    }
    return 0;
  }

  int KVImplHelper::mdel(vector<string>& key, vector<string>& tablename, vector<std::shared_ptr<KVData<string>>>& ret){
    /* Do multidel and send the response via 'ret' vector */
    int sz = key.size();
    std::vector<string> v;
    for(int i=0;i<sz;i++){
      v.push_back("CreateTable");
      v.push_back(tablename[i]);
      v.push_back("Del");
      v.push_back(key[i]);
    }
    c_kvsclient->send(v);
    std::vector<string> rcv=c_kvsclient->receive();
    std::vector<std::shared_ptr<KVData<string>>> res = parseResults(rcv);
    for(std::shared_ptr<KVData<string>>& kd: res){
      ret.push_back(kd);
    }
    return 0;
  }
}
