
G++ = g++ -std=c++11 -g

ifeq ($(kvs),leveldb)
	KVSTORE = -lkvs_leveldb_v2
	#DEFINES = -DCONF=string\(\"10.129.28.44:8091\"\)
	#DEFINES = -DCONF=string\(\"127.1.1.1:8090\"\)
	DEFINES = -DCONF=string\(\"10.129.28.44:8090\"\)
else
        KVSTORE = -lkvs_redis_v3
	DEFINES = -DCONF=string\(\"10.129.28.141:7001\"\)
endif


ifeq ($(kvs),memcached)
	KVSTORE = -lmemcached -lkvs_memcached_v2 -lmemcached 
	DEFINES = -DCONF=string\(\"--SERVER=10.129.26.154:11211\"\)
endif

LIBS = -lboost_serialization -pthread $(KVSTORE)

all:	test

test:
	$(G++) TestKVStoreImpl_Basic.cpp $(LIBS) $(DEFINES) -o Basic_KVStore.out

run:
	./Basic_KVStore.out
