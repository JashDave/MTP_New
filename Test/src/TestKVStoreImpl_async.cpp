/*
  g++ -std=c++11 TestKVStoreImpl_v2.cpp -lkvstore_v2 -lkvs_redis_v2 -lboost_serialization
  g++ -std=c++11 TestKVStoreImpl_v2.cpp KVStoreHeader_v2.cpp KVImplRedisHiredisVip.cpp -pthread -lboost_serialization -lhiredis_vip -fpermissive -w
*/


#include <kvstore/KVStoreHeader_v2.h>
#include <unistd.h>

#include "TestUtils.h"
#define IP "10.129.28.141"
#define PORT "7003"

using namespace kvstore;


void getCallBack(std::shared_ptr<KVData<string>> kd,void *data){
  // if(kd->ierr!=0){
  //   cout<<"get Callback  serr:"<<kd->serr<<endl;
  // } else {
  //   // cout<<"Value :"<<kd->value<<endl;
  // }
}

void callBack(std::shared_ptr<KVData<string>> kd,void *data){
  // if(kd->ierr!=0){
  //   // cout<<"Callback  serr:"<<kd->serr<<endl;
  // } else {
  //   // cout<<"success"<<endl;
  // }
}

int main(){
  string conf = string(IP)+":"+string(PORT);
  string tablename = "TestTable123";

  KVStore<int,string> ks;
  if(!ks.bind(conf,tablename)){
    cerr<<"Connection Error"<<endl;
  }


  auto r = ks.put(2,"Two");
  cout<<r->ierr<<endl;
  r = ks.del(2);
  cout<<r->ierr<<endl;
// int i=1e5;
// long long s = currentMicros();
// while(i){
  // ks.async_put(1,"one",callBack,NULL);
  // ks.async_get(1,getCallBack,NULL);
  // ks.async_put(2,"two",callBack,NULL);
  // ks.async_del(2,callBack,NULL);
  // ks.async_put(3,"three",callBack,NULL);
  // ks.async_get(2,getCallBack,NULL);
  // ks.async_get(3,getCallBack,NULL);
  // ks.async_get(4,getCallBack,NULL);
//   i--;
// }
// long long e = currentMicros();
  // cout<<"sleeping"<<endl;
  // sleep(5);
  // cout<<"woke up"<<endl;
  // cout<<"Time:"<<(e-s)/(1e6)<<endl;
  exit(0);
  // r = ks.put(4,"Four");
  //
  // r = ks.get(1);
  // cout<<r->value<<endl;
  // r = ks.get(2);
  // cout<<r->value<<endl;
  // r = ks.get(3);
  // cout<<r->value<<endl;
  // r = ks.get(4);
  // cout<<r->value<<endl;
  // r = ks.del(4);
  // r = ks.get(4);
  // cout<<r->serr<<endl;
  //
  //
  // KVRequest kr;
  // kr.bind(conf);
  // kr.put<int,string>(5,"Five",tablename);
  // kr.put<int,string>(6,"Six",tablename);
  // kr.get<int,string>(2,tablename);
  // kr.get<int,string>(3,tablename);
  // kr.get<int,string>(4,tablename); //deleted
  //
  // std::shared_ptr<KVResultSet> rs = kr.execute();
  // cout<< rs->get<string>(2)->value << endl;
  // cout<< rs->get<string>(3)->value << endl;
  // cout<< rs->get<string>(4)->serr << endl;
  // cout<< rs->get<string>(10)->serr << endl;

}
