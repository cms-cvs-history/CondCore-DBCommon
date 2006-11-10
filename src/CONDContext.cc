#include "CondCore/DBCommon/interface/CONDContext.h"
#include "POOLCore/POOLContext.h"
#include "SealKernel/Context.h"
#include <vector>
seal::Context*
cond::CONDContext::getPOOLContext(){
  return pool::POOLContext::context();
}
seal::Context*
cond::CONDContext::getOwnContext(){
  static seal::Handle< seal::Context > s_context = new seal::Context;
  return s_context.get();
}
