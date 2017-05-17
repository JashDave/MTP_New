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
  void *dataholder=NULL;
public:
  KVImplHelper();
  ~KVImplHelper();
	KVImplHelper(const KVImplHelper&); /* copy constructor */
  bool bind(string connection,string tablename);
  KVData<string> get(string const& key);
  KVData<string> put(string const& key,string const& val);
  KVData<string> del(string const& key);
	void async_get(string key, void (*fn)(KVData<string>,void *), void *vfn);
	void async_put(string key, string val, void (*fn)(KVData<string>,void *), void *vfn);
	void async_del(string key, void (*fn)(KVData<string>,void *), void *vfn);
  bool clear();

  int mget(vector<string>& key, vector<string>& tablename, vector<KVData<string>>& ret);
  int mdel(vector<string>& key, vector<string>& tablename, vector<KVData<string>>& ret);
  int mput(vector<string>& key, vector<string>& val, vector<string>& tablename, vector<KVData<string>>& ret);

	void async_get(string key, string tablename, void (*fn)(KVData<string>,void *), void *vfn);
	void async_put(string key, string val, string tablename, void (*fn)(KVData<string>, void *), void *vfn);
	void async_del(string key, string tablename, void (*fn)(KVData<string>,void *), void *vfn);

};
}
#endif
