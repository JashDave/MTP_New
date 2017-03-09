#include <iostream>
#include <vector>

using namespace std;

class KVImplHelper{
private:
  void *dataholder;
public:
  KVImplHelper();
  ~KVImplHelper();
  bool bind(string connection,string tablename);
  shared_ptr<KVData<string>> get(string const& key);
  shared_ptr<KVData<string>> put(string const& key,string const& val);
  shared_ptr<KVData<string>> del(string const& key);
  bool clear();
};
