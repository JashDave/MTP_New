// Why both implementation and declaration are in same file
// Refer : https://isocpp.org/wiki/faq/templates
//        http://stackoverflow.com/questions/3040480/c-template-function-compiles-in-header-but-not-implementation
// Note that only implementation of templates are here since their true implementation is defered till their use.

#ifndef __KVSTORE_H__
#define __KVSTORE_H__

#include "KVStoreHeader_v2.h"
#include "KVImpl.h"


namespace kvstore {
/*
	KVRequest non template implementation
*/
	KVRequest::KVRequest(){}
	KVRequest::~KVRequest(){}		//For distroying connection object

	bool KVRequest::bind(string connection){
		return kh.bind(connection,"randomtable_123321011422");
	}

	shared_ptr<KVResultSet> KVRequest::execute(){
		vector<shared_ptr<KVData<string>>> mput_res;
		vector<shared_ptr<KVData<string>>> mget_res;
		vector<shared_ptr<KVData<string>>> mdel_res;
		int mp = mput(put_key, put_value, put_tablename, mput_res);
		int mg = mget(get_key, get_tablename, mget_res);
		int md = mdel(del_key, del_tablename, mdel_res);

		/* Combine the results in given order. */
		vector<shared_ptr<KVData<string>>> combined_res;
		int sz=operation_type.size();
		int pi=0,gi=0,di=0;
		for(int i=0;i<sz;i++){
			switch(operation_type){
				case OPR_TYPE_PUT:
					combined_res.push_back(mput_res[pi]);
					pi++;
					break;
				case OPR_TYPE_GET:
					combined_res.push_back(mget_res[gi]);
					gi++;
					break;
				case OPR_TYPE_DEL:
					combined_res.push_back(mdel_res[di]);
					di++;
					break;
				default :
					cerr<<"Invalid operation type in "<<__FILE__<<", "<<__FUNCTION__<<endl;
			}
		}
		shared_ptr<KVResultSet> rs = make_shared<KVResultSet>(combined_res,operation_type);
		return rs;
	}
	void KVRequest::reset(){
		operation_type.clear();
		put_tablename.clear();
		put_key.clear();
		put_value.clear();
		get_tablename.clear();
		get_key.clear();
		del_tablename.clear();
		del_key.clear();
	}
#endif
