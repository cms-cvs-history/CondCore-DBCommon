#include "CondCore/DBCommon/interface/Ref.h"
#include "CondCore/DBCommon/interface/DBSession.h"
#include "CondCore/DBCommon/interface/PoolStorageManager.h"
#include "CondCore/DBCommon/interface/Exception.h"
#include "CondCore/DBCommon/interface/ConnectMode.h"
#include "CondCore/DBCommon/interface/MessageLevel.h"
#include "CondCore/DBCommon/interface/ContainerIterator.h"
#include "testCondObj.h"
#include <string>
#include <iostream>
int main(){
  cond::DBSession* session=new cond::DBSession(std::string("sqlite_file:test2.db"));
  try{
    cond::PoolStorageManager& pooldb=session->poolStorageManager("file:mycatalog.xml");
    session->open(true);
    pooldb.connect(cond::ReadWriteCreate);
    pooldb.startTransaction(false);
    for(int i=0; i<10; ++i){
      testCondObj* myobj=new testCondObj;
      myobj->data.insert(std::make_pair(1+i,"strangestring1"));
      myobj->data.insert(std::make_pair(100+i,"strangestring2"));
      cond::Ref<testCondObj> myref(pooldb,myobj);
      myref.markWrite("mycontainer");
      std::string token=myref.token();
      std::cout<<"token "<<i<<" "<<token<<std::endl;
    }
    pooldb.commit();
    pooldb.startTransaction(true);
    std::vector<std::string> containers=pooldb.containers();
    for(std::vector<std::string>::iterator it=containers.begin();
	it!=containers.end(); ++it){
      std::cout<<*it<<std::endl;
    }
    pooldb.commit();
    pooldb.startTransaction(false);
    cond::ContainerIterator<testCondObj> col(pooldb, "mycontainer");
    std::cout<<"collection name "<<col.name()<<std::endl;
    while(col.next()){
      col.dataRef().markDelete();
    }
    pooldb.commit();
    pooldb.disconnect();
    session->close();    
  }catch(cond::Exception& er){
    std::cout<<er.what()<<std::endl;
  }catch(std::exception& er){
    std::cout<<er.what()<<std::endl;
  }catch(...){
    std::cout<<"Funny error"<<std::endl;
  }
  delete session;
}
