#include "CondCore/DBCommon/interface/Ref.h"
#include "CondCore/DBCommon/interface/DBSession.h"
#include "CondCore/DBCommon/interface/Exception.h"
#include "CondCore/DBCommon/interface/ServiceLoader.h"
#include "CondCore/DBCommon/interface/ConnectMode.h"
#include "CondCore/DBCommon/interface/MessageLevel.h"
#include "testCondObj.h"
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
    session->commit();
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
