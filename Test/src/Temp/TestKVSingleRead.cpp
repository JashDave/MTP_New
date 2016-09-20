//CSV file formate
//http://www.computerhope.com/issues/ch001356.htm

#include <iostream>
#include <chrono>
#include <fstream>
#include "TestUtils.h"
#include "../../KVStore.h"

#define conn_str "10.129.28.101:8090" //To be passed during make test CONN="conn stri$
//#define conn_str "127.1.1.1:8090" //To be passed during make test CONN="conn string"
//#define iter 10000
#define ll long long
#define ull unsigned long long


using namespace std;
using namespace std::chrono;
using namespace kvstore;


//
// const std::string currentDateTime() {
//   //Ref: http://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c
//     time_t     now = time(0);
//     struct tm  tstruct;
//     char       buf[80];
//     tstruct = *localtime(&now);
//     // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
//     // for more information about date/time format
//     strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
//
//     return buf;
// }
//
//
//
// class Measure {
// private:
//   double min=numeric_limits<double>::max(),max=-1,avg,sum=0,dur;
//   int count=0;
//   high_resolution_clock::time_point t1,t2;
//   vector<double> entries;
// public:
//   inline void start(){
//     t1=high_resolution_clock::now();
//   }
//   inline void end(){
//   //record end time and calc min,max,sum
//     t2=high_resolution_clock::now();
//     dur = duration_cast<microseconds>(t2 -t1).count();
//     entries.push_back(dur);
//     count++;// entries.size()
//     sum+=dur;
//     if(min>dur){
//       min=dur;
//     } else if(max<dur){
//       max=dur;
//     }
//   }
//
//   void print(string desc){
//     cout<<desc<<endl;
//     avg=sum/count;
//     cout<<"Min\t"<<min<<"us"<<endl;
//     cout<<"Max\t"<<max<<"us"<<endl;
//     cout<<"Avg\t"<<avg<<"us"<<endl;
//     cout<<"Count\t"<<count<<endl;
//   }
//
//   void saveToFile(string desc,string filename,bool overridetime=false){
//   ofstream file;
//   if(overridetime){
//     file.open(filename);
//   } else {
//     int sep = filename.find_last_of("/\\");
//     string dir="";
//     string fn="";
//     if(sep<0){
//       fn=filename;
//     } else {
//       dir = filename.substr(0,sep+1);
//       fn = filename.substr(sep+1);
//     }
//     file.open(dir+currentDateTime()+"_"+fn);
//   }
//   file << desc << "\n";
//   avg=sum/count;
//   file << "Min,Max,Avg,Count\n";
//   file << min << "," << max << "," << avg << "," << count << "\n";
//   for(ll i=0;i<entries.size();i++){
//     file << entries[i] << "\n";
//   }
//   file.close();
//   }
// };

void singleRead(ull datasize,ull iter,string filename,string tb_name=""){
    ll i;
    Measure m;
    KVStore<ll,string> kvs;
    KVData<string> kd;
    kvs.bind(conn_str,"table_single_rw"+tb_name);
    for(i=0;i<iter;i++){
      m.start();
      kd = kvs.get(i);
      m.end();
      if(kd.ierr<0){
        cout<<"Error in single read "<<datasize<<"B at i="<<i<<" : "<<kd.serr<<endl;
      }
    }
    string desc = "Single Read,KeyType:long long,KeySize:64bits,ValueType:string,ValueSize:"+to_string(datasize)+"Bytes";
    m.print(desc);
    m.saveToFile(desc,filename);
}


void singleWrite(ull datasize,ull iter,string filename,string tb_name=""){
    ll i;
    string data="";
    for(i=0;i<datasize;i++){
      data+=char('a'+i%26);
    }
    //cout<<data;
    Measure m;
    KVStore<ll,string> kvs;
    KVData<string> kd;
    kvs.bind(conn_str,"table_single_rw"+tb_name);
    for(i=0;i<iter;i++){
      m.start();
      kd = kvs.put(i,data);
      m.end();
      if(kd.ierr<0){
        cout<<"Error in single write "<<datasize<<"B at i="<<i<<" : "<<kd.serr<<endl;
      }
    }
    string desc="Single Write,KeyType:long long,KeySize:64bits,ValueType:string,ValueSize:"+to_string(datasize)+"Bytes";
    m.print(desc);
    m.saveToFile(desc,filename);
}


int main(int argc,char *argv[]) {
  //cout<<currentDateTime()<<endl;

  ull K = 1e3;
  ull M = 1e6;
  string folder = "SameMachine_LabPC_2/";
  // singleWrite(10,100000,folder+"SingleWrite_10B_100000.csv");
  // singleRead (10,100000,folder+"SingleRead_10B_100000.csv");
  //
  // singleWrite(K,100000,folder+"SingleWrite_1KB_100000.csv");
  // singleRead (K,100000,folder+"SingleRead_1KB_100000.csv");

  singleWrite(2*K,100000,folder+"SingleWrite_2KB_100000.csv");
  singleRead (2*K,100000,folder+"SingleRead_2KB_100000.csv");

  // singleWrite(100*K,1000,folder+"SingleWrite_100KB_1000.csv");
  // singleRead (100*K,1000,folder+"SingleRead_100KB_1000.csv");
  //
  // singleWrite(1*M,1000,folder+"SingleWrite_1MB_1000.csv");
  // singleRead (1*M,1000,folder+"SingleRead_1MB_1000.csv");

  return 0;
}
