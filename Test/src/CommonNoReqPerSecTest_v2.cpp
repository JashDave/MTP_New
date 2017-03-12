/*
   g++ --std=c++0x -g -I../install/include -I. RMCLoadTestMultiThreaded.cpp  -o RMCLoadTestMultiThreaded.out -L../install/bin -lramcloud -Wl,-rpath=../install/bin -pthread
 */

// -DDS="Redis"

#define RUN_TIME 20
#define THREAD_COUNT 300
#define KEY_SIZE 5
#define VALUE_SIZE 10
#define DATASET_SIZE 10000
#define SRAND_ON 1

#define UNIFORM "Uniform"
#define ZIPF "Zipf"
#define DIST UNIFORM
// #define DIST ZIPF

//#define IP "10.129.26.81"
//#define PORT "11100"
#define IP "10.129.28.141"
#define PORT "7003"
// #define PORT "8090"
#define SERVER_CTRL_PORT 8091


// #include "../../Implementation/RAMCloud/client/src/KVStore.h"
#include <iostream>
#include <thread>
#include <stdio.h>
#include <cstdlib> //rand()
#include <vector>
#include <cstring>
#include <string>
#include <ctime>
#include <unistd.h>
#include <chrono>
#include <sstream>  // stringstream
#include "TestUtils.h"
#include "KVImplementation.h"
#include "MessageClient.cpp"
#include "RandomNumberGenerator.cpp"
// #include "ramcloud/RamCloud.h"

// #ifdef REDIS
//   #include <redis>
//   using namespace kvstore;
// #endif /* REDIS */

using namespace std;
using namespace std::chrono;
using namespace MessageClientNS;

// using namespace RAMCloud;
// using namespace kvstore;

#ifdef DEFAULTIMPL
  class KVImplementation {
  public:
    bool bind(string ip, string port){return false;}
    bool put(string key, string value){return false;}
    string get(string key){return "Err";}
  };
#endif


#ifdef MDEBUG
 #define TRACE(x) {x}
#else
 #define TRACE(x) {}
#endif



class ServerCommands{
  private:
    MessageClient *mc;
	public:
		ServerCommands(string ip, int port){
			mc = new MessageClient(ip,port);
		}

    void sendCommand(string cmd){
			static vector<string> cmd_vec(1,cmd);
			mc->send(cmd_vec);
    }

    void sendCommands(vector<string> cmd_vec){
      mc->send(cmd_vec);
    }

		void startSARatServer(string desc,string folder="./"){
			static vector<string> cmd_vec(1);
			cmd_vec[0]="sar -o "+folder+"perf_data_"+desc+" -u 1";
			mc->send(cmd_vec);
		}

		void stopSARatServer(){
			static vector<string> cmd_vec(1);
			cmd_vec[0]="pkill -SIGINT sar";
			mc->send(cmd_vec);
		}
};


void doSystem(string cmd){
	system(cmd.c_str());
}



class Experiment{
private:
  vector<string> key;
  vector<string> value;
  int thread_count;
  int run_time;
  double readp;
  int dataSz;
  vector<Measure> m;
  bool run;
public:
  Experiment(vector<string> &k, vector<string> &v,int tc,int rt){
    key = k;
    value = v;
    dataSz = k.size();
    thread_count = tc;
    run_time = rt;
    readp = 0.5;
    m.resize(tc);
    run = false;
    loadData(); //? to be invoked here or not?
  }
  void setRuntime(int rt){
    run_time = rt;
  }
  void setThreadCount(int tc){
    thread_count = tc;
    m.resize(tc);
  }
  void setData(vector<string> &k, vector<string> &v) {
    key = k;
    value = v;
    dataSz = k.size();
    loadData(); //? to be invoked here or not?
  }
  void loadData(){
    KVImplementation k;
    k.bind(IP,PORT);
    for(int i=0;i<dataSz;i++){
      if(!k.put(key[i],value[i])){
        cerr << "Failure in loading data at index "<<i<<endl;
      }
    }
  }
  void setReadProb(double rp){
    readp = rp;
  }
  double getReadProb(){
    return readp;
  }

  void worker(int id, KVImplementation k){
    double rp = readp * RAND_MAX;
    int r1,r2;
    bool succ;
    string val;
    //-------------------------------
    // key[id] = "key"+to_string(id%3+3);
    // k.put(key[id],value[id]);
    //-------------------------------
    // string key = "Key"+to_string(tid);
    while(!run);
    TRACE(cout<<"Tid "<<tid<<" started"<<endl;)
    while(run){
      r1 = rand();
      if(DIST==UNIFORM){
        r2 = RandomNumberGenerator::uniform(0,dataSz-1);
      } else if (DIST==ZIPF){
        r2 = RandomNumberGenerator::zipf(0,dataSz-1);
        // if(r2<0 || r2>=dataSz)
        //   cerr<<"ERROR: in zipf "<<r2<<endl;
      }
      //------------------------TEST------------
      // r2=id;
      //=---------------------------------------

      if(r1<rp){
        //Do read
        m[id].start();
        val = k.get(key[r2]);
        m[id].end();
        if(val != value[r2]){
          m[id].incfcount();
        }
      } else {
        //Do Write
        m[id].start();
        succ = k.put(key[r2],value[r2]);
        m[id].end();
        if(!succ){
          m[id].incfcount();
        }
      }
    }
    TRACE(cout<<"Tid "<<tid<<" ended"<<endl;)
  }

  void runExperiment(string filename){
    vector<thread> td(thread_count);
    run = false;
    for(int i=0;i<thread_count;i++){
      m[i].reset();
      KVImplementation k;
      k.bind(IP,PORT);
      td[i] = thread(&Experiment::worker,this,i,k);
      pinThreadToCPU(&td[i],i);
    }
    run=true;
    sleep(run_time);
    // cout<<"Timer Expired"<<endl;
    run=false;
    for (int i = 0; i < thread_count; i++) {
      if (td[i].joinable()) {
        td[i].join();
      }
    }

    long num_cpus = std::thread::hardware_concurrency();
    string desc = string("Data Store,Work Load,Data Size(bytes),Thread Count,Run Time") + string("\n") + string(DS) +","+string(DIST)+" ReadProb:"+to_string(getReadProb())+","+to_string(VALUE_SIZE)+","+to_string(thread_count)+","+to_string(run_time);
    string detailes = "CPU count, CPU util, NW rx(KBps), NW tx(KBps), NW util, Mem Size, Mem util, Disk util, Page Faults";
    // string client_detailes = getClientDetailes();
    // string server_detailes = getServerDetailes();

    Measure merged;
    merged.mergeAll(m);
    merged.saveToFile(desc,filename,true);
    merged.print(desc);
  }

};


class DataSetGenerator{
public:
  //Generate a random string of length len
  static string getRandomString(int len){
    string ret(len,'0');
    for(int i=0;i<len;i++){
      ret[i] = '0'+rand()%75;
    }
    return ret;
  }

  //Generates sz uniformly random strings for length len
  static vector<string> getRandomStrings(int sz, int len){
    vector<string> ret;
    for(int i=0;i<sz;i++){
      ret.push_back(getRandomString(len));
    }
    return ret;
  }
};



int main(int argc, char *argv[]){
  if(argc!=2){
    cerr<<"Argument 1 required"<<endl;
    return -1;
  }
  if(SRAND_ON){
    srand(time(NULL));
  }
  vector<string> key = DataSetGenerator::getRandomStrings(DATASET_SIZE,KEY_SIZE);
  vector<string> value = DataSetGenerator::getRandomStrings(DATASET_SIZE,VALUE_SIZE);

  int thread_count = THREAD_COUNT;
  int run_time = RUN_TIME;
  Experiment e = Experiment(key,value,thread_count,run_time);
  string details;

  string sep="/";
  string DATE=currentDateTime("%Y-%m-%d");
  string folder=string(DS)+sep+DATE+sep+string(argv[1])+sep;
  cout<<folder<<endl;
  doSystem("mkdir -p "+folder);

  // e.setReadProb(0.5);
  // e.runExperiment(folder + "RP_"+to_string(e.getReadProb()));
  //
  // e.setReadProb(1);
  // e.runExperiment(folder + "RP_"+to_string(e.getReadProb()));

  e.setReadProb(0);
  e.runExperiment(folder + "RP_"+to_string(e.getReadProb()));
  return 0;
}
