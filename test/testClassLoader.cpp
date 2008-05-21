#include "CondCore/DBCommon/interface/ClassInfoLoader.h"
#include "StorageSvc/DbReflex.h"
#include <string>

#include <iostream>


std::string const tokens[] = {
  "[DB=00000000-0000-0000-0000-000000000000][CNT=CSCPedestalsRcd][CLID=E1D4BE86-63E6-21C8-41B3-1116BCFBDE24][TECH=00000B01][OID=00000004-00000003]",
    "[DB=00000000-0000-0000-0000-000000000000][CNT=Fake][CLID=E1D4BE86-0000-21C8-41B3-1116BCFBDE24][TECH=00000B01][OID=00000004-00000003]"
    };

size_t N=2;

int main() {
  
  for (size_t i=0; i<N; i++) {
    std::string const & token = tokens[i];
    const pool::Guid guid(cond::classID(token));
    ROOT::Reflex::Type type = pool::DbReflex::forGuid(guid);
    if (!type) {
      if (!cond::loadClassByToken(token)) 
	std::cout << "dict unknown for " << token << std::endl;
      type = pool::DbReflex::forGuid(guid);
      if (!type)  
	std::cout << "problem with DbReflex for " << token << std::endl;
    }
    std::cout << "class " << type.Name(ROOT::Reflex::SCOPED)
	      << " for " << token << std::endl;
  }

  return 0;

}
