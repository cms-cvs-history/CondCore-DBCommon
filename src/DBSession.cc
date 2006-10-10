#include "CondCore/DBCommon/interface/DBSession.h"
#include "CondCore/DBCommon/interface/Exception.h"
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
cond::DBSession::DBSession( const std::string& con ):m_con(con),m_cat(new pool::IFileCatalog),m_svc( pool::DataSvcFactory::instance(m_cat)), m_catalogcon(""),m_db(0){}
cond::DBSession::DBSession( const std::string& con, 
			    const std::string& catalogcon )
  :m_con(con),m_cat(new pool::IFileCatalog),m_svc( pool::DataSvcFactory::instance(m_cat)), m_catalogcon(catalogcon), m_db(0){}
cond::DBSession::~DBSession(){
  delete m_cat;
  delete m_svc;
  if(m_db) delete m_db;
}
void cond::DBSession::setCatalog( const std::string& catalogCon ){
  m_catalogcon=catalogCon;
}
void cond::DBSession::connect( cond::ConnectMode mode ){
  pool::DatabaseConnectionPolicy policy;
  switch(mode){
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
    throw cond::Exception(std::string("DBSession::connect unknown connect mode"));
  }
  //pool::URIParser p(m_catalogcon);
  //p.parse();
  //m_cat->setWriteCatalog(p.contactstring());
  if(mode==cond::ReadOnly){
    m_cat->addReadCatalog(m_catalogcon);
  }else{
    m_cat->setWriteCatalog(m_catalogcon);
  }
  m_cat->connect();
  m_cat->start();
  m_svc->session().setDefaultConnectionPolicy(policy);
}
void cond::DBSession::disconnect(){
  m_svc->session().disconnectAll();
  m_cat->commit();
  m_cat->disconnect();
}
void cond::DBSession::startUpdateTransaction(){
  try{
    m_svc->session().transaction().start(pool::ITransaction::UPDATE);
  }catch(const pool::Exception& er){
    throw cond::Exception(std::string("DBSession::startUpdate caught pool::Exception ")+er.what());
  }catch(...){
    throw cond::Exception(std::string("DBSession::startUpdate caught unknown exception in "));
  }
}
void cond::DBSession::startReadOnlyTransaction(){
  try{
    m_svc->session().transaction().start(pool::ITransaction::READ);
  }catch(const pool::Exception& er){
    throw cond::Exception(std::string("DBSession::startReadOnly caught pool::Exception ")+er.what());
  }catch(...){
    throw cond::Exception(std::string("DBSession::startReadOnly caught unknown exception in "));
  }
}
void cond::DBSession::rollback(){
  m_svc->session().transaction().rollback();
}
void cond::DBSession::commit(){
  try{
    m_svc->session().transaction().commit();
  }catch( const pool::Exception& er){
    throw cond::Exception( std::string("DBSession::commit caught pool::Exception ")+ er.what() );
  }catch( ... ){
    throw cond::Exception( std::string("DBSession::commit caught unknown exception ") );
  }
}
const std::string cond::DBSession::connectionString() const{
  return m_con;
}
std::vector<std::string> cond::DBSession::containers(){
  if(!m_db){
    m_db=m_svc->session().databaseHandle(m_con, pool::DatabaseSpecification::PFN);
    if(!m_db){ throw cond::Exception( "DBSession::containers could not obtain database handle" ); }
}
return m_db->containers();
}
pool::IDataSvc& cond::DBSession::DataSvc() const{
  return *m_svc;
}
pool::IFileCatalog& cond::DBSession::Catalog() const{
  return *m_cat;
}
