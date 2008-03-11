#include "CondCore/DBCommon/interface/ObjectRelationalMappingUtility.h"
#include "ObjectRelationalAccess/ObjectRelationalMappingUtilities.h"
#include "ObjectRelationalAccess/ObjectRelationalMappingPersistency.h"
#include "RelationalAccess/ISessionProxy.h"
cond::ObjectRelationalMappingUtility::ObjectRelationalMappingUtility( coral::ISessionProxy* coralsessionHandle ):m_coralsessionHandle(coralsessionHandle){
  m_mappingutil=new pool::ObjectRelationalMappingUtilities( coralsessionHandle );
}
cond::ObjectRelationalMappingUtility::~ObjectRelationalMappingUtility(){
  delete m_mappingutil;
}
void cond::ObjectRelationalMappingUtility::buildAndStoreMappingFromBuffer( const std::string& buffer ){
  m_mappingutil->buildAndMaterializeMappingFromBuffer( buffer.c_str(),false,false );
}
void cond::ObjectRelationalMappingUtility::buildAndStoreMappingFromFile( const std::string& filename ){
  m_mappingutil->buildAndMaterializeMapping( filename,"",false,false );
}

/*void cond::ObjectRelationalMappingUtility::listMappings( std::vector<std::string>& mappinglist ){
  mappinglist=m_mappingutil->listMappings();
}
*/
bool cond::ObjectRelationalMappingUtility::existsMapping(const std::string& version){
  pool::ObjectRelationalMappingPersistency mappingPersistency(m_coralsessionHandle->nominalSchema());
  if(!mappingPersistency.existsMappingDatabase()) return false;
  return m_mappingutil->existsMapping(version);
}

void cond::ObjectRelationalMappingUtility::removeMapping(const std::string& version,bool removeTables){
  m_mappingutil->removeMapping(version,removeTables);
}


