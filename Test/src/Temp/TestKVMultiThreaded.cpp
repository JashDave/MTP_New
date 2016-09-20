//CSV file formate
//http://www.computerhope.com/issues/ch001356.htm

#include <iostream>
#include <thread>
#include <functional> //std::ref()
#include "TestUtils.h"
#include "../../KVStore.h"

// #define conn_str "127.1.1.1:8090" //To be passed during make test CONN="conn string"
#define conn_str "10.129.28.101:8090" //To be passed during make test CONN="conn string"
//#define iter 10000

#define ll long long
#define ull unsigned long long

using namespace std;
using namespace kvstore;



const int thread_count=50000;
thread threads[thread_count];
float avg[thread_count];



void singleRead(int tid,ull datasize,ull iter,Measure *m,string tb_name){
  ll i;
  KVStore<ll,string> kvs;
  KVData<string> kd;
  kvs.bind(conn_str,"table_single_rw"+tb_name);
  for(i=0;i<iter;i++){
    m->start();
    // kd = kvs.get(i+tid*iter);
    kd = kvs.get(i);
    m->end();
    if(kd.ierr<0){
      cout<<"Error in single read "<<datasize<<"B at i="<<i<<" : "<<kd.serr<<endl;
    }
  }
  //avg[tid]=m.getAvg();
  //string desc = "Single Read,KeyType:long long,KeySize:64bits,ValueType:string,ValueSize:"+to_string(datasize)+"Bytes";
  //m.print(desc);
  //m.saveToFile(desc,filename);
}


void singleWrite(int tid,ull datasize,ull iter,Measure *m,string tb_name){

  ll i;
  string data="";
  for(i=0;i<datasize;i++){
    data+=char('a'+i%26);
  }
  KVStore<ll,string> kvs;
  KVData<string> kd;
  kvs.bind(conn_str,"table_single_rw"+tb_name);
  for(i=0;i<iter;i++){
    m->start();
    //kd = kvs.put(i+tid*iter,data);
    kd = kvs.put(i,data);
    m->end();
    if(kd.ierr<0){
      cout<<"Error in single write "<<datasize<<"B at i="<<i<<" : "<<kd.serr<<endl;
    }
  }

  // avg[tid]=m.getAvg();
  // string desc="Single Write,KeyType:long long,KeySize:64bits,ValueType:string,ValueSize:"+to_string(datasize)+"Bytes";
  // m.print(desc);
  // m.saveToFile(desc,filename);
}


void threadedRead(){

}

void threadedWrite(int no_of_threads,ull datasize,ull iter,string filename,string tb_name=""){
  // cout<<"WRITES"<<endl;
  // vector<Measure> m(no_of_threads);
  Measure* m = new Measure[no_of_threads];
  int i;
    for (i = 0; i < no_of_threads; i++) {
      threads[i] = thread(singleWrite,i,datasize,iter,&m[i],tb_name);
    }
    for (i = 0; i < no_of_threads; i++) {
      if (threads[i].joinable()) {
        threads[i].join();
      }
    }
    double sum=0;
    for (i = 0; i < no_of_threads; i++) {
      sum+=m[i].getAvg();
    }
    cout<<"Avg latency:"<<(sum/no_of_threads)<<endl;
}



int main(int argc,char *argv[]) {

  ull K = 1e3;
  ull M = 1e6;
  string folder = "SameMachine_LabPC_2/";

  system(("mkdir -p "+folder).c_str());


  // int i;
  // int no_threads;
  //
  // int j=1000;

  threadedWrite(1000, 2*K, 100000,"filename","");


  // cout<<"READS"<<endl;
  // //  for(int j=50;j<=400;j+=50)
  //   {
  //
  //   for (i = 0; i < j; i++) {
  //   		threads[i] = thread(singleRead,i,2*K,1000,"","");
  //   	}
  //   	for (i = 0; i < j; i++) {
  //   		if (threads[i].joinable()) {
  //   			threads[i].join();
  //   		}
  //   }
  //   cout<<"Done "<<j<<" threads"<<endl;
  //   double sum=0;
  //     for (i = 0; i < j; i++) {
  //     		sum+=avg[i];
  //     	}
  //   cout<<"Avg latency:"<<(sum/j)<<endl;
  // }


  //singleWrite(2*K,100000,folder+"SingleWrite_2KB_100000.csv");
  //singleRead (2*K,100000,folder+"SingleRead_2KB_100000.csv");

  return 0;
}
