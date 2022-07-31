#ifndef PKGWITHSUBPKSWITHUSERERRORS_A_HPP_
#define PKGWITHSUBPKSWITHUSERERRORS_A_HPP_

#include <string>

namespace PkgWithSubpkgsWithUserErrors {

  // return a string containing "A"
  std::string getA();

  // return a string describing the dependencies of "A", recursively
  std::string depsA();

}


#endif /* PKGWITHSUBPKSWITHUSERERRORS_A_HPP_ */
