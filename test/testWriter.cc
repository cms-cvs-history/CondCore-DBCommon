#include "CondCore/DBCommon/interface/DBWriter.h"
#include "CondCore/DBCommon/interface/DBSession.h"
#include "CondCore/DBCommon/interface/Exception.h"
#include "CondCore/DBCommon/interface/ServiceLoader.h"
#include "CondCore/DBCommon/interface/ConnectMode.h"
#include "CondFormats/Calibration/interface/Pedestals.h"
#include "CondCore/IOVService/interface/IOV.h"
#include <string>
#include <iostream>
int main(){
  std::cout<<1<<'\n';
  cond::ServiceLoader* loader=new cond::ServiceLoader;
  std::cout<<2<<'\n';
  loader->loadMessageService();
  std::cout<<3<<'\n';
  cond::DBSession* session=new cond::DBSession(std::string("sqlite_file:test.db"));
  std::cout<<4<<'\n';
  try{
  session->connect(cond::ReadWriteCreate);
  std::cout<<5<<'\n';
  cond::DBWriter pwriter(*session, "Pedestals");
  std::cout<<6<<'\n';
  Pedestals* ped1=new Pedestals;
  std::cout<<7<<'\n';
  Pedestals::Item item;
  item.m_mean=0.1;
  item.m_variance=0.2;
  ped1->m_pedestals.push_back(item);
  std::cout<<8<<'\n';
  session->startUpdate();
  std::string pedtok1=pwriter.markWrite<Pedestals>(ped1);
  Pedestals* ped2=new Pedestals;
  ped2->m_pedestals.push_back(item);
  std::string pedtok2=pwriter.markWrite<Pedestals>(ped2);
  session->commit();
  cond::IOV* initiov=new cond::IOV;
  initiov->iov.insert(std::make_pair(1,pedtok1));
  initiov->iov.insert(std::make_pair(2,pedtok2));
  cond::DBWriter iovwriter(*session, "IOV");
  std::string iovtok=iovwriter.markWrite<cond::IOV>(initiov);
  session->commit();
  session->startUpdate();
  cond::IOV* bestiov=new cond::IOV;
  bestiov->iov.insert(std::make_pair(10,pedtok1));
  bestiov->iov.insert(std::make_pair(20,pedtok2));
  std::string bestiovtok=iovwriter.markWrite<cond::IOV>(bestiov);
  session->commit();
  session->disconnect();
  }catch(cond::Exception& er){
    std::cout<<er.what()<<std::endl;
  }catch(seal::Exception& er){
    std::cout<<er.what()<<std::endl;
  }catch(...){
    std::cout<<"Funny error"<<std::endl;
  }
  delete session;
  delete loader;
}
