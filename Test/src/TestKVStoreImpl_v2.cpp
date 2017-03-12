/*
  g++ -std=c++11 TestKVStoreImpl_v2.cpp -lkvstore_v2 -lkvs_redis_v2 -lboost_serialization
  g++ -std=c++11 TestKVStoreImpl_v2.cpp KVStoreHeader_v2.cpp KVImplRedisHiredisVip.cpp -pthread -lboost_serialization -lhiredis_vip -fpermissive -w
*/

#include <kvstore/KVStoreHeader_v2.h>

#define IP "10.129.28.141"
#define PORT "7003"

using namespace kvstore;

int main(){
  string conf = string(IP)+":"+string(PORT);
  string tablename = "TestTable123";

  KVStore<int,string> ks;
  if(!ks.bind(conf,tablename)){
    cerr<<"Connection Error"<<endl;
  }
  std::shared_ptr<KVData<string>> r;
  r = ks.put(1,"One");
  r = ks.put(2,"Two");
  r = ks.put(3,"Three");
  r = ks.put(4,"Four");
  r = ks.get(1);
  cout<<r->value<<endl;
  r = ks.get(2);
  cout<<r->value<<endl;
  r = ks.get(3);
  cout<<r->value<<endl;
  r = ks.get(4);
  cout<<r->value<<endl;
  r = ks.del(4);
  r = ks.get(4);
  cout<<r->serr<<endl;


  KVRequest kr;
  kr.bind(conf);
  kr.put<int,string>(5,"Five",tablename);
  kr.put<int,string>(6,"Six",tablename);
  kr.get<int,string>(2,tablename);
  kr.get<int,string>(3,tablename);
  kr.get<int,string>(4,tablename); //deleted

  std::shared_ptr<KVResultSet> rs = kr.execute();
  cout<< rs->get<string>(2)->value << endl;
  cout<< rs->get<string>(3)->value << endl;
  cout<< rs->get<string>(4)->serr << endl;
  cout<< rs->get<string>(10)->serr << endl;

}
