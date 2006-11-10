#include "CondCore/DBCommon/interface/Ref.h"
#include "CondCore/DBCommon/interface/DBSession.h"
#include "CondCore/DBCommon/interface/Exception.h"
#include "CondCore/DBCommon/interface/ServiceLoader.h"
#include "CondCore/DBCommon/interface/ConnectMode.h"
#include "CondCore/DBCommon/interface/MessageLevel.h"
#include "testCondObj.h"
#include <exception>
#include <string>
#include <iostream>
int main(){
  cond::ServiceLoader* loader=new cond::ServiceLoader;
  loader->usePOOLContext();
  loader->loadMessageService(cond::Info);
  //loader->loadMessageService(cond::Error);
  cond::DBSession* session=new cond::DBSession(std::string("sqlite_file:test.db"));
  try{
    cond::PoolStorageManager& pooldb=session->poolStorageManager("file:mycatalog.xml");
    session->open(true);
    pooldb.connect(cond::ReadWriteCreate);
    testCondObj* myobj=new testCondObj;
    myobj->data.insert(std::make_pair(1,"strangestring1"));
    myobj->data.insert(std::make_pair(100,"strangestring2"));
    cond::Ref<testCondObj> myref(pooldb,myobj);
    pooldb.startTransaction(false);
    myref.markWrite("mycontainer");
    std::string token=myref.token();
    std::cout<<"token "<<token<<std::endl;
    pooldb.commit();
    pooldb.startTransaction(true);
    cond::Ref<testCondObj> myinstance(pooldb,token);
    std::cout<<"mem pointer "<<myinstance.ptr()<<std::endl;
    std::cout<<"read back 1   "<<myinstance->data[1]<<std::endl;
    std::cout<<"read back 100 "<<myinstance->data[100]<<std::endl;
    pooldb.commit();
    myinstance->data[1]="updatedstring";
    myinstance->data.insert(std::make_pair(1000,"newstring"));
    pooldb.startTransaction(false);
    myinstance.markUpdate();
    token=myref.token();
    std::cout<<"same token "<<token<<std::endl;
    pooldb.commit();
    pooldb.startTransaction(true);
    cond::Ref<testCondObj> myrefback(pooldb,token);
    pooldb.commit();
    std::cout<<"ref belongs to container "<<myrefback.containerName()<<std::endl;
    std::cout<<"pointer "<<myrefback.ptr()<<std::endl;
    std::cout<<"read back 1   "<<myrefback->data[1]<<std::endl;
    std::cout<<"read back 100 "<<myrefback->data[100]<<std::endl;
    std::cout<<"read back 1000 "<<myrefback->data[1000]<<std::endl;
    pooldb.startTransaction(false);
    myrefback.markDelete();
    pooldb.commit();
    pooldb.startTransaction(true);
    cond::Ref<testCondObj> result(pooldb,token);
    pooldb.commit();
    if( !result.ptr() ){
      std::cout<<"object deleted, null pointer retrieved"<<std::endl;
    }
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
  delete loader;
}
