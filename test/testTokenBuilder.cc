#include "CondCore/DBCommon/interface/TokenBuilder.h"
#include "CondFormats/Calibration/interface/Pedestals.h"
#include "SealKernel/Exception.h"
#include <string>
#include <iostream>
int main(){
  cond::TokenBuilder tk;
  try{
    std::cout<<"build token by classid "<<std::endl;
    tk.setDB("3E60FA40-D105-DA11-981C-000E0C4DE431");
    tk.setContainer("2F16F0A9-79D5-4881-CE0B-C271DD84A7F1","Pedestals");
    tk.setOID(0);
    std::cout<<"result "<<tk.tokenAsString()<<std::endl;
    std::cout<<"build token by dict "<<std::endl;
    tk.setContainerFromDict("CondFormatsCalibration","Pedestals","Pedestals");
    std::cout<<"result "<<tk.tokenAsString()<<std::endl;
  }catch (const seal::Exception &e){
    std::cout << "test ERROR: "<< e.what() << std::endl;
  }catch (const std::exception &e) {
    std::cout << "test ERROR: "<< e.what() << std::endl;
  }catch(...){
    std::cout<<"funny error"<<std::endl;
  }
}
