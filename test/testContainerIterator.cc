#include "CondCore/DBCommon/interface/Ref.h"
#include "CondCore/DBCommon/interface/DBSession.h"
#include "CondCore/DBCommon/interface/Exception.h"
#include "CondCore/DBCommon/interface/ServiceLoader.h"
#include "CondCore/DBCommon/interface/ConnectMode.h"
#include "CondCore/DBCommon/interface/MessageLevel.h"
#include "CondCore/DBCommon/interface/ContainerIterator.h"
#include "testCondObj.h"
#include <string>
#include <iostream>
int main(){
  cond::ServiceLoader* loader=new cond::ServiceLoader;
  loader->loadMessageService(cond::Error);
  cond::DBSession* session=new cond::DBSession(std::string("sqlite_file:test2.db"));
  try{
    session->connect(cond::ReadWriteCreate);
    session->startUpdateTransaction();
    for(int i=0; i<10; ++i){
      testCondObj* myobj=new testCondObj;
      myobj->data.insert(std::make_pair(1+i,"strangestring1"));
      myobj->data.insert(std::make_pair(100+i,"strangestring2"));
      cond::Ref<testCondObj> myref(*session,myobj);
      myref.markWrite("mycontainer");
      std::string token=myref.token();
      std::cout<<"token "<<i<<" "<<token<<std::endl;
    }
    session->commit();
    session->startReadOnlyTransaction();
    std::vector<std::string> containers=session->containers();
    for(std::vector<std::string>::iterator it=containers.begin();
	it!=containers.end(); ++it){
      std::cout<<*it<<std::endl;
    }
    session->commit();
    session->startUpdateTransaction();
    cond::ContainerIterator<testCondObj> col(*session, "mycontainer");
    std::cout<<"collection name "<<col.name()<<std::endl;
    while(col.next()){
      col.dataRef().markDelete();
    }
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
