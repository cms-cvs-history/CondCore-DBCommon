#include "CondCore/DBCommon/interface/TypedRef.h"
#include "CondCore/DBCommon/interface/DBSession.h"
#include "CondCore/DBCommon/interface/Exception.h"
#include "CondCore/DBCommon/interface/SessionConfiguration.h"
#include "CondCore/DBCommon/interface/ConnectMode.h"
#include "CondCore/DBCommon/interface/MessageLevel.h"
#include "testCondObj.h"
#include <exception>
#include <string>
#include <iostream>
#include <cstdlib>
int main(){
  cond::DBSession* session=new cond::DBSession;
  session->sessionConfiguration().setMessageLevel(cond::Error);
  session->sessionConfiguration().setAuthenticationMethod(cond::XML);
  try{
    session->open();
    //::putenv(const_cast<char*>("CORAL_AUTH_PATH=/afs/cern.ch/user/x/xiezhen/localhome/work/CMSSW/dev130/CMSSW_1_2_0/src/CondCore/DBCommon/test/auth"));
    cond::PoolStorageManager pooldb("sqlite_file:test.db","file:mycatalog.xml",session);
    pooldb.connect();
    testCondObj* myobj=new testCondObj;
    myobj->data.insert(std::make_pair(1,"strangestring1"));
    myobj->data.insert(std::make_pair(100,"strangestring2"));
    pooldb.startTransaction(false);
    cond::Ref<testCondObj> myref(pooldb,myobj);
    myref.markWrite("mycontainer");
    std::string token=myref.token();
    std::cout<<"token "<<token<<std::endl;
    pooldb.commit();
    pooldb.startTransaction(true);
    cond::Ref<testCondObj> myinstance(pooldb,token);
    std::cout<<"mem pointer "<<myinstance.ptr()<<std::endl;
    std::cout<<"read back 1   "<<myinstance->data[1]<<std::endl;
    std::cout<<"read back 100 "<<myinstance->data[100]<<std::endl;
    std::cout<<"committed"<<std::endl;
    std::cout<<myobj<<std::endl;
    std::cout<<"hemm"<<std::endl;
    //std::cout<<myobj->data[1]<<std::endl;
    myinstance->data[1]="updatedstring";
    myinstance->data.insert(std::make_pair(1000,"newstring"));
    pooldb.commit();
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
}
