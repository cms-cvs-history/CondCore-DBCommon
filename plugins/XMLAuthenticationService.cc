#include "CondCore/DBCommon/interface/DecodingKey.h"
#include "CondCore/DBCommon/interface/FileUtils.h"
#include "CondCore/DBCommon/interface/Exception.h"
#include "CoralCommon/AuthenticationCredentials.h"
#include "CoralCommon/Cipher.h"
#include "RelationalAccess/AuthenticationServiceException.h"
#include "CoralKernel/IPropertyManager.h"
#include "CoralKernel/IProperty.h"
//#include "CoralKernel/MessageStream.h"
#include "CoralKernel/Context.h"
//#include "CoralKernel/CoralPluginDef.h"
#include "CondCore/DBCommon/interface/CoralServiceMacros.h"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/framework/MemBufInputSource.hpp"
#include "xercesc/dom/DOM.hpp"
#include "xercesc/sax/HandlerBase.hpp"
#include "xercesc/util/XMLString.hpp"
#include "xercesc/util/PlatformUtils.hpp"
#include "XMLAuthenticationService.h"


#include <memory>
#include <cstdlib>
#include <fstream>
#include <sys/stat.h>
#include <fcntl.h>
#include <boost/filesystem.hpp>
//#include <iostream>
static std::string XML_AUTHENTICATION_FILE("authentication.xml");


namespace cond {

  namespace XMLAuthenticationService {
    /*
    class CallbackForInputFileName {
      public:
        explicit CallbackForInputFileName( cond::XMLAuthenticationService::XMLAuthenticationService& authenticationService )
          : m_authenticationService( authenticationService )
        {
        }

        void operator() ( const coral::Property& p )
        {
          std::string fileName = boost::any_cast< std::string >( p.get() );
          m_authenticationService.setInputFileName( fileName );
        }
      private:
        cond::XMLAuthenticationService::XMLAuthenticationService& m_authenticationService;
    };
    */ 
  }  
}

cond::XMLAuthenticationService::DataSourceEntry::DataSourceEntry( const std::string& serviceName,
                                                                  const std::string& connectionName ):
  m_serviceName( serviceName ),
  m_connectionName( connectionName ),
  m_default( new coral::AuthenticationCredentials( serviceName ) ),
  m_data()
{}

cond::XMLAuthenticationService::DataSourceEntry::~DataSourceEntry()
{
  delete m_default;
  for ( std::map< std::string, coral::AuthenticationCredentials* >::iterator iData = m_data.begin();
        iData != m_data.end(); ++iData )
    delete iData->second;
}

void
cond::XMLAuthenticationService::DataSourceEntry::appendCredentialItem( const std::string& item,
                                                                       const std::string& value )
{
  m_default->registerItem( item, value );
}

void
cond::XMLAuthenticationService::DataSourceEntry::appendCredentialItemForRole( const std::string& item,
                                                                              const std::string& value,
                                                                              const std::string& role )
{
  std::map< std::string, coral::AuthenticationCredentials* >::iterator iRole = m_data.find( role );
  if ( iRole == m_data.end() ) {
    iRole = m_data.insert( std::make_pair( role, new coral::AuthenticationCredentials( m_serviceName ) ) ).first;
  }
  iRole->second->registerItem( item, value );
}

const coral::IAuthenticationCredentials&
cond::XMLAuthenticationService::DataSourceEntry::credentials() const
{
  return *m_default;
}

const coral::IAuthenticationCredentials&
cond::XMLAuthenticationService::DataSourceEntry::credentials( const std::string& role ) const
{
  std::map< std::string, coral::AuthenticationCredentials* >::const_iterator iRole = m_data.find( role );
  if ( iRole == m_data.end() )
    throw coral::UnknownRoleException( m_serviceName,
                                       m_connectionName,
                                       role );
  return *( iRole->second );
}

cond::XMLAuthenticationService::XMLAuthenticationService::XMLAuthenticationService( const std::string& key )
  : coral::Service( key ),
    m_isInitialized( false ),
    m_inputFileName( "" ),
    m_data()
{
  coral::Context::instance().PropertyManager().property("AuthenticationFile")->set(XML_AUTHENTICATION_FILE);
}

cond::XMLAuthenticationService::XMLAuthenticationService::~XMLAuthenticationService()
{
  for ( std::map< std::string, cond::XMLAuthenticationService::DataSourceEntry* >::iterator iConnection = m_data.begin();
        iConnection != m_data.end(); ++iConnection ) delete iConnection->second;
}

void
cond::XMLAuthenticationService::XMLAuthenticationService::setInputFileName(  const std::string& inputFileName )
{
  m_inputFileName = inputFileName;
  reset();
}

bool
cond::XMLAuthenticationService::XMLAuthenticationService::processFile( const std::string& inputFileName )
{
  //seal::MessageStream log( this, this->name(),seal::Msg::Verbose );
  //log << seal::Msg::Debug << "Processing file \""<< inputFileName<<"\"" << seal::flush;
  bool result = true;

  cond::FileReader inputFile;
  std::string cont("");
  try{
    inputFile.read(inputFileName);
    cont = inputFile.content();
  } catch (const cond::Exception& exc){
    //seal::MessageStream log( this, this->name(),seal::Msg::Verbose );
    //log << seal::Msg::Error << exc.what() << seal::flush;
    return false;
  }
  
  // check the 
  boost::filesystem::path filePath( inputFileName );
  std::string name = filePath.leaf();
  if(name!=XML_AUTHENTICATION_FILE){
    cond::DecodingKey key;
    try{
      key.readUserKeyString(cont);

      //seal::MessageStream log( this, this->name(),seal::Msg::Verbose );
      //log << seal::Msg::Debug << "Decoding content of file \""<< key.dataSource()<<"\"" << seal::flush;
        
      cond::FileReader dataFile;
      dataFile.read(key.dataSource());
      cont = dataFile.content();
      cont = coral::Cipher::decode(cont,key.key());      
    } catch (const cond::Exception& exc){
      //seal::MessageStream log( this, this->name(),seal::Msg::Verbose );
      //log << seal::Msg::Error << exc.what() << seal::flush;
      return false;
    }
    
  } else {
    //seal::MessageStream log( this, this->name(),seal::Msg::Verbose );
    //log << seal::Msg::Debug << "Authentication file is expected standard XML."<< seal::flush;    
  }
  
  xercesc::MemBufInputSource* memBufInputSource = 0;
  
  try
  {
    xercesc::XercesDOMParser parser;
    parser.setValidationScheme( xercesc::XercesDOMParser::Val_Always );
    parser.setDoNamespaces( true );
    
    xercesc::HandlerBase errorHandler; 
    parser.setErrorHandler( &errorHandler );

    
    const char* bufferId="InMemoryDocument";
    const char* buffer = cont.c_str();

    memBufInputSource = new xercesc::MemBufInputSource( (const XMLByte*)buffer,strlen(buffer),bufferId,false );

    parser.parse(*memBufInputSource);

    xercesc::DOMDocument* document = parser.getDocument();

    XMLCh tempStr[20];
    xercesc::XMLString::transcode( "connection", tempStr, 19);

    xercesc::DOMNodeList* connectionList = document->getElementsByTagName( tempStr );
    
    if ( connectionList )
    {
      XMLSize_t numberOfConnections = connectionList->getLength();
      
      for ( XMLSize_t iConnection = 0; iConnection < numberOfConnections; ++iConnection )
      {
        xercesc::DOMNode* connectionNode = connectionList->item( iConnection );

        if ( connectionNode )
        {
          char*       connectionName  = xercesc::XMLString::transcode( connectionNode->getAttributes()->item( 0 )->getNodeValue() );
          std::string sConnectionName = connectionName;
          xercesc::XMLString::release( &connectionName );

          // Locate the credential
          cond::XMLAuthenticationService::DataSourceEntry* credential = 0;
          std::map< std::string, cond::XMLAuthenticationService::DataSourceEntry* >::iterator iConnection = m_data.find( sConnectionName );
          if ( iConnection != m_data.end() ) {
            credential = iConnection->second;
            // Issue a warning here.
            //coral::MessageStream log( this, this->name(),seal::Msg::Verbose );
            //log << coral::Msg::Warning << "Credential parameters for connection string \""
            //    << sConnectionName
            //    << "\" have already been defined. Only new elements are appended, while existing will be ignored."
            //    << coral::flush;
          } else {
            credential = new cond::XMLAuthenticationService::DataSourceEntry( this->name(), sConnectionName );
            m_data.insert( std::make_pair( sConnectionName, credential ) );
          }

          xercesc::DOMNodeList* parameterList = connectionNode->getChildNodes();
          
          if ( parameterList )
          {
            XMLSize_t numberOfParameters = parameterList->getLength();
            
            for ( XMLSize_t iParameter = 0; iParameter < numberOfParameters; ++iParameter )
            {
              xercesc::DOMNode* parameterNode = parameterList->item( iParameter );

              if ( parameterNode && parameterNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE )
              {
                char* nodeName = xercesc::XMLString::transcode( parameterNode->getNodeName() );
                std::string sNodeName = nodeName;
                xercesc::XMLString::release( &nodeName );
                
                if ( sNodeName == "parameter" ) { // The default parameters
                  char* parameterName = xercesc::XMLString::transcode( parameterNode->getAttributes()->item( 0 )->getNodeValue() );
                  std::string sParameterName = parameterName;
                  xercesc::XMLString::release( &parameterName );
                  char* parameterValue = xercesc::XMLString::transcode( parameterNode->getAttributes()->item( 1 )->getNodeValue() );
                  std::string sParameterValue = parameterValue;
                  xercesc::XMLString::release( &parameterValue );

                  credential->appendCredentialItem( sParameterName, sParameterValue );
                }
                else if ( sNodeName == "role" ) { // A role
                  char* roleName  = xercesc::XMLString::transcode( parameterNode->getAttributes()->item( 0 )->getNodeValue() );
                  std::string sRoleName = roleName;
                  xercesc::XMLString::release( &roleName );

                  // Retrieve the parameters for the role
                  xercesc::DOMNodeList* roleParameterList = parameterNode->getChildNodes();


                  if ( roleParameterList )
                  {
                    XMLSize_t numberOfRoleParameters = roleParameterList->getLength();
            
                    for ( XMLSize_t iRoleParameter = 0; iRoleParameter < numberOfRoleParameters; ++iRoleParameter )
                    {
                      xercesc::DOMNode* roleParameterNode = roleParameterList->item( iRoleParameter );
                      if ( roleParameterNode && roleParameterNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE )
                      {
                        char* roleNodeName = xercesc::XMLString::transcode( roleParameterNode->getNodeName() );
                        std::string sRoleNodeName = roleNodeName;
                        xercesc::XMLString::release( &roleNodeName );
                        
                        if ( sRoleNodeName == "parameter" ) {
                          char* roleParameterName = xercesc::XMLString::transcode( roleParameterNode->getAttributes()->item( 0 )->getNodeValue() );
                          std::string sRoleParameterName = roleParameterName;
                          xercesc::XMLString::release( &roleParameterName );
                          char* roleParameterValue = xercesc::XMLString::transcode( roleParameterNode->getAttributes()->item( 1 )->getNodeValue() );
                          std::string sRoleParameterValue = roleParameterValue;
                          xercesc::XMLString::release( &roleParameterValue );
                          
                          credential->appendCredentialItemForRole( sRoleParameterName, sRoleParameterValue, sRoleName );
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }

    parser.reset();
  }
  catch ( const xercesc::XMLException& toCatch )
  {
    char* message = xercesc::XMLString::transcode( toCatch.getMessage() );
    //coral::MessageStream log( this, this->name(),coral::Msg::Verbose );
    //log << coral::Msg::Error << message << coral::flush;
    xercesc::XMLString::release( &message );
    result = false;
  }
  catch ( const xercesc::DOMException& toCatch )
  {
    char* message = xercesc::XMLString::transcode( toCatch.msg );
    //seal::MessageStream log( this, this->name(),seal::Msg::Verbose );
    //log << seal::Msg::Error << message << seal::flush;
    xercesc::XMLString::release( &message );
    result = false;
  }
  catch ( const xercesc::SAXException& toCatch )
  {
    char* message = xercesc::XMLString::transcode( toCatch.getMessage() );
    //seal::MessageStream log( this, this->name(),seal::Msg::Verbose );
    //log << seal::Msg::Error << message << seal::flush;
    xercesc::XMLString::release( &message );
    result = false;
  }
  catch (...){
    //seal::MessageStream log( this, this->name(),seal::Msg::Verbose );
    //log << seal::Msg::Error << "Unexpected Exception parsing file \"" << inputFileName << "\"" << seal::flush;
    result = false;
  }
  if(memBufInputSource) delete memBufInputSource;
  return result;
}


bool
cond::XMLAuthenticationService::XMLAuthenticationService::initialize()
{  
  std::set< std::string > inputFileNames = this->verifyFileName();
  if ( inputFileNames.empty() )
  {
    //seal::MessageStream log( this, this->name(),seal::Msg::Verbose );
    //log << seal::Msg::Error << "Could not open \"" << m_inputFileName << "\" for reading" << seal::flush;
    return false;
  }

  try
  {
    xercesc::XMLPlatformUtils::Initialize();
  }
  catch ( const xercesc::XMLException& toCatch )
  {
    char* message = xercesc::XMLString::transcode( toCatch.getMessage() );
    //seal::MessageStream log( this, this->name(),seal::Msg::Verbose );
    //log << seal::Msg::Error << message << seal::flush;
    xercesc::XMLString::release( &message );
    return false;
  }

  bool result = false;
  for ( std::set< std::string >::const_reverse_iterator iFileName = inputFileNames.rbegin();
        iFileName != inputFileNames.rend(); ++iFileName ) {
    if ( this->processFile( *iFileName ) ) {
      result = true;
    }
  }

  xercesc::XMLPlatformUtils::Terminate();

  m_isInitialized = result;
  if(!m_isInitialized) reset();
  return result;
}

void cond::XMLAuthenticationService::XMLAuthenticationService::reset(){
  for ( std::map< std::string, cond::XMLAuthenticationService::DataSourceEntry* >::iterator iConnection = m_data.begin();
        iConnection != m_data.end(); ++iConnection ) delete iConnection->second;
  m_data.clear();
  m_isInitialized = false;  
}



const coral::IAuthenticationCredentials&
cond::XMLAuthenticationService::XMLAuthenticationService::credentials( const std::string& connectionString ) const
{
  boost::mutex::scoped_lock lock(m_mutexLock);
  if ( ! m_isInitialized ) {
    const_cast< cond::XMLAuthenticationService::XMLAuthenticationService* >( this )->initialize();
  }
  std::map< std::string, cond::XMLAuthenticationService::DataSourceEntry* >::const_iterator iConnection = m_data.find( connectionString );
  if ( iConnection == m_data.end() )
    throw coral::UnknownConnectionException( this->name(), connectionString );
  return iConnection->second->credentials();
}


const coral::IAuthenticationCredentials&
cond::XMLAuthenticationService::XMLAuthenticationService::credentials( const std::string& connectionString,
                                                                       const std::string& role ) const
{
  boost::mutex::scoped_lock lock(m_mutexLock);
  if ( ! m_isInitialized ) {
    const_cast< cond::XMLAuthenticationService::XMLAuthenticationService* >( this )->initialize();
  }
  std::map< std::string, cond::XMLAuthenticationService::DataSourceEntry* >::const_iterator iConnection = m_data.find( connectionString );
  if ( iConnection == m_data.end() )
    throw coral::UnknownConnectionException( this->name(), connectionString );
  return iConnection->second->credentials( role );
}


std::set< std::string >
cond::XMLAuthenticationService::XMLAuthenticationService::verifyFileName()
{

  {
    //seal::MessageStream log( this, this->name(),seal::Msg::Verbose );
    //log << seal::Msg::Debug << "Verifying file name: \"" << m_inputFileName << "\"." << seal::flush;
  }
  
  std::set< std::string > fileNames;

  // Try the file name as is...
  boost::filesystem::path filePath( m_inputFileName );
  if ( boost::filesystem::exists( m_inputFileName ) ) {
    if(boost::filesystem::is_directory( m_inputFileName )){
      //seal::MessageStream log( this, this->name(),seal::Msg::Verbose );
      //log << seal::Msg::Error << "Provided path \"" << m_inputFileName << "\" is a directory." << seal::flush;
      return fileNames;
    }
    boost::filesystem::path& fullPath = filePath.normalize();
    fileNames.insert( fullPath.string() );
    if(filePath.is_complete()) return fileNames;
  }
  
  // Try to find other files in the path variable
  const char* thePathVariable = ::getenv( "CORAL_AUTH_PATH" );
  if ( ! thePathVariable ) return fileNames;
  //seal::MessageStream log( this, this->name(),seal::Msg::Verbose );
  //log << seal::Msg::Debug << "File \"" << m_inputFileName << "\" not found in the current directory. Trying in the search path." << seal::flush;

  std::string searchPath(thePathVariable);
  if(boost::filesystem::exists(searchPath)){
    if(!boost::filesystem::is_directory( searchPath )){
      //seal::MessageStream log( this, this->name(),seal::Msg::Verbose );
      //log << seal::Msg::Error << "Search path \"" << searchPath << "\" is not a directory." << seal::flush;
      return fileNames;
    }
    boost::filesystem::path fullPath( searchPath );
    fullPath /= filePath;
    fileNames.insert( fullPath.string() );
  } else {
    //seal::MessageStream log( this, this->name(),seal::Msg::Verbose );
    //log << seal::Msg::Error << "Search path \"" << searchPath << "\" does not exist." << seal::flush;
    return fileNames;    
  }
  
  return fileNames;
}

DEFINE_CORALSERVICE(cond::XMLAuthenticationService::XMLAuthenticationService,"COND/Services/XMLAuthenticationService");