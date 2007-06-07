#include "CondCore/DBCommon/interface/DBCatalog.h"
#include "FileCatalog/IFileCatalog.h"
#include <string>
#include <iostream>
int main(){
  cond::DBCatalog mycat;
  std::string lfn("/dev/CMS_COND_GENERAL");
  std::pair<std::string,std::string> logicalService=mycat.logicalservice(lfn);
  std::cout<<"cat name "<<mycat.defaultDevCatalogName()<<std::endl;
  if( !logicalService.first.empty() ){
    std::cout<<"service name "<<logicalService.first<<std::endl;
    std::cout<<"data name "<<logicalService.second<<std::endl;
    mycat.poolCatalog().setWriteCatalog(mycat.defaultDevCatalogName());
    mycat.poolCatalog().connect();
    mycat.poolCatalog().start();
    std::string pf=mycat.getPFN(mycat.poolCatalog(),lfn, false);
    mycat.poolCatalog().commit();
    std::cout<<"pfn "<<pf<<std::endl;
    mycat.poolCatalog().start();
    pf=mycat.getPFN(mycat.poolCatalog(),lfn,true);
    mycat.poolCatalog().commit();
     std::cout<<"pfn with cache "<<pf<<std::endl;
    mycat.poolCatalog().disconnect();
  }
  return 0;
}
