#ifndef COND_DBCommon_RelationalStorageManager_h
#define COND_DBCommon_RelationalStorageManager_h
#include <string>
#include "CondCore/DBCommon/interface/ConnectMode.h"
namespace seal{
  class Context;
}
namespace coral{
  class ISessionProxy;
}
namespace cond{
  /**
   * Class manages pure CORAL session and transaction 
   */
  class RelationalStorageManager{
  public:
    RelationalStorageManager(const std::string& con, seal::Context* context);
    ~RelationalStorageManager();
    void connect(cond::ConnectMode mod);
    void disconnect();
    void startTransaction(bool isReadOnly=true);
    void commit();
    void rollback();
    //coral::ISessionProxy& sessionProxy();
  private:
    std::string m_con;
    seal::Context* m_context;
    coral::ISessionProxy* m_proxy;
    bool m_readOnlyMode;
  };
}//ns cond
#endif
