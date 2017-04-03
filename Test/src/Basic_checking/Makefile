
G++ = g++ -std=c++11 -g

ifeq ($(kvs),leveldb)
	KVSTORE = -lkvs_leveldb_v2
	#DEFINES = -DCONF=string\(\"10.129.28.44:8091\"\)
	#DEFINES = -DCONF=string\(\"127.1.1.1:8090\"\)
	DEFINES = -DCONF=string\(\"10.129.28.44:8090\"\)
else
        KVSTORE = -lkvs_redis_v2
	DEFINES = -DCONF=string\(\"10.129.28.141:7003\"\)
endif
LIBS = -lkvstore_v2 -lboost_serialization -pthread $(KVSTORE)

all:	test

test:
	$(G++) TestKVStoreImpl_Basic.cpp $(LIBS) $(DEFINES) -o Basic_KVStore.out
	$(G++) TestKVRequestImpl_Basic.cpp $(LIBS) $(DEFINES) -o Basic_KVRequest.out
	$(G++) TestKVStoreImpl_Async_Basic.cpp $(LIBS) $(DEFINES) -o Async_KVStore.out
	$(G++) TestKVRequestImpl_Async_Basic.cpp $(LIBS) $(DEFINES) -o Async_KVRequest.out
	$(G++) TestKVStore_AsyncVsBlocking_SingleUserThread.cpp $(LIBS) $(DEFINES) -o AsyncVsBlocking_KVStore.out
	$(G++) TestKVRequest_AsyncVsBlocking_SingleUserThread.cpp $(LIBS) $(DEFINES) -o AsyncVsBlocking_KVRequest.out

run:
	./Basic_KVStore.out
	./Basic_KVRequest.out
	./Async_KVStore.out
	./Async_KVRequest.out
	./AsyncVsBlocking_KVStore.out
	./AsyncVsBlocking_KVRequest.out
