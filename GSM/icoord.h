#ifndef ICOORD_H
#define ICOORD_H

#include "stringtools.h"
#include "pTable.h"
#include "grad.h"

class ICoord {

  private:

  //move gradient back to private!
    int g_inited;

    int nretry;

//    int* atypes;                  //array of MM atom types  
    double* amasses;              //array of atomic masses
    double* amasses3;
//    double* charge;               //array of MM atomic charges
//    string comment;
    double* dxm1;

   //stepsize controllers
    double dEpre; 
    double smag; 

    int** imptor;

    int nfrags;
    int* frags;

    int max_bonds;
    int max_angles;
    int max_torsions;
    int max_imptor;

    int max_nonbond;
    int n_nonbond;
    int** nonbond;
    double* nonbondd;

  void structure_read(string xyzfile);
  void alloc_mem();
  void make_bonds();
  void coord_num();
  void make_angles();
  void make_torsions();

  void make_frags();
  void bond_frags();
  void hbond_frags();
  void linear_ties();
  void h2o_torsions();
  void tm_ties();

  void make_imptor(); 
// ic creation function when bonds are already made
  void make_imptor_nobonds(); 

  int make_nonbond();

//
  void update_bonds();
  void update_angles();
  void update_torsion();
  void update_imptor();
  void update_nonbond();

  void create_xyz();

  double close_bond(int i);
  double close_angle(int i);
  double close_tor(int i);
  int ixflag;

  // MM force field params
  double* ffR;
  double* ffeps;
  double ffbondd(int i, int j);
  double ffbonde(int i, int j);
  double ffangled(int i, int j);
  double ffanglee(int i, int j);
  double fftord(int i, int j, int k, int l); 
  double fftore(int i, int j, int k, int l); 
  double fftorm(int i, int j, int k, int l); //multiplicity 
  double ffimptore(int i, int j, int k, int l); 
  double ffimptord(int i, int j, int k, int l); 
  // function to make arrays?

  int isTM(int anum);

  // Gradient terms
  double* bmat;
  double* bmatti;

  void print_grad();
  void print_gradq();
  void bond_grad_all();
  void bond_grad_1(int i, int j);
  double bond_stretch(int i, int j);
  void angle_grad_all();
  void angle_grad_1(int i, int j, int k);
  void torsion_grad_all();
  void torsion_grad_1(int i, int j, int k, int l);
  void imptor_grad_all();
  void imptor_grad_1(int i, int j, int k, int l);
  void vdw_grad_all();
  void vdw_grad_1(int i, int j, double scale);

  //Optimizer
  //void update_ic_sd();
  //void update_ic_cg();
  void update_ic_qn();
  void update_ic_eigen();
  void update_ic_eigen_h(double* C, double* D);
  void update_ic_eigen_ts(double* C);
  void walk_up();

  void force_notbonds();

  void bmatp_dqbdx(int a1, int a2, double* dqbdx);
  void bmatp_dqadx(int a1, int a2, int a3, double* dqadx);
  void bmatp_dqtdx(int a1, int a2, int a3, int a4, double* dqtdx);

//  ofstream xyzfile;
  void print_xyzf(ofstream xyzfile); // print xyz coords to file

 //for avoiding regions of space
  int nnodes;
  double prima_force();
  double* prima;
  int* aprima;
  double* Cp;
  double mdist;
  void lin_grad_1(int i, int j, double scale);

  char* sbuff;

  double* Gmh; //Gm^1/2
  double* Gmih; //Gm^-1/2
  void get_gm();
  void create_mwHint_EV(double* Lm, double* Lme);
 

  public:

  int revertOpt;
  Gradient grad1;
  string printout;

  double FMAG;
  double OPTTHRESH;
  double MAXAD;
  double DMAX;
  double DMIN0;
  double SCALEQN0;
  double SCALEQN;

  double V0;
  int* frozen;

  double farBond;
  int isTSnode;
  int nneg;
  int newHess;
  int optCG;
  int do_bfgs;
  int noptdone;
  double path_overlap;
  int path_overlap_n;
  double path_overlap_e_g;
  void update_bfgs();
  void update_bfgsp(int makeHint);
  void update_bofill();
  void save_hess();

  double getR(int i);

  int nicd;
    int nicd0; //before constraint applied

    double* bondd;
    double* anglev;
    double* torv;
    double* torv0;
    double* torfix;
    double* imptorv;

  int** bonds;
  int nbonds;
  int** angles;
  int nangles;
  int** torsions;
  int ntor;

  double* grad;
 // double* pgrad;
  double* gradq;
  double* pgradq;
  double* gradqprim;
  double* pgradqprim;
  double* dq0;
  double* dqm1;
  double* dqprim;
  double* Hint;
  double* Hintp;
  double* Hinv;
  double* q;
  int useExactH;
  int isOpt;
  int stage1opt;
  double pgradrms;
  double gradrms;
  void make_Hint();
  void Hintp_to_Hint();
  int davidson_H(int neigen);
  int isDavid;

  int id; //for geoms[id] in zstruct
  int pid; // previous structure id

  int natoms;
  double* coords;
  double* coordsts;
  double* coords0;
  string* anames;               //array of atomic symbols 
  int* anumbers;                //array of atomic indices 
  int* coordn;                  //coordination number
  int nimptor;

  int ic_create();
  int ic_create_nobonds();
  int mm_grad();
  int mm_grad(ICoord shadow);
  int grad_to_q();
  int ic_to_xyz();
  int ic_to_xyz_opt();
//  int qchem_init(string infilename, int ncpu, int run, int rune);
  int grad_init(string infilename, int ncpu, int run, int rune, int use_knnr, int q1);
  string runends;
  string runend2;

  double* bmatp; // in primitives
  int get_tangent();
  int distance_matrix_ic(ICoord ic1, ICoord ic2);
  int union_ic(ICoord ic1, ICoord ic2);
  int copy_ic(ICoord ic1);
  void write_ic(string filename);
  int read_ics(string filename);

  int opt();
  int opt(string xyzfile, ICoord shadow);
  int opt(string xyzfile);
  void opt_constraint(double* C);
  double opt_a(int nnewb, int* newb, int nnewt, int* newt, string xyzfile_string, int nsteps);
  double opt_b(string xyzfile_string, int nsteps);
  double opt_c(string xyzfile_string, int nsteps, double* C, double* C0);
  double opt_r(string xyzfile_string, int nsteps, double* C, double* C0, double* D, int type);
  double opt_eigen_ts(string xyzfile_string, int nsteps, double* C, double* C0);
  void update_ic();
  void mm_init();

  int bmat_alloc();
  int bmat_free();
  int bmatp_finite();
  int bmatp_create();
  int bmatp_to_U();
  int bmat_create();

  double* Ut;
  double* Ut0;

// help functions for iso
  int bond_exists(int b1, int b2);
  int bond_num(int b1, int b2);
  int angle_num(int b1, int b2, int b3);
  int tor_num(int b1, int b2, int b3, int b4);
  int hpair(int a1, int a2);
  int h2count();

  int same_struct(double* xyz);

  int init(string xyzfile);
  int init(int natoms, string* anames, int* anumbers, double* xyz);
  int alloc(int size); 
  int reset(int natoms, string* anames, int* anumbers, double* xyz);
  int reset(double* xyz);
  void print_q();
  void print_ic();
  void print_bonds();
  void print_xyz();
  void print_xyz_save(string filename);
  void print_xyz_save(string xyzfile_string, double energy);


  double distance(int i, int j);
  double angle_val(int i, int j, int k);
  double torsion_val(int i, int j, int k, int l); // for imptor and torsion

  void freemem();

 //for Hessian tangent
  int use_constraint;
  int ridge;

  int create_prima(int nnodes, int nbonds, int nangles, int ntor, double** tangents);
  void save_hesspu(string filename); 
  void save_hessp(string filename);
  void read_hessp(string filename);
  void read_hessxyz(string filename, int write);

};



#endif

