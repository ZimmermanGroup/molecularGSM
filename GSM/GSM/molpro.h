#ifndef MOLPRO_H
#define MOLPRO_H


#include <iostream>
#include <fstream>
#include <stdio.h>

#include "stringtools.h"
#include "utils.h"

class Molpro
{

  private:

    string infile;
    string outfile;
    string scratchname;

    int nhf_lines; 
    string* hf_lines;

    string basis;
 
    int nclosed;
    int nocc;
    int nelec;

    int nstates;
    int natoms;
   
    int tstate;

    string* anames;
    double* E;
    double* xyz;
    double* grad;
    double* dvec;
    
    int read_E();

    int NPROCS;

  public:

    void init(int nstates0, int nclosed0, int nocc0, int nelec0, int natoms0, string* anames0, double* xyz0, int NPROCS0, string basis0);
    void init_hf(int nhf_lines1, string* hf_lines1);
    void reset(double* xyz);
    void runname(string name);
    int seed(); //run RHF to get initial orbitals
    int run(int n, int m); // n is current state, m is target state for derivative coupling
    double getE(int n);
    int getGrad(double* grads);
    int getDVec(double* D);
    void clean_scratch();
  
    void freemem();

    int nrun;

};


#endif
