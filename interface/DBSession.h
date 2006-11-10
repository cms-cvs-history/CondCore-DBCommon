#ifndef COND_DBCommon_DBSession_h
#define COND_DBCommon_DBSession_h
#include <string>
namespace cond{
  class PoolStorageManager;
  class RelationalStorageManager;
  class ServiceLoader;
  class ConnectionConfiguration;
  class SessionConfiguration;
  /*
  **/
  class DBSession{
  public:
    explicit DBSession( const std::string& con );
    ~DBSession();
    void open( bool usePoolContext=true );
    void close();
    PoolStorageManager& poolStorageManager( const std::string& catalog );
    RelationalStorageManager& relationalStorageManager();
    ServiceLoader& serviceLoader();
    ConnectionConfiguration& connectionConfiguration();
    SessionConfiguration& sessionConfiguration();
    const std::string connectionString() const;
    bool hasOpenConnections() const;
    void purgeConnections();
  private:
    bool m_isActive;
    std::string m_con;
    PoolStorageManager* m_pool;
    RelationalStorageManager* m_coral;
    ServiceLoader* m_loader;
    ConnectionConfiguration* m_connectConfig;
    SessionConfiguration* m_sessionConfig;
  };
}//ns cond
#endif
// DBSESSION_H
