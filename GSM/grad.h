#ifndef GRAD_H
#define GRAD_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdio>
#include <vector>
#include <cstring>
#include <math.h>

#include "stringtools.h"
#include "qchem.h"
#include "gaussian.h"
#include "ase.h"
#include "knnr.h"
#include "orca.h"
#include "molpro.h"
#include "qchemsf.h"

class Gradient 
{
  
  private:
  
   int runNum;
   int runend;
   string runends;
   string runName;
   string runName0;

   int natoms;
   int N3;
   string* anames;
   int* anumbers;
   int CHARGE; //total system charge
   int MULT; //multiplicity

   QChem qchem1;
   QChemSF qchemsf1;
   GAUSSIAN gaus1;
   ASE ase1;
   ORCA orca1;
   Molpro mp1;

   int knn_k;
   KNNR knnr1;
   int knnr_inited;

   int nforce;
   int* fa;
   double* fv;
   double* fk;

   int read_nstates();
   void read_molpro_settings(int& nstates, int& nclosed, int& nocc, int& nelec, string& basis);
   int read_molpro_init(string* &hf_lines);
   int force_init(string ffile);


  public:

   int hessian(double* H);
   double grads(double* coords, double* grad, double* Ut, int type);
   void add_force(double* coords, double* grad);
   void init(string infilename, int natoms, int* anumbers, string* anames, double* coords, int run, int rune, int ncpu, int use_knnr, int q1);
   void update_knnr();
   void freemem();
   void write_xyz_grad(double* coords, double* grad, string filename);
   int external_grad(double* coords, double* grads);

   int knnr_active;
   int always_do_exact;
   int write_on;
   int wrote_grad;
   int xyz_grad;
   int gradcalls;
   int nscffail;
   double V0;
   double fdE; //force * distance energy

   double energy0;
   double energy;

  //for molpro
   int nstates;
   int wstate;
   int wstate2;
   int wstate3;
   double** grada; //multistate gradients
   double* E; //for multiple states

   int seedType;
   
   int res_t; //restart found files

};

#endif
