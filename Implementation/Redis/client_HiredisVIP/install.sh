#!/bin/bash
git clone https://github.com/vipshop/hiredis-vip.git
cd hiredis-vip
make
sudo make install
#LD_LIBRARY_PATH=/usr/local/lib
#export $LD_LIBRARY_PATH
cd ..

echo "********"

mkdir -p /usr/local/include/kvscommon
cp ../../../Interface/KVStoreHeader.h /usr/local/include/kvscommon/

#mkdir -p deps
#cd deps
#git clone https://github.com/redis/hiredis.git
#cd hiredis
#make
#cd ..

#ar -cq libredisclient.a RedisClient.o

cp -p ./src/* /usr/local/include/redis

rm -r tmp
mkdir tmp
cd tmp
ar x ../hiredis-vip/libhiredis_vip.a
#pwd
g++ -std=c++11 -c ../src/KVStoreNonTemplateImplRedis.cpp -Wno-deprecated -w -fpermissive
ar cq libkvs_redis.a *.o
#ar crsT libkvs_redis.a /usr/lib/libredisclient.a /usr/local/lib/libhiredis.a
#ar crsT libkvs_redis.a redis_client/src/libredisclient.a hiredis/libhiredis.a
cp -p libkvs_redis.a /usr/lib


