/*
   g++ --std=c++0x -g -I../install/include -I. RMCLoadTestMultiThreaded.cpp  -o RMCLoadTestMultiThreaded.out -L../install/bin -lramcloud -Wl,-rpath=../install/bin -pthread
 */


#include "redis/KVStore.h"
#include <iostream>
#include <thread>

#include <stdio.h>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <string>
#include <ctime>

// #include "MessageClient.cpp"
//#include "ramcloud/RamCloud.h"
#include "TestUtils.h"


using namespace std;
using namespace kvstore;
// using namespace MessageClientNS;

#define NUM_ENTRIES 5000   //Key Space
// #define MAX_DATA_SIZE 2000 // in Bytes
long MAX_DATA_SIZE=2000; // in Bytes
#define RUN_COUNT (int)1e5
#define RUN_TIME 20 //in seconds
#define MAX_THREAD_COUNT 1000
//#define THREAD_COUNT 16
#define IMPL_NAME "Redis"



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
		KVData<string> kvd;
		// while(!go_start); //wait
		// while(run){
		for(int ii=0;i<RUN_COUNT;i++){
			pm[tid].start();
			//cluster->write(table, key[i].c_str(), key_len[i], value[i].c_str(), value_len[i]);
			kvd = k->put(key[i],value[i]);
			pm[tid].end();
			i++;
			i%=NUM_ENTRIES;
			if(kvd.ierr!=0){
				cout<<"PUT Tid:"<<tid<<" ERR:"<<kvd.serr<<endl;
			}
		}
		printf("PUT Thread #%d ended\n",tid);
		pm[tid].print("PUT Thread "+to_string(tid)+" ended\n");
		delete k;
}




void do_get(int tid, KVStore<string,string> *k){
		printf("GET Thread #%d started\n",tid);
		int i=0;
		KVData<string> kvd;
		// while(!go_start); //wait
		// while(run){
	  for(int ii=0;i<RUN_COUNT;i++){
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
			printf("Usage: %s Server_IPAddress ITER_NUM MAX_DATA_SIZE\n", argv[0]);
			exit(0);
		}

 	  MAX_DATA_SIZE = stoi(argv[3]);
		int iter_num = stoi(argv[2]);
		SERVER_IP = string(argv[1]);

		int i=0;
		string config=SERVER_IP+":7001";
		string table_name="TestTable";

		num_cpus = std::thread::hardware_concurrency();
		init_data();

		vector<int> TC={60};
		// vector<int> TC={2,6,8,12,24,36,60,96,144,192};

		for(int THREAD_COUNT:TC) {

			cout<<"THREAD COUNT="<<THREAD_COUNT<<endl;
			for (int i = 0; i < THREAD_COUNT; i++) {
				pm[i].reset();
				gm[i].reset();
			}

			//----Multithreaded Write------
			for (i = 0; i < THREAD_COUNT; i++) {
			  KVStore<string,string> *k=new KVStore<string,string>();
				cout<< k->bind(config,table_name)<<endl;
				put_threads[i] = thread(do_put, i, k);
				pinThreadToCPU(&put_threads[i],i);
			}
			sleep(RUN_TIME);
			for (i = 0; i < THREAD_COUNT; i++) {
				if (put_threads[i].joinable()) {
					put_threads[i].join();
				}
			}
			for (i = 0; i < THREAD_COUNT; i++) {
				pm[i].print("Redis writes with TC"+to_string(i));
				pm[i].reset();
			}
			sleep(1);





			//-----Multithreaded Read-------
			for (i = 0; i < THREAD_COUNT; i++) {
			  KVStore<string,string> *k=new KVStore<string,string>();
				k->bind(config,table_name);
				get_threads[i] = thread(do_get, i, k);

				pinThreadToCPU(&get_threads[i],i);
			}
			sleep(RUN_TIME);
			for (i = 0; i < THREAD_COUNT; i++) {
				if (get_threads[i].joinable()) {
					get_threads[i].join();
				}
			}
			for (i = 0; i < THREAD_COUNT; i++) {
				gm[i].print("Redis reads with TC"+to_string(i));
				gm[i].reset();
			}
			sleep(1);




			//----Multithreaded Mixed (Read+Write)---------
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
			sleep(RUN_TIME);

			for (i = 0; i < THREAD_COUNT; i++) {
				if (put_threads[i].joinable()) {
					put_threads[i].join();
				}
				if (get_threads[i].joinable()) {
					get_threads[i].join();
				}
			}
			for (i = 0; i < THREAD_COUNT; i++) {
				pm[i].print("Redis M_write with TC"+to_string(i));
				gm[i].print("Redis M_reads with TC"+to_string(i));
			}

		}//End of THREAD_COUNT loop

		return 0;
		}
