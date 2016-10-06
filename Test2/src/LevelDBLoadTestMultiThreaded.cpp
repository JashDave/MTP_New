/*
   g++ --std=c++0x -g -I../install/include -I. RMCLoadTestMultiThreaded.cpp  -o RMCLoadTestMultiThreaded.out -L../install/bin -lramcloud -Wl,-rpath=../install/bin -pthread
 */


#include "../../Implementation/LevelDB_Disk/client/src/KVStore.h"
#include <iostream>
#include <thread>

#include <stdio.h>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <string>
#include <ctime>

#include "MessageClient.cpp"
//#include "ramcloud/RamCloud.h"
#include "TestUtils.h"


using namespace std;
using namespace kvstore;
using namespace MessageClientNS;

#define NUM_ENTRIES 5000   //Key Space
//#define MAX_DATA_SIZE 2000 // in Bytes
long MAX_DATA_SIZE = 2000; // in Bytes
#define RUN_TIME 20 //in seconds
#define MAX_THREAD_COUNT 1000
//#define THREAD_COUNT 16
#define IMPL_NAME "LevelDB"



Measure pm[MAX_THREAD_COUNT];
Measure gm[MAX_THREAD_COUNT];

thread put_threads[MAX_THREAD_COUNT];
thread get_threads[MAX_THREAD_COUNT];
thread sar_threads[5];

string key[NUM_ENTRIES];
int key_len[NUM_ENTRIES];
string value[NUM_ENTRIES];
int value_len[NUM_ENTRIES];

string SERVER_IP="127.1.1.1";

bool run = true; //Read olny for threads  - Race condition
bool go_start = false;//Read olny for threads  - Race condition


class ServerCommands{
	public:
		MessageClient *mc;
		ServerCommands(string ip, int port){
			mc = new MessageClient(ip,port);
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


void init_data(){
	string data="";
	for(int i=0;i<MAX_DATA_SIZE/100;i++){
		data+="0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789";
	}

	for(int i=0;i<NUM_ENTRIES;i++){
		key[i]=to_string(i);
		key_len[i]=key[i].size();
		value[i]=data+to_string(i);
		value_len[i]=value[i].size()+1;
	}
}


void do_put(int tid, KVStore<string,string> *k){
		printf("PUT Thread #%d started\n",tid);
		int i=0;
		while(!go_start); //wait
		while(run){
			pm[tid].start();
			//cluster->write(table, key[i].c_str(), key_len[i], value[i].c_str(), value_len[i]);
			k->put(key[i],value[i]);
			pm[tid].end();
			i++;
			i%=NUM_ENTRIES;
		}
		printf("PUT Thread #%d ended\n",tid);
		pm[tid].print("PUT Thread "+to_string(tid)+" ended\n");
		delete k;
}




void do_get(int tid, KVStore<string,string> *k){
		printf("GET Thread #%d started\n",tid);
		int i=0;
		KVData<string> kvd;
		while(!go_start); //wait
		while(run){
			gm[tid].start();
			kvd = k->get(key[i]);
			gm[tid].end();
			if(kvd.ierr==0){
			// string ret_val=kvd->value;
			// if(ret_val.compare(value[i])!=0){
			// cerr<<"Error in GET  Tid:"<<tid<<" i:"<<i<<" got data:"<<ret_val<<endl;
			//printf("%s:%s\n",key[i].c_str(),str);
			// }
			} else {
			  cout<<"GET Tid:"<<tid<<" ERR:"<<kvd.serr<<endl;
			}
			i++;
			i%=NUM_ENTRIES;
		}
		printf("GET Thread #%d ended\n",tid);
		gm[tid].print("GET Thread "+to_string(tid)+" ended\n");
		delete k;
}





long num_cpus;
cpu_set_t cpuset;
int rc;

void pinThreadToCPU(thread *th,int i){
				CPU_ZERO(&cpuset);
				CPU_SET(i%num_cpus, &cpuset);
				rc = pthread_setaffinity_np(th->native_handle(), sizeof(cpu_set_t), &cpuset);
				if (rc != 0) {
					std::cerr << "Error calling pthread_setaffinity_np: " << rc << "\n";
				}
}

int main(int argc, char *argv[]) {
		if (argc != 4) {
			printf("Usage: %s Server_IPAddress   ITER_NUM   MAX_DATA_SIZE\n", argv[0]);
			exit(0);
		}


 	  MAX_DATA_SIZE = stoi(argv[3]);
		string sep="/";
		string DATE=sep+currentDateTime("%Y-%m-%d")+sep;
		int iter_num = stoi(argv[2]);
		string prefix="";
		string st1 = prefix+"PerformanceData"+sep;
		SERVER_IP = string(argv[1]);


		int i=0;
		string desc1="";
		string desc2="";
		string config=SERVER_IP+":8090";
		string table_name="TestTable";

		num_cpus = std::thread::hardware_concurrency();
		ServerCommands sc(SERVER_IP,8091);
		init_data();


		// vector<int> TC={1,2,4,6,8,10,12,14,16,32,48,64};
		vector<int> TC={2,6,12,24,36,48,60,72,120};
		//vector<int> TC={1,2};
		for(int THREAD_COUNT:TC) {

			cout<<"THREAD COUNT="<<THREAD_COUNT<<endl;
			for (int i = 0; i < THREAD_COUNT; i++) {
				pm[i].reset();
				gm[i].reset();
			}

			string st2 = IMPL_NAME+DATE+to_string(iter_num)+sep+to_string(MAX_DATA_SIZE/1000)+"KB"+sep+"TC"+to_string(THREAD_COUNT)+sep;
			string c_folder=st1+"Client"+sep+st2;
			system(("mkdir -p "+c_folder).c_str());

			string s_folder=st1+"Server"+sep+st2;
			vector<string> cmd_vec(1);
			cmd_vec[0]="mkdir -p "+s_folder;
			sc.mc->send(cmd_vec);





			//----Multithreaded Write------
			go_start=false;
			for (i = 0; i < THREAD_COUNT; i++) {
			  KVStore<string,string> *k=new KVStore<string,string>();
				k->bind(config,table_name);
				put_threads[i] = thread(do_put, i, k);

				pinThreadToCPU(&put_threads[i],i);
			}
			run=true;
			go_start=true;
			sar_threads[0]=thread(doSystem,"sar -o "+c_folder+"sar_threadedwrite 1");
			sc.startSARatServer("ThreadedWrite_DataSize"+to_string(MAX_DATA_SIZE)+"Bytes_Iter"+to_string(NUM_ENTRIES)+"_TC"+to_string(THREAD_COUNT)+".sarop",s_folder);
			sleep(RUN_TIME);
			system("pkill -SIGINT sar");
			sc.stopSARatServer();
			run=false;
			go_start=false;
			for (i = 0; i < THREAD_COUNT; i++) {
				if (put_threads[i].joinable()) {
					put_threads[i].join();
				}
			}
			desc1="RamCloud,Multithreaded Write,Iter:"+to_string(NUM_ENTRIES)+",Data Size:"+to_string(MAX_DATA_SIZE)+"Bytes,THREAD_COUNT:"+to_string(THREAD_COUNT);
			for (i = 0; i < THREAD_COUNT; i++) {
				pm[i].saveToFile(desc1,c_folder+"ThreadedWrite_TID"+to_string(i)+".csv",true);
				pm[i].reset();
			}
			sleep(1);





			//-----Multithreaded Read-------
			go_start=false;
			for (i = 0; i < THREAD_COUNT; i++) {
			  KVStore<string,string> *k=new KVStore<string,string>();
				k->bind(config,table_name);
				get_threads[i] = thread(do_get, i, k);

				pinThreadToCPU(&get_threads[i],i);
			}
			run=true;
			go_start=true;
			sar_threads[1]=thread(doSystem,"sar -o "+c_folder+"sar_threadedread 1");
			sc.startSARatServer("ThreadedRead_DataSize"+to_string(MAX_DATA_SIZE)+"Bytes_Iter"+to_string(NUM_ENTRIES)+"_TC"+to_string(THREAD_COUNT)+".sarop",s_folder);
			sleep(RUN_TIME);
			system("pkill -SIGINT sar");
			sc.stopSARatServer();
			run=false;
			go_start=false;
			for (i = 0; i < THREAD_COUNT; i++) {
				if (get_threads[i].joinable()) {
					get_threads[i].join();
				}
			}
			desc1="RamCloud,Multithreaded Read,Iter:"+to_string(NUM_ENTRIES)+",Data Size:"+to_string(MAX_DATA_SIZE)+"Bytes,THREAD_COUNT:"+to_string(THREAD_COUNT);
			for (i = 0; i < THREAD_COUNT; i++) {
				gm[i].saveToFile(desc1,c_folder+"ThreadedRead_TID"+to_string(i)+".csv",true);
				gm[i].reset();
			}
			sleep(1);




			//----Multithreaded Mixed (Read+Write)---------
			go_start=false;
			for (i = 0; i < THREAD_COUNT; i++) {
			  KVStore<string,string> *k=new KVStore<string,string>();
				k->bind(config,table_name);
				put_threads[i] = thread(do_put, i, k);
				k=new KVStore<string,string>();
				k->bind(config,table_name);
				get_threads[i] = thread(do_get, i, k);

				pinThreadToCPU(&put_threads[i],i);
				pinThreadToCPU(&get_threads[i],i);
			}
			run=true;
			go_start=true;
			sar_threads[2]=thread(doSystem,"sar -o "+c_folder+"sar_mixed 1");
			sc.startSARatServer("ThreadedMixedRW_DataSize"+to_string(MAX_DATA_SIZE)+"Bytes_Iter"+to_string(NUM_ENTRIES)+"_2xTC"+to_string(THREAD_COUNT)+".sarop",s_folder);
			sleep(RUN_TIME);
			system("pkill -SIGINT sar");
			sc.stopSARatServer();
			run=false;
			go_start=false;

			for (i = 0; i < THREAD_COUNT; i++) {
				if (put_threads[i].joinable()) {
					put_threads[i].join();
				}
				if (get_threads[i].joinable()) {
					get_threads[i].join();
				}
			}
			desc1="RamCloud,Mixed Multithreaded Write,Iter:"+to_string(NUM_ENTRIES)+",Data Size:"+to_string(MAX_DATA_SIZE)+"Bytes,THREAD_COUNT:"+to_string(THREAD_COUNT);
			desc2="RamCloud,Mixed Multithreaded Read,Iter:"+to_string(NUM_ENTRIES)+",Data Size:"+to_string(MAX_DATA_SIZE)+"Bytes,THREAD_COUNT:"+to_string(THREAD_COUNT);
			for (i = 0; i < THREAD_COUNT; i++) {
				pm[i].saveToFile(desc1,c_folder+"MixedThreadedWrite_TID"+to_string(i)+".csv",true);
				// pm[i].reset();
				gm[i].saveToFile(desc2,c_folder+"MixedThreadedRead_TID"+to_string(i)+".csv",true);
				// gm[i].reset();
			}

			cout<<"Waiting for SAR threads..."<<endl;
			for (i = 0; i < 3; i++) {
				if (sar_threads[i].joinable()) {
					sar_threads[i].join();
				}
			}
		}//End of THREAD_COUNT loop

		return 0;
		}
