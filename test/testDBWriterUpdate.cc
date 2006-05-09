#include "CondCore/DBCommon/interface/DBWriter.h"
#include "CondCore/DBCommon/interface/DBSession.h"
#include "CondCore/DBCommon/interface/Exception.h"
#include "CondCore/DBCommon/interface/ServiceLoader.h"
#include "CondCore/DBCommon/interface/ConnectMode.h"
#include "CondCore/DBCommon/interface/MessageLevel.h"
#include "CondFormats/Calibration/interface/Pedestals.h"
#include "CondCore/IOVService/interface/IOV.h"
#include <string>
#include <iostream>
int main(){
  cond::ServiceLoader* loader=new cond::ServiceLoader;
  //loader->loadMessageService(cond::Info);
  loader->loadMessageService(cond::Error);
  cond::DBSession* session=new cond::DBSession(std::string("sqlite_file:test.db"));
  std::string iovToken;
  try{
    session->connect(cond::ReadWriteCreate);
    cond::DBWriter iovwriter(*session, "IOV");
    session->startUpdateTransaction();
    ///init iov sequence
    cond::IOV* initiov=new cond::IOV;
    initiov->iov.insert(std::make_pair(1,"faketok1"));
    initiov->iov.insert(std::make_pair(3,"faketok2"));
    initiov->iov.insert(std::make_pair(5,"faketok3"));
    iovToken=iovwriter.markWrite<cond::IOV>(initiov);
    session->commit();
    std::cout<<"written "<<iovToken<<std::endl;
    session->startUpdateTransaction();
    cond::IOV* me=iovwriter.markUpdate<cond::IOV>(iovToken);
    me->iov[5]="faketok3-updated";
    me->iov.insert(std::make_pair(7,"faketok7"));
    session->commit();
    std::cout<<"updated "<<iovToken<<std::endl;
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
