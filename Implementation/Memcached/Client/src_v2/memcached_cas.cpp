
// #include <libmemcached/memcached_pool.h>
#include <iostream>
#include <vector>
#include <libmemcached/memcached.h>
using namespace std;


class KVStoreMemcached{
  string config_string;
  memcached_st *memc;
  string tablename;
  memcached_result_st *mrs;
  uint64_t version;
public:

  bool bind(string conf, string tb) {
    config_string = conf;
    tablename = tb;
    memc = memcached(config_string.c_str(), config_string.size());
    if(memc == NULL){
      return false;
    }
    memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_SUPPORT_CAS, 1);
    mrs = memcached_result_create(memc, NULL);
    return true;
  }

  bool put(string key, string val) {
    key = tablename + key;
    memcached_return_t rc = memcached_set(memc, key.c_str(), key.size(), val.c_str(), val.size(), (time_t)0, (uint32_t)0);
    if (rc != MEMCACHED_SUCCESS)
    {
      cerr<<"set failure"<<endl;
      return false;
    }
    return true;
  }

  string get(string key){
    key = tablename + key;
    char *op;
    size_t value_length;
    uint32_t flags = 0; //MEMCACHED_BEHAVIOR_SUPPORT_CAS;
    memcached_return_t error;
    op = memcached_get(memc, key.c_str(), key.size(), &value_length, &flags, &error);
    if(error != MEMCACHED_SUCCESS){
      cerr<<"GET Error"<<endl;
      return string(memcached_strerror(NULL,error));
                    // libmemcached_strerror
    }
    string ret(op,value_length);
    free(op);
    return ret;
  }

  string gets(string key){
    key = tablename + key;
    uint32_t flags;
    memcached_return_t error;
    memcached_return_t rc;
    const memcached_result_st *rs;

    char *keys[1];
    size_t key_length[1];
    keys[0] = key.c_str();
    key_length[0] = key.size();
    rc = memcached_mget(memc, keys, key_length, 1);
    if(rc != MEMCACHED_SUCCESS){
      cerr<<""<<__FILE__<<" :"<<__LINE__<<" Error in memcached_gets :"<<string(memcached_strerror(NULL,rc))<<endl;
      return string(memcached_strerror(NULL,error));
    }

    rs = memcached_fetch_result(memc, NULL, &error);
    // if(error ==  MEMCACHED_END){
    //   cerr<<"Ended"<<endl;
    //   break;
    // } else
    if (error == MEMCACHED_SUCCESS){
      char *op = memcached_result_value(rs);
      version = memcached_result_cas(rs);
      string ret = string(op);
      free(op);
      return ret;
    }
    cerr<<""<<__FILE__<<" :"<<__LINE__<<" Error in memcached_gets :"<<string(memcached_strerror(NULL,rc))<<endl;
    return string(memcached_strerror(NULL,error));
  }

  bool cas(string key, string val) {
    key = tablename + key;
    cout<<"version"<<version<<endl;
    memcached_return_t rc = memcached_cas(memc, key.c_str(), key.size(), val.c_str(), val.size(), (time_t)0, (uint32_t)0, version);
    if (rc != MEMCACHED_SUCCESS)
    {
      // cerr<<"CAS failure"<<endl;
      return false;
    }
    return true;
  }

  bool del(string key){
    key = tablename + key;
    memcached_return_t error = memcached_delete(memc, key.c_str(), key.size(), (time_t)0);
    if(error != MEMCACHED_SUCCESS){
      cerr<<"Del error"<<endl;
      return false;
    }
    return true;
  }
};

int main(){
  KVStoreMemcached km;
  string config_string = "--SERVER=10.129.26.154";
  string tablename = "TB1";
  bool succ = km.bind(config_string,tablename);
  if(!succ){
    std::cerr << "Bind error." << std::endl;
  }

  vector<string> keys = {"k1","k2","k3"};
  vector<string> vals = {"v1","v2","v3"};

  for(int i = 0; i<keys.size(); i++){
    succ = km.put(keys[i],vals[i]);
    if(!succ){
      std::cerr << "Put error at i:" << i << std::endl;
    }
  }

  for(int i = 0; i<keys.size(); i++){
    string val = km.get(keys[i]);
    if(val != vals[i]){
      std::cerr << "Get error at i:" << i << " val:" << val << std::endl;
    }
  }

 string v1 = km.gets(keys[0]);
 std::cout << "v1:"<<v1 << std::endl;
 succ = km.cas(keys[0],"casv1");
 if(!succ){
   std::cerr << "Error: CAS1 failed." << std::endl;
 }

 v1 = km.gets(keys[0]);
 std::cout << "v1:"<<v1 << std::endl;
 km.put(keys[0],"temp");
 succ = km.cas(keys[0],"casv2_will_fail_due_to_put");
 if(succ){
   std::cerr << "Error: CAS2 succeded." << std::endl;
 }

 v1 = km.gets(keys[0]);
 std::cout << "v1:"<<v1 << std::endl;
 return 0;
}
