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
  namespace coral{
  class IRelationalService;
  }
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
    /// factory methold. hand over the ownership to user;
    ServiceLoader();
    ~ServiceLoader();
    void loadMessageService( cond::MessageLevel level=cond::Error );
    bool hasMessageService() const;
    void loadAuthenticationService( cond::AuthenticationMethod method=cond::Env );
    bool hasAuthenticationService() const;
    coral::IRelationalService& loadRelationalService();
    void loadConnectionService();
    /// load the default streaming service
    void loadBlobStreamingService();
    /// load external streaming service
    void loadBlobStreamingService( const std::string& componentName );
  private:
    seal::Handle< seal::Context > m_context;
    seal::Handle< seal::ComponentLoader > m_loader;
  };
}//ns cond
#endif