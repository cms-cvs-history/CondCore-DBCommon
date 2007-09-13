#include "CondCore/DBCommon/interface/FipProtocolParser.h"
#include <string>
#include <iostream>
int main(){
  cond::FipProtocolParser fipp;
  std::string realconnect=fipp.getRealConnect("sqlite_fip:CondCore/SQLiteData/data/mydata.db");
  std::cout<<"realconnect "<<realconnect<<std::endl;
}
