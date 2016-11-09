#include <iostream>
#include <fstream>
#include <stdio.h>

#include "gstring.h"


using namespace std;


int main(int argc, char* argv[]){
  string inpfile;
  string xyzfile;
  string nprocs;
  switch (argc){
  case 1:
    inpfile="inpfileq";
    xyzfile="initial.xyz";
    nprocs="1";
    break;
  case 2:
    inpfile="inpfileq";
    xyzfile=argv[1];
    nprocs="1";
    break;
  case 3:
    inpfile="inpfileq";
    xyzfile=argv[1];
    nprocs=argv[2];
    break;
  default:
    cout << "Invalid command line options." << endl;
    return -1;
  }

  int nnprocs = atoi(nprocs.c_str());
  printf(" Number of QC processors: %i \n",nnprocs);
  int name = atoi(xyzfile.c_str());
  GString gstr;
  gstr.init(inpfile, name, nnprocs);
  gstr.String_Method_Optimization();


  return 0;
}
