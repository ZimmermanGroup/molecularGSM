#ifndef XTB_H
#define XTB_H

#include <sys/stat.h>
#include "stringtools.h"
#include "pTable.h"
#include "icoord.h"

class XTB {
  
  private:
  
   int natoms;
   int* anumbers;
   string* anames;
   int charge;
   int nfrz; //total frozen atoms
   int nfrz0; //total "moved" frozen atoms
   int* frzlist;
   int* frzlistb;

   int nskip;
   int* skip;

   void write_ic_input(ofstream& inpfile, int anum, ICoord icoords);

  public:

   string sdir;

   double grads(string filename);
   double opt();
   double opt(string filename);
   double opt_check(string filename);
   double opt(string filename, ICoord icoords);
   void opt_write();
   void opt_write(string filename);
   void opt_write(string filename, ICoord icoords);

   double read_grad(string filename);
   double read_output(string filename);
   void xyz_read(string filename);
   void xyz_read_aux(string filename);
   void xyz_save(string filename);


   void set_charge(int c0);

   void alloc(int natoms);
   void init(int natoms, int* anumbers, string* anames, double* xyz);
   void reset(int natoms, int* anumbers, string* anames, double* xyz);
   void freemem();

   void freeze(int* frzlist, int nfrz, int nfrz0);
   void freeze_d(int* frzlist);

   double energy0;
   double energy;

   double* xyz0;
   double* xyz;
   double* grad;

};

#endif
