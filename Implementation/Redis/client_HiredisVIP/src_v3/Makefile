# Make for version 3

# Dependencies 

INSTALL_PATH=/usr/local
LIB_NAME=libkvs_redis_v3.a
G++ = g++ -O3 -std=c++11



install:	install_common $(LIB_NAME)
	cp -f bin/$(LIB_NAME) $(INSTALL_PATH)/lib

install_common:
	cd ../../../../Interface/ && make -f Makefile_v3
	

KVImplRedisHiredisVip.o:	KVImplRedisHiredisVip.cpp
	$(G++) -c KVImplRedisHiredisVip.cpp -o KVImplRedisHiredisVip.o

$(LIB_NAME):	KVImplRedisHiredisVip.o
	rm -f -r bin
	mkdir -p bin
	mv -f *.o bin/
	cp ../hiredis-vip/libhiredis_vip.a bin/
	cd bin && ar -x libhiredis_vip.a && rm libhiredis_vip.a && ar -cvq $(LIB_NAME) *

clean:
	rm -f -r bin
	rm -f *.o

uninstall:
	rm $(INSTALL_PATH)/lib/$(LIB_NAME)

