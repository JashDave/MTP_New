
#include<iostream>
#ifdef JDEBUG
#define TRACE(x) {x;};
#else
#define TRACE(x) {}
#endif

#define IS_REACHABLE TRACE(std::cout<<"Reached:"<<__LINE__<<std::endl;)
#define jAssert(x,y) if(x){std::cout<<"At line "<<__LINE__<<":";y;exit(-1);}
