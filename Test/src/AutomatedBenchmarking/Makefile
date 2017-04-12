redis:
	g++ CommonNoReqPerSecTest_v2.cpp -std=c++11 -lkvstore_v2 -lkvs_redis_v2 -DDS=\"REDIS\" -pthread -lboost_serialization

leveldb:
	g++ CommonNoReqPerSecTest_v2.cpp -std=c++11 -lkvstore_v2 -lkvs_leveldb_v2 -DDS=\"LEVELDB\" -pthread -lboost_serialization

memc:
	g++ CommonNoReqPerSecTest_v2.cpp -std=c++11 -lkvstore_v2 -lmemcached -lkvs_memcached_v2 -DDS=\"MEMC\" -pthread -lboost_serialization -lmemcached
