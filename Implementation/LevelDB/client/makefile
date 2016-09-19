GCC = g++ -std=c++11
LIBS = -lboost_serialization
WARN = 0
SHOWWARN = -Wno-deprecated
OPLEVEL = 0
OPL =
TC = 50


IMPLPATH = src
BINPATH = bin
TESTPATH = test
TESTSRC = $(TESTPATH)/src
TESTBIN = $(TESTPATH)/bin

ifeq ($(WARN),1)
SHOWWARN =
endif

ifneq ($(OPLEVEL),0)
OPL = -O$(OPLEVEL)
endif

all:	$(BINPATH)/KVStoreNonTemplateImplLevelDB.o


$(BINPATH)/KVStoreNonTemplateImplLevelDB.o:	$(IMPLPATH)/kvstore_client.cpp $(IMPLPATH)/KVStoreImplLevelDB.cpp $(IMPLPATH)/KVStoreNonTemplateImplLevelDB.cpp KVStoreHeader.h KVStore.h
	$(GCC) $(SHOWWARN) $(OPL) -c $(IMPLPATH)/KVStoreNonTemplateImplLevelDB.cpp -o $(BINPATH)/KVStoreNonTemplateImplLevelDB.o

install:	$(BINPATH)/KVStoreNonTemplateImplLevelDB.o
	ar -cq $(BINPATH)/libkvstore.a $(BINPATH)/KVStoreNonTemplateImplLevelDB.o
	cp -p $(BINPATH)/libkvstore.a /usr/lib


test:	$(TESTBIN)/TestKVStore.o $(TESTBIN)/TestKVStorePerformance.o $(TESTBIN)/TestKVStoreNoReqPerSec.o


$(TESTBIN)/TestKVStore.o: $(IMPLPATH)/kvstore_client.cpp $(IMPLPATH)/KVStoreImplLevelDB.cpp KVStoreHeader.h KVStore.h $(TESTSRC)/TestKVStore.cpp
	$(GCC) $(SHOWWARN) $(OPL) $(TESTSRC)/TestKVStore.cpp  -o $(TESTBIN)/TestKVStore.o $(LIBS)


$(TESTBIN)/TestKVStorePerformance.o: $(IMPLPATH)/kvstore_client.cpp $(IMPLPATH)/KVStoreImplLevelDB.cpp KVStoreHeader.h KVStore.h $(TESTSRC)/TestKVStorePerformance.cpp
	$(GCC) $(SHOWWARN) $(OPL) $(TESTSRC)/TestKVStorePerformance.cpp  -o $(TESTBIN)/TestKVStorePerformance.o $(LIBS) -Dthread_count=$(TC)


$(TESTBIN)/TestKVStoreNoReqPerSec.o: $(IMPLPATH)/kvstore_client.cpp $(IMPLPATH)/KVStoreImplLevelDB.cpp KVStoreHeader.h KVStore.h $(TESTSRC)/TestKVStoreNoReqPerSec.cpp
	$(GCC) $(SHOWWARN) $(OPL) $(TESTSRC)/TestKVStoreNoReqPerSec.cpp  -o $(TESTBIN)/TestKVStoreNoReqPerSec.o $(LIBS) -Dthread_count=$(TC)

clean:
	rm -f *~ $(TESTBIN)/* bin/*
