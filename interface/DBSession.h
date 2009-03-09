#ifndef COND_DBCommon_DBSession_h
#define COND_DBCommon_DBSession_h
//
// Package:    CondCore/DBCommon
// Class:      DBSession
//
/**\class DBSession DBSession.h CondCore/DBCommon/interface/DBSession.h
 Description: Class to prepare database connection setup
*/
//
// Author:      Zhen Xie
// $Id: DBSession.h,v 1.10 2008/11/13 18:31:12 xiezhen Exp $
//
#include <string>
//#include "CoralKernel/Context.h"
//#include "CoralKernel/IHandle.h"
namespace coral{
  class IConnectionService;
  class IRelationalService;
  class IAuthenticationService;
  class IMonitoringReporter;
  class IWebCacheControl;
}
namespace pool{
  class IBlobStreamingService;
}
namespace cond{
  class SessionConfiguration;
  class CoralServiceManager;
  /*
  **/
  class DBSession{
  public:
    DBSession();
    ~DBSession();
    void open();
    //void close();
    coral::IConnectionService& connectionService();
    coral::IRelationalService& relationalService();
    coral::IAuthenticationService& authenticationService() ;
    const coral::IMonitoringReporter& monitoringReporter() const;
    coral::IWebCacheControl& webCacheControl();
    pool::IBlobStreamingService& blobStreamingService();
    cond::SessionConfiguration& configuration();
  private:
    //    coral::IHandle<coral::Context> m_context;
    SessionConfiguration* m_sessionConfig;
    CoralServiceManager* m_pluginmanager;
  };
}//ns cond
#endif
// DBSESSION_H
