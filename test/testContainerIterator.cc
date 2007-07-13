#include "CondCore/DBCommon/interface/ConnectionHandler.h"
#include "CondCore/DBCommon/interface/DBSession.h"
#include "CondCore/DBCommon/interface/SessionConfiguration.h"
#include "CondCore/DBCommon/interface/PoolContainerManager.h"
#include "CondCore/DBCommon/interface/Exception.h"
#include "CondCore/DBCommon/interface/ConnectMode.h"
#include "CondCore/DBCommon/interface/MessageLevel.h"
#include "CondCore/DBCommon/interface/ContainerIterator.h"
#include "testCondObj.h"
#include <string>
#include <iostream>
int main(){
  cond::DBSession* session=new cond::DBSession;
  session->configuration().setMessageLevel(cond::Error);
  session->configuration().setAuthenticationMethod(cond::XML);
  static cond::ConnectionHandler& conHandler=cond::ConnectionHandler::Instance();
  conHandler.registerConnection("mydata","sqlite_file:mydata.db","file:mycatalog.xml",0);
  session->open();
  try{
    conHandler.connect(session);
    cond::Connection* myconnection=conHandler.getConnection("mydata");    
    cond::PoolTransaction& poolTransaction=myconnection->poolTransaction(false);
    poolTransaction.start();
    for(int i=0; i<10; ++i){
      testCondObj* myobj=new testCondObj;
      myobj->data.insert(std::make_pair(1+i,"strangestring1"));
      myobj->data.insert(std::make_pair(100+i,"strangestring2"));
      cond::TypedRef<testCondObj> myref(poolTransaction,myobj);
      myref.markWrite("mycontainer");
      std::string token=myref.token();
      std::cout<<"token "<<i<<" "<<token<<std::endl;
    }
    //poolTransaction.commit();
    cond::PoolContainerManager poolContainers(poolTransaction);
    //poolTransaction.start();
    std::vector<std::string> containers;
    poolContainers.listAll(containers);
    std::cout<<"number of containers "<<containers.size()<<std::endl;
    for(std::vector<std::string>::iterator it=containers.begin();
	it!=containers.end(); ++it){
      std::cout<<*it<<std::endl;
    }
    poolTransaction.commit();
  }catch(cond::Exception& er){
    std::cout<<er.what()<<std::endl;
  }catch(std::exception& er){
    std::cout<<er.what()<<std::endl;
  }catch(...){
    std::cout<<"Funny error"<<std::endl;
  }
  delete session;
}
