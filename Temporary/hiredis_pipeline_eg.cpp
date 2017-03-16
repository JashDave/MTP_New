#include<stdio.h>
#include<hiredis-vip/hircluster.h>
#include<iostream>
#include<thread>
#include<atomic>
#include<unistd.h>
 using namespace std;
int main()
{
	redisClusterContext *cc = redisClusterConnect("10.129.28.101:7003",HIRCLUSTER_FLAG_NULL);
	redisReply *reply;
	if(cc == NULL || cc->err)
	{
		printf("connect error : %s\n", cc == NULL ? "NULL" : cc->errstr);
		return -1;
	}

	redisClusterAppendCommand(cc, "set key1 value1");
	redisClusterAppendCommand(cc, "mset key2 value2 key3 value3");
	redisClusterAppendCommand(cc, "mget key2 key3");

	redisClusterGetReply(cc, &reply);  //for "set key1 value1"
	freeReplyObject(reply);
	redisClusterGetReply(cc, &reply);  //for "mset key2 value2 key3 value3"
	freeReplyObject(reply);
	redisClusterGetReply(cc, &reply);  //for "mget key2 key3"
	freeReplyObject(reply);

atomic<long> count(0);

	thread td1([&]{
		while(1) {
			if(redisClusterAppendCommand(cc, "get key1") != REDIS_ERR){
				count++;
				if(redisClusterGetReply(cc, &reply)==REDIS_OK){
					cout<<reply->type<<endl;
					cout<<reply->str<<endl;
					freeReplyObject(reply);
					count--;
				} else {
					cout<<"Error"<<endl;
					// redisClusterReset(cc);
				}
			}
			else{
				cout<<"\n\n\nAppend error\n\n\n"<<endl;
			}
			// usleep(100);
		}
	});
	thread td3([&]{
		while(1) {
			if(redisClusterAppendCommand(cc, "get key2") != REDIS_ERR){
				count++;
				if(redisClusterGetReply(cc, &reply)==REDIS_OK){
					cout<<reply->type<<endl;
					cout<<reply->str<<endl;
					freeReplyObject(reply);
					count--;
				} else {
					cout<<"Error"<<endl;
					// redisClusterReset(cc);
				}
			}
			else{
				cout<<"\n\n\nAppend error\n\n\n"<<endl;
			}
			// usleep(100);
		}
	});

	// thread td2([&]{
	// 	while(1){
	// 		usleep(100);
	// 		// reply = __redisBlockForReply(c);
	// 		// if(reply!=NULL){
	// 		if(count<0){cout<<"Negative count"<<endl;}
	// 		while(count==0){usleep(1000); cout<<"sleeping"<<endl;}
	// 		if(redisClusterGetReply(cc, &reply)==REDIS_OK){
	// 			cout<<reply->type<<endl;
	// 			cout<<reply->str<<endl;
	// 			freeReplyObject(reply);
	// 			count--;
	// 		} else {
	// 			cout<<"Error"<<endl;
	// 			// redisClusterReset(cc);
	// 		}
	// 	}
	// });

  td1.join();
	// td2.join();

	redisClusterReset(cc);

	redisClusterFree(cc);
	return 0;
}
