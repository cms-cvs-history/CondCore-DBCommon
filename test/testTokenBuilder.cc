#include "CondCore/DBCommon/interface/TokenBuilder.h"
#include "CondCore/DBCommon/interface/TokenInterpreter.h" 
#include "CondCore/DBCommon/interface/Exception.h"
#include "CondCore/DBCommon/interface/Time.h"
#include <string>
#include <iostream>
int main(){
  cond::TokenBuilder tk;
  std::string tok1,tok2;
  
  try{
    tk.set("3E60FA40-D105-DA11-981C-000E0C4DE431",
	   "testDict",
	   "testCondObj",
	   "testCondObj",
	   0);
    tok1=tk.tokenAsString();
    std::cout<<"result 1 "<<tok1<<std::endl;
    tk.resetOID(2);
    tok2=tk.tokenAsString();
    std::cout<<"result 2 "<<tok2<<std::endl;
  }catch (const cond::Exception &e){
    std::cout << "test ERROR: "<< e.what() << std::endl;
  }catch(...){
    std::cout<<"funny error"<<std::endl;
  }

  cond::TokenInterpreter tokenteller(tok1);
  std::string containerName=tokenteller.containerName();
  std::string className=tokenteller.className();
  std::cout<<"token "<<tok1<<" belongs to container "<<containerName<<" class "<<className<<std::endl;
  //cond::TimeTypeList timetypelist;
  std::cout<<"Time type "<< cond::TimeTypeList[cond::runnumber]<<std::endl;
  std::cout<<"Time type "<< cond::TimeTypeList[cond::timestamp]<<std::endl;
  std::cout<<"Time type "<<  cond::TimeTypeList[cond::lumiid]<<std::endl;
}
