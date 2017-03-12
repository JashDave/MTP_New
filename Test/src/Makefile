PATH_TO_RAMCLOUD = /home/$(USER)/RAMCloud

redis:
	g++ CommonNoReqPerSecTest.cpp RedisKVImplementation.cpp -std=c++11 -pthread -lkvs_redis
	g++ CommonNoReqPerSecTest_v2.cpp RedisKVImplementation.cpp -std=c++11 -pthread -lkvs_redis -o r2.out -DDS=\"REDIS\"
	g++ CommonNoReqPerSecTest_v2.cpp RedisKVImplementation_hiredis_vip.cpp -I/usr/local/include/hiredis/ -lhiredis_vip -fpermissive -std=c++11 -pthread -I /usr/local/include/hiredis-vip/ -w -DDS=\"HIREDIS_VIP\" -o r3.out
	#g++ -std=c++11  -o RedisLoadTestMultiThreaded.o RedisLoadTestMultiThreaded.cpp -lkvs_redis -lboost_serialization -lpthread

ramcloud:
	g++ CommonNoReqPerSecTest.cpp RamCloudKVImplementation.cpp -std=c++11 -o RamCloudKVImplementation.out -g -I $(PATH_TO_RAMCLOUD)/install/include -L$(PATH_TO_RAMCLOUD)/install/bin -lramcloud -Wl,-rpath=$(PATH_TO_RAMCLOUD)/install/bin -pthread
	#g++ -std=c++11  -I$(PATH_TO_RAMCLOUD)/install/include -o RAMCloudLoadTestMultiThreaded.o RAMCloudLoadTestMultiThreaded.cpp -L$(PATH_TO_RAMCLOUD)/install/bin -lkvstore -lboost_serialization -lpthread -lramcloud -Wl,-rpath=$(PATH_TO_RAMCLOUD)/install/bin -Wno-deprecated
	#g++ -std=c++11  -I$(PATH_TO_RAMCLOUD)/install/include -o TestKVStore.o TestKVStore.cpp -L$(PATH_TO_RAMCLOUD)/install/bin -lkvstore -lboost_serialization -lpthread -lramcloud -Wl,-rpath=$(PATH_TO_RAMCLOUD)/install/bin #-Wno-deprecated
	
leveldb:	
	g++ -std=c++11 CommonNoReqPerSecTest.cpp LevelDBKVImplementation.cpp  -lkvstore -lboost_serialization -lpthread -o LevelDB.out
	#g++ -std=c++11  -o LevelDBLoadTestMultiThreaded.o LevelDBLoadTestMultiThreaded.cpp -lkvstore -lboost_serialization -lpthread


#all:
#	g++ -std=c++11 -o TestKVMultiThreaded.o TestKVMultiThreaded.cpp -lkvstore -lboost_serialization -lpthread -Wno-deprecated
#	g++ -std=c++11 TestKVSingleRead.cpp -o TestKVSingleRead.o -lkvstore -lboost_serialization -Wno-deprecated

clean:
	rm *.o *.out
