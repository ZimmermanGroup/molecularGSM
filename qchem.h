#ifndef QCHEM_H
#define QCHEM_H

#define QCHEM 0
#define QCHEMSF 1
#define USE_MOLPRO 0
#define USE_ASE 0
#define USE_GAUSSIAN 0
#define USE_ORCA 0
#define THREADS_ON 1
#define WRITE_FILES 0
#define USE_KNNR 0
#define KNN_K 3
#define KNNR_MAX_DIST 0.3

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

class QChem {
  
  private:
  
  int nscffail;
  int firstrun;

   int runNum;
   int runend;
  string qcinfile;   
  string inpfile;  
  string qcoutfile;
  string qcoutfileh;
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

};

#endif
