#include "CondCore/DBCommon/interface/ServiceLoader.h"
#include "CondCore/DBCommon/interface/CONDContext.h"
#include "CondCore/DBCommon/interface/Exception.h"
#include "CondCore/DBCommon/interface/ConnectionConfiguration.h"
#include "PluginManager/PluginManager.h"
#include "SealKernel/Context.h"
#include "SealKernel/ComponentLoader.h"
#include "SealKernel/Component.h"
#include "SealKernel/IMessageService.h"
#include "RelationalAccess/IAuthenticationService.h"
#include "RelationalAccess/IRelationalService.h"
#include "RelationalAccess/IConnectionService.h"
#include "RelationalAccess/IMonitoringService.h"
#include "RelationalAccess/IConnectionServiceConfiguration.h"
#include "RelationalStorageService/IBlobStreamingService.h"
cond::ServiceLoader::ServiceLoader():m_isPoolContext(true),m_context(0) {
}
cond::ServiceLoader::~ServiceLoader(){
}
void cond::ServiceLoader::usePOOLContext(){
  m_context=cond::CONDContext::getPOOLContext();
}
void cond::ServiceLoader::useOwnContext(){
  m_isPoolContext=false;
  m_context=cond::CONDContext::getOwnContext();
}
seal::Context* cond::ServiceLoader::context(){
  return m_context;
}
void cond::ServiceLoader::initLoader(){
  seal::PluginManager* pm = seal::PluginManager::get();
  if ( ! pm ) {
    throw cond::Exception( "Could not get the plugin manager instance" );
  }
  pm->initialise();
  if(!m_context){
    m_context=cond::CONDContext::getPOOLContext(); //default
  }
  m_loader = new seal::ComponentLoader( m_context );
}
void cond::ServiceLoader::loadMessageService( cond::MessageLevel messagelevel ){
  std::cout<<"cond::ServiceLoader::loadMessageService"<<std::endl;
  if(!m_loader) {
    std::cout<<"about to init"<<std::endl;
    this->initLoader();
    std::cout<<"after init"<<std::endl;
  }
  std::cout<<"about to load messageservice"<<std::endl;
  m_loader->load("SEAL/Services/MessageService");
  std::vector< seal::IHandle<seal::IMessageService> > v_msgSvc;
  m_context->query( v_msgSvc );
  if ( v_msgSvc.empty() ) {
    throw cond::Exception( "could not locate the seal message service" );
  }
  switch ( messagelevel ) {
  case cond::Error :
    v_msgSvc.front()->setOutputLevel( seal::Msg::Error );
    break;
  case cond::Warning :
    v_msgSvc.front()->setOutputLevel( seal::Msg::Warning );
    break;
  case cond::Debug :
    v_msgSvc.front()->setOutputLevel( seal::Msg::Debug );
    break;
  case cond::Info :
    v_msgSvc.front()->setOutputLevel( seal::Msg::Info );
    break;
  default:
    v_msgSvc.front()->setOutputLevel( seal::Msg::Error );
  } 
  return;
}
void cond::ServiceLoader::loadAuthenticationService(cond::AuthenticationMethod method){
  if(!m_loader) {
    this->initLoader();
  }
  std::vector< seal::IHandle<coral::IAuthenticationService> > v_svc;
  switch ( method ) {
  case cond::Env :
    m_loader->load( "CORAL/Services/EnvironmentAuthenticationService" );
    m_context->query( v_svc );
    if ( v_svc.empty() ) {
      throw cond::Exception( "could not locate the coral authentication service" );
    }
    break;
  case cond::XML :
    m_loader->load( "CORAL/Services/XMLAuthenticationService" );
    m_context->query( v_svc );
    if ( v_svc.empty() ) {
      throw cond::Exception( "could not locate the coral authentication service" );
    }
    break;
  default:
    m_loader->load( "CORAL/Services/EnvironmentAuthenticationService" );
    m_context->query( v_svc );
    if ( v_svc.empty() ) {
      throw cond::Exception( "could not locate the coral authentication service" );
    }
  }
}
void cond::ServiceLoader::loadRelationalService(){
  if(!m_loader) {
    this->initLoader();
  }
  m_loader->load( "CORAL/Services/RelationalService" );
  std::vector< seal::IHandle<coral::IRelationalService> > v_svc;
  m_context->query( v_svc );
  if ( v_svc.empty() ) {
    throw cond::Exception( "could not locate the coral relational service" );
  }
  return;
}
void cond::ServiceLoader::loadConnectionService(cond::ConnectionConfiguration& config){
  if(!m_loader) {
    this->initLoader();
  }
  m_loader->load( "CORAL/Services/ConnectionService" );
  
  seal::IHandle<coral::IConnectionService> iHandle=
  m_context->query<coral::IConnectionService>( "CORAL/Services/ConnectionService" );
  if (! iHandle ) {
    throw cond::Exception( "could not locate the coral connection service" );
  }
  coral::IConnectionServiceConfiguration& conserviceConfig = iHandle->configuration();
  if( config.isConnectionSharingEnabled() ){
    conserviceConfig.enableConnectionSharing();
  }
  conserviceConfig.setConnectionRetrialPeriod( config.connectionRetrialPeriod() );
  conserviceConfig.setConnectionRetrialTimeOut( config.connectionRetrialTimeOut() );
  conserviceConfig.setConnectionTimeOut( config.connectionTimeOut() );
  return;
}
void cond::ServiceLoader::loadBlobStreamingService( const std::string& componentName){
  if(!m_loader) {
    this->initLoader();
  }
  if(componentName.empty()){
    m_loader->load( "COND/Services/DefaultBlobStreamingService" );
  }else{
    m_loader->load(componentName);
  }
  std::vector< seal::IHandle<pool::IBlobStreamingService> > v_svc;
  m_context->query( v_svc );
  if ( v_svc.empty() ) {
    throw cond::Exception( "could not locate the BlobStreamingService" );
  }
}
/*
  void cond::ServiceLoader::loadLookupService(){
  if(!m_loader) {
  this->initLoader();
  }
  m_loader->load( "CORAL/Services/DefaultBlobStreamingService" );
  std::vector< seal::IHandle<pool::IBlobStreamingService> > v_svc;
  m_context()->query( v_svc );
  if ( v_svc.empty() ) {
  throw cond::Exception( "could not locate the BlobStreamingService" );
  }
  }
*/
/*void cond::ServiceLoader::loadUserMonitoringService(){
  if(!m_loader) {
    this->initLoader();
  }
  m_loader->load( "CORAL/Services/UserMonitoringService" );
  std::vector< seal::IHandle<coral::monitor::IMonitoringService> > v_svc;
  m_context->query( v_svc );
  if ( v_svc.empty() ) {
    throw cond::Exception( "could not locate the UserMonitoringService" );
  }
}
*/
