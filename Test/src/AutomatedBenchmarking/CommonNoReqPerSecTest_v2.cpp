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

#include <kvstore/KVStoreHeader_v2.h>
#include "TestUtils.h"
//#include "KVImplementation.h"
#include "MessageClient.cpp"
#include "RandomNumberGenerator.cpp"
#include "DataSetGenerator.cpp"
#include "Experiment.cpp"
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

void doAsyncSystem(string cmd){
  thread td([=]{system(cmd.c_str());});
  td.detach();
}

class SystemMonitor{
private:
  vector<string> server_list;
  vector<string> ip_list;
  vector<shared_ptr<MessageClient>> msgc_list;
  string folder;
public:
  SystemMonitor(vector<string> &sl, string f){
    server_list = sl;
    folder = f;
    for(string& connection : server_list){
      int colon = connection.find(":");
      string ip = connection.substr(0,colon);
      int port = stoi(connection.substr(colon+1),nullptr,10);
      ip_list.push_back(ip);
      msgc_list.push_back( make_shared<MessageClient>(ip,port) );
    }
  }

  void startMonitoring(){
			vector<string> cmd_vec;
			cmd_vec.push_back("rm -f ~/perf_data");
      cmd_vec.push_back("sar -o ~/perf_data 1");
			for(auto msgc: msgc_list){
        msgc->send(cmd_vec);
      }
      string cmd = "sar -o "+folder+"perf_data 1";
      cout<<cmd<<endl;
      doAsyncSystem(cmd);
      // system(cmd.c_str());
  }

  void stopMonitoring(){
			vector<string> cmd_vec;
			cmd_vec.push_back("pkill -SIGINT sar");
			for(auto msgc: msgc_list){
        msgc->send(cmd_vec);
      }

      string cmd = "pkill -SIGINT sar";
      doAsyncSystem(cmd);
      // system(cmd.c_str());
      sleep(1);

			for(auto ip: ip_list){
        string cmd = "scp jash2@"+ip+":~/perf_data "+folder+"Perf_data"+ip;
        // doAsyncSystem(cmd);
        system(cmd.c_str());
      }

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

  vector<string> server_list = {"10.129.28.101:8091","10.129.28.141:8091","10.129.26.81:8091"};
  vector<string> key = DataSetGenerator::getRandomStrings(DATASET_SIZE,KEY_SIZE);
  vector<string> value = DataSetGenerator::getRandomStrings(DATASET_SIZE,VALUE_SIZE);

  int thread_count = THREAD_COUNT;
  int run_time = RUN_TIME;
  Experiment e = Experiment(key, value, string(IP)+":"+string(PORT), thread_count, run_time,0); //vector<string> &k, vector<string> &v, string con, int tc, int rt, int dist

  string sep="/";
  string DATE=currentDateTime("%Y-%m-%d");
  string iter_num = string(argv[1]);
  string folder=string(DS)+sep+DATE+sep+"TC"+to_string(thread_count)+sep+iter_num+sep;
  cout<<folder<<endl;
  system((string("mkdir -p ")+folder).c_str());
  SystemMonitor sm(server_list,folder);

  // e.setReadProb(0.5);
  // e.runExperiment(folder + "RP_"+to_string(e.getReadProb()));
  //
  // e.setReadProb(1);
  // e.runExperiment(folder + "RP_"+to_string(e.getReadProb()));

  sm.startMonitoring();
  e.setReadProb(0);
  e.runExperiment(folder + "RP_"+to_string(e.getReadProb()));
  sm.stopMonitoring();

  // long num_cpus = std::thread::hardware_concurrency();
  // string detailes = "CPU count, CPU util, NW rx(KBps), NW tx(KBps), NW util, Mem Size, Mem util, Disk util, Page Faults";

  return 0;
}
