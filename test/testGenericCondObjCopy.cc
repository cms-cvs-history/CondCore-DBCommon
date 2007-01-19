#include "CondCore/DBCommon/interface/DBSession.h"
#include "CondCore/DBCommon/interface/Exception.h"
#include "CondCore/DBCommon/interface/SessionConfiguration.h"
#include "CondCore/DBCommon/interface/PoolStorageManager.h"
#include "CondCore/DBCommon/interface/ConnectMode.h"
#include "CondCore/DBCommon/interface/MessageLevel.h"
#include "CondCore/DBCommon/interface/Ref.h"
#include "testCondObj.h"
#include <string>
#include <iostream>
int main(){
  cond::DBSession* session=new cond::DBSession(true);
  session->sessionConfiguration().setMessageLevel(cond::Error);
  session->sessionConfiguration().setAuthenticationMethod(cond::XML);
  try{
    session->open();
    cond::PoolStorageManager sourcedb("sqlite_file:source.db",
				      "file:sourcecatalog.xml",
				      session);
    sourcedb.connect();
    testCondObj* myobj=new testCondObj;
    myobj->data.insert(std::make_pair(1,"strangestring1"));
    myobj->data.insert(std::make_pair(100,"strangestring2"));
    cond::Ref<testCondObj> myref(sourcedb,myobj);
    sourcedb.startTransaction(false);
    myref.markWrite("testCondObjRcd");
    std::string objectToken=myref.token();
    std::cout<<"token "<<objectToken<<std::endl;
    sourcedb.commit();
    //end of source db creation
    //start generic copy test
    sourcedb.startTransaction(true);
    std::string objectName("testCondObj");
    cond::PoolStorageManager destdb("sqlite_file:dest.db",
				    "file:destcatalog.xml",
				    session);
    destdb.connect();
    destdb.startTransaction(false);
    sourcedb.copyObjectTo( destdb,"testCondObj",objectToken );
    destdb.commit();
    destdb.disconnect();
    sourcedb.commit();
    sourcedb.disconnect();
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
