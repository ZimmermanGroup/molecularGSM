#ifndef GSTRING_H
#define GSTRING_H

//standard library includes
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <unistd.h>
#include <string>
#include <cctype>
#include <ctime>
#include <sys/stat.h>

#include "grad.h"
#include "icoord.h"
#include "utils.h"
#include "stringtools.h"
#include "pTable.h"
#include "eckart.h"

void print_xyz_gen(int natoms, string* anames, double* coords);

class GString {
 private:

  int isRestart;
  int killcounter;
  int ngrowth;
  int growing;
  int oi;
  int endearly;
  int bondfrags;
 
  int isSSM; //shooting string flag
  int hessSSM; //starting SSM hessian given
  int isFSM; //freezing string flag
  int use_exact_climb; //whether to climb or do exact TS search
  void set_fsm_active(int nnR, int nnP);

  int ptsn;
  double newclimbscale;

  void growth_iters(int max_iter, double& totalgrad, double& gradrms, double endenergy, string strfileg, int& tscontinue, double gaddmax, int osteps, int oesteps, double** dqa, double* dqmaga, double** ictan);
  void opt_iters(int max_iter, double& totalgrad, double& gradrms, double endenergy, string strfileg, int& tscontinue, double gaddmax, int osteps, int oesteps, double** dqa, double* dqmaga, double** ictan, int finder, int climber, int do_tp, int& tp);

  int nfound;
  int nbond;
  int nadd;
  int nbrk;
  int nangle;
  int ntors;
  int* bond;
  int* add;
  int* brk;
  int* angles;
  double* anglet;
  int* tors;
  double* tort;
  int isomer_init(string isofilename);
  void set_ssm_bonds(ICoord &ic1);
  double get_ssm_dqmag(double bdist);
  int past_ts();
  int n0; //first node along current block
  int find_peaks(int type);
  int find_uphill(double cutoff);
  void trim_string();
  void trim_string(int nextmin);
  int add_linear(); //adds an angle along linear add bond (SSM)
  int using_break_planes;
  int break_planes_ssm(ICoord ic1); //adds torsion driver to flat add atom (SSM)

 
  //*** Node Data ***

  int natoms;			//number of atoms in each structure
//  string comment;		//comment line of first structure read
  int ncpu; // number of cpu for qchem
  int runNum; // unique id for this gstring instance
  int runend; // random id
  string runends;
  string infile0;

  double** coords;
  ICoord* icoords;
  double** tangents;
  double** grads;
  double** perp_grads;

  int climb;
  int find; //eigenvector following TS search
  int TSnode0;
  int cTSnode;
  double lastdispr;

  double xdist; //spacing for xyz all printout

  int nnew_bond;
  int* new_bond;
  ICoord bondsic;
  ICoord newic;
  ICoord intic;
  ICoord int2ic;
  ICoord newic_dm;
  ICoord intic_dm;
  ICoord int2ic_dm;
  int ic_reparam_steps;
  void get_eigenv_bofill();
  void get_eigenv_finite(int enode);
  void get_eigenv_finite(int enode, double** ictan);
  void starting_string(double* dq, int nnodes);
  void starting_string_dm(double* dq);
  int addNode(int n1, int n2, int n3);
  int addCNode(int n1);
  void add_last_node(int type);
  void com_rotate_move(int iR, int iP, int iN, double ff);
  void scan_r(int eigen);
  void opt_tr();
  void opt_r();
  void opt_steps(double** dqa, double** ictan, int osteps, int oesteps);
  int knnr_vs_opt(int n);
  void ic_reparam_g(double** dqa, double* dqmaga);
  void ic_reparam(double** dqa, double* dqmaga, int type);
  void ic_reparam_h(double** dqa, double* dqmaga, int type);
  void ic_reparam_dm(double** dqa, double* dqmaga, int type);
  void ic_reparam_new(double** dqa, double* dqmaga, int type);
  void tangent_1(double* ictan);
  double tangent_1b(double* ictan);
  void get_tangents_1(double** dqa, double* dqmaga, double** ictan);
  void get_tangents_1e(double** dqa, double* dqmaga, double** ictan);
  void get_tangents_1g(double** dqa, double* dqmaga, double** ictan);
  void get_tangents(double** dqa, double* dqmaga, double** ictan);
  void get_tangents_dm(double** dqa, double* dqmaga, double** ictan);
  void get_distances(double* dqmaga, double** ictan);
  void get_distances_dm(double* dqmaga, double** ictan);
 
  int close_dist_fix(int type);
  int check_close_dist(int n, double* dist, int* newbonds);
  void add_bonds(int nadd, int* newbonds);
  void add_angles(int nadd, int* newangles);
  void align_rxn();

  void align_string(ICoord ic1, ICoord ic2);
  void rotate_structure(double* xyz0, int* a);

  void print_em(int nmaxp);

 //for prima
  void set_prima(string pstring);
  int read_string(string stringfile, double** coordsn, double* energies);
  int pTSnode;
  double* pTSnodecoords;
  void print_string_clump_p(int STEPS, double grad, double** allcoords, string xyzstring);

  int twin_peaks();
  int nsplit;
  int find_ints();
  void ic_reparam_cut(int min, double** dqa, double* dqmaga, int type);
  int check_for_reaction_g(int type);
  int check_for_reaction(int& wts, int& wint);

  double* V_profile;
  double V0; // zero reference E

  int* active;
  int* frozen;

  int nn;
  int nnR;
  int nnP;
  int nnmax;
  int nnmax0; //input value of nnmax

  double SCALING;  
  int INTERP_MODE;
  int GROWD;

  string* anames;		//array of atomic symbols (for creating input QC file)
  int* anumbers;		//array of atomic indices (for looking up period table stuff)
  double* amasses;		//array of atomic masses (used for mass-weighting coordinates)

  string stringfilename;	//file where the initial string structures are stored (usually initial.xyz)

  int STEP_OPT_ITERS;
  int MAX_OPT_ITERS;
  double CONV_TOL;
  double ADD_NODE_TOL;
  double HESS_INIT;
  int CHARGE;                   //charge of the molecular complex
  int SPIN;
  double NODE_SPACING;                     //spin of the complex
  int NUM_INTERP;
  int NUM_STEPS;
  double DQMAG_SSM_MAX;
  double DQMAG_SSM_MIN;
  double QDISTMAX;
  double PEAK4_EDIFF;
  int tstype;
  double prodelim;
  int lastOpt;
  int initialOpt;

  //*** Places to keep some simulation data ***
  int gradJobCount;		//keeps track of the total number of gradient jobs performed
  int gradFailCount;

  //*** quantum software choice ***
  //QChem qchem1;
  Gradient grad1;

  //**** Initialization functions *****
  void general_init(string infilename);
  void parameter_init(string infilename);
  void structure_init(string xyzfile);
  void allelse_init();
  void restart_string(string pstring);

 public:

  double** allcoords;

  void String_Method_Optimization();
  void reparam_and_getTangents_with_LST(double** fstring);

  void init(string infilename, string xyzfile);
  void init(string infilename, int runNum, int nprocs);

  double min_rms_structure_distance(double* struct1, double* struct2, int natoms);

  void print_string(int STEPS, double** allcoords, string xyzstring);
  void print_string_clump(int STEPS, double grad, double** allcoords, string xyzstring);
  void write_string_file(int iter);
  void write_SVfile(int iter);
  void write_geom_file(int iter);


  void shift_node(int nnOld, int nnNew);

  void build_fstring(double** fstring);
};

#endif


 
 
