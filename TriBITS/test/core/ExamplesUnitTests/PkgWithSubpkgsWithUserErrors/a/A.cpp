#include "A.hpp"

std::string PkgWithSubpkgsWithUserErrors::getA() {
  return std::string("A");
}

std::string PkgWithSubpkgsWithUserErrors::depsA() {
  return "no_deps_for_A";
}
