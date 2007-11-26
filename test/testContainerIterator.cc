#include "CondCore/DBCommon/interface/ConnectionHandler.h"
#include "CondCore/DBCommon/interface/DBSession.h"
#include "CondCore/DBCommon/interface/SessionConfiguration.h"
#include "CondCore/DBCommon/interface/PoolContainerManager.h"
#include "CondCore/DBCommon/interface/Exception.h"
#include "CondCore/DBCommon/interface/ConnectMode.h"
#include "CondCore/DBCommon/interface/MessageLevel.h"
#include "CondCore/DBCommon/interface/ContainerIterator.h"
#include "CondCore/DBCommon/interface/TypedRef.h"
#include "testCondObj.h"
#include <string>
#include <iostream>
int main(){
  cond::DBSession* session=new cond::DBSession;
  session->configuration().setMessageLevel(cond::Error);
  session->configuration().setAuthenticationMethod(cond::XML);
  static cond::ConnectionHandler& conHandler=cond::ConnectionHandler::Instance();
  conHandler.registerConnection("mydata","sqlite_file:mydata.db",0);
  session->open();
  try{
    conHandler.connect(session);
    std::string token;
    cond::Connection* myconnection=conHandler.getConnection("mydata");    
    cond::PoolTransaction& poolTransaction=myconnection->poolTransaction();
    poolTransaction.start(false);
    for(int i=0; i<10; ++i){
      testCondObj* myobj=new testCondObj;
      myobj->data.insert(std::make_pair(1+i,"strangestring1"));
      myobj->data.insert(std::make_pair(100+i,"strangestring2"));
      cond::TypedRef<testCondObj> myref(poolTransaction,myobj);
      myref.markWrite("mycontainer");
      token=myref.token();
      //std::cout<<"token "<<i<<" "<<token<<std::endl;
    }
    poolTransaction.commit();
    std::cout<<"about to start the second part"<<std::endl;
    cond::PoolTransaction& coltrans=myconnection->poolTransaction();
    coltrans.start(true);
    cond::TypedRef<testCondObj> myinstance(coltrans,token);
    std::cout<<"mem pointer "<<myinstance.ptr()<<std::endl;
    std::cout<<"read back 1   "<<myinstance->data[1]<<std::endl;
    std::cout<<"read back 100 "<<myinstance->data[100]<<std::endl;
    
    cond::PoolContainerManager poolContainers(coltrans);
    std::vector<std::string> containers;
    poolContainers.listAll(containers);
    std::cout<<"number of containers "<<containers.size()<<std::endl;
    for(std::vector<std::string>::iterator it=containers.begin();
	it!=containers.end(); ++it){
      std::cout<<*it<<std::endl;
      cond::ContainerIterator<testCondObj>* cit=poolContainers.newContainerIterator<testCondObj>(*it);
      std::cout<<"collection name "<<cit->name()<<std::endl;
      while(cit->next()){
	std::cout<<"token "<<cit->dataToken()<<std::endl;
	std::cout<<"ref "<<cit->dataRef().token()<<std::endl;
      }
      delete cit;
    }
    coltrans.commit();
  }catch(cond::Exception& er){
    std::cout<<er.what()<<std::endl;
  }catch(std::exception& er){
    std::cout<<er.what()<<std::endl;
  }catch(...){
    std::cout<<"Funny error"<<std::endl;
  }
  delete session;
}
