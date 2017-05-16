#g++ -std=c++11 -g -c -I/home/$(USER)/RAMCloud/install/include KVImplRAMCloud.cpp -lboost_serialization -pthread -lramcloud -Wl,-rpath=/home/$(USER)/RAMCloud/install/bin

PATH_TO_RAMCLOUD = /home/$(USER)/RAMCloud

GCC = g++ -std=c++11 -g #Remove -g later
#LIBS = -lboost_serialization -pthread -lramcloud -Wl,-rpath=$(PATH_TO_RAMCLOUD)/install/bin #-Wl,linker options = ld linker options
INCLUDES = -I$(PATH_TO_RAMCLOUD)/install/include/

OPLEVEL = 0
WARN = 0
SHOWWARN = #-Wno-deprecated

IMPLPATH = .
BINPATH = bin

ifeq ($(WARN),1)
SHOWWARN =
endif

ifneq ($(OPLEVEL),0)
OPL = -O$(OPLEVEL)
endif

all:	$(BINPATH)/KVImplRAMCloud.o


$(BINPATH)/KVImplRAMCloud.o:	$(IMPLPATH)/KVImplRAMCloud.cpp
	$(GCC) $(SHOWWARN) $(OPL) -c $(INCLUDES) $(IMPLPATH)/KVImplRAMCloud.cpp -o $(BINPATH)/KVImplRAMCloud.o 

install:	$(BINPATH)/KVImplRAMCloud.o
	rm -r $(BINPATH)/libkvs_ramcloud_v2.a
	ar -cq $(BINPATH)/libkvs_ramcloud_v2.a $(BINPATH)/KVImplRAMCloud.o
	cp -p $(BINPATH)/libkvs_ramcloud_v2.a /usr/local/lib

uninstall:
	echo "Uninstall!!! Yet to be implemented."

clean:
	rm -f *~ bin/*
