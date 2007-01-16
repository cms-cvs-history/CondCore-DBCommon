#include "CondCore/DBCommon/interface/TokenBuilder.h"
#include "CondCore/DBCommon/interface/Exception.h"
#include "testCondObj.h"
#include <string>
#include <iostream>
int main(){
  cond::TokenBuilder tk;
  try{
    tk.set("3E60FA40-D105-DA11-981C-000E0C4DE431",
	   "testDict",
	   "testCondObj",
	   "testCondObj",
	   0);
    std::cout<<"result 1 "<<tk.tokenAsString()<<std::endl;
    tk.resetOID(2);
    std::cout<<"result 2 "<<tk.tokenAsString()<<std::endl;
  }catch (const cond::Exception &e){
    std::cout << "test ERROR: "<< e.what() << std::endl;
  }catch(...){
    std::cout<<"funny error"<<std::endl;
  }
}
