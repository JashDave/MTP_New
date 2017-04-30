#ifndef __KVIMPL_H__
#define __KVIMPL_H__

#include <iostream>
#include <vector>
#include <memory>

#define KVERR std::cerr
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

		KVData(){
			serr = "Default: Unknown error.";
			ierr = -1;
		}
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

  bool bind(string& connection,string& tablename);

  KVData<string> get(string const& key);
  KVData<string> put(string const& key,string const& val);
  KVData<string> del(string const& key);

  vector<KVData<string>> mget(vector<string> const& key);
  vector<KVData<string>> mdel(vector<string> const& key);
  vector<KVData<string>> mput(vector<string> const& key, vector<string> const& val);

	bool clear();
};


/*
  AsyncKVImplHelper
*/

/*
class AsyncKVImplHelper{
	private:
		void *dataholder=NULL;
	public:
	  AsyncKVImplHelper();
	  ~AsyncKVImplHelper();
		AsyncKVImplHelper(const AsyncKVImplHelper&);

	  bool bind(string& connection,string& tablename);

		void async_get(string key, void (*fn)(KVData<string>,void *, void *),void *data, void *vfn);
		void async_put(string key, string val, void (*fn)(KVData<string>,void *, void *),void *data, void *vfn);
		void async_del(string key, void (*fn)(KVData<string>,void *, void *),void *data, void *vfn);

		void async_mget(vector<string> key, void (*fn)(vector<KVData<string>>,void *, void *),void *data, void *vfn);
		void async_mput(vector<string> key, vector<string> val, void (*fn)(vector<KVData<string>>,void *, void *),void *data, void *vfn);
		void async_mdel(vector<string> key, void (*fn)(vector<KVData<string>>,void *, void *),void *data, void *vfn);
};
*/
}
#endif
