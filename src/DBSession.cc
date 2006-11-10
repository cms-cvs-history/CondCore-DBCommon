#include "CondCore/DBCommon/interface/DBSession.h"
#include "CondCore/DBCommon/interface/ServiceLoader.h"
#include "CondCore/DBCommon/interface/PoolStorageManager.h"
#include "CondCore/DBCommon/interface/RelationalStorageManager.h"
#include "CondCore/DBCommon/interface/SessionConfiguration.h"
#include "CondCore/DBCommon/interface/ConnectionConfiguration.h"
cond::DBSession::DBSession( const std::string& con ):m_isActive(false),m_con(con),m_pool(0),m_coral(0),m_loader(new cond::ServiceLoader),m_connectConfig(new cond::ConnectionConfiguration),m_sessionConfig(new cond::SessionConfiguration){ 
}
cond::DBSession::~DBSession(){
  if(m_pool) delete m_pool;
  if(m_coral) delete m_coral;
  delete m_loader;
  delete m_connectConfig;
  delete m_sessionConfig;
}
void cond::DBSession::open( bool usePoolContext ){
  if(usePoolContext){
    m_loader->usePOOLContext();
  }else{
    m_loader->useOwnContext();
  }
  //necessary
  m_loader->loadMessageService( m_sessionConfig->messageLevel() );
  m_loader->loadConnectionService( *m_connectConfig );
  m_loader->loadAuthenticationService( m_sessionConfig->authenticationMethod() );
  //optional
  if(  m_sessionConfig->hasStandaloneRelationalService() ){
    m_loader->loadRelationalService();
  }
  m_isActive=true;
}
void cond::DBSession::close(){
  m_isActive=false;
}
cond::PoolStorageManager& cond::DBSession::poolStorageManager( const std::string& catalog ){
  if( m_sessionConfig->hasBlobStreamService() ){
    m_loader->loadBlobStreamingService( m_sessionConfig->blobStreamerName() );
  }
  if(!m_pool){
    m_pool=new cond::PoolStorageManager(m_con,catalog);
  }
  return *m_pool;
}
cond::RelationalStorageManager& cond::DBSession::relationalStorageManager(){
  if(!m_coral){
    m_coral=new cond::RelationalStorageManager(m_con,m_loader->context());
  }
  return *m_coral;
}
cond::ServiceLoader& cond::DBSession::serviceLoader(){
  return *m_loader;
}
cond::ConnectionConfiguration& cond::DBSession::connectionConfiguration(){
  return *m_connectConfig;
}
cond::SessionConfiguration& cond::DBSession::sessionConfiguration(){
  return *m_sessionConfig;
}
const std::string cond::DBSession::connectionString() const{
  return m_con;
}
bool cond::DBSession::hasOpenConnections() const{
  return false;
}
void cond::DBSession::purgeConnections(){ 
}
