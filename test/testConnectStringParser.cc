#include "CondCore/DBCommon/interface/ConnectStringParser.h"
#include "CondCore/DBCommon/interface/Exception.h"
#include <string>
#include <iostream>
int main(){
  try{
    cond::ConnectStringParser p("/oracle/dev/CMS_COND_GENERAL");
    if( !p.isLogical() ) {
      std::cout<<"wrong"<<std::endl;
    }
    if( p.result()!= std::string("oracle://devdb10/CMS_COND_GENERAL") ){
      std::cout<<"error "<<p.result()<<std::endl;
      exit(1);
    }
    p.reset("/frontier/int/CMS_COND_ECAL");
    if( !p.isLogical() ) {
      std::cout<<"wrong"<<std::endl;
    }
    if( p.result()!= std::string("frontier://FrontierInt/CMS_COND_ECAL") ){
      std::cout<<"error "<<p.result()<<std::endl;
      exit(1);
    }
    p.reset("/sqlite/int/CMS_COND_ECAL");
    if( p.result()!= std::string("sqlite_file://int-CMS_COND_ECAL.db") ){
      std::cout<<"error "<<p.result()<<std::endl;
      exit(1);
    }
    p.reset("oracle://devdb10/CMS_COND_GENERAL");
    if( p.isLogical() ) {
      std::cout<<"wrong"<<std::endl;
    }
    if( p.result() != std::string("/oracle/dev/CMS_COND_GENERAL") ){
      std::cout<<"error "<<p.result()<<std::endl;
      exit(1);
    }
  }catch (const cond::Exception &e){
    std::cout << "test ERROR: "<< e.what() << std::endl;
  }catch(...){
    std::cout<<"funny error"<<std::endl;
  }
}
