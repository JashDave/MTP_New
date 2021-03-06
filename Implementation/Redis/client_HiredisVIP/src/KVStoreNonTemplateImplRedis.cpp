#include "KVStore.h"

namespace kvstore {


  // KVMgmt kvm;

  bool KVStoreClient::init(string ip, string port){
    rc = redisClusterConnect(string(ip+":"+port).c_str(), HIRCLUSTER_FLAG_NULL);
    if(rc == NULL || rc->err)
    {
      //printf("connect error : %s\n", rc == NULL ? "NULL" : rc->errstr);
      return false;
    }
    return true;
  }

  // class KVStoreClient{
  // public:
  //   CRedisClient rc;
  //   string tbname;
  //   ~KVStoreClient(){
  //     delete rc;
  //   }
  // };

  KVResultSet::KVResultSet(vector<string> r){
    count=r.size()/3;
    res=r;
  }

  int KVResultSet::size(){
    return count;
  }

	int KVResultSet::ierror(int idx){
    return stoi(res[idx*3]);
  }

  void KVRequest::bind(string connection){
    int colon = connection.find(":");
    string ip = connection.substr(0,colon);
    string port = connection.substr(colon+1);
    kvsclient = (void *) new KVStoreClient();
    if(!c_kvsclient->init(ip,port)){
      cerr<<"connection error"<<endl;
    }
  }
  KVRequest::~KVRequest(){
    delete(c_kvsclient);
    //For distroying connection object
  }

  // void printvec(vector<string> &v){
  //   for(string s:v){
  //     cout<<s<<endl;
  //   }
  // }

  string flattenVec(vector<string> in){
    string ret="";
    int sz = in.size();
    for(int i=0;i<sz-1;i++){
      ret+= (in[i]+" ");
    }
    ret+=in[sz-1];
    return ret;
  }

  KVResultSet KVRequest::execute(){
    int RC_SUCCESS=1;
    // cout<<"KVReq Execute"<<endl;
    vector<string> res;
    int pr=1,gr=1;

    if(vputk.size()!=0){
      int sz=vputk.size();
      for(int i=0;i<sz;i++){
        redisReply *reply = redisClusterCommand(c_kvsclient->rc, "mset %s %s", vputk[i].c_str(), vputv[i].c_str());
        if(reply == NULL)
        {
          // printf("reply is null[%s]\n", c_kvsclient->rc->errstr);
          //redisClusterFree(c_kvsclient->rc); //??
          pr=0;
        } else {
          freeReplyObject(reply);
        }
      }
    }

    vector<string> getres;
    if(vget.size()!=0){
        int sz=vget.size();
        for(int i=0;i<sz;i++){
          redisReply *reply = redisClusterCommand(c_kvsclient->rc, "get %s", vget[i].c_str());
          if(reply == NULL)
          {
            // printf("reply is null[%s]\n", c_kvsclient->rc->errstr);
            //redisClusterFree(c_kvsclient->rc); //??
            gr=0;
          }  else if(reply->str == NULL){
            gr=0;
            freeReplyObject(reply);
          } else {
            getres.push_back(string(reply->str));
            freeReplyObject(reply);
          }
        }
    }

    vector<int> delres(vdel.size());
    for(int i=0;i<vdel.size();i++){
        redisReply *reply = redisClusterCommand(c_kvsclient->rc, "del %s", vdel[i].c_str());
        if(reply == NULL)
        {
          // printf("reply is null[%s]\n", c_kvsclient->rc->errstr);
          //redisClusterFree(c_kvsclient->rc); //??
          delres[i]=0;
        } else {
          delres[i]=reply->integer;
          freeReplyObject(reply);
        }
    }

    int getidx=0;
    int delidx=0;
    for(int i=0;i<v.size();i++){
      switch(v[i][0]){
        case 'p' :
        if(pr == RC_SUCCESS){
          // cout<<"Put success"<<endl;
          res.push_back("0");
          res.push_back("");
          res.push_back("");
        } else {
          res.push_back("-1");
          res.push_back("connection error");
          res.push_back("");
        }
        break;

        case 'g' :
        if(gr == RC_SUCCESS){

          // cout<<"Get success :"<<getres[getidx]<<endl;
          if(getres[getidx].empty()){
            res.push_back("-1");
            res.push_back("Value not found");
            res.push_back("");
          }else {
            res.push_back("0");
            res.push_back("");
            res.push_back(getres[getidx]);
          }
        } else {
          res.push_back("-1");
          res.push_back("connection error");
          res.push_back("");
        }
        getidx++;
        break;

        case 'd' :
        if(delres[delidx] == RC_SUCCESS){
          //cout<<"Del success"<<endl;
          res.push_back("0");
          res.push_back("");
          res.push_back("");
        } else {
          //cout<<"Del unsuccess"<<endl;
          res.push_back("-1");
          res.push_back("connection error");
          res.push_back("");
        }
        delidx++;
        break;
      }
    }

    return KVResultSet(res);
  }

  void KVRequest::reset(){
    v.clear();
    vputk.clear();
    vputv.clear();
    vget.clear();
    vdel.clear();
  }

}
