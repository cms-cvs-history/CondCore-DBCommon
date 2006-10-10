#ifndef COND_DBSESSION_H
#define COND_DBSESSION_H
#include "CondCore/DBCommon/interface/ConnectMode.h"
#include <string>
#include <vector>
namespace pool{
  class IDataSvc;
  class IFileCatalog;
  class IDatabase;
}
namespace cond{
  class DBSession{
  public:
    explicit DBSession( const std::string& con );
    DBSession( const std::string& con, 
	       const std::string& catalogcon
	       );
    ~DBSession();
    void setCatalog( const std::string& catalogcon );
    void connect(  cond::ConnectMode mode=cond::ReadWriteCreate );
    void disconnect();
    void startUpdateTransaction();
    void startReadOnlyTransaction();
    void commit();
    void rollback();
    const std::string connectionString() const;
    std::vector< std::string > containers();
    pool::IDataSvc& DataSvc() const;
    pool::IFileCatalog& Catalog() const;
  private:
    std::string m_con;
    pool::IFileCatalog* m_cat;
    pool::IDataSvc* m_svc;
    std::string m_catalogcon;
    pool::IDatabase* m_db;
  };
}//ns cond
#endif
// DBSESSION_H
