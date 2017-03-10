// Why both implementation and declaration are in same file
// Refer : https://isocpp.org/wiki/faq/templates
//        http://stackoverflow.com/questions/3040480/c-template-function-compiles-in-header-but-not-implementation
// Note that only implementation of templates are here since their true implementation is defered till their use.

#ifndef __KVSTORE_H__
#define __KVSTORE_H__

#include "KVImpl.h" 

#include <stdio.h>
#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include <vector>

//For returning hetrogeneous array as return of execute()
//#include <tuple> //Not used

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
// #include <boost/serialization/vector.hpp>
// #include <boost/serialization/array.hpp>
// #include <boost/serialization/vector.hpp>
// #include <boost/serialization/list.hpp>
// #include <boost/serialization/slist.hpp>
// #include <boost/serialization/set.hpp>
// #include <boost/serialization/bitset.hpp>
// #include <boost/serialization/string.hpp>
// #include <boost/serialization/map.hpp>
// #include <boost/shared_ptr.hpp>
// --------Depricated Warning-------
// #include <boost/serialization/hash_set.hpp>
// #include <boost/serialization/hash_map.hpp>


using namespace std;
// using namespace boost;

namespace kvstore {

#define OPR_TYPE_GET string("get")
#define OPR_TYPE_PUT string("put")
#define OPR_TYPE_DEL string("del")

/* Class declaration */
	template<typename KeyType, typename ValType>
	class KVStore;
	class KVResultSet;
	class KVRequest;
	template<typename ValType>
	class KVData;

/*
 Helper declarations
*/
	template<typename T>
	string toBoostString(T const &obj);

	template<typename T>
	T toBoostObject(string sobj);

/*
	KVResultSet holds results of KVRequest.execute();
*/
	class KVResultSet {
	private:
		// int count;
		std::shared_ptr< KVData<string> > extra;
		vector< std::shared_ptr< KVData<string> > > res;
		vector<string> operation_type;
	public:
		KVResultSet(vector<std::shared_ptr<KVData<string> > > r, vector<string> opr_type);
		int size();
		string oprType(int idx);

		template<typename ValType>
		std::shared_ptr<KVData<ValType>> get(int idx);
	};

	template<typename ValType>
	std::shared_ptr<KVData<ValType>> KVResultSet::get(int idx){
    std::shared_ptr<KVData<ValType>> ret = std::make_shared<KVData<ValType>>();
		int count = size();
    if(idx>=0 && idx<count){
      ret->ierr = res[idx]->ierr;
      ret->serr = res[idx]->serr;
			if(ret->ierr==0 && operation_type[idx]==OPR_TYPE_GET){
				ret->value = toBoostObject<ValType>(res[idx]->value);
			}
    } else {
      ret->ierr=-1;
      if(count==0)
	      ret->serr="Empty KVResultSet due to empty request queue, please ensure that your request queue is not empty.";
      else
	      ret->serr="KVResultSet index out of bound. Valid range is 0 to "+to_string(count-1)+" found "+to_string(idx);
    }
    return ret;
  }


/*
	This class mearges multiple operations into one request.
*/
	class KVRequest {
	private:
		void *dataholder;
		KVImplHelper kh;

		// int count=0;
		std::vector<string> operation_type;

		std::vector<string> put_tablename;
		std::vector<string> put_key;
		std::vector<string> put_value;

		std::vector<string> get_tablename;
		std::vector<string> get_key;

		std::vector<string> del_tablename;
		std::vector<string> del_key;
	public:
		KVRequest();
		~KVRequest();		//For distroying connection object

		bool bind(string connection);

		template<typename KeyType, typename ValType>
		void get(KeyType const& key,string tablename);

		template<typename KeyType, typename ValType>
		void put(KeyType const& key,ValType const& val,string tablename);

		template<typename KeyType, typename ValType>
		void del(KeyType const& key,string tablename);

		std::shared_ptr<KVResultSet> execute();
		void reset();
	};

	/*-------KVRequest::get()----------*/
	template<typename KeyType, typename ValType>
	void KVRequest::get(KeyType const& key,string tablename){
		string skey=toBoostString(key);
		operation_type.push_back(OPR_TYPE_GET);
		get_key.push_back(skey);
		get_tablename.push_back(tablename);
		// kh.get(skey,tablename);
	}
	/*-------KVRequest::put()----------*/
	template<typename KeyType, typename ValType>
	void KVRequest::put(KeyType const& key,ValType const& val,string tablename){
		string skey=toBoostString(key);
		string sval=toBoostString(val);
		operation_type.push_back(OPR_TYPE_PUT);
		put_key.push_back(skey);
		put_value.push_back(sval);
		put_tablename.push_back(tablename);
		// kh.put(skey,sval,tablename);
	}
	/*-------KVRequest::del()----------*/
	template<typename KeyType, typename ValType>
	void KVRequest::del(KeyType const& key,string tablename){
		string skey=toBoostString(key);
		operation_type.push_back(OPR_TYPE_DEL);
		del_key.push_back(skey);
		del_tablename.push_back(tablename);
		// kh.del(skey,tablename);
	}


/*
	Interface to Key-Value Store
*/
	template<typename KeyType, typename ValType>
	class KVStore {
	private:
		KVImplHelper kh;
		void *kvsclient;
	public:
		KVStore();
		~KVStore();
		bool bind(string connection,string tablename);
		std::shared_ptr<KVData<ValType>> get(KeyType const& key);
		std::shared_ptr<KVData<ValType>> put(KeyType const& key,ValType const& val);
		std::shared_ptr<KVData<ValType>> del(KeyType const& key);
		bool clear();
	};
	/*-------KVStore::KVStore()----------*/
	template<typename KeyType, typename ValType>
	KVStore<KeyType,ValType>::KVStore(){
	}
	/*-------KVStore::~KVStore()----------*/
	template<typename KeyType, typename ValType>
	KVStore<KeyType,ValType>::~KVStore(){
	}
	/*---------KVStore::bind()--------*/
	template<typename KeyType, typename ValType>
	bool KVStore<KeyType,ValType>::bind(string connection,string tablename){
		return kh.bind(connection,tablename);
	}
	/*-------KVStore::get()----------*/
  template<typename KeyType, typename ValType>
  std::shared_ptr<KVData<ValType>>  KVStore<KeyType,ValType>::get(KeyType const& key){
    string skey=toBoostString(key);
    string sval;
    std::shared_ptr<KVData<ValType>> kvd = std::make_shared<KVData<ValType>>();
		std::shared_ptr<KVData<string>> res = kh.get(skey);
		kvd->ierr = res->ierr;
		kvd->serr = res->serr;
		if(kvd->ierr==0){
			kvd->value = toBoostObject<ValType>(res->value);
		}
		return kvd;
	}
	/*-------KVStore::put()----------*/
	template<typename KeyType, typename ValType>
	std::shared_ptr<KVData<ValType>>  KVStore<KeyType,ValType>::put(KeyType const& key,ValType const& val){
		string skey=toBoostString(key);
		string sval=toBoostString(val);
		std::shared_ptr<KVData<ValType>> kvd = std::make_shared<KVData<ValType>>();
		std::shared_ptr<KVData<string>> res = kh.put(skey,sval);
		kvd->ierr = res->ierr;
		kvd->serr = res->serr;
		return kvd;
	}
	/*-------KVStore::del()----------*/
	template<typename KeyType, typename ValType>
	std::shared_ptr<KVData<ValType>>  KVStore<KeyType,ValType>::del(KeyType const& key){
		string skey=toBoostString(key);
		std::shared_ptr<KVData<ValType>> kvd = std::make_shared<KVData<ValType>>();
		std::shared_ptr<KVData<string>> res = kh.del(skey);
		kvd->ierr = res->ierr;
		kvd->serr = res->serr;
		return kvd;
	}
	/*-------KVStore::clear()----------*/
	template<typename KeyType, typename ValType>
	bool KVStore<KeyType,ValType>::clear(){
		return kh.clear();
	}



/*
 ------------Common implementation--------------------
*/
	template<typename T>
	string toBoostString(T const &obj){
		stringstream ofs;
		boost::archive::text_oarchive oa(ofs);
		oa << obj;
		return ofs.str();
	}

	template<typename T>
	T toBoostObject(string sobj){
		T obj;
		stringstream ifs;
		ifs<<sobj;
		boost::archive::text_iarchive ia(ifs);
		ia >> obj;
		return obj;
	}

	// template<typename T>
	// T const & toBoostObject(string sobj){
	// 	T *obj = new(T);
	// 	stringstream ifs;
	// 	ifs<<sobj;
	// 	boost::archive::text_iarchive ia(ifs);
	// 	ia >> (*obj);
	// 	return *obj;
	// }
}
#endif
