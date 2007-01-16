#include "CondCore/DBCommon/interface/Ref.h"
#include "CondCore/DBCommon/interface/DBSession.h"
#include "CondCore/DBCommon/interface/SessionConfiguration.h"
#include "CondCore/DBCommon/interface/PoolStorageManager.h"
#include "CondCore/DBCommon/interface/Exception.h"
#include "CondCore/DBCommon/interface/ConnectMode.h"
#include "CondCore/DBCommon/interface/MessageLevel.h"
#include "CondCore/DBCommon/interface/ContainerIterator.h"
#include "testCondObj.h"
#include <string>
#include <iostream>
int main(){
  cond::DBSession* session=new cond::DBSession(true);
  session->sessionConfiguration().setMessageLevel(cond::Error);
  session->sessionConfiguration().setAuthenticationMethod(cond::XML);
  try{
    cond::PoolStorageManager pooldb("sqlite_file:test2.db","file:mycatalog.xml",session);
    session->open();
    pooldb.connect();
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
