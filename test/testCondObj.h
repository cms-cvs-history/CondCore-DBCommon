#ifndef testCondObj_H
#define testCondObj_H

#include <map>
#include <string>
  class testCondObj {
  public:
    testCondObj(){}
    virtual ~testCondObj(){}
    std::map<unsigned long long,std::string> data;
  };
#endif
