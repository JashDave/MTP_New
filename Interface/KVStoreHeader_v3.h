// Why both implementation and declaration are in same file
// Refer : https://isocpp.org/wiki/faq/templates
//        http://stackoverflow.com/questions/3040480/c-template-function-compiles-in-header-but-not-implementation
// Note that only implementation of templates are here since their true implementation is defered till their use.

#ifndef __KVSTORE_H__
#define __KVSTORE_H__

#include "KVImpl_v3.h"

#include <stdio.h>
#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include <vector>
#include <thread>

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

#define KVDEBUG

#ifdef KVDEBUG
#include <assert.h>
#define kvassert(x) {assert(x);}
#else
#define kvassert(x) {}
#endif

using namespace std;
// using namespace boost;

namespace kvstore {
	// const int KV_SUCCESS = 0;

	/*
	Helper declarations
	*/
	template<typename T>
	string toBoostString(T const &obj);

	template<typename T>
	T toBoostObject(string& sobj);


	/* Class declaration */
	template<typename KeyType, typename ValType>
	class KVStore;
	// template<typename KeyType, typename ValType>
	// class AsyncKVStore;
	template<typename ValType>
	class KVData;

	/*
	Interface to Key-Value Store
	*/
	template<typename KeyType, typename ValType>
	class KVStore {
	private:
		KVImplHelper kh; //RR LB object 
		void *kvsclient;
	public:
		KVStore();
		~KVStore();
		inline bool bind(string connection,string tablename);

		inline KVData<ValType> get(KeyType const& key);
		inline KVData<ValType> put(KeyType const& key, ValType const& val);
		inline KVData<ValType> del(KeyType const& key);

		inline vector<KVData<ValType>> mget(vector<KeyType> const& key);
		inline vector<KVData<ValType>> mput(vector<KeyType> const& key, vector<ValType> const& val);
		inline vector<KVData<ValType>> mdel(vector<KeyType> const& key);

		inline bool clear();
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
	inline bool KVStore<KeyType,ValType>::bind(string connection,string tablename){
		return kh.bind(connection,tablename);
	}
	/*-------KVStore::get()----------*/
	template<typename KeyType, typename ValType>
	inline KVData<ValType>  KVStore<KeyType,ValType>::get(KeyType const& key){
		string skey=toBoostString(key);
		KVData<ValType> kvd = KVData<ValType>();
		KVData<string> res = kh.get(skey);
		kvd.ierr = res.ierr;
		kvd.serr = res.serr;
		if(kvd.ierr==0){
			kvd.value = toBoostObject<ValType>(res.value);
		}
		return kvd;
	}
	/*-------KVStore::put()----------*/
	template<typename KeyType, typename ValType>
	inline KVData<ValType>  KVStore<KeyType,ValType>::put(KeyType const& key,ValType const& val){
		string skey=toBoostString(key);
		string sval=toBoostString(val);
		KVData<ValType> kvd = KVData<ValType>();
		KVData<string> res = kh.put(skey,sval);
		kvd.ierr = res.ierr;
		kvd.serr = res.serr;
		return kvd;
	}
	/*-------KVStore::del()----------*/
	template<typename KeyType, typename ValType>
	inline KVData<ValType>  KVStore<KeyType,ValType>::del(KeyType const& key){
		string skey=toBoostString(key);
		KVData<ValType> kvd = KVData<ValType>();
		KVData<string> res = kh.del(skey);
		kvd.ierr = res.ierr;
		kvd.serr = res.serr;
		return kvd;
	}
	/*-------KVStore::mget()----------*/
	template<typename KeyType, typename ValType>
	inline vector<KVData<ValType>> KVStore<KeyType,ValType>::mget(vector<KeyType> const& key){
		int sz = key.size();
		vector<string> skey(sz);
		for(int i=0;i<sz;i++){
			skey[i] = toBoostString(key[i]);
		}
		vector<KVData<string>> res = kh.mget(skey);
		kvassert(res.size() == sz && "Incorrect KVImpl Implementation. Error : input and return size mismatch");
		vector<KVData<ValType>> ret(sz);
		for(int i=0;i<sz;i++){
			ret[i].ierr = res[i].ierr;
			ret[i].serr = res[i].serr;
			if(ret[i].ierr==0){
				ret[i].value = toBoostObject<ValType>(res[i].value);
			}
		}
		return ret;
	}
	/*-------KVStore::mput()----------*/
	template<typename KeyType, typename ValType>
	inline vector<KVData<ValType>> KVStore<KeyType,ValType>::mput(vector<KeyType> const& key, vector<ValType> const& val){
		int sz = key.size();
		vector<string> skey(sz);
		vector<string> sval(sz);
		for(int i=0;i<sz;i++){
			skey[i] = toBoostString(key[i]);
			sval[i] = toBoostString(val[i]);
		}
		vector<KVData<string>> res = kh.mput(skey,sval);
		kvassert(res.size() == sz && "Incorrect KVImpl Implementation. Error : input and return size mismatch");
		vector<KVData<ValType>> ret(sz);
		for(int i=0;i<sz;i++){
			ret[i].ierr = res[i].ierr;
			ret[i].serr = res[i].serr;
		}
		return ret;
	}
	/*-------KVStore::mdel()----------*/
	template<typename KeyType, typename ValType>
	inline vector<KVData<ValType>> KVStore<KeyType,ValType>::mdel(vector<KeyType> const& key){
		int sz = key.size();
		vector<string> skey(sz);
		for(int i=0;i<sz;i++){
			skey[i] = toBoostString(key[i]);
		}
		vector<KVData<string>> res = kh.mdel(skey);
		kvassert(res.size() == sz && "Incorrect KVImpl Implementation. Error : input and return size mismatch");
		vector<KVData<ValType>> ret(sz);
		for(int i=0;i<sz;i++){
			ret[i].ierr = res[i].ierr;
			ret[i].serr = res[i].serr;
		}
		return ret;
	}
	/*-------KVStore::clear()----------*/
	template<typename KeyType, typename ValType>
	inline bool KVStore<KeyType,ValType>::clear(){
		return kh.clear();
	}



	// template<typename KeyType, typename ValType>
	// class AsyncKVStore {
	// private:
	// 	AsyncKVImplHelper kh;
	// 	void *kvsclient;
	// public:
	// 	AsyncKVStore();
	// 	~AsyncKVStore();
	//
	// 	bool bind(string connection,string tablename);
	//
	// 	void async_get(KeyType const& key, void (*fn)(KVData<ValType>,void *), void *data);
	// 	void async_put(KeyType const& key, ValType const& val, void (*fn)(KVData<ValType>,void *), void *data);
	// 	void async_del(KeyType const& key, void (*fn)(KVData<ValType>,void *), void *data);
	//
	// 	void async_mget(vector<KeyType> const& key, void (*fn)(vector<KVData<ValType>>,void *), void *data);
	// 	void async_mput(vector<KeyType> const& key, vector<ValType> const& val, void (*fn)(vector<KVData<ValType>>,void *), void *data);
	// 	void async_mdel(vector<KeyType> const& key, void (*fn)(vector<KVData<ValType>>,void *), void *data);
	// };
	// /*-------AsyncKVStore::AsyncKVStore()----------*/
	// template<typename KeyType, typename ValType>
	// AsyncKVStore<KeyType,ValType>::AsyncKVStore(){
	// }
	// /*-------AsyncKVStore::~AsyncKVStore()----------*/
	// template<typename KeyType, typename ValType>
	// AsyncKVStore<KeyType,ValType>::~AsyncKVStore(){
	// }
	// /*---------AsyncKVStore::bind()--------*/
	// template<typename KeyType, typename ValType>
	// bool AsyncKVStore<KeyType,ValType>::bind(string connection,string tablename){
	// 	return kh.bind(connection,tablename);
	// }
	// /*-------AsyncKVStore::async_get()--------*/
	// template<typename KeyType, typename ValType>
	// void AsyncKVStore<KeyType,ValType>::async_get(KeyType const& key, void (*fn)(KVData<ValType>,void *),void *data){
	// 	string skey=toBoostString(key);
	// 	auto lambda_fn = [](KVData<string> res,void *pdata, void *vfn)->void{
	// 		KVData<ValType> kvd = KVData<ValType>();
	// 		kvd.ierr = res.ierr;
	// 		kvd.serr = res.serr;
	// 		if(kvd.ierr==0){
	// 			kvd.value = toBoostObject<ValType>(res.value);
	// 		}
	// 		void (*fn)(KVData<ValType>,void*) = (void (*)(KVData<ValType>,void*))vfn;
	// 		fn(kvd,pdata);
	// 	};
	// 	kh.async_get(skey,lambda_fn,data,(void *)fn);
	// }
	// /*-------AsyncKVStore::async_put()--------*/
	// template<typename KeyType, typename ValType>
	// void AsyncKVStore<KeyType,ValType>::async_put(KeyType const& key,ValType const& val, void (*fn)(KVData<ValType>,void *),void *data){
	// 	string skey=toBoostString(key);
	// 	string sval=toBoostString(val);
	// 	auto lambda_fn = [](KVData<string> res,void *pdata, void *vfn)->void{
	// 		KVData<ValType> kvd = KVData<ValType>();
	// 		kvd.ierr = res.ierr;
	// 		kvd.serr = res.serr;
	// 		void (*fn)(KVData<ValType>,void*) = (void (*)(KVData<ValType>,void*))vfn;
	// 		fn(kvd,pdata);
	// 	};
	// 	kh.async_put(skey,sval,lambda_fn,data,(void *)fn);
	// }
	// /*-------AsyncKVStore::async_del()--------*/
	// template<typename KeyType, typename ValType>
	// void AsyncKVStore<KeyType,ValType>::async_del(KeyType const& key, void (*fn)(KVData<ValType>,void *),void *data){
	// 	string skey=toBoostString(key);
	// 	auto lambda_fn = [](KVData<string> res,void *pdata, void *vfn)->void{
	// 		KVData<ValType> kvd = KVData<ValType>();
	// 		kvd.ierr = res.ierr;
	// 		kvd.serr = res.serr;
	// 		void (*fn)(KVData<ValType>,void*) = (void (*)(KVData<ValType>,void*))vfn;
	// 		fn(kvd,pdata);
	// 	};
	// 	kh.async_del(skey,lambda_fn,data,(void*)fn);
	// }
	// /*-------AsyncKVStore::async_mget()--------*/
	// template<typename KeyType, typename ValType>
	// void AsyncKVStore<KeyType,ValType>::async_mget(vector<KeyType> const& key, void (*fn)(vector<KVData<ValType>>,void *),void *data){
	// 	auto lambda_fn = [](vector<KVData<string>> res,void *pdata, void *vfn)->void{		vector<KVData<ValType>> ret(sz);
	// 		int sz = res.size();
	// 		vector<KVData<ValType>> ret(sz);
	// 		for(int i=0;i<sz;i++){
	// 			ret[i].ierr = res[i].ierr;
	// 			ret[i].serr = res[i].serr;
	// 			if(ret[i].ierr==0){
	// 				ret[i].value = toBoostObject<ValType>(res[i].value);
	// 			}
	// 		}
	// 		void (*fn)(vector<KVData<ValType>>,void*) = (void (*)(vector<KVData<ValType>>,void*))vfn;
	// 		fn(ret,pdata);
	// 	};
	// 	int sz = key.size();
	// 	vector<string> skey(sz);
	// 	for(int i=0;i<sz;i++){
	// 		skey[i] = toBoostString(key[i]);
	// 	}
	// 	kh.async_get(skey,lambda_fn,data,(void *)fn);
	// }
	// /*-------AsyncKVStore::async_mput()--------*/
	// template<typename KeyType, typename ValType>
	// void AsyncKVStore<KeyType,ValType>::async_mput(vector<KeyType> const& key,vector<ValType> const& val, void (*fn)(vector<KVData<ValType>>,void *),void *data){
	// 	auto lambda_fn = [](vector<KVData<string>> res,void *pdata, void *vfn)->void{
	// 		int sz = res.size();
	// 		vector<KVData<ValType>> ret(sz);
	// 		for(int i=0;i<sz;i++){
	// 			ret[i].ierr = res[i].ierr;
	// 			ret[i].serr = res[i].serr;
	// 		}
	// 		void (*fn)(vector<KVData<ValType>>,void*) = (void (*)(vector<KVData<ValType>>,void*))vfn;
	// 		fn(ret,pdata);
	// 	};
	// 	int sz = key.size();
	// 	vector<string> skey(sz);
	// 	vector<string> sval(sz);
	// 	for(int i=0;i<sz;i++){
	// 		skey[i] = toBoostString(key[i]);
	// 		sval[i] = toBoostString(val[i]);
	// 	}
	// 	kh.async_put(skey,sval,lambda_fn,data,(void *)fn);
	// }
	// /*-------AsyncKVStore::async_mdel()--------*/
	// template<typename KeyType, typename ValType>
	// void AsyncKVStore<KeyType,ValType>::async_mdel(vector<KeyType> const& key, void (*fn)(vector<KVData<ValType>>,void *),void *data){
	// 	auto lambda_fn = [](vector<KVData<string>> res,void *pdata, void *vfn)->void{
	// 		int sz = res.size();
	// 		vector<KVData<ValType>> ret(sz);
	// 		for(int i=0;i<sz;i++){
	// 			ret[i].ierr = res[i].ierr;
	// 			ret[i].serr = res[i].serr;
	// 		}
	// 		void (*fn)(vector<KVData<ValType>>,void*) = (void (*)(vector<KVData<ValType>>,void*))vfn;
	// 		fn(ret,pdata);
	// 	};
	// 	int sz = key.size();
	// 	vector<string> skey(sz);
	// 	for(int i=0;i<sz;i++){
	// 		skey[i] = toBoostString(key[i]);
	// 	}
	// 	kh.async_del(skey,lambda_fn,data,(void*)fn);
	// }



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
	T toBoostObject(string &sobj){
		T obj;
		stringstream ifs;
		ifs<<sobj;
		boost::archive::text_iarchive ia(ifs);
		ia >> obj;
		return obj;
	}
}
#endif
