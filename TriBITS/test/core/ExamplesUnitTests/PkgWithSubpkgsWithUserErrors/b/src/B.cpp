#include "B.hpp"

#include "A.hpp"


std::string PkgWithSubpkgsWithUserErrors::getB() {
  return std::string("B");
}


std::string PkgWithSubpkgsWithUserErrors::depsB() {
  std::string B_deps;
  B_deps += (std::string("A ") + depsA() + std::string(" "));
  return B_deps;
}
