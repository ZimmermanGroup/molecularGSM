#include <iostream>

#include "PkgWithUserErrorsLib.hpp"

int main() {
  std::cout << "PkgWithUserErrorsLib returns "
            << PkgWithUserErrors::theThing() << "\n";
  return 0;
}
