#ifndef COND_SERVICELOADER_H
#define COND_SERVICELOADER_H
#include "SealKernel/Context.h"
#include "AuthenticationMethod.h"
#include "MessageLevel.h"
#include <string>
//#include <map>
namespace seal{
  //class IMessageService;
  class ComponentLoader;
  //class Handle;
  //class Component;
}
//  namespace coral{
//  class IRelationalService;
//  }
/*class IAuthenticationService;
  }
  namespace pool{
  class IBlobStreamingService;
  }
*/
namespace cond{
  //
  //wrapper around loading LCG services
  //
  class ServiceLoader{
  public:
    ServiceLoader();
    ~ServiceLoader();
    void usePOOLContext();
    void useOwnContext();
    seal::Context* context();
    void loadMessageService( cond::MessageLevel messagelevel=cond::Error );
    void loadAuthenticationService( cond::AuthenticationMethod method=cond::Env );
    void loadRelationalService();
    void loadConnectionService();
    /// load the default streaming service
    void loadBlobStreamingService();
    /// load external streaming service
    void loadBlobStreamingService( const std::string& componentName );
    //void loadLookupService();    
    void loadUserMonitoringService();
  private:
    void initLoader();
  private:
    bool m_isPoolContext;
    seal::Context* m_context;
    seal::Handle<seal::ComponentLoader> m_loader;
  };
}//ns cond
#endif
