PATH_TO_RAMCLOUD = /home/$(USER)/RAMCloud

redis:
	g++ -std=c++11  -o RedisLoadTestMultiThreaded.o RedisLoadTestMultiThreaded.cpp -lkvs_redis -lboost_serialization -lpthread -fpermissive
ramcloud:
	g++ -std=c++11  -I$(PATH_TO_RAMCLOUD)/install/include -o RAMCloudLoadTestMultiThreaded.o RAMCloudLoadTestMultiThreaded.cpp -L$(PATH_TO_RAMCLOUD)/install/bin -lkvstore -lboost_serialization -lpthread -lramcloud -Wl,-rpath=$(PATH_TO_RAMCLOUD)/install/bin -Wno-deprecated
	#g++ -std=c++11  -I$(PATH_TO_RAMCLOUD)/install/include -o TestKVStore.o TestKVStore.cpp -L$(PATH_TO_RAMCLOUD)/install/bin -lkvstore -lboost_serialization -lpthread -lramcloud -Wl,-rpath=$(PATH_TO_RAMCLOUD)/install/bin #-Wno-deprecated
	
leveldb:	
	g++ -std=c++11  -o LevelDBLoadTestMultiThreaded.o LevelDBLoadTestMultiThreaded.cpp -lkvstore -lboost_serialization -lpthread


#all:
#	g++ -std=c++11 -o TestKVMultiThreaded.o TestKVMultiThreaded.cpp -lkvstore -lboost_serialization -lpthread -Wno-deprecated
#	g++ -std=c++11 TestKVSingleRead.cpp -o TestKVSingleRead.o -lkvstore -lboost_serialization -Wno-deprecated

clean:
	rm *.o *.out
