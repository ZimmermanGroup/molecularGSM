#ifndef TURBOMOLE_H
#define TURBOMOLE_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdio>
#include <vector>
#include <cstring>
#include <math.h>

#include "stringtools.h"
#include "pTable.h"
#include "constants.h"
#include "utils.h"

class Turbomole {
  
  private:
  
  int nscffail;
  int firstrun;

   int runNum;
   int runend;
  string turboinfile;   
  string inpfile;  
  string turbooutfile;
  string turbooutfileh;
  string scrdir;  
  string scrBaseDir;
  string runName;
  string runName0;
  string fileloc;

   int natoms;
   int* anumbers;
   string* anames;

   double read_output(string filename);
   double read_grad(string filename);
   void xyz_read(string filename);
   void xyz_save(string filename);
   double get_energy(string filename);
   int scangradient(string file, double* grad, int natoms);

  public:

   int read_hess(double* hess);
   double grads(double* coords, double* grads);
   void alloc(int natoms);
   void init(string infilename, int natoms, int* anumbers, string* anames, int run, int rune);
   void freemem();
   void write_xyz_grad(double* coords, double* grad, string filename);

   int ncpu;
   int gradcalls;

   double energy0;
   double energy;

   bool RI;
   bool COSMO;
   string turboDIR;

};

#endif
