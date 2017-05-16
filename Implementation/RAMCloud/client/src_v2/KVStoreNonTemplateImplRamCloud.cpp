#include "KVStore.h"

namespace kvstore {

  TableManager tm;


 bool getTableId(string tablename,RamCloud *cluster,string opr, vector<uint64_t> &vt, vector<string> &v){
   uint64_t tableId=-1;
   tm.mtx.lock();
   if(tm.um.find(tablename) == tm.um.end()){
     try{
       if(opr=="put"){
        tableId = cluster->createTable(tablename.c_str());
        tm.um[tablename] = tableId;
       } else {
        tableId = cluster->getTableId(tablename.c_str());
      }
     } catch(...){//TableDoesntExistException
       v.push_back(opr+":error TableDoesntExistException OR Connection Error");
       tm.mtx.unlock();
       return false;
     }
   }
   vt.push_back(tm.um[tablename]);
   tm.mtx.unlock();
   return true;
 }



  KVResultSet::KVResultSet(vector<string> r){
    count=r.size()/3;
    res=r;
  }

  int KVResultSet::size(){
    return count;
  }

  void KVRequest::bind(string connection){
    kvsclient = (void *) new KVSClientData();
    c_kvsclient->cluster = new RamCloud(connection.c_str(),"test_cluster");
  }
  KVRequest::~KVRequest(){
    delete c_kvsclient->cluster;
    delete c_kvsclient;
  }


  KVResultSet KVRequest::execute(){
    // c_kvsclient->send(v);
    // vector<string> rcv=c_kvsclient->receive();
    //cout<<"Received size"<<rcv.size()<<"  "<<rcv[0]<<endl;
    vector<string> res;
    //PUT
    int sz=vputt.size();
    MultiWriteObject *mwo[sz];
    for(int i=0;i<sz;i++) {
      mwo[i] = new MultiWriteObject(vputt[i], vputk[i].c_str(), vputk[i].size(), vputv[i].c_str(), vputv[i].size());
    }
    c_kvsclient->cluster->multiWrite(mwo, sz);

    //GET
    sz=vgett.size();
    MultiReadObject *mro[sz];
    Tub<ObjectBuffer> retval[sz];
    for(int i=0;i<sz;i++) {
      mro[i] = new MultiReadObject(vgett[i], vget[i].c_str(), vget[i].size(), &retval[i]);
    }
    c_kvsclient->cluster->multiRead(mro,sz);

    //DEL
    sz=vdelt.size();
    MultiRemoveObject *mdo[sz];
    for(int i=0;i<sz;i++) {
      mdo[i] = new MultiRemoveObject(vdelt[i], vdel[i].c_str(), vdel[i].size());
    }
    c_kvsclient->cluster->multiRemove(mdo,sz);

    int getidx=0;
    for(int i=0;i<v.size();i++){
      switch(v[i][0]){
        case 'p' :
        if(v[i].size()==3){
          //cout<<"Put success"<<endl;
          res.push_back("0");
          res.push_back("");
          res.push_back("");
        } else {
          res.push_back("-1");
          res.push_back("TableDoesntExistException");
          res.push_back("");
        }
        break;

        case 'g' :
        if(v[i].size()==3){
          if(retval[getidx].get()==NULL){
            res.push_back("-1");
            res.push_back("Enrty doesn't exists");
            res.push_back("");
        } else {
        res.push_back("0");
        res.push_back("");
        string str=((char *)retval[getidx].get()->getValue());
        res.push_back(str);
        }
          //cout<<"Got:"<<str<<endl;
          getidx++;
        } else {
          res.push_back("-1");
          res.push_back("TableDoesntExistException");
          res.push_back("");
        }
        break;

        case 'd' :
        if(v[i].size()==3){
          //cout<<"Del success"<<endl;
          res.push_back("0");
          res.push_back("");
          res.push_back("");
        } else {
          //cout<<"Del unsuccess"<<endl;
          res.push_back("-1");
          res.push_back("TableDoesntExistException");
          res.push_back("");
        }
        break;
      }
    }

    //delete
    sz=vputt.size();
    for(int i=0;i<sz;i++) {
      delete mwo[i];
    }

    sz=vgett.size();
    for(int i=0;i<sz;i++) {
      delete mro[i];
    }

    //DEL
    sz=vdelt.size();
    for(int i=0;i<sz;i++) {
      delete mdo[i];
    }

    return KVResultSet(res);
  }

  void KVRequest::reset(){
    v.clear();
    vputt.clear();
    vputk.clear();
    vputv.clear();
    vgett.clear();
    vget.clear();
    vdelt.clear();
    vdel.clear();
  }

}
