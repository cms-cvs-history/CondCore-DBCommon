#include "CondCore/DBCommon/interface/DbConnection.h"
#include "CondCore/DBCommon/interface/DbTransaction.h"
#include "CondCore/DBCommon/interface/PoolContainerManager.h"
#include "CondCore/DBCommon/interface/Exception.h"
#include "CondCore/DBCommon/interface/ContainerIterator.h"
#include "FWCore/PluginManager/interface/PluginManager.h"
#include "FWCore/PluginManager/interface/standard.h"
#include "testCondObj.h"
#include <string>
#include <iostream>
int main(){
  edmplugin::PluginManager::Config config;
  edmplugin::PluginManager::configure(edmplugin::standard::config());
  cond::DbConnection connection;
  connection.configuration().setMessageLevel(coral::Error);
  connection.configure();  
  try{
    cond::DbSession session = connection.createSession();
    session.open("sqlite_file:mydata.db");
    std::string token;
    session.transaction().start(false);
    for(int i=0; i<10; ++i){
      testCondObj* myobj=new testCondObj;
      myobj->data.insert(std::make_pair(1+i,"strangestring1"));
      myobj->data.insert(std::make_pair(100+i,"strangestring2"));
      pool::Ref<testCondObj> myref = session.storeObject(myobj, "mycontainer");
      token=myref.toString();
      //std::cout<<"token "<<i<<" "<<token<<std::endl;
    }
    session.transaction().commit();
    std::cout<<"about to start the second part"<<std::endl;
    session.transaction().start(true);
    pool::Ref<testCondObj> myinstance = session.getTypedObject<testCondObj>(token);
    std::cout<<"mem pointer "<<myinstance.ptr()<<std::endl;
    std::cout<<"read back 1   "<<myinstance->data[1]<<std::endl;
    std::cout<<"read back 100 "<<myinstance->data[100]<<std::endl;
    
    cond::PoolContainerManager poolContainers( session );
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
    session.transaction().commit();
  }catch(cond::Exception& er){
    std::cout<<er.what()<<std::endl;
  }catch(std::exception& er){
    std::cout<<er.what()<<std::endl;
  }catch(...){
    std::cout<<"Funny error"<<std::endl;
  }
}
