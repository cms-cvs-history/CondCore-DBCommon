

std::string const tokens[] {



}

size_t N;

int main() {

  for (size_t i=0; i<N) {
    std::string const & token = tokens[i];
    const pool::Guid guid(cond::classID(token));
    const ROOT::Reflex::Type type = pool::DbReflex::forGuid(guid);
    if (!type) {
      if (!cond::loadClassByToken(token)) 
	std::cout << "dict unknown for " << token << std::endl;
      type = pool::DbReflex::forGuid(guid);
      if (!type)  
	std::cout << "problem with DbReflex for " << token << std::endl;
    }
    std::cout << "class " << type.Name(ROOT::Reflex::SCOPED)
	      << " for " << token << std::endl;
  }

  return 0;

}
