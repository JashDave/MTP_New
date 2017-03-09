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
#include <memory>

//For returning hetrogeneous array as return of execute()
//#include <tuple> //Not used

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/hash_map.hpp>
#include <boost/serialization/hash_set.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/slist.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/bitset.hpp>
#include <boost/serialization/string.hpp>
#include <boost/shared_ptr.hpp>


using namespace std;
using namespace boost;

namespace kvstore {

#define OPR_TYPE_GET "get"
#define OPR_TYPE_PUT "put"
#define OPR_TYPE_DEL "del"

/* Class declaration */
	template<typename KeyType, typename ValType>
	class KVStore;
	template<typename ValType>
	class KVData;
	class KVResultSet;
	class KVRequest;


/*
	KVResultSet holds results of KVRequest.execute();
*/
	class KVResultSet {
	private:
		// int count;
		vector<shared_ptr<KVData<string>>> res;
		vector<string> operation_type;
	public:
		KVResultSet(vector<shared_ptr<KVData<string>>> r, vector<string> opr_type);
		int size();
		string oprType(int idx);

		template<typename ValType>
		KVData<ValType> get(int idx);
	};


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

		shared_ptr<KVResultSet> execute();
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
		shared_ptr<KVData<ValType>> get(KeyType const& key);
		shared_ptr<KVData<ValType>> put(KeyType const& key,ValType const& val);
		shared_ptr<KVData<ValType>> del(KeyType const& key);
		bool clear();
	};
	/*-------KVStore::KVStore()----------*/
	KVStore<KeyType,ValType>::KVStore(){
	}
	/*-------KVStore::~KVStore()----------*/
	KVStore<KeyType,ValType>::~KVStore(){
	}
	/*---------KVStore::bind()--------*/
	template<typename KeyType, typename ValType>
	bool KVStore<KeyType,ValType>::bind(string connection,string tablename){
		return kh.bind(connection,tablename);
	}
	/*-------KVStore::get()----------*/
  template<typename KeyType, typename ValType>
  shared_ptr<KVData<ValType>>  KVStore<KeyType,ValType>::get(KeyType const& key){
    string skey=toBoostString(key);
    string sval;
    shared_ptr<KVData<ValType>> kvd = make_shared<KVData<ValType>>;
		shared_ptr<KVData<string>> res = kh.get(skey);
		kvd->ierr = res.ierr;
		kvd->serr = res.serr;
		if(kvd->ierr==0){
			kvd->value = toBoostObject<ValType>(res.value);
		}
		return kvd;
	}
	/*-------KVStore::put()----------*/
	template<typename KeyType, typename ValType>
	shared_ptr<KVData<ValType>>  KVStore<KeyType,ValType>::put(KeyType const& key,ValType const& val){
		string skey=toBoostString(key);
		string sval=toBoostString(val);
		shared_ptr<KVData<ValType>> kvd = make_shared<KVData<ValType>>;
		shared_ptr<KVData<string>> res = kh.put(skey,sval);
		kvd->ierr = res.ierr;
		kvd->serr = res.serr;
		return kvd;
	}
	/*-------KVStore::del()----------*/
	template<typename KeyType, typename ValType>
	shared_ptr<KVData<ValType>>  KVStore<KeyType,ValType>::del(KeyType const& key){
		string skey=toBoostString(key);
		shared_ptr<KVData<ValType>> kvd = make_shared<KVData<ValType>>;
		shared_ptr<KVData<string>> res = kh.del(skey);
		kvd->ierr = res.ierr;
		kvd->serr = res.serr;
		return kvd;
	}
	/*-------KVStore::clear()----------*/
	template<typename KeyType, typename ValType>
	shared_ptr<KVData<ValType>>  KVStore<KeyType,ValType>::clear(){
		return kh.clear();
	}


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
 Helper declarations
*/
	template<typename T>
	string toBoostString(T const &obj);

	template<typename T>
	T toBoostObject(string sobj);

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
