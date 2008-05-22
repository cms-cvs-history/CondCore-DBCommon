#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/PluginManager/interface/ProblemTracker.h"
#include "CondCore/DBCommon/interface/ClassInfoLoader.h"
#include "StorageSvc/DbReflex.h"
#include <string>

#include <iostream>


std::string const tokens[] = {
  "[DB=00000000-0000-0000-0000-000000000000][CNT=CSCPedestalsRcd][CLID=E1D4BE86-63E6-21C8-41B3-1116BCFBDE24][TECH=00000B01][OID=00000004-00000003]",
    "[DB=00000000-0000-0000-0000-000000000000][CNT=Fake][CLID=E1D4BE86-0000-21C8-41B3-1116BCFBDE24][TECH=00000B01][OID=00000004-00000003]",
  "[DB=00000000-0000-0000-0000-000000000000][CNT=CSCPedestalsRcd][CLID=E1D4BE86-63E6-21C8-41B3-1116BCFBDE24][TECH=00000B01][OID=00000004-00000005]"
    };

size_t N=3;

int main() {

  edm::AssertHandler ah;
  
  for (size_t i=0; i<N; i++) {
    std::string const & token = tokens[i];
    ROOT::Reflex::Type type = cond::reflexTypeByToken(token);
    std::cout << "class " << type.Name(ROOT::Reflex::SCOPED)
	      << " for " << token << std::endl;
  }

  return 0;

}
