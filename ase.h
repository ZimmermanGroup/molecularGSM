#ifndef ASE_H
#define ASE_H

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

class ASE
{
  private:
  
  int nscffail;
  int firstrun;

   int runNum;
   int runend;
   string aseoutfile;
   string scrdir;
   string scrBaseDir;
   string runName;
   string runName0;
   string fileloc;

   int natoms;
   int* anumbers;
   string* anames;

   double get_energy_grad(string file, double* grad, int natoms);

  public:

   int CHARGE;
   int MULT;

   double grads(double* coords, double* grads);
   void alloc(int natoms);
   void init(string infilename, int natoms, int* anumbers, string* anames, int run, int rune);
   void freemem();
   void write_xyz_grad(double* coords, double* grad, string filename);

   int ncpu;
   int gradcalls;

   double energy0;
   double energy;

};

#endif
