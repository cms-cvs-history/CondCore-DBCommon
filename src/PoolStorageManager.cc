#include "CondCore/DBCommon/interface/PoolStorageManager.h"
#include "CondCore/DBCommon/interface/Exception.h"
#include "CondCore/DBCommon/interface/ConnectMode.h"
#include "FileCatalog/URIParser.h"
#include "FileCatalog/FCSystemTools.h"
#include "FileCatalog/IFileCatalog.h"
#include "PersistencySvc/DatabaseConnectionPolicy.h"
#include "PersistencySvc/ISession.h"
#include "PersistencySvc/IDatabase.h"
#include "PersistencySvc/ITransaction.h"
#include "DataSvc/DataSvcFactory.h"
#include "DataSvc/IDataSvc.h"
#include "POOLCore/Exception.h"
cond::PoolStorageManager::PoolStorageManager(const std::string& con,
					     const std::string& catalog): m_catalogstr(catalog),m_con(con),m_cat(new pool::IFileCatalog),m_svc( pool::DataSvcFactory::instance(m_cat)),m_db(0){  
}
cond::PoolStorageManager::~PoolStorageManager(){
  delete m_cat;
  delete m_svc;
  if(m_db) delete m_db;
}
void cond::PoolStorageManager::connect(cond::ConnectMode mod){
  pool::DatabaseConnectionPolicy policy;
  switch(mod){
  case cond::ReadWriteCreate:
    policy.setWriteModeForNonExisting(pool::DatabaseConnectionPolicy::CREATE);
    policy.setWriteModeForExisting(pool::DatabaseConnectionPolicy::UPDATE);
    policy.setReadMode(pool::DatabaseConnectionPolicy::UPDATE);
    break;
  case cond::ReadWrite:
    policy.setWriteModeForNonExisting(pool::DatabaseConnectionPolicy::RAISE_ERROR);
    policy.setWriteModeForExisting(pool::DatabaseConnectionPolicy::UPDATE);
    policy.setReadMode(pool::DatabaseConnectionPolicy::UPDATE);
    break;
  case cond::ReadOnly:
    policy.setWriteModeForNonExisting(pool::DatabaseConnectionPolicy::RAISE_ERROR);
    policy.setWriteModeForExisting(pool::DatabaseConnectionPolicy::RAISE_ERROR);
    policy.setReadMode(pool::DatabaseConnectionPolicy::READ);
    break;
  default:
    throw cond::Exception(std::string("PoolStorageManager::connect unknown connect mode"));
  }
  if(mod==cond::ReadOnly){
    m_cat->addReadCatalog(m_catalogstr);
  }else{
    m_cat->setWriteCatalog(m_catalogstr);
  }
  m_cat->connect();
  m_cat->start();
  m_svc->session().setDefaultConnectionPolicy(policy);
}
void cond::PoolStorageManager::disconnect(){
  m_svc->session().disconnectAll();
  m_cat->commit();
  m_cat->disconnect();
}
void cond::PoolStorageManager::startTransaction(bool isReadOnly){
  if(!isReadOnly){
    m_svc->transaction().start( pool::ITransaction::UPDATE );
  }else{
    m_svc->transaction().start( pool::ITransaction::READ );
  }
}
void cond::PoolStorageManager::commit(){
  m_svc->transaction().commit();
}
void cond::PoolStorageManager::rollback(){
  m_svc->transaction().rollback();
}
std::string cond::PoolStorageManager::catalogStr() const{
  return m_catalogstr;
}
std::vector<std::string> cond::PoolStorageManager::containers(){
  if(!m_db){
    m_db=m_svc->session().databaseHandle(m_con, pool::DatabaseSpecification::PFN);
    if(!m_db){ throw cond::Exception( "PoolStorageManager::containers could not obtain database handle" ); }
  }
  return m_db->containers();
}
pool::IDataSvc& cond::PoolStorageManager::DataSvc(){
  return *m_svc;
}
