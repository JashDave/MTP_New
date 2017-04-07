// #include <mem_config.h>
// #include <libmemcached-1.0/memcached.h>
// #include <libmemcached/memcached_pool.h>

#include <iostream>
#include <vector>
#include <libmemcached/memcached.h>
using namespace std;


class KVStoreMemcached{
  string config_string;
  memcached_st *memc;
  string tablename;

public:

  bool bind(string conf, string tb) {
    config_string = conf;
    tablename = tb;
    memc = memcached(config_string.c_str(), config_string.size());
    if(memc == NULL){
      return false;
    }
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
    uint32_t flags;
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

  vector<string> keys = {"k_^%$#@:12","k2","k3"};
  vector<string> vals = {"v1","v 2","v3"};
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

  for(int i = 0; i<keys.size(); i++){
    succ = km.del(keys[i]);
    if(!succ){
      std::cerr << "Del error at i:" << i << std::endl;
    }
  }

  for(int i = 0; i<keys.size(); i++){
    string val = km.get(keys[i]);
    if(val != vals[i]){
      std::cerr << "Get required error at i:" << i << " error:" << val << std::endl;
    }
  }

  // if(1==0)
  {
    succ = km.put("foo","VFOO");
    if(!succ){
      std::cerr << "Put error" << std::endl;
    }
    succ = km.put("son","VSO N %^$@! \n Value on new line");
    if(!succ){
      std::cerr << "Put error" << std::endl;
    }
    memcached_st *memc = memcached(config_string.c_str(), config_string.size());
    if(memc == NULL){
      cout<<"connection error"<<endl;
    }
    memcached_return_t rc;
    char *keys[]= {"TB1foo", "TB1foo", "fudge", "TB1son", "food"};
    size_t key_length[]= {6, 6, 5, 6, 4};
    unsigned int x;
    uint32_t flags;

    char return_key[MEMCACHED_MAX_KEY];
    size_t return_key_length;
    char *return_value;
    size_t return_value_length;

    rc= memcached_mget(memc, keys, key_length, 5);
    x= 0;
    // while ((return_value= memcached_fetch(memc, return_key, &return_key_length, &return_value_length, &flags, &rc)))
    // {
    //   cout<<"K:"<< return_key<<"V:"<<return_value<<endl;
    //   free(return_value);
    //   x++;
    // }
    while(x!=6){
       memcached_return_t error;
      //  memcached_result_st rs;
       const memcached_result_st *rs;
      //  cout<<"DP1"<<endl;
      //  memcached_fetch_result(memc, &rs, &error);
       rs = memcached_fetch_result(memc, NULL, &error);
      //  cout<<"DP2"<<endl;
       if(error ==  MEMCACHED_END){
         cout<<"Ended"<<endl;
         break;
       } else if (error == MEMCACHED_SUCCESS){
        //  cout<<"DP3"<<endl;
        //  char *op = memcached_result_value(&rs);
         char *op = memcached_result_value(rs);
        //  cout<<"DP4"<<endl;
         cout<<op<<endl;
        //  free(op);
       } else {
        //  cout<<"DP5"<<endl;
         cout<<"Err:"<<memcached_strerror(NULL,error)<<endl;
       }
       x++;
    }
    // cout<<"DP1"<<endl;
  }
 return 0;
}
