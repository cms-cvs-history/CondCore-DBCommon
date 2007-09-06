#include "CondCore/DBCommon/interface/TokenAnalyzer.h"
#include <string>
#include <iostream>
int main(){
  cond::TokenAnalyzer tkana;
  std::string strToken("[DB=C46BDA90-7A5C-DC11-BE66-0016761581E6][CNT=cond::IOV][CLID=2F16F0A9-79D5-4881-CE0B-A271DD84A7F1][TECH=00000B01][OID=00000004-00000000]");
  std::string result=tkana.getFID(strToken);
  std::cout<<"result "<<result<<std::endl;
}
