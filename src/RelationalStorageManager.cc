#include "CondCore/DBCommon/interface/RelationalStorageManager.h"
#include "CondCore/DBCommon/interface/Exception.h"
#include "CondCore/DBCommon/interface/ServiceLoader.h"
#include "SealKernel/Context.h"
#include "SealKernel/ComponentLoader.h"
#include "SealKernel/Component.h"
#include "RelationalAccess/IConnectionService.h"
#include "RelationalAccess/ISessionProxy.h"
#include "RelationalAccess/ITransaction.h"
#include "CoralBase/Exception.h"
#include <vector>
cond::RelationalStorageManager::RelationalStorageManager(const std::string& con, seal::Context* context):m_con(con),m_context(context){}
cond::RelationalStorageManager::~RelationalStorageManager(){}
void cond::RelationalStorageManager::connect(cond::ConnectMode mod){
  if(mod==cond::ReadOnly){
    m_readOnlyMode=true;
  }
  if ( m_proxy ) {
    delete m_proxy;
    m_proxy= 0;
  }
  // Connect to the database
  // Retrieve an IConnectionService
  std::vector< seal::IHandle< coral::IConnectionService > > loadedServices;
  m_context->query( loadedServices );
  if ( loadedServices.empty() ) {
    //m_context->query( loadedServices );
    throw cond::Exception( std::string("RelationalStorageManager::connect: ConnectionService is not loaded") );
  }
  seal::IHandle< coral::IConnectionService > connectionService = loadedServices.front();
  try{
    m_proxy = connectionService->connect(m_con, ( mod == cond::ReadOnly ) ? coral::ReadOnly : coral::Update );
  }catch (const coral::Exception& e) {
    if(m_proxy) delete m_proxy;
    m_proxy = 0;
    throw cond::Exception( std::string("RelationalStorageManager::connect: couldn't connect to the database ")+e.what() );
  }
}
void cond::RelationalStorageManager::disconnect(){
  try{
    coral::ITransaction& transaction = m_proxy->transaction();
    if ( transaction.isActive() && ( ! m_proxy->isConnectionShared() ) ) {
      if ( transaction.isReadOnly() ) {
        if ( m_proxy->isConnectionShared() ) {
          transaction.commit();
        }
      }else {
        transaction.rollback();
      }
    }
  }catch(const coral::Exception&)
  {
  }
  if ( m_proxy ) delete m_proxy;
  m_proxy = 0;
}
void cond::RelationalStorageManager::startTransaction(bool isReadOnly){
  coral::ITransaction& transaction = m_proxy->transaction();
  bool sharedConnection = m_proxy->isConnectionShared();
  bool activeTransaction = transaction.isActive();
  try {
    if(!activeTransaction) transaction.start(isReadOnly);
  }catch(const coral::Exception& e){
    if(m_proxy) delete m_proxy;
    m_proxy = 0;
    throw cond::Exception( std::string("RelationalStorageManager::startTransaction:")+e.what() );
  }
}
void cond::RelationalStorageManager::commit(){
  coral::ITransaction& transaction = m_proxy->transaction();
  bool sharedConnection = m_proxy->isConnectionShared();
  bool activeTransaction = transaction.isActive();
  try {
    if(!activeTransaction && ! sharedConnection ) {
      if(m_proxy) delete m_proxy;
      m_proxy = 0;
      throw cond::Exception("RelationalStorageManager::commit cannot commit inactive transaction");
    }
    if ( ! sharedConnection ) transaction.commit();
  }catch(const coral::Exception& e){
    if(m_proxy) delete m_proxy;
    m_proxy = 0;
    throw cond::Exception( std::string("RelationalStorageManager::commit:")+e.what() );
  }
}
void cond::RelationalStorageManager::rollback(){
  coral::ITransaction& transaction = m_proxy->transaction();
  bool sharedConnection = m_proxy->isConnectionShared();
  bool activeTransaction = transaction.isActive();
  if ( activeTransaction ) {
    if ( ! sharedConnection ) transaction.rollback();
  }
}
