#ifndef MOPAC_H
#define MOPAC_H

#include "stringtools.h"
#include "pTable.h"
#include "icoord.h"

#define SKIPMOPAC 0

class Mopac {
  
  private:
  
   int natoms;
   int* anumbers;
   string* anames;
   int nfrz; //total frozen atoms
   int nfrz0; //total "moved" frozen atoms
   int* frzlist;
   int* frzlistb;
   int charge;

   int gradcalls; 
   int rnum;
   string id;

   void opt_header(ofstream& inpfile);
   void grad_header(ofstream& inpfile);
   void write_ic_input(ofstream& inpfile, int anum, ICoord icoords);
   double read_output(string filename);
   double read_grad(string filename);
   void xyz_read(string filename);
   void xyz_save(string filename);

  public:

   double opt();
   double opt(string filename);
   double opt(string filename, ICoord icoords);
   double grads(string filename);
   void alloc(int natoms);
   void init(int natoms, int* anumbers, string* anames, double* xyz);
   void reset(int natoms, int* anumbers, string* anames, double* xyz);
   void reset(double* xyz_i);
   void set_charge(int c0);
   void freemem();
   void write_xyz_grad(string filename);

   void freeze(int* frzlist, int nfrz, int nfrz0);

   double energy0;
   double energy;

   double* xyz0;
   double* xyz;
   double* grad;

};

#endif
