#include "CondCore/DBCommon/interface/DBWriter.h"
#include "CondFormats/Calibration/interface/Pedestals.h"
#include <string>
#include <iostream>
int main(){
  cond::DBWriter w(std::string("sqlite_file:test.db"));
  Pedestals* ped=new Pedestals;
  if( w.containerExists( "Pedestals" )){
    std::cout<<"container Exisits"<<std::endl;
    w.openContainer("Pedestals");
    w.startTransaction();
    Pedestals::Item item;
    item.m_mean=0.1;
    item.m_variance=0.2;
    ped->m_pedestals.push_back(item);
    std::string tok=w.write<Pedestals>(ped);
    w.commitTransaction();
  }else{
    std::cout<<1<<std::endl;
    w.createContainer("Pedestals");
    w.startTransaction();
    Pedestals::Item item;
    item.m_mean=0.3;
    item.m_variance=0.4;
    ped->m_pedestals.push_back(item);
    std::string tok=w.write<Pedestals>(ped);
    w.commitTransaction();
  }
}
