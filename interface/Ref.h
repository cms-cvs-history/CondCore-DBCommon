#ifndef COND_REF_H
#define COND_REF_H
#include <string>
#include "DataSvc/Ref.h"
#include "POOLCore/Exception.h"
#include "StorageSvc/DbType.h"
#include "PersistencySvc/Placement.h"
#include "CondCore/DBCommon/interface/Exception.h"
namespace cond{
  class DBSession;
  /* 
     CMS simplified wrapper of pool::Ref smart pointer
  */
  template <typename T>
  class Ref{
  public:
    Ref( cond::DBSession& session, const std::string& token ):
      m_session(&session),
      m_data( pool::Ref<T>(&(session.DataSvc()), token) ),
      m_place(0){
    }
    Ref( cond::DBSession& session, T* obj ):
      m_session(&session),
      m_data( pool::Ref<T>(&(m_session.DataSvc()), obj) ),
      m_place(0){
    }
    virtual ~Ref(){
      if(m_place) delete m_place;
    }
    void markWrite( const std::string& containerName ) {
      try{
	if(!m_place){
	  m_place = new pool::Placement;
	  m_place->setTechnology(pool::POOL_RDBMS_HOMOGENEOUS_StorageType.type());
	  m_place->setDatabase(m_session.connectionString(), pool::DatabaseSpecification::PFN);
	  m_place->setContainerName(containerName);
	}
	m_data.markWrite(*m_place);
      }catch( const pool::Exception& er){
	throw cond::RefException("markWrite",er.what());
      }
    }
    void markUpdate(){
      try{
	m_data.markUpdate();
      }catch( const pool::Exception& er){
	throw cond::RefException("markUpdate",er.what());
      }
    }
    void markDelete(){
      try{
	m_data.markDelete();
      }catch( const pool::Exception& er){
	throw cond::RefException("markDelete",er.what());
      }
    }
    std::string token() const{
      return m_data.toString();
    }
    void clear( ){
      m_data.clear() ;
    }
    void reset( ){
      m_data.reset() ;
    }
    //user does not have ownership
    T* ptr() const{
      T* result=0;
      try{
	result=m_data.ptr();
      }catch(const pool::Exception& er){
	throw cond::RefException("ptr",er.what());
      }
      return result;
    }
    T* operator->() const{
      return this->ptr();
    }
    T& operator * () const{
      T& result;
      try{
	result=*m_data;
      }catch(const pool::Exception& er){
	throw cond::RefException( "operator * ",er.what() );
      }
    }
    // assignment operator
    /*Ref<T>& operator=(const Ref<T>& i){
      if(this->m_data != i.m_data ){ 
	m_session=i.m_session;
	this->m_data=i.m_data;
	m_place=i.m_place;
      }
      return *this;
    }
    */
  private:
    Ref<T>& operator=(const Ref<T>& i);
    cond::DBSession* m_session;
    pool::Ref<T> m_data;
    pool::Placement* m_place;
  };
}//ns cond
#endif
// COND_REF_H












