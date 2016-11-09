#ifndef QCHEMSF_H
#define QCHEMSF_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdio>
#include <vector>
#include <cstring>
#include <math.h>

#include "qchem.h"
#include "stringtools.h"
#include "pTable.h"
#include "constants.h"
#include "utils.h"

class QChemSF {
  
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


   int nstates;
   double* grad1;
   double* grad2;
   double* grad3;
   double* grad4;
   double* E;

   double read_output(string filename);
   double read_grad(string filename);
   void xyz_read(string filename);
   void xyz_save(string filename);
   double get_energy();
   void get_grads();
   int scangradient(string file, double* grad, int natoms);

  public:

   int read_hess(double* hess);
   double calc_grads(double* coords);
   double getE(int ws);
   void getGrad(int ws, double* grad);
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
