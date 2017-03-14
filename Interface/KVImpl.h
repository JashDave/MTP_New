#ifndef __KVIMPL_H__
#define __KVIMPL_H__

#include <iostream>
#include <vector>
#include <memory>

using namespace std;

namespace kvstore {
/*
	Data object for returning response of any key value operation
*/
	template<typename ValType>
	class KVData {
	public:
		string serr;
		int ierr;
		ValType value;
		//void set(string se,int ie,string val);
	};


/*
  KVImplHelper
  To be implemented for specific key value stores.
*/
class KVImplHelper{
private:
  void *dataholder;
public:
  KVImplHelper();
  ~KVImplHelper();
	KVImplHelper(KVImplHelper&); /* copy constructor */
  bool bind(string connection,string tablename);
  std::shared_ptr<KVData<string>> get(string const& key);
  std::shared_ptr<KVData<string>> put(string const& key,string const& val);
  std::shared_ptr<KVData<string>> del(string const& key);
	void async_get(string key, void (*fn)(std::shared_ptr<KVData<string>>,void *),void *data);
	void async_put(string key,string val, void (*fn)(std::shared_ptr<KVData<string>>,void *),void *data);
	void async_del(string key, void (*fn)(std::shared_ptr<KVData<string>>,void *),void *data);
  bool clear();

  int mget(vector<string>& key, vector<string>& tablename, vector<std::shared_ptr<KVData<string>>>& ret);
  int mdel(vector<string>& key, vector<string>& tablename, vector<std::shared_ptr<KVData<string>>>& ret);
  int mput(vector<string>& key, vector<string>& val, vector<string>& tablename, vector<std::shared_ptr<KVData<string>>>& ret);
};
}
#endif
