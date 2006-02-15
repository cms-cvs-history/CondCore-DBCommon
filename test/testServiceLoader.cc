#include "CondCore/DBCommon/interface/ServiceLoader.h"
#include "CondCore/DBCommon/interface/Exception.h"
#include "SealKernel/Exception.h"
#include <string>
#include <iostream>
int main(){
  cond::ServiceLoader* loader=new cond::ServiceLoader;
  try{
    loader->loadMessageService();
    std::cout<<"message service loaded"<<std::endl;
  }catch(seal::Exception& er){
    std::cout<<"caught seal exception "<<er.what()<<std::endl;
  }catch(...){
    std::cout<<"funny error"<<std::endl;
  }
  try{
    loader->loadAuthenticationService();
    std::cout<<"authentication service loaded"<<std::endl;
  }catch(seal::Exception& er){
    std::cout<<"caught seal exception "<<er.what()<<std::endl;
  }catch(...){
    std::cout<<"funny error"<<std::endl;
  }
  try{
    loader->loadBlobStreamingService();
    std::cout<<"streaming service loaded"<<std::endl;
  }catch(cond::Exception& er){
    std::cout<<"caught cond exception "<<er.what()<<std::endl;
  }catch(seal::Exception& er){
    std::cout<<"caught seal exception "<<er.what()<<std::endl;
  }catch(...){
    std::cout<<"funny error"<<std::endl;
  }
  delete loader;
}
