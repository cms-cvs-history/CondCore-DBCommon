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
  cond::ServiceLoader* loader=new cond::ServiceLoader;
  loader->loadMessageService();
  cond::DBSession* session=new cond::DBSession(std::string("sqlite_file:test.db"));
  try{
    session->connect(cond::ReadWriteCreate);
    cond::DBWriter pwriter(*session, "Pedestals");
    cond::DBWriter iovwriter(*session, "IOV");
    session->startUpdateTransaction();
    ///init iov sequence
    cond::IOV* initiov=new cond::IOV;
    std::vector<std::string> pedtoks;
    for(int i=0; i<4;++i){
      Pedestals* ped=new Pedestals;
      for(int ichannel=1; ichannel<=5; ++ichannel){
	Pedestals::Item item;
	item.m_mean=0.1*ichannel;
	item.m_variance=0.2*ichannel;
	ped->m_pedestals.push_back(item);
      }
      std::string pedtok=pwriter.markWrite<Pedestals>(ped);
      pedtoks.push_back(pedtok);
      if(i<2){
	initiov->iov.insert(std::make_pair(1,pedtok));
      }else{
	initiov->iov.insert(std::make_pair(2,pedtok));
      }
    }
    iovwriter.markWrite<cond::IOV>(initiov);
    session->commit();
    //create a new iov sequence
    session->startUpdateTransaction();
    cond::IOV* bestiov=new cond::IOV;
    int counter=0;
    for(std::vector<std::string>::iterator it=pedtoks.begin(); 
	it!=pedtoks.end(); ++it){
      ++counter;
      if(counter<3){
	bestiov->iov.insert(std::make_pair(1,*it));
      }else{
	bestiov->iov.insert(std::make_pair(2,*it));
      }
    }
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
