//
// unit test, include files as they exists only in the plugin...
//
#define private public
#include "CondCore/DBCommon/plugins/BlobStreamingService.cc"
#undef private
#include "CondCore/DBCommon/plugins/TBufferBlobStreamingService.cc"


#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>

int main() {

  using namespace cond;

  BlobStreamingService streamer;

  // white box tests...
  for (size_t i=0; i!=BlobStreamingService::nVariants; ++i)
    std::cout << i << " " << std::hex << BlobStreamingService::variantIds[i].first << "-" 
	      << BlobStreamingService::variantIds[i].second << std::dec << std::endl;
  
  std::vector<unsigned char> crap(1024);
  for(size_t i=0; i!=crap.size(); ++i)
    crap[i] = i%256;
  crap[3]=5; crap[10]=123;

  BlobStreamingService::Variant id = BlobStreamingService::findVariant(&crap.front());
  std::cout << "shall be zero " << id << std::endl;

  *reinterpret_cast<BlobStreamingService::uuid*>(&crap.front()) = BlobStreamingService::variantIds[BlobStreamingService::COMPRESSED_TBUFFER];
  id = BlobStreamingService::findVariant(&crap.front());
  std::cout << "shall be one " << id << std::endl;

  boost::shared_ptr<coral::Blob> blob1 = BlobStreamingService::compress(&crap.front(), crap.size());
  unsigned long long s = *reinterpret_cast<unsigned long long*>((unsigned char*)(blob1->startingAddress())+ BlobStreamingService::m_idsize);
  std::cout << "compressed " <<  blob1->size() << " " << s << std::endl;

  boost::shared_ptr<coral::Blob> blob2 = BlobStreamingService::expand(*blob1);
  std::cout << "expanded (shall be " << crap.size() <<  ") "<<  blob2->size() << std::endl;
  std::vector<unsigned char> crap2(1024); 
  ::memcpy(&crap2.front(),blob2->startingAddress(),blob2->size());
  if (crap!=crap2) std::cout << "compress - expansion failed" << std::endl;

  // back-box test
  std::vector<float>  vf(1024);
  for(size_t i=0; i!=vf.size(); ++i)
    vf[i] = float(i)+float(i)/1000.;
      
  Reflex::Type vType = Reflex::Type::ByTypeInfo( typeid(std::vector<float>));
  
  {
    // old mode
    cond::TBufferBlobStreamingService tstreamer;
    boost::shared_ptr<coral::Blob> blob = tstreamer.write(&vf,vType);
    std::cout << "old format size " << blob.size() << std::endl;
    BlobStreamingService::Variant id = BlobStreamingService::findVariant(blob->startingAddress());
    std::cout << "shall be zero " << id << std::endl;

    void * p;
    streamer.read(blob,p,vType);
    std::vector<float>  const & vf2 = *reinterpret_cast<std::vector<float> >(p);
    if (vf!=vf2) std::cout << "reading old format failed" << std::endl;
  }
  {
    // new mode
    boost::shared_ptr<coral::Blob> blob = streamer.write(&vf,vType);
    std::cout << "new format size " << blob.size() << std::endl;
    BlobStreamingService::Variant id = BlobStreamingService::findVariant(blob->startingAddress());
    std::cout << "shall be one " << id << std::endl;
    void * p;
    streamer.read(blob,p,vType);
    std::vector<float>  const & vf2 = *reinterpret_cast<std::vector<float> >(p);
    if (vf!=vf2) std::cout << "reading new format failed" << std::endl;
  }
  

  return 0;

}
