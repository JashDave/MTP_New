#include "TestUtils.h"
#include <sstream>
int main(){
  cout<<currentMilis()<<endl;
  cout<<currentMicros()<<endl;
  stringstream ss;
  ss<<"T1"<<endl;
  ss<<"T2"<<endl;
  cout<<ss.str()<<endl;
  return 0;
}
