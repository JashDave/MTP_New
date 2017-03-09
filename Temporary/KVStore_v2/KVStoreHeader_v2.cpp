// Why both implementation and declaration are in same file
// Refer : https://isocpp.org/wiki/faq/templates
//        http://stackoverflow.com/questions/3040480/c-template-function-compiles-in-header-but-not-implementation
// Note that only implementation of templates are here since their true implementation is defered till their use.

#include "KVStoreHeader_v2.h"

namespace kvstore {
/*
	KVResultSet non template implementation
*/
		KVResultSet::KVResultSet(vector<std::shared_ptr<KVData<string>>> r, vector<string> opr_type){
			res=r;
			operation_type=opr_type;
		}
		/* Return the size of ResultSet */
		int KVResultSet::size(){
			return res.size();
		}
		/* Returns the operation type */
		string KVResultSet::oprType(int idx){
			if(idx<0 || idx>=size()){
      if(size()==0)
	      return "Empty KVResultSet due to empty request queue, please ensure that your request queue is not empty.";
      else
	      return "KVResultSet index out of bound. Valid range is 0 to "+to_string(size()-1)+" found "+to_string(idx);
			}
			return operation_type[idx];
		}


/*
	KVRequest non template implementation
*/
	KVRequest::KVRequest(){}
	KVRequest::~KVRequest(){}		//For distroying connection object

	bool KVRequest::bind(string connection){
		return kh.bind(connection,"randomtable_123321011422");
	}

	std::shared_ptr<KVResultSet> KVRequest::execute(){
		vector<std::shared_ptr<KVData<string>>> mput_res;
		vector<std::shared_ptr<KVData<string>>> mget_res;
		vector<std::shared_ptr<KVData<string>>> mdel_res;
		int mp = kh.mput(put_key, put_value, put_tablename, mput_res);
		int mg = kh.mget(get_key, get_tablename, mget_res);
		int md = kh.mdel(del_key, del_tablename, mdel_res);

		/* Combine the results in given order. */
		vector<std::shared_ptr<KVData<string>>> combined_res;
		int sz=operation_type.size();
		int pi=0,gi=0,di=0;
		for(int i=0;i<sz;i++){
			if(operation_type[i] == OPR_TYPE_PUT){
				combined_res.push_back(mput_res[pi]);
				pi++;
			} else if(operation_type[i] == OPR_TYPE_GET){
				combined_res.push_back(mget_res[gi]);
				gi++;
			} else if(operation_type[i] == OPR_TYPE_DEL){
				combined_res.push_back(mdel_res[di]);
				di++;
			} else {
				cerr<<"Invalid operation type in "<<__FILE__<<", "<<__FUNCTION__<<endl;
			}
		}
		std::shared_ptr<KVResultSet> rs = std::make_shared<KVResultSet>(combined_res,operation_type);
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
}
