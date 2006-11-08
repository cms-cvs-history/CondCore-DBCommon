#ifndef COND_DBCommon_PoolStorageManager_h
#define COND_DBCommon_PoolStorageManager_h
#include "CondCore/DBCommon/interface/ConnectMode.h"
#include <string>
#include <vector>
namespace pool{
  class IFileCatalog;
  class IDataSvc;
  class IDatabase;
}
namespace cond{
  class DBSession;
  //class ContainerIterator;
  class PoolStorageManager{
  public:
    PoolStorageManager(const std::string& con,const std::string& catalog);
    ~PoolStorageManager();
    void connect(cond::ConnectMode mod);
    void disconnect();
    void startTransaction(bool isReadOnly=true);
    void commit();
    void rollback();
    std::string catalogStr() const;
    std::vector<std::string> containers();
    pool::IDataSvc& DataSvc();
  //ContainerIterator* newContainerIterator(const std::string& containername);
  private:
    std::string m_catalogstr;
    std::string m_con;
    pool::IFileCatalog* m_cat;
    pool::IDataSvc* m_svc;
    pool::IDatabase* m_db;
  };
}//ns cond
#endif
