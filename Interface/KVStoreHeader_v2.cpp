// Why both implementation and declaration are in same file
// Refer : https://isocpp.org/wiki/faq/templates
//        http://stackoverflow.com/questions/3040480/c-template-function-compiles-in-header-but-not-implementation
// Note that only implementation of templates are here since their true implementation is defered till their use.

#include "KVStoreHeader_v2.h"

namespace kvstore {
/*
	KVResultSet non template implementation
*/
		KVResultSet::KVResultSet(vector<KVData<string>> r, vector<string> opr_type){
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
	// KVRequest::KVRequest(KVRequest& kr){
	// 	dataholder=kr.dataholder;
	// 	kh=kr.kh;
	// 	operation_type=kr.operation_type;
	// 	put_tablename=kr.put_tablename;
	// 	put_key=kr.put_key;
	// 	put_value=kr.put_value;
	// 	get_tablename=kr.get_tablename;
	// 	get_key=kr.get_key;
	// 	del_tablename=kr.del_tablename;
	// 	del_key=kr.del_key;
	// }
	KVRequest::~KVRequest(){}		//For distroying connection object

	bool KVRequest::bind(string connection){
		return kh.bind(connection,"randomtable_123321011422");
	}

	KVResultSet KVRequest::execute(){
		vector<KVData<string>> mput_res;
		vector<KVData<string>> mget_res;
		vector<KVData<string>> mdel_res;
		int mp = kh.mput(put_key, put_value, put_tablename, mput_res);
		int mg = kh.mget(get_key, get_tablename, mget_res);
		int md = kh.mdel(del_key, del_tablename, mdel_res);

		/* Combine the results in given order. */
		vector<KVData<string>> combined_res;
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
		KVResultSet rs = KVResultSet(combined_res,operation_type);
		return rs;
	}

/*---------------Helper for async_execute --------------------*/
	class async_execute_data{
	public:
		void *data;
		vector<string> operation_type;
		async_execute_data(void *d,vector<string> &ot){
			data=d;
			operation_type=ot;
		}
	};

  void fun(KVData<string> kd, void *data, void *vfn){
		static vector<KVData<string>> combined_res;
		combined_res.push_back(kd);
		if(data != NULL){
			async_execute_data *ad = (async_execute_data*)data;
			KVResultSet rs = KVResultSet(combined_res,ad->operation_type);
			if(vfn!=NULL){
				void (*fn)(KVResultSet, void *) = (void (*)(KVResultSet, void *))vfn;
				fn(rs,ad->data);
			}
			delete(ad);
			combined_res.clear();
		}
	}

	void KVRequest::async_execute(void (*fn)(KVResultSet, void *), void *data){
		int sz=operation_type.size()-1;  /*NOTE -1 for last operation*/
		int gi=0,pi=0,di=0;
		for(int i=0;i<sz;i++){
			if(operation_type[i] == OPR_TYPE_GET){
				kh.async_get(get_key[gi], get_tablename[gi], fun, NULL, NULL);
				gi++;
			} else if(operation_type[i] == OPR_TYPE_PUT){
				kh.async_put(put_key[pi], put_value[pi], put_tablename[pi], fun, NULL, NULL);
				pi++;
			} else {
				kh.async_del(del_key[di], del_tablename[di], fun, NULL, NULL);
				di++;
			}
		}
		/*Last operation*/
		struct async_execute_data *ad = new async_execute_data(data,operation_type);
		if(operation_type[sz] == OPR_TYPE_GET){
			kh.async_get(get_key[gi], get_tablename[gi], fun, (void*)ad, (void*)fn);
			gi++;
		} else if(operation_type[sz] == OPR_TYPE_PUT){
			kh.async_put(put_key[pi], put_value[pi], put_tablename[pi], fun, (void*)ad, (void*)fn);
			pi++;
		} else {
			kh.async_del(del_key[di], del_tablename[di], fun, (void*)ad, (void*)fn);
			di++;
		}
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
