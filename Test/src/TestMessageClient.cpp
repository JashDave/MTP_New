#include "MessageClient.cpp"

int main(){
  MessageClient kv("10.129.28.101",8091);
  vector<string> data;
  data.push_back("sar -o testop2 -u 1");
  kv.send(data);
  string x;
  cin>>x;
  data[0] = "pkill -SIGINT sar";
  kv.send(data);
  cout<<data[0]<<endl;
}
