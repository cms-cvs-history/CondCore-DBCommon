#include "CondCore/DBCommon/interface/DBWriter.h"
#include "CondFormats/Calibration/interface/Pedestals.h"
#include "CondCore/IOVService/interface/IOV.h"
#include "SealKernel/Service.h"
#include "POOLCore/POOLContext.h"
#include "SealKernel/Context.h"
#include <string>
#include <iostream>
int main(){
  pool::POOLContext::loadComponent( "SEAL/Services/MessageService" );
  pool::POOLContext::setMessageVerbosityLevel( seal::Msg::Error );
  cond::DBWriter w(std::string("sqlite_file:test.db"));
  w.startTransaction();
  Pedestals* ped1=new Pedestals;
  Pedestals::Item item;
  item.m_mean=0.1;
  item.m_variance=0.2;
  ped1->m_pedestals.push_back(item);
  std::string pedtok1=w.write<Pedestals>(ped1,"Pedestals");
  Pedestals* ped2=new Pedestals;
  ped2->m_pedestals.push_back(item);
  std::string pedtok2=w.write<Pedestals>(ped2, "Pedestals");
  cond::IOV* initiov=new cond::IOV;
  initiov->iov.insert(std::make_pair(1,pedtok1));
  initiov->iov.insert(std::make_pair(2,pedtok2));
  std::string iovtok=w.write<cond::IOV>(initiov, "IOV");
  w.commitTransaction();
  w.startTransaction();
  cond::IOV* bestiov=new cond::IOV;
  bestiov->iov.insert(std::make_pair(10,pedtok1));
  bestiov->iov.insert(std::make_pair(20,pedtok2));
  std::string bestiovtok=w.write<cond::IOV>(bestiov, "IOV");
  w.commitTransaction();
}
