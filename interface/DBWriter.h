#ifndef COND_DBWRITER_H
#define COND_DBWRITER_H
#include <string>
#include "DataSvc/Ref.h"
#include "POOLCore/Exception.h"
#include "CondCore/DBCommon/interface/Exception.h"
namespace cond{
  class DBSession;
  class DBWriter{
  public:
    DBWriter( cond::DBSession& session,
	      const std::string& containerName );
    DBWriter( cond::DBSession& session,
	      const std::string& containerName,
	      const std::string& mappingFileName);
    ~DBWriter();
    /**pin the object into the object cache
     */
    template<typename ObjType>
    std::string markWrite( ObjType* obj ){
      pool::Ref<ObjType> myref(&(m_session.DataSvc()), obj);
      try{
	myref.markWrite(*m_placement);
      }catch( const pool::Exception& er){
	throw cond::Exception( std::string("caught pool::Exception ")+ er.what() );
      }catch ( const std::exception& er ) {
	throw cond::Exception( std::string("caught std::exception ")+ er.what() );
      }catch ( ... ) {
	throw cond::Exception( std::string("caught unknown exception "));
      }
      return myref.toString();
    }
    template<typename ObjType>
      void markDelete(const std::string& objToken ){
      pool::Ref<ObjType> myref(&(m_session.DataSvc()),objToken);
      if(!myref){
	throw cond::Exception( std::string("object with token")+objToken+" not found " );
      }
      myref.markDelete();
    }  
  private:
    cond::DBSession& m_session;
    const std::string m_containerName;
    pool::Placement* m_placement;
    std::string m_mappinginput;
  };
}
#endif
// DBWRITER_H













