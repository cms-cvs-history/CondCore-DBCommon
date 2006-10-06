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
  //loader->loadMessageService(cond::Info);
  loader->loadMessageService(cond::Error);
  cond::DBSession* session=new cond::DBSession(std::string("sqlite_file:test.db"));
  try{
    session->connect(cond::ReadWriteCreate);
    testCondObj* myobj=new testCondObj;
    myobj->data.insert(std::make_pair(1,"strangestring1"));
    myobj->data.insert(std::make_pair(100,"strangestring2"));
    cond::Ref<testCondObj> myref(*session,myobj);
    session->startUpdateTransaction();
    myref.markWrite("mycontainer");
    std::string token=myref.token();
    std::cout<<"token "<<token<<std::endl;
    session->commit();
    session->startReadOnlyTransaction();
    cond::Ref<testCondObj> myinstance(*session,token);
    std::cout<<"mem pointer "<<myinstance.ptr()<<std::endl;
    std::cout<<"read back 1   "<<myinstance->data[1]<<std::endl;
    std::cout<<"read back 100 "<<myinstance->data[100]<<std::endl;
    session->commit();
    myinstance->data[1]="updatedstring";
    myinstance->data.insert(std::make_pair(1000,"newstring"));
    session->startUpdateTransaction();
    myinstance.markUpdate();
    token=myref.token();
    std::cout<<"same token "<<token<<std::endl;
    session->commit();
    session->startReadOnlyTransaction();
    cond::Ref<testCondObj> myrefback(*session,token);
    session->commit();
    std::cout<<"pointer "<<myrefback.ptr()<<std::endl;
    std::cout<<"read back 1   "<<myrefback->data[1]<<std::endl;
    std::cout<<"read back 100 "<<myrefback->data[100]<<std::endl;
    std::cout<<"read back 1000 "<<myrefback->data[1000]<<std::endl;
    session->startUpdateTransaction();
    myrefback.markDelete();
    session->commit();
    session->startReadOnlyTransaction();
    cond::Ref<testCondObj> result(*session,token);
    session->commit();
    if( !result.ptr() ){
      std::cout<<"object deleted, null pointer retrieved"<<std::endl;
    }
    session->disconnect();
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
