#include "CondCore/DBCommon/interface/DBWriter.h"
#include "CondFormats/Calibration/interface/Pedestals.h"
#include "SealKernel/Service.h"
#include "POOLCore/POOLContext.h"
#include "SealKernel/Context.h"
#include <string>
#include <iostream>
int main(){
  pool::POOLContext::loadComponent( "SEAL/Services/MessageService" );
  pool::POOLContext::setMessageVerbosityLevel( seal::Msg::Error );
  cond::DBWriter w(std::string("sqlite_file:test.db"));
  if( w.containerExists( "Pedestals" ) ){
    Pedestals* ped1=new Pedestals;
    std::cout<<"container exists"<<std::endl;
    w.openContainer("Pedestals");
    w.startTransaction();
    Pedestals::Item item;
    item.m_mean=0.1;
    item.m_variance=0.2;
    ped1->m_pedestals.push_back(item);
    std::string tok=w.write<Pedestals>(ped1);
    Pedestals* ped2=new Pedestals;
    ped2->m_pedestals.push_back(item);
    w.write<Pedestals>(ped2);
    w.commitTransaction();
  }else{
    std::cout<<"container doesnot exist"<<std::endl;
    w.createContainer("Pedestals");
    Pedestals* ped1=new Pedestals;
    w.startTransaction();
    Pedestals::Item item;
    item.m_mean=0.3;
    item.m_variance=0.4;
    ped1->m_pedestals.push_back(item);
    std::string tok=w.write<Pedestals>(ped1);
    Pedestals* ped2=new Pedestals;
    ped2->m_pedestals.push_back(item);
    w.write<Pedestals>(ped2);
    std::cout<<"about to commit"<<std::endl;
    w.commitTransaction();
    std::cout<<"committed"<<std::endl;
  }
}
