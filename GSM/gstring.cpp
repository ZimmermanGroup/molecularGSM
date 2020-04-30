#include "gstring.h"
#include "omp.h"
//#include "GitSHA1.h"
using namespace std;

//opt should quit right after gradient converges

//memory bug with < 30 nodes?


#define DQMAG_SSM_SCALE 1.5

 //implement symmetry breaker?
#define DRIVE_ADD_TETRA 0
#define USE_DAVID 0
#define SSM_BOND_FRAGS 0

// new path
#define USE_PRIMA 0
//not using this for MOLPRO
#define HESS_TANG 1
#define RIBBONS 0

#define FINAL_FREQ 0

//skip one opt round for converged node
#define ONE_SKIP 0

//kNNR settings
#define EKTS 10.0
#define GKOPT 0.0025
#define KNNR_INTERVAL 5

//Rotate reactants into place
#define ALIGN_RXN 0

//NOTE: added third tie to bond_frags and changed to H-bonds
//NOTE: split_string now adds new bond before opting

#define SPLIT_STRING 0
#define REPARAM_G_INTERIOR 1
#define CLOSE_DIST_ADD 0

#define ADD_EXTRA_BONDS 0

/**
 * The main driver for DE and SE methods, growth and optimization. Can only 
 * be called after coordinates have been set.
 */



void GString::optimize_ts(int wn, int max_iters, double& gradrms, double& gradmax, int& ngrad, int& overlapn, double& overlap, double& ETSf, double** dqa, double* dqmaga, double** ictan)
{
  printf("  in optimize_ts for node %i \n",wn);

  //need ictan for TS node
  int nbonds = newic.nbonds;
  int nangles = newic.nangles;
  int ntor = newic.ntor;
  int size_ic = newic.nbonds+newic.nangles+newic.ntor+newic.nxyzic;
  int len_d = newic.nicd0;

  get_tangents_1e(dqa,dqmaga,ictan);

  double* C = new double[size_ic]();
  double* C0 = new double[size_ic]();
        
  icoords[wn].OPTTHRESH = TS_CONV_TOL;
  icoords[wn].OPTMAX = TS_GRAD_MAX_TOL;
  //icoords[wn].use_xyz_conv = TS_USE_XYZ_CONV;
  icoords[wn].update_ic();
  
  double norm = 0.;
  for (int i=0;i<size_ic;i++)
    norm += ictan[wn][i]*ictan[wn][i];
  norm = sqrt(norm);
  for (int i=0;i<size_ic;i++)
    C[i] = ictan[wn][i]/norm;

  for (int i=0;i<nbonds;i++)
    C0[i] = icoords[wn].bondd[i]*C[i];
  for (int i=0;i<nangles;i++)
    C0[nbonds+i] = icoords[wn].anglev[i]*3.14159/180*C[nbonds+i];
  for (int i=0;i<ntor;i++)
    C0[nbonds+nangles+i] = icoords[wn].torv[i]*3.14159/180*C[nbonds+nangles+i];

  int tssteps = 10;
  int m;
  for (m=0;m<max_iters;m++)
  {
    V_profile[wn] = ETSf = icoords[wn].opt_eigen_ts("scratch/xyzfile.xyzts",tssteps,C,C0);
    printf(" %s",icoords[wn].printout.c_str());

    gradrms = icoords[wn].gradrms;
    gradmax = icoords[wn].gradmax;
    overlap = overlap = icoords[wn].path_overlap;
    ngrad += icoords[wn].noptdone;
    printf("\n opt_iter(TS): %2i current ETS: %7.3f gradrms: %8.6f gradmax: %8.6f overlap: %5.3f \n",m+1,ETSf,gradrms,gradmax,overlap);
 
    if (overlap<0.25) { printf("    error: overlap too low to proceed,\n       refine string further before final ts optimization \n"); break; }
    else if (overlap<0.5) printf("    warning: overlap is low \n");

    if (gradrms<TS_CONV_TOL && gradmax<TS_GRAD_MAX_TOL) break;
  }
  if (m==max_iters) printf("\n warning: maximum iterations reached \n");

  delete [] C;
  delete [] C0;

  return;
}



void GString::String_Method_Optimization()
{

  cout << "****************************************" << endl;
  cout << "****************************************" << endl;
  if (isFSM>0)
    cout << "****** Starting IC-FSM calculation *****" << endl;
  else if (isSSM>0)
    cout << "****** Starting IC-SSM calculation *****" << endl;
  else if (isSSM==-1 && isFSM==-1)
    cout << "****** Starting OPT calculation *****" << endl;
  else
    cout << "****** Starting IC-GSM calculation *****" << endl;
  cout << "****************************************" << endl;
  cout << "****************************************" << endl;

  int climber = 1;
  int finder = 1;
  if (use_exact_climb==0)
    climber = finder = 0;
  else if (use_exact_climb==1)
    finder = 0;
  int tscontinue = 1;
  int do_tp = 0;
  int tp = 0;
  double endenergy = 550.;
  int nscffail = 0;

#if USE_PRIMA
  climber = 1;
  finder = 0;
#endif

  double emax;
  double emaxp;
  double emin;
  double overlap;
  int overlapn;
  int nmax;
  gradJobCount = 0;
  gradFailCount = 0;
  climb = 0;
  find = 0;
  nsplit = 0;
  n0 = 0;
  growing = 1;
  double rn3m6 = sqrt(3*natoms-6);


  //align initial string
  double* masses = new double[1+natoms];
  for (int i=0;i<natoms;i++){
#if 1
    masses[i] = 1.0;
#else
    masses[i] = amasses[i];
    if (masses[i]<1.05)
      masses[i] = 0.01;
    printf(" masses[%i]: %1.3f",i,masses[i]);
#endif
  }
  printf("\n");

  nnR = 1;
  nnP = 1;
  nn = nnR+nnP;

#if 0
  if (!isSSM)
  {
 //bug when ethylene is rotated 180.
    Eckart::Eckart_align(coords[0], coords[nnmax-1], masses, natoms);
//  Eckart::Eckart_align(coords[0], coords[nnmax-1], masses, natoms, 1.0);
    printf(" after Eckart_align \n");
  }
#else
  printf(" skipped Eckart_align \n");
#endif

  cout << fixed;
  cout << " " << natoms << endl << endl;
  for (int i=0;i<natoms;i++)
    cout << anames[i] << " " << coords[0][3*i+0] << " "  << coords[0][3*i+1] << " " << coords[0][3*i+2] << endl;
  if (!isSSM)
  {
    cout << " " << natoms << endl << endl;
    for (int i=0;i<natoms;i++)
      cout << anames[i] << " " << coords[nnmax-1][3*i+0] << " " << coords[nnmax-1][3*i+1] << " " << coords[nnmax-1][3*i+2] << endl;
  }

  double xdistmin = 1000.;
  double xdistmax = -1000.;
  for (int i=0;i<natoms;i++)
  if (coords[0][3*i]>xdistmax)
    xdistmax = coords[0][3*i];
  for (int i=0;i<natoms;i++)
  if (coords[0][3*i]<xdistmin)
    xdistmin = coords[0][3*i];
  xdist = xdistmax-xdistmin + 1.0 + 0.25;

  bool add_R_node = false;
  bool add_P_node = false;

  printf("\n NOTICES \n");
#if USE_DAVID
  printf(" Using Davidson \n");
#else
  printf(" Not using Davidson \n");
#endif
#if CLOSE_DIST_ADD
  printf(" Using close dist add \n");
#else
  printf(" Not using close dist add \n");
#endif
#if ONE_SKIP
  printf(" Using ONE_SKIP \n");
#else
  printf(" Not using ONE_SKIP \n");
#endif
#if HESS_TANG && !USE_MOLPRO
  printf(" Using HESS_TANG \n");
#else
  printf(" Not using HESS_TANG \n");
#endif
  //printf(" get_eigen_finite uses Bmat prim \n");
  printf(" END NOTICES \n\n");

  //#include "savefile.cpp"
  //printf("Version: %s \n\n",g_GIT_SHA1);

  for (int i=0;i<nnmax;i++)
    active[i] = -1;
  active[0] = 0;
  active[nnmax-1] = 0;


  //printf(" nnmax: %i nn: %i \n",nnmax,nn);
  int N3 = natoms*3;
  icoords = new ICoord[nnmax+1];
  for (int i=0;i<nnmax;i++)
    icoords[i].alloc(natoms);
  for (int i=0;i<nnmax;i++)
    icoords[i].gradrms = 1.;
  for (int i=1;i<nnmax-1;i++)
    icoords[i].reset(natoms,anames,anumbers,coords[0]);

  //icoords[0].init(natoms,anames,anumbers,coords[0]);
  //icoords[nn-1].init(natoms,anames,anumbers,coords[nn-1]);
  icoords[0].reset(natoms,anames,anumbers,coords[0]);
  icoords[nnmax-1].reset(natoms,anames,anumbers,coords[nnmax-1]);


  ICoord ic1,ic2,ic3; 
  ic1.alloc(natoms);
  ic2.alloc(natoms);
  ic3.alloc(natoms);
  
  ic1.isOpt = 1; ic1.farBond = 1.;
  ic2.isOpt = 1; ic2.farBond = 1.;
  if (isSSM && bondfrags==0)
  { 
    ic1.isOpt = 0;
    ic2.isOpt = 0;
  }
  if (bondfrags==2)
  {
    ic1.isOpt = 2;
    ic2.isOpt = 2;
  }
  if (bondfrags==3)
  {
    printf(" turning on XYZ-IC mode \n");
    ic1.use_xyz = 2;
    ic2.use_xyz = 2;
  }

  ic1.reset(natoms,anames,anumbers,coords[0]);
  ic2.reset(natoms,anames,anumbers,coords[nnmax-1]);
  ic1.ic_create();
  ic2.ic_create();
  ic1.frozen = frozen;
  ic2.frozen = frozen;


 //add bonds in isomers list
  if (isSSM)
  {
    break_planes_ssm(ic1);
    set_ssm_bonds(ic1);
  }
  else
    set_ssm_bonds(ic1);

#if ADD_EXTRA_BONDS
  printf(" adding torsion %i \n",ic1.ntor);
  ic1.torsions[ic1.ntor][0] = 7;
  ic1.torsions[ic1.ntor][1] = 6;
  ic1.torsions[ic1.ntor][2] = 1;
  ic1.torsions[ic1.ntor][3] = 13;;
  ic1.ntor++;
  printf(" total tor: %i \n",ic1.ntor);
//  ic1.bonds[ic1.nbonds][0] = 0;
//  ic1.bonds[ic1.nbonds][1] = 1;
//  ic1.nbonds++;
#endif



#if 1
  printf(" printing ic1 ic's \n");
  ic1.print_ic();
  if (!isSSM)
  {
    printf(" printing ic2 ic's \n");
    ic2.print_ic();
  }
#endif

  allcoords = new double*[nnmax];
  for (int i=0;i<nnmax;i++)
    allcoords[i] = icoords[i].coords;



// create union_ic
  newic.alloc(natoms);
  intic.alloc(natoms);
  int2ic.alloc(natoms);
  newic.reset(natoms,anames,anumbers,icoords[0].coords);
  intic.reset(natoms,anames,anumbers,icoords[nnmax-1].coords);
  int2ic.reset(natoms,anames,anumbers,icoords[nnmax-1].coords);
  newic.frozen = frozen;
  intic.frozen = frozen;
  int2ic.frozen = frozen;

#if 1
  newic.union_ic(ic1,ic2);  
  intic.copy_ic(newic);
#if 0
  newic.union_ic(intic,ic3);
  intic.copy_ic(newic);
#endif
  int2ic.copy_ic(newic);
#else
  newic.distance_matrix_ic(ic1,ic2);
  intic.copy_ic(newic);
  int2ic.copy_ic(newic);
#endif

  printf("\n actual IC's \n");
  newic.print_ic();

  newic.bmat_alloc();
  newic.bmatp_create();
  newic.bmatp_to_U();
  newic.bmat_create();
  intic.bmat_alloc();
  intic.bmatp_create();
  intic.bmatp_to_U();
  intic.bmat_create();
  int2ic.bmat_alloc();
  int2ic.bmatp_create();
  int2ic.bmatp_to_U();
  int2ic.bmat_create();

  int size_ic = newic.nbonds + newic.nangles + newic.ntor + newic.nxyzic;
  int len_d = newic.nicd0;

  printf("\n");
  for (int n=0;n<nnmax;n++)
    icoords[n].copy_ic(newic);
  for (int n=0;n<nnmax;n++)
    icoords[n].bmat_alloc();
#if 1
    icoords[0].bmatp_create();
    icoords[nnmax-1].bmatp_create();
    icoords[0].bmatp_to_U();
    icoords[nnmax-1].bmatp_to_U();
    icoords[0].bmat_create();
    icoords[nnmax-1].bmat_create();
#else
  for (int n=0;n<nnmax;n++)
    icoords[n].bmatp_to_U();
  for (int n=0;n<nnmax;n++)
    icoords[n].bmat_create();
#endif
  for (int n=0;n<nnmax;n++)
    icoords[n].id = n;
  for (int n=0;n<nnmax;n++)
  for (int i=0;i<len_d;i++) 
    icoords[n].dq0[i] = 0.;
  for (int n=0;n<nnmax;n++)
    icoords[n].frozen = frozen;

  for (int i=0;i<len_d;i++) newic.dq0[i] = 0.;
  for (int i=0;i<len_d;i++) intic.dq0[i] = 0.;
  for (int i=0;i<len_d;i++) int2ic.dq0[i] = 0.;

  double** ictan = new double*[nnmax];
  for (int i=0;i<nnmax;i++)
    ictan[i] = new double[size_ic+100];
  double* dq = new double[len_d+100];
  double dqmag = 0.;

  double** dqa = new double*[nnmax];
  for (int i=0;i<nnmax;i++)
    dqa[i] = new double[len_d+100];
  double* dqmaga = new double[nnmax];
  for (int n=0;n<nnmax;n++) dqmaga[n] =0.;

//  printf("\n actual IC's \n");
//  icoords[0].print_ic();
  for (int i=0;i<nnmax;i++) icoords[i].gradrms = 1000.;


  string nstr0 = StringTools::int2str(runNum,4,"0");
  string strfile0 = "restart.xyz"+nstr0;
#if USE_PRIMA
  //prima setup
  set_prima(strfile0);
#endif

#if RIBBONS
  if (RIBBONS==3)
    scan_r(4);
  else if (RIBBONS==2)
    opt_tr();
  else
    opt_r();
#endif

  //create template for R/P pair
  //printf(" creating bondsic \n");
  ic1.isOpt = 0; ic2.isOpt = 0;
  ic1.ic_create();
  ic2.ic_create();
  bondsic.alloc(natoms);
  bondsic.reset(natoms,anames,anumbers,coords[0]);
  bondsic.union_ic(ic1,ic2);
#if SPLIT_STRING
  printf("\n printing reactant/product bonds \n");
  bondsic.print_bonds();
#endif

#if 0
//cartesian rotation, incomplete
  align_string(ic1,ic2);

  return;
#endif


#ifdef _OPENMP
  printf(" Number of OpenMP threads: %i \n",omp_get_max_threads());
#endif

  //prepare grads
  printf("\n\n ---- Now preparing gradients ---- \n");
  string nstr = StringTools::int2str(runNum,4,"0");
  //icoords[1].write_ic("scratch/qcsave"+nstr+".ics");
  grad1.init(infile0,natoms,anumbers,anames,icoords[1].coords,runNum,runend,ncpu,1,CHARGE);
  newic.grad_init(infile0,ncpu,runNum,runend-1,0,CHARGE);
#if !USE_MOLPRO
  for (int n=0;n<nnmax0;n++)
    icoords[n].grad_init(infile0,ncpu,runNum,runend+n,0,CHARGE); //level 3 is exact kNNR only, 0 is QM grad always
#else
  //molpro gradients will call seed() for initial orbitals
  // doing so from starting geometry only
  for (int n=0;n<nnmax0;n++)
  {
    printf("\n Node %2i \n",n+1); fflush(stdout);

    icoords[n].grad1.seedType = 0; //seedType set later
    if (isRestart) icoords[n].grad1.seedType = 3;
    else if (n==0) icoords[n].grad1.seedType = 1; //seed from INIT1
    else if (n==nnmax0-1) icoords[n].grad1.seedType = 2; //seed from INIT2

    icoords[n].grad_init(infile0,ncpu,runNum,runend+n,0,CHARGE);
    if (n==1 && !isRestart) icoords[n].grad1.seedType = -1; //copy from previous node
    if (n==nnmax0-2 && !isRestart) icoords[n].grad1.seedType = -2; //copy from "next" node
  }
#endif
  printf(" ---- Done preparing gradients ---- \n\n");

#if USE_MOLPRO || QCHEMSF
  printf("\n MOLPRO/QCHEMSF mode: turning off H-follow (t/ol) in opt \n");
  printf("\n MOLPRO/QCHEMSF mode: turning off r in opt \n");
  for (int n=0;n<nnmax0;n++)
    icoords[n].revertOpt = 0;
#endif


  if (initialOpt>0 && !isRestart)
  {
    printf("\n preopt_iter: opting first node \n");
    string nstr0 = StringTools::int2str(runNum,4,"0");
    icoords[0].OPTTHRESH = CONV_TOL*3.;
    if (isSSM==-1 && isFSM==-1) 
    {
      icoords[0].SCALEQN0 = SCALING;
      icoords[0].revertOpt = 1;
      icoords[0].OPTTHRESH = CONV_TOL;
    }
    icoords[0].make_Hint();
    V_profile[0] = icoords[0].opt_b("scratch/firstnode.xyz"+nstr,initialOpt);
    icoords[0].OPTTHRESH = CONV_TOL;
    gradJobCount += icoords[0].noptdone;
    printf(" %s \n",icoords[0].printout.c_str()); 
    printf(" energy of first node: %9.6f \n",V_profile[0]/627.5);
    V0 = icoords[0].grad1.E[0] = V_profile[0];
  }
  if (isRestart)
  {
    icoords[0].grad1.add_force(icoords[0].coords,NULL);
    icoords[nnmax-1].grad1.add_force(icoords[nnmax-1].coords,NULL);
  }

  if (isSSM==-1 && isFSM==-1)
  {
    string optfname = "scratch/firstnode.xyz"+nstr;
    printf("\n done optimizing, output in: %s \n",optfname.c_str());
    printf(" exiting now \n");
    exit(1);
  }

//printf(" exit early! \n");
//exit(-1);

  //Grow the string
  printf("\n\n Begin Growing the String \n");
  nnP = 1;
  nnR = 1;
  nn = 4;
  if (isSSM) nn = 2;

#if ALIGN_RXN
  printf(" Before align, V_profile[0] = %4.3f V0 = %4.3f \n",V_profile[0],V0);
  if (!isSSM)
    align_rxn();
  printf(" after align, V_profile[0] = %4.3f V0 = %4.3f \n",V_profile[0],V0);
#endif

  if (isSSM)
  {
    add_linear(); //CPMZ new
  }
  if (!isRestart)
  {
    if (!isSSM)
      starting_string(dq,4);
    else if (!isRestart)
      starting_string(dq,3);
    active[1] = active[nnmax-2] = 1;
    if (isSSM) active[nnmax-2] = -1;
    if (GROWD==1 && !isSSM)
    {
      printf(" removing product node (GROWD==1) \n");
      nnP = 1; active[nnmax-2] = -1; nn--;  
    }
    if (GROWD==2 && !isSSM)
    {
      printf(" removing reactant node (GROWD==2) \n");
      nnR = 1; active[1] = -1; nn--;  
    }
    printf("  nnR: %2i nnP: %2i \n",nnR,nnP);
    ic_reparam_steps = 4;
    if (!isSSM)
      ic_reparam_g(dqa,dqmaga);

  }// if not restart
  else
    restart_string(strfile0);


 //after IC's are ready
  grad1.write_on = 0;
  int nstates = icoords[0].grad1.nstates;
  if (initialOpt<1 || !isSSM || isRestart)
  {
    V0 = grad1.grads(coords[0], grads[0], icoords[0].Ut, 3);
#if QCHEMSF || USE_MOLPRO
    //V0 = grad1.E[0];
    for (int i=0;i<nstates;i++)
      icoords[0].grad1.E[i] = grad1.E[i];
#endif
  }
//  else V0 = icoords[0].grad1.E[0];
  V_profile[0] = 0.;
  if (!isSSM && !isRestart)
  {
    V_profile[nnmax-1] = grad1.grads(coords[nnmax-1], grads[nnmax-1], icoords[0].Ut, 3) - V0;
#if QCHEMSF || USE_MOLPRO
    for (int i=0;i<nstates;i++)
      icoords[nnmax-1].grad1.E[i] = grad1.E[i];
#endif
  }

  printf("  setting V0 to: %8.1f (%12.8f au) \n",V0,V0/627.5);
  newic.V0 = V0;
  for (int n=0;n<nnmax0;n++)
    icoords[n].V0 = V0;
  gradJobCount++; 
  if (!isSSM) gradJobCount++;

  if (isSSM && !isRestart)
  {
    active[nnmax-2] = 0;
    if (hessSSM)
      icoords[1].read_hessxyz("scratch/initial"+nstr+".hess",0); 
    else
      icoords[1].make_Hint();
#if 0
    int len0 = icoords[0].nbonds+icoords[0].nangles+icoords[0].ntor;
    for (int i=0;i<len0*len0;i++)
      icoords[0].Hintp[i] = icoords[1].Hintp[i];
    icoords[0].save_hessp("hess.icp");
#endif
  }


  if (isSSM)
    printf("\n at beginning, starting V is 0.0 (%8.6f) \n",V0/627.5);
  else
    printf("\n at beginning, starting V's are %8.6f %8.6f \n",V_profile[0],V_profile[nnmax-1]);



  double totalgrad  = 100.;
  double gradrms = 100.;
  emin = V_profile[0];

  double gaddmax = ADD_NODE_TOL/rn3m6;
  newic.SCALEQN0 = SCALING;
  for (int i=0;i<nnmax;i++) icoords[i].SCALEQN0 = SCALING*1.0;
  newic.optCG = 0;
  for (int i=0;i<nnmax;i++) icoords[i].optCG = 0;

  string strfileg = "scratch/stringfile.xyz"+nstr+"g";

  oi = 0; //now class variable
  int osteps = 2;
  if (isFSM)
  {
    osteps = STEP_OPT_ITERS;
    set_fsm_active(1,nnmax-2);
  }
  else if (isSSM)
  {
    osteps = STEP_OPT_ITERS;
    if (!isRestart)
      set_fsm_active(1,1);
  }
  int oesteps = 0;
  int max_iter = MAX_OPT_ITERS;

 //new growth loop in here
  if (!isRestart)
    growth_iters(max_iter,totalgrad,gradrms,endenergy,strfileg,tscontinue,gaddmax,osteps,oesteps,dqa,dqmaga,ictan);


  if (isFSM)
  {
    printf(" FSM run over \n");
    tscontinue = 0;
  }


  printf(" writing grown string %s \n",strfileg.c_str());
#if QCHEM || QCHEMSF || USE_ORCA || USE_GAUSSIAN
  if (!isSSM && !isFSM && !isRestart)
    printf(" Warning: last added node(s) do not have energies yet \n");
#endif
  print_string(nnmax,allcoords,strfileg);


  if (!isFSM && tscontinue && !isRestart)
  {
    printf(" \n initial ic_reparam \n");
    ic_reparam_steps = 25;
    if ((nn==nnmax && !isFSM) || (isSSM && tscontinue))
      ic_reparam(dqa,dqmaga,0);
  }


//  if (!isFSM && !isSSM)
  if (!isFSM && tscontinue)
  printf("\n\n Starting String opt \n");
  osteps = 3;
  oesteps = osteps*2;
  ic_reparam_steps = 5; //CPMZ was 2!

  int maxw = 10000;
  while (1)
  {
    if (tscontinue==1)
    {
      opt_iters(max_iter,totalgrad,gradrms,endenergy,strfileg,tscontinue,gaddmax,osteps,oesteps,dqa,dqmaga,ictan,finder,climber,do_tp,tp);
    }
    if (tscontinue==2 && nn < nnmax)
    {
      osteps = STEP_OPT_ITERS;
      growth_iters(max_iter,totalgrad,gradrms,endenergy,strfileg,tscontinue,gaddmax,osteps,oesteps,dqa,dqmaga,ictan);
      osteps = 3;
      ic_reparam_steps = 25;
      if (tscontinue==1) ic_reparam(dqa,dqmaga,0);
      ic_reparam_steps = 5;
    }

    if (isSSM && nn==nnmax && tscontinue==2)
    {
      printf("\n Max'd out nodes for SSM (nn: %i nnmax0: %i) \n",nn,nnmax0);
      exit(1);
    }
    if (tscontinue==0) 
    {
      tscontinue = 1;
      break;
    }
  } //main while loop


  emax = -10000;
  nmax = 1;
  for (int i=1;i<nnmax;i++)
  if (V_profile[i]>emax)
  {
    emax = V_profile[i];
    nmax = i;
  } 
  overlapn = icoords[TSnode0].path_overlap_n;
  overlap = icoords[TSnode0].path_overlap;

  printf("\n opt_iters over: totalgrad: %5.3f gradrms: %6.4f tgrads: %4i  ol(%i): %3.2f max E: %5.1f Erxn: %4.1f nmax: %2i TSnode: %2i ",totalgrad,gradrms,gradJobCount,overlapn,overlap,emax-emin,V_profile[nnmax-1],nmax,TSnode0);
  int converged = 0;
  if (isFSM)
    printf("   -FSM done-");
//  else if (isSSM)
//    printf("   -SSM done-");
  else if (tp>1 && !tscontinue && oi < max_iter)
    printf("   -multi-step-exit-");
  else if (!tscontinue)
    printf("   -exit early-");
  else if (endearly==2) 
    printf("   -diss growth-");
  else if (nmax==TSnode0 && overlapn==0 && oi < max_iter && !endearly)
  {
    printf("   -XTS-");
    converged = 2;
  }
  else if (nmax==TSnode0 && oi < max_iter && !endearly)
  {
    printf("   -TS-");
    converged = 1;
  }
  else if (oi==max_iter)
  {
    printf("   -max_iter-");
    tscontinue = 0;
  }
  if (nsplit) printf(" split");
  printf(" \n");

  printf("\n oi: %i nmax: %i TSnode0: %i overlapn: %i \n",oi,nmax,TSnode0,overlapn);

  if (converged && do_post_ts)
  {
    printf("\n will reoptimize TS to higher tolerance %8.5f / %8.5f \n",TS_CONV_TOL,TS_GRAD_MAX_TOL);
    int gradTSCount = 0;
    double ETSf = 0.;
    int ts_iters = ts_opt_steps / 10; if (ts_iters<1) ts_iters = 1;
 
    double gradmax = 0.;
    optimize_ts(TSnode0,ts_iters,gradrms,gradmax,gradTSCount,overlapn,overlap,ETSf,dqa,dqmaga,ictan);
 
    printf("\n opt_iters over (TS): gradrms: %6.4f gradmax: %8.6f tgrads: %4i  ol(%i): %3.2f max E: %5.1f \n",gradrms,gradmax,gradTSCount,overlapn,overlap,ETSf);
  }

#if USE_KNNR
  printf(" recomputing energies for kNNR nodes \n");
  printf(" WARNING: doesn't skip already computed nodes \n");
#ifdef _OPENMP
#if QCHEM || QCHEMSF || USE_ORCA
 #pragma omp parallel for
#endif
#endif
  for (int n=1;n<nnmax;n++)
  if (active[n]>0)
  {
#ifdef _OPENMP
  if (omp_get_num_threads()>1 && active[n]>0)
    printf(" tid: %i/%i node: %i status: %i \n",omp_get_thread_num()+1,omp_get_num_threads(),n,active[n]);
#endif
    icoords[n].grad1.write_on = 0;
    V_profile[n] = icoords[n].opt_b("scratch/xyzfile.xyz"+nstr,0);
  }
#endif


  if (isFSM)
  {
    nstr = StringTools::int2str(runNum,4,"0");
    string strfile = "stringfile.xyz"+nstr+"f";
    print_string(nnmax,allcoords,strfile);
  }

  if (isSSM && lastOpt>0 && icoords[nnmax-1].gradrms>CONV_TOL && !endearly)
  {
    printf("\n adding and opting last node \n");
    int noptsteps = lastOpt-15;
    add_last_node(1);
    nnmax = nnR;
    if (noptsteps>0)
      V_profile[nnmax-1] = icoords[nnmax-1].opt_b("scratch/lastnode.xyz"+nstr,noptsteps);
    gradJobCount += icoords[nnmax-1].noptdone;
    printf(" %s \n",icoords[nnmax-1].printout.c_str());     
  }
  else if (isSSM && lastOpt>0 && !endearly)
    printf(" last node already optimized \n");

  printf(" string E (kcal/mol): ");
  for (int i=0;i<nnmax;i++)
    printf(" %1.1f",V_profile[i]);  
  printf(" \n");
  printf(" string E (au): ");
  for (int i=0;i<nnmax;i++)
    printf(" %12.8f",V_profile[i]/627.5+V0/627.5);  
  printf(" \n");
  printf(" string E (au) - force*distance: ");
  for (int i=0;i<nnmax;i++)
    printf(" %12.8f",V_profile[i]/627.5+V0/627.5-icoords[i].grad1.fdE);  
  printf(" \n");
  printf(" max E: %8.6f for node: %i \n",emax,nmax);

  print_em(nnmax);


#if FINAL_FREQ
  int nnegfound = 0;
  if (converged && !endearly) 
    nnegfound = icoords[TSnode0].davidson_H(3);
  printf(" found %i negative eigenvalue",nnegfound);
  if (nnegfound!=1) printf("s");
  printf(" \n");
#endif


#if 0
  align_string(ic1,ic2);
#endif

//  Eckart::Eckart_align_string(allcoords,nnmax,amasses,natoms);
#if 0
//this doesn't work
  for (int i=0;i<20;i++)
  for (int n=1;n<nnmax-1;n++)
    com_rotate_move(0,nnmax-1,n,2.0*n/nnmax);
#endif

#if 1
  printf(" creating final string file  \n");
  nstr = StringTools::int2str(runNum,4,"0");
  string strfile = "stringfile.xyz"+nstr;
  print_string(nnmax,allcoords,strfile);
#endif

#if 0
  printf(" saving primitive hessians \n");
  for (int n=1;n<nnmax-1;n++)
  {
    string nstr1 = StringTools::int2str(runNum,4,"0");
    nstr = StringTools::int2str(n,2,"0");
    string hfile = "scratch/hessian"+nstr1+"."+nstr+".icp";
    icoords[n].save_hessp(hfile);
  }
#endif

  // final data for zstruct

  if (endearly==1) //termination due to dissociation at opt stage
  {
    int fp = find_peaks(1);
    if (fp==-2)
    {
     //dissociative, can still save (not true for growth dissociation)
      endearly = 0;
    }
  }

  printf("\n about to write tsq.xyz, tscontinue: %i endearly: %i \n",tscontinue,endearly);
  int wts,wint;
  int rxnocc = check_for_reaction(wts,wint);
  double ebts = 0.;
  for (int i=0;i<wts;i++)
  if (V_profile[i]<ebts)
    ebts = V_profile[i];
  printf(" E of min node before TS: %4.1f \n",ebts);

  ofstream final_ts_file;
  nstr=StringTools::int2str(runNum,4,"0");
  string final_ts_file_string = "scratch/tsq"+nstr+".xyz";
  final_ts_file.open(final_ts_file_string.c_str());
  final_ts_file.setf(ios::fixed);
  final_ts_file.setf(ios::left);
  final_ts_file << setprecision(6);
  final_ts_file << " " << natoms << endl;
  if (tscontinue && !endearly)
    final_ts_file << (emax+V0)/627.5 << " " << V0/627.5 << " " << (V_profile[nnmax-1]+V0)/627.5 << endl;
  else
    final_ts_file << (emax+V0+1000.)/627.5 << " " << V0/627.5 << " " << (V_profile[nnmax-1]+V0)/627.5 << endl;
  for (int i=0;i<natoms;i++)
    final_ts_file << anames[i] << " " << allcoords[nmax][3*i+0] << " " << allcoords[nmax][3*i+1] << " " << allcoords[nmax][3*i+2] << endl;
  final_ts_file << endl;
  final_ts_file.close();

  if (nsplit)
  {
    nstr=StringTools::int2str(runNum,4,"0");

    //string cmdmv = "mv final.xyz_"+nstr+"    
    ofstream final_file;
    string final_file_string = "scratch/final.xyz1_"+nstr;
    final_file.open(final_file_string.c_str());
    final_file.setf(ios::fixed);
    final_file.setf(ios::left);
    final_file << setprecision(6);
    final_file << " " << natoms << endl;
    final_file << (V_profile[nnmax-1]+V0)/627.5 << endl;
    for (int i=0;i<natoms;i++)
      final_file << anames[i] << " " << allcoords[nnmax-1][3*i+0] << " " << allcoords[nnmax-1][3*i+1] << " " << allcoords[nnmax-1][3*i+2] << endl;
    final_file << endl;
    final_file.close();

    string cmd1 = "mv scratch/initial"+nstr+".xyz scratch/initial"+nstr+".xyz1";
    system(cmd1.c_str());
    ofstream initial2_file;
    string initial2_file_string = "scratch/initial"+nstr+".xyz";
    initial2_file.open(initial2_file_string.c_str());
    initial2_file.setf(ios::fixed);
    initial2_file.setf(ios::left);
    initial2_file << setprecision(6);
    initial2_file << " " << natoms << endl;
    initial2_file << (V_profile[0]+V0)/627.5 << endl;
    for (int i=0;i<natoms;i++)
      initial2_file << anames[i] << " " << allcoords[0][3*i+0] << " " << allcoords[0][3*i+1] << " " << allcoords[0][3*i+2] << endl;
    initial2_file << " " << natoms << endl;
    initial2_file << (V_profile[nnmax-1]+V0)/627.5 << endl;
    for (int i=0;i<natoms;i++)
      initial2_file << anames[i] << " " << allcoords[nnmax-1][3*i+0] << " " << allcoords[nnmax-1][3*i+1] << " " << allcoords[nnmax-1][3*i+2] << endl;
    initial2_file << endl;
    initial2_file.close();
  }



  return;

  delete [] masses;

}



int GString::isomer_init(string isofilename)
{

  printf(" reading isomers \n");
  if (bondfrags == 1)
    printf("  WARNING: ignoring BONDS in ISOMERS file because BOND_FRAGMENTS == 0 or 1 \n");

  nfound = 0;

  nadd = 0;
  nbrk = 0;
  nangle = 0;
  ntors = 0;

  int maxab = 10;
  bond = new int[2*maxab];
  add = new int[2*maxab]; 
  brk = new int[2*maxab];
  angles = new int[3*maxab];
  anglet = new double[3*maxab];
  tors = new int[4*maxab];
  tort = new double[4*maxab];
  for (int i=0;i<2*maxab;i++) bond[i] = -1;
  for (int i=0;i<2*maxab;i++) add[i] = -1;
  for (int i=0;i<2*maxab;i++) brk[i] = -1;
  for (int i=0;i<3*maxab;i++) angles[i] = -1;
  for (int i=0;i<3*maxab;i++) anglet[i] = 999.;
  for (int i=0;i<4*maxab;i++) tors[i] = -1;
  for (int i=0;i<4*maxab;i++) tort[i] = 999.;

  ifstream output(isofilename.c_str(),ios::in);
  if (!output)
  {
    printf(" couldn't find ISOMERS file: %s \n",isofilename.c_str());
    return 0;
  }

  string line;
  vector<string> tok_line;
  while(!output.eof())
  {
    getline(output,line);
    //cout << " RR " << line << endl;
    if (line.find("BOND")!=string::npos && bondfrags==0)
    {
      tok_line = StringTools::tokenize(line, " \t");
      bond[2*nbond] = atoi(tok_line[1].c_str()) -1;
      bond[2*nbond+1] = atoi(tok_line[2].c_str()) -1;
      printf(" bond for coordinate system: %i %i \n",bond[2*nbond]+1,bond[2*nbond+1]+1);
      nbond++;
      if (nbond>maxab) break;
    }
    if (line.find("ADD")!=string::npos)
    {
      tok_line = StringTools::tokenize(line, " \t");
      add[2*nadd] = atoi(tok_line[1].c_str()) -1;
      add[2*nadd+1] = atoi(tok_line[2].c_str()) -1;
      printf(" adding bond: %i %i \n",add[2*nadd]+1,add[2*nadd+1]+1);
      //if (!geoms[id].bond_exists(add[nfound][2*nadd],add[nfound][2*nadd+1]))
        nadd++;
      if (nadd>maxab) break;
    }
    if (line.find("BREAK")!=string::npos)
    {
      tok_line = StringTools::tokenize(line, " \t");
      brk[2*nbrk] = atoi(tok_line[1].c_str()) -1;
      brk[2*nbrk+1] = atoi(tok_line[2].c_str()) -1;
      printf(" breaking bond: %i %i \n",brk[2*nbrk]+1,brk[2*nbrk+1]+1);
      //if (geoms[id].bond_exists(brk[nfound][2*nbrk],brk[nfound][2*nbrk+1]))
        nbrk++;
      if (nbrk>maxab) break;
    }
    if (line.find("ANGLE")!=string::npos)
    {
      tok_line = StringTools::tokenize(line, " \t");
      angles[3*nangle+0] = atoi(tok_line[1].c_str()) -1;
      angles[3*nangle+1] = atoi(tok_line[2].c_str()) -1;
      angles[3*nangle+2] = atoi(tok_line[3].c_str()) -1;
      anglet[nangle] = atof(tok_line[4].c_str());
      printf(" angle: %i %i %i align to %4.3f \n",angles[3*nangle+0]+1,angles[3*nangle+1]+1,angles[3*nangle+2]+1,anglet[nangle]);
      nangle++;
      if (nangle>maxab) break;
    }
    if (line.find("TORSION")!=string::npos)
    {
      tok_line = StringTools::tokenize(line, " \t");
      tors[4*ntors+0] = atoi(tok_line[1].c_str()) -1;
      tors[4*ntors+1] = atoi(tok_line[2].c_str()) -1;
      tors[4*ntors+2] = atoi(tok_line[3].c_str()) -1;
      tors[4*ntors+3] = atoi(tok_line[4].c_str()) -1;
      tort[ntors] = atof(tok_line[5].c_str());
      printf(" tor: %i %i %i %i align to %4.3f \n",tors[4*ntors+0]+1,tors[4*ntors+1]+1,tors[4*ntors+2]+1,tors[4*ntors+3]+1,tort[ntors]);
      ntors++;
      if (ntors>maxab) break;
    }
  }
  if (nadd > 0 || nbrk > 0 || nangle > 0 || ntors > 0) nfound++;

  printf(" found %i isomer",nfound);
  if (nfound!=1) printf("s");
  printf("\n\n");

#if DRIVE_ADD_TETRA
  //printf("\n now adding tetrahedral destinations as drivers \n");
  //Note: implemented in break_planes_ssm()
#endif


  return nfound;
}

/**
 * initializes starting variables related to the string.xyz file, ISOMERS file, nprocs, etc.
 */
void GString::init(string infilename, int run, int nprocs){
  //  name_selector();
  cout <<"***** Starting Initialization *****" << endl;

  //srand ( time(NULL) );
  //runend = rand()%100+1;
  runend = 1;
  cout << " runend " << runend << endl;

  string nstr=StringTools::int2str(run,4,"0");
  string xyzfile = "scratch/initial"+nstr+".xyz";
  ncpu = nprocs;
  runNum = run;
  runends = nstr;
  cTSnode = 0;
  ptsn = 0;
  newclimbscale = 2.;
  GROWD = 0;
  endearly = 0;
  using_break_planes = 0;
  bondfrags = 1;
  do_post_ts = 0;
  ts_opt_steps = 0;

  printf("  -structure filename from input: %s \n",xyzfile.c_str());
  //general_init(infilename);
  parameter_init(infilename);

  if (isRestart)
  {
    if (isFSM)
    {
      printf("\n\n ERROR: cannot restart FSM runs \n");
      exit(1);
    }
    if (isRestart<0)
    {
      printf("\n\n ERROR: isRestart must be 0, 1, or 2 \n");
      exit(1);
    }
  }

  newic.grad1.seedType = 1;
  grad1.seedType = 1;
  if (isRestart)
  {
    newic.grad1.seedType = 3;
    grad1.seedType = 3;
  }

  string isomerfile = "ISOMERS"+nstr;
  struct stat sts;
  if (stat(isomerfile.c_str(), &sts) != -1)
    printf(" using ISOMERS file: %s \n",isomerfile.c_str());
  else
    isomerfile = "scratch/ISOMERS"+nstr;
  int nfound = isomer_init(isomerfile);
  if (isSSM>0)
  {
    if (nfound!=1)
    {
      printf("\n didn't find any isomers, exiting \n");
      exit(1);
    }
  }
  structure_init(xyzfile);
  infile0 = infilename;
  active = new int[1+nnmax];
  killcounter = 0;
  ngrowth = 0;

  printf("\n");
  //cout << endl << "***** Initialization complete *****" << endl;
}

/*
 * Reads parameter file, normally defined at command line as inpfileq
 */
void GString::parameter_init(string infilename)
{
  nnmax = 9;
  hessSSM = 0; //default no Hessian given
  tstype = 0;
  prodelim = 1000.;
  lastOpt = 0; //default do not optimize last SSM node 
  initialOpt = 0; //default do not optimize first node 
  DQMAG_SSM_MAX = 0.8; 
  DQMAG_SSM_MIN = 0.2; 
  QDISTMAX = 5.0;
  PEAK4_EDIFF = 2.0;
  isRestart = 0;
  isSSM = 0;
  isFSM = 0;
  use_exact_climb = 2;
  CONV_TOL = 0.001;  
  TS_CONV_TOL = 0.0005;
  GRAD_MAX_TOL = 0.15;
  TS_GRAD_MAX_TOL = 0.02;

  printf("Initializing Tolerances and Parameters... \n");
  printf("  -Opening %s \n",infilename.c_str());

  ifstream infile;
  infile.open(infilename.c_str());
  if (!infile){
    printf("\n Error opening %s \n",infilename.c_str());
    exit(-1);
  }

  printf("  -reading file... \n");

  // pass infile to stringtools and get the line containing tag
  string tag="String Info";
  bool found=StringTools::findstr(infile, tag);
  if (!found) { cout << "Could not find tag for Default Info" << endl; exit(-1);}
  string line, templine, tagname;

  // parse the input section
  bool stillreading=true;
  while (stillreading)
  {
    stillreading=false;
    // set to false and set back to true if we read something
    // get filename
    getline(infile, line);
    vector<string> tok_line = StringTools::tokenize(line, " ,\t");
    templine=StringTools::newCleanString(tok_line[0]);
    tagname=StringTools::trimRight(templine);
    // these variables are denoted by strings with same name
    if (tagname=="MAX_OPT_ITERS") {
      MAX_OPT_ITERS=atoi(tok_line[1].c_str());
      stillreading=true;
      cout <<"  -MAX_OPT_ITERS: " << MAX_OPT_ITERS << endl;
    }
    if (tagname=="STEP_OPT_ITERS") {
      STEP_OPT_ITERS = atoi(tok_line[1].c_str());
      stillreading = true;
      cout <<"  -STEP_OPT_ITERS: " << STEP_OPT_ITERS << endl;
    }
    if (tagname=="RESTART") {
      isRestart = atoi(tok_line[1].c_str());
      stillreading = true;
      cout <<"  -RESTART: " << isRestart << endl;
    }
    if (tagname=="TS_FINAL_TYPE") {
      tstype = atoi(tok_line[1].c_str());
      stillreading = true;
      cout <<"  -TS_FINAL_TYPE: " << tstype << endl;
      if (tstype!=0 && tstype!=1 && tstype!=2)
      {
        printf("  TS_FINAL_TYPE must be 0, 1 or 2 \n");
        exit(1);
      }
    }
    if (tagname=="TS_CONV_TOL") {
      TS_CONV_TOL=atof(tok_line[1].c_str());
      stillreading=true;
      cout <<"  -TS_CONV_TOL = " << TS_CONV_TOL << endl;
      do_post_ts = 1;
      if (ts_opt_steps==0) ts_opt_steps = 50;
    }
    if (tagname=="TS_GRAD_MAX_TOL") {
      TS_GRAD_MAX_TOL=atof(tok_line[1].c_str());
      stillreading=true;
      cout <<"  -TS_GRAD_MAX_TOL = " << TS_GRAD_MAX_TOL << endl;
      do_post_ts = 1;
    }
    if (tagname=="TS_FINAL_OPT") {
      ts_opt_steps=atoi(tok_line[1].c_str());
      stillreading=true;
      cout <<"  -TS_FINAL_OPT = " << ts_opt_steps << endl;
      do_post_ts = 1;
      if (TS_CONV_TOL==0) TS_CONV_TOL = CONV_TOL;
    }
    if (tagname=="PRODUCT_LIMIT") {
      prodelim = atof(tok_line[1].c_str());
      stillreading = true;
      cout <<"  -PRODUCT_LIMIT: " << prodelim << endl;
    }
    if (tagname=="FINAL_OPT") {
      lastOpt = atoi(tok_line[1].c_str());
      stillreading = true;
      cout <<"  -FINAL_OPT: " << lastOpt << endl;
    }
    if (tagname=="INITIAL_OPT") {
      initialOpt = atoi(tok_line[1].c_str());
      stillreading = true;
      cout <<"  -INITIAL_OPT: " << initialOpt << endl;
    }
    if (tagname=="SSM_DQMAX") {
      DQMAG_SSM_MAX = atof(tok_line[1].c_str());
      DQMAG_SSM_MIN = DQMAG_SSM_MAX/4.;
      stillreading = true;
      cout <<"  -SSM_DQMAX: " << DQMAG_SSM_MAX << endl;
      cout <<"  -SSM_DQMIN: " << DQMAG_SSM_MIN << endl;
    }
    if (tagname=="MIN_SPACING") {
      QDISTMAX = atof(tok_line[1].c_str());
      stillreading = true;
      cout <<"  -SSM_MIN_SPACING: " << QDISTMAX << endl;
    }
    if (tagname=="INT_THRESH") {
      PEAK4_EDIFF = atof(tok_line[1].c_str());
      stillreading = true;
      cout <<"  -INT_THRESH: " << PEAK4_EDIFF << endl;
    }
    if (tagname=="SM_TYPE") {
      if (tok_line[1]=="SSM")
      {
        printf("  -using SSM \n");
        isFSM = 0;
        isSSM = 1;
      }
      else if (tok_line[1]=="SSMFR")
      {
        printf("  -using SSM with initial Hessian \n");
        isFSM = 0;
        isSSM = 1;
        hessSSM = 1;
      }
      else if (tok_line[1]=="FSM")
      {
        printf("  -using FSM \n");
        isFSM = 1;
        isSSM = 0;
      }
      else if (tok_line[1]=="OPT")
      {
        printf("  -using OPT \n");
        isFSM = -1;
        isSSM = -1;
      }
      else
      {
        printf("  -using GSM \n");
        isFSM = 0;
        isSSM = 0;
      }
      stillreading=true;
    }
    if (tagname=="CONV_TOL") {
      CONV_TOL=atof(tok_line[1].c_str());
      stillreading=true;
      cout <<"  -CONV_TOL = " << CONV_TOL << endl;
    }
    if (tagname=="GRAD_MAX_TOL") {
      GRAD_MAX_TOL=atof(tok_line[1].c_str());
      stillreading=true;
      cout <<"  -GRAD_MAX_TOL = " << GRAD_MAX_TOL << endl;
    }
    if (tagname=="ADD_NODE_TOL"){
      ADD_NODE_TOL=atof(tok_line[1].c_str());
      stillreading=true;
      cout <<"  -ADD_NODE_TOL = " << ADD_NODE_TOL << endl;
    }
    if (tagname=="SCALING"){
      SCALING = atof(tok_line[1].c_str());
      stillreading = true;
      cout <<"  -SCALING = " << SCALING << endl;
    }
    if (tagname=="BOND_FRAGMENTS"){
      bondfrags = atoi(tok_line[1].c_str());
      stillreading = true;
      cout <<"  -BOND_FRAGMENTS = " << bondfrags << endl;
    }
    if (tagname=="GROWTH_DIRECTION"){
      GROWD = atoi(tok_line[1].c_str());
      stillreading = true;
      cout <<"  -GROWTH_DIRECTION = " << GROWD << endl;
    }
    if (tagname=="CLIMB_TS"){
      use_exact_climb = atoi(tok_line[1].c_str());
      stillreading = true;
      cout <<"  -CLIMB_TS = " << use_exact_climb << endl;
    }
    if (tagname=="nnodes" || tagname=="NNODES"){
      nnmax = atoi(tok_line[1].c_str());
      stillreading = true;
      cout <<"  -NNODES = " << nnmax << endl;
      if (nnmax < 3)
      {
        printf("\n\n ERROR: NNODES cannot be less than 3 \n");
        exit(1);
      }
    }

  } //while stillreading
  infile.close();
 
  if (tstype==2)
  {
    printf("  TS_FINAL_TYPE == 2, turning off climbing image and TS search \n");
    use_exact_climb = 0;
  }
  nnmax0 = nnmax;

  printf(" Done reading inpfileq \n\n");
}

/**
 * Reads xyz file defined from commnand line
 */
void GString::structure_init(string xyzfile)
{
  printf("Reading and initializing string coordinates \n");
  printf("  -Opening structure file \n");

  ifstream infile;
  infile.open(xyzfile.c_str());
  if (!infile){
    printf("\n Error opening xyz file: %s \n",xyzfile.c_str());
    exit(-1);
  }

  printf("  -reading file... \n");

  string line;
  bool success=true;
  success=getline(infile, line);
  if (success){
    int length=StringTools::cleanstring(line);
    natoms=atoi(line.c_str());
  }
  cout <<"  -The number of atoms is: " << natoms << endl;

  success=getline(infile, line);
  vector<string> tok_line0 = StringTools::tokenize(line, " \t");
  CHARGE = 0;
  if (tok_line0.size()>0)
    CHARGE = atoi(tok_line0[0].c_str());
  if (CHARGE>5 || CHARGE<-5)
  {
    printf("   invalid charge value in initial.xyz: %2i \n",CHARGE);
    exit(1);
  }

  anumbers = new int[1+natoms];
  amasses = new double[1+natoms];
  anames = new string[1+natoms];
  frozen = new int[1+natoms]();

  cout <<"  -Reading the atomic names...";
  for (int i=0;i<natoms;i++){
    success=getline(infile, line);
    int length=StringTools::cleanstring(line);
    vector<string> tok_line = StringTools::tokenize(line, " \t");
    anames[i]=tok_line[0];
    anumbers[i]=PTable::atom_number(anames[i]);
    amasses[i]=PTable::atom_mass(anumbers[i]);
  }

  infile.close();

  coords = new double*[1+nnmax];
  tangents = new double*[1+nnmax];
  grads = new double*[1+nnmax];
  perp_grads = new double*[1+nnmax];


  V_profile = new double[1+nnmax];
  for (int i=0;i<nnmax;i++)
    V_profile[i] = 0.;

  for (int i=0;i<nnmax;i++){
    coords[i] = new double[1+natoms*3];
    tangents[i] = new double[1+natoms*3];
    grads[i] = new double[1+natoms*3];
    perp_grads[i] = new double[1+natoms*3];
  }

  cout <<"  -Reading coordinates...";
  printf("Opening xyz file \n");
  infile.open(xyzfile.c_str());
  fflush(stdout);
  //cout << "xyzfile opened" << endl;


  for (int i=0;i<2;i++)
  {
    if (isSSM && i==1) break;
    success=getline(infile, line);
    success=getline(infile, line);
    for (int j=0;j<natoms;j++)
    {
      if (infile.eof())
      {
        printf("   end of xyz file reached early, exiting \n");
        exit(1);
      }
      success=getline(infile, line);
      int length=StringTools::cleanstring(line);
      vector<string> tok_line = StringTools::tokenize(line, " \t");
//      cout << " i: " << i << " string: " << line << endl;
      int n;
      if (i==0) n = 0;
      else if (i==1) n = nnmax-1;
      coords[n][3*j+0]=atof(tok_line[1].c_str());
      coords[n][3*j+1]=atof(tok_line[2].c_str());
      coords[n][3*j+2]=atof(tok_line[3].c_str());
      perp_grads[i][3*j+0] = 0.0;
      perp_grads[i][3*j+1] = 0.0;
      perp_grads[i][3*j+2] = 0.0;
//      printf("  line: %s size: %i \n",line.c_str(),tok_line.size());
      if (tok_line.size()>4) frozen[j] = 1;
    }
  }

  if (isSSM)
  for (int i=0;i<3*natoms;i++)
    coords[nnmax-1][i] = coords[0][i];

  //cout << " done" << endl;
  infile.close();

  double zero = 0.;
  for (int i=0;i<3*natoms;i++)
    zero += coords[0][i]*coords[0][i];
  if (zero<0.0001) 
  {
    printf("\n ERROR: initial.xyz has NULL coordinates \n");
    exit(1);
  }

  printf("  printing frozen list:");
  for (int i=0;i<natoms;i++)
    printf(" %i",frozen[i]);
  printf("\n");

  cout << "Finished reading information from structure file" << endl;

  return;
}


///calls get_eigenv_finite after constructing tangent
void GString::get_eigenv_finite(int enode)
{
  //printf(" this function doesn't work (yet) \n"); fflush(stdout);

  int size_ic = newic.nbonds + newic.nangles + newic.ntor + newic.nxyzic;
  int len_d = newic.nicd0;
  //printf(" nnmax: %i size_ic: %i len_d: %i \n",nnmax,size_ic,len_d); fflush(stdout);

  double** ictan = new double*[nnmax];
  for (int i=0;i<nnmax;i++)
    ictan[i] = new double[size_ic+100];

  double** dqa = new double*[nnmax];
  for (int i=0;i<nnmax;i++)
    dqa[i] = new double[len_d+100];
  double* dqmaga = new double[nnmax];
  for (int n=0;n<nnmax;n++) dqmaga[n] = 0.;

  get_tangents_1(dqa,dqmaga,ictan);
  get_eigenv_finite(enode,ictan);

  for (int i=0;i<nnmax;i++)
    delete [] ictan[i];
  delete [] ictan;
  for (int i=0;i<nnmax;i++)
    delete [] dqa[i];
  delete [] dqa;
  delete [] dqmaga;

  return;
}

///modifies hessian using RP direction
void GString::get_eigenv_finite(int enode, double** ictan) 
{
//  int nmax = TSnode0;
  int en = enode;

  icoords[en].bmatp_create();
  icoords[en].bmatp_to_U();
  icoords[en].bmat_create();
  
  newic.reset(natoms,anames,anumbers,icoords[en].coords);
  newic.bmatp_create();
  newic.bmatp_to_U();

  int size_ic = newic.nbonds+newic.nangles+newic.ntor+newic.nxyzic;
  int len = newic.nicd0;
  //printf(" get_eigenv_finite, nicd0: %i 3N-6: %i \n",len,3*natoms-6);
  int N3 = 3*natoms;

  double* tan = new double[len];
  double* tan0 = new double[size_ic];

  double E0 = V_profile[en]/627.5;
  double Em1 = V_profile[en-1]/627.5;
  double Ep1;
  if (en+1<nnmax)
    Ep1 = V_profile[en+1]/627.5;
  else 
    Ep1 = Em1;

#if 0
  if (en!=TSnode0) 
  {
    Em1 = 0.0005;
    E0 = 0.0;
    Ep1 = 0.0005;
  }
#elif 0
  if (Em1==0. && en!=1)
  {  
    printf(" forcing Em1 \n");
    Em1 = 2*E0 - Ep1;
  }
  if (Ep1==0. && en!=nnmax-2)
  {
    printf(" forcing Ep1 \n");
    Ep1 = 2*E0 - Em1;
  }
  printf(" Em1: %4.3f E0: %4.3f Ep1: %4.3f \n",Em1,E0,Ep1);
#endif

  newic.opt_constraint(ictan[en]);
  newic.bmat_create();
  double q0 = newic.q[len-1];
  for (int i=0;i<size_ic;i++) 
    tan0[i] = newic.Ut[(len-1)*size_ic+i];

  newic.reset(natoms,anames,anumbers,icoords[en-1].coords);
  newic.bmatp_create();
  newic.bmat_create();
  double qm1 = newic.q[len-1];
  double qp1;
  if (en+1<nnmax)
  {
    newic.reset(natoms,anames,anumbers,icoords[en+1].coords);
    newic.bmatp_create();
    newic.bmat_create();
    qp1 = newic.q[len-1];
  }
  else
    qp1 = qm1;
 
  newic.reset(natoms,anames,anumbers,icoords[en].coords);
  newic.bmatp_create();
  newic.bmatp_to_U();
  newic.bmat_create();
//  newic.make_Hint();
  if (en==TSnode0)
    printf(" TS Hess init'd w/existing Hintp \n");
  for (int i=0;i<size_ic*size_ic;i++)
    newic.Hintp[i] = icoords[en].Hintp[i];
  newic.Hintp_to_Hint();

  for (int i=0;i<len;i++) tan[i] = 0.;
  for (int i=0;i<len;i++)
  for (int j=0;j<size_ic;j++)
    tan[i] += tan0[j]*newic.Ut[i*size_ic+j];

#if 0
  double* eigen1 = new double[len];
  icoords[en].Hintp_to_Hint();
  Diagonalize(icoords[en].Hint,eigen1,len);
  printf(" eigenvalues (before):");
  for (int i=0;i<5;i++)
    printf(" %1.3f",eigen1[i]);
  printf("\n");
  delete [] eigen1;
#endif

  double* Ht = new double[len];
  double tHt = 0.;
  for (int i=0;i<len;i++) Ht[i] = 0.;
  for (int i=0;i<len;i++)
  for (int j=0;j<len;j++)
    Ht[i] += newic.Hint[i*len+j]*tan[j];
  for (int i=0;i<len;i++)
    tHt += tan[i]*Ht[i];

  double a = abs(q0 - qm1);
  double b = abs(qp1 - q0);
  double C = 2*( Em1/a/(a+b) - E0/a/b + Ep1/b/(a+b) );
  printf(" tHt: %1.3f a: %1.1f b: %1.1f C: %1.3f \n",tHt,a,b,C);

  double* ttt = new double[len*len];
  for (int i=0;i<len;i++)
  for (int j=0;j<len;j++)
    ttt[i*len+j] = tan[i]*tan[j];

  for (int i=0;i<len;i++)
  for (int j=0;j<len;j++)
    newic.Hint[i*len+j] += (C-tHt)*ttt[i*len+j];

  for (int i=0;i<len*len;i++)
    icoords[en].Hint[i] = newic.Hint[i];

#if 0
  double* eigen = new double[len];
  Diagonalize(newic.Hint,eigen,len);
  printf(" eigenvalues:");
  for (int i=0;i<5;i++)
    printf(" %1.3f",eigen[i]);
  printf("\n");
  delete [] eigen;
#endif

  if (en==TSnode0)
    icoords[en].newHess = 5;
  else
    icoords[en].newHess = 2;
  icoords[en].optCG = 0;
  icoords[en].gradrms = 1.;
  icoords[en].pgradrms = 10000.;

  delete [] Ht;
  delete [] ttt;
  delete [] tan;
  delete [] tan0;

  return;
}



void GString::get_eigenv_bofill() 
{
  int nmax = TSnode0;
  icoords[nmax].bmatp_create();
  icoords[nmax].bmatp_to_U();
  icoords[nmax].bmat_create();
  //icoords[nmax].make_Hint();
  
  int size_ic = newic.nbonds+newic.nangles+newic.ntor+newic.nxyzic;
  int len = newic.nicd0;
  int N3 = 3*natoms;

  double* g0 = new double[len];
  double* gp = new double[len];
  double* c0 = new double[N3];
  double* cp = new double[N3];
  double* q0 = new double[len];
  double* qp = new double[len];

  for (int i=0;i<len;i++) g0[i] = icoords[nmax].gradq[i];
  for (int i=0;i<len;i++) gp[i] = icoords[nmax-1].gradq[i];
  for (int i=0;i<N3;i++) c0[i] = icoords[nmax].coords[i];
  for (int i=0;i<N3;i++) cp[i] = icoords[nmax-1].coords[i];

  newic.reset(natoms,anames,anumbers,c0);
  newic.bmatp_create();
  newic.bmatp_to_U();
  newic.bmat_create();
//  newic.make_Hint();
  printf(" Hess init'd w/existing Hintp");
  for (int i=0;i<size_ic*size_ic;i++)
    newic.Hintp[i] = icoords[nmax].Hintp[i];
  newic.Hintp_to_Hint();
  for (int i=0;i<len;i++) q0[i] = newic.q[i];

  newic.reset(natoms,anames,anumbers,cp);
  newic.bmat_create();
  for (int i=0;i<len;i++) qp[i] = newic.q[i];
  for (int i=0;i<len;i++) newic.dq0[i] = q0[i] - qp[i];
  for (int i=0;i<len;i++) newic.gradq[i] = g0[i];
  for (int i=0;i<len;i++) newic.pgradq[i] = gp[i];
  newic.update_bofill();

  if (nmax+1<nnmax)
  {
    for (int i=0;i<N3;i++) cp[i] = icoords[nmax+1].coords[i];
    for (int i=0;i<len;i++) gp[i] = icoords[nmax+1].gradq[i];
    newic.reset(natoms,anames,anumbers,cp);
    newic.bmat_create();
    for (int i=0;i<len;i++) qp[i] = newic.q[i];
    for (int i=0;i<len;i++) newic.dq0[i] = q0[i] - qp[i];
    for (int i=0;i<len;i++) newic.pgradq[i] = gp[i];
    newic.update_bofill();
  }

  for (int i=0;i<len*len;i++) icoords[nmax].Hint[i] = newic.Hint[i];

#if 0
  double* eigen = new double[len];
  Diagonalize(newic.Hint,eigen,len);
  printf(" eigenvalues:");
  for (int i=0;i<5;i++)
    printf(" %1.3f",eigen[i]);
  printf("\n");
  delete [] eigen;
#endif


  icoords[nmax].gradrms = 1.;
  icoords[nmax].pgradrms = 10000.;
  icoords[nmax].SCALEQN0 = 1.*SCALING; //1.5 over 5 was okay (1 over 3 okay if DMAX set)
  icoords[nmax].MAXAD = icoords[nmax].MAXAD/1.;
  icoords[nmax].newHess = 5;
//  icoords[nmax].isTSnode = 1;
  icoords[nmax].optCG = 0;

  delete [] q0;
  delete [] qp;
  delete [] c0;
  delete [] cp;
  delete [] g0;
  delete [] gp;
}

///Calculates the DE internal coordinate tangent vector.
void GString::tangent_1(double* ictan)
{
  int nbonds = newic.nbonds;
  int nangles = newic.nangles;
  int ntor = newic.ntor;
  int size_icp = newic.nbonds+newic.nangles+newic.ntor;
  int size_ic = size_icp + newic.nxyzic;
  int len_d = newic.nicd0;

 //full redundant tangent
  for (int i=0;i<nbonds;i++)
    ictan[i] = newic.bondd[i] - intic.bondd[i];
  for (int i=0;i<nangles;i++)
    ictan[nbonds+i] = (newic.anglev[i] - intic.anglev[i])*3.14159/180.;
  for (int i=0;i<ntor;i++)
  {
    ictan[nbonds+nangles+i] = (newic.torv[i] - intic.torv[i])*3.14159/180.;
    if (ictan[nbonds+nangles+i]>3.14159)
      ictan[nbonds+nangles+i] = -1*(2*3.14159 - ictan[nbonds+nangles+i]);
    if (ictan[nbonds+nangles+i]<-3.14159)
      ictan[nbonds+nangles+i] = 2*3.14159 + ictan[nbonds+nangles+i];
  }
  int cxyzic = 0;
  if (newic.nxyzic>0)
  for (int i=0;i<natoms;i++)
  if (newic.xyzic[i])
  {
    ictan[size_icp+cxyzic++] = newic.coords[3*i+0] - intic.coords[3*i+0];
    ictan[size_icp+cxyzic++] = newic.coords[3*i+1] - intic.coords[3*i+1];
    ictan[size_icp+cxyzic++] = newic.coords[3*i+2] - intic.coords[3*i+2];
  }

  return;
}


///bond tangent for SSM
double GString::tangent_1b(double* ictan)
{
  printf("\n");
  int nbonds = newic.nbonds;
  int nangles = newic.nangles;
  int ntor = newic.ntor;
  int size_icp = newic.nbonds+newic.nangles+newic.ntor;
  int size_ic = size_icp + newic.nxyzic;
  int len_d = newic.nicd0;

  double bdist = 0.;
  for (int i=0;i<size_ic;i++) ictan[i] = 0.;
  for (int i=0;i<nadd;i++)
  {
    int a1 = add[2*i+0];
    int a2 = add[2*i+1];
    int wbond = newic.bond_num(a1,a2);
    if (wbond==-1)
    {
      printf(" WARNING: bond %i %i not found! \n",a1+1,a2+1);
      exit(1);
    }
    double d0 = (newic.getR(a1) + newic.getR(a2))/2.8;
    //if (newic.anumbers[a1]==1 && newic.anumbers[a2]==1) d0 = 0.7;
    ictan[wbond] = -1 * (d0 - newic.distance(a1,a2));
    if (ictan[wbond] < 0.)
      ictan[wbond] = 0.;
    printf(" bond %i %i d0: %4.3f diff: %4.3f \n",a1+1,a2+1,d0,ictan[wbond]);
    bdist += ictan[wbond] * ictan[wbond];
  }

 //break tangent
  double breakdq = 0.3;
  for (int i=0;i<nbrk;i++)
  {
    int b1 = brk[2*i+0];
    int b2 = brk[2*i+1];
    int wbond = newic.bond_num(b1,b2);
    if (wbond==-1)
    {
      printf(" WARNING: bond %i %i not found! \n",b1+1,b2+1);
      exit(1);
    }
    double d0 = (newic.getR(b1) + newic.getR(b2))*2.0;
//    ictan[wbond] = -1 * (d0 - newic.distance(b1,b2));
    if (newic.distance(b1,b2)<d0)
    {
      if (nadd>0)
        ictan[wbond] = -breakdq * nadd;
      else
        ictan[wbond] = -breakdq;
    }
    else
      ictan[wbond] = -breakdq;
    printf(" bond %i %i d0: %4.3f diff: %4.3f \n",b1+1,b2+1,d0,ictan[wbond]);
  }

 //angle tangent
  for (int i=0;i<nangle;i++)
  {
    int b1 = angles[3*i+0];
    int b2 = angles[3*i+1];
    int b3 = angles[3*i+2];

    int an1 = newic.angle_num(b1,b2,b3);
    printf(" tangent angle: %i %i %i is %i \n",b1,b2,b3,an1);
    printf(" anglev: %4.3f anglet: %4.3f diff(rad): %4.3f \n",newic.anglev[an1],anglet[i],(anglet[i] - newic.anglev[an1])*3.14159/180.);

    ictan[nbonds+an1] = - (anglet[i] - newic.anglev[an1])*3.14159/180.0;
  }

 //torsion tangent
  for (int i=0;i<ntors;i++)
  {
    int b1 = tors[4*i+0];
    int b2 = tors[4*i+1];
    int b3 = tors[4*i+2];
    int b4 = tors[4*i+3];

    int an1 = newic.tor_num(b1,b2,b3,b4);

    double tordiff = tort[i] - newic.torv[an1];
    double torfix = 0.;
    if (tordiff>180.)
      torfix = -360.;
    else if (tordiff<-180.)
      torfix = 360.;

    ictan[nbonds+nangles+an1] = - (tordiff + torfix)*3.14159/180.0;

    printf(" tangent tor: %i %i %i %i is #%i \n",b1+1,b2+1,b3+1,b4+1,an1);
    printf(" torv: %4.3f tort: %4.3f diff(rad): %4.3f \n",newic.torv[an1],tort[i],(tordiff+torfix)*3.14159/180.);
  }

  //Cartesian tangent, NOT USING
  int cxyzic = 0;
  int len_icp = size_icp;
  if (newic.nxyzic>0)
  for (int i=0;i<natoms;i++)
  if (newic.xyzic[i])
  {
    ictan[len_icp+cxyzic++] = newic.coords[3*i+0] - intic.coords[3*i+0];
    ictan[len_icp+cxyzic++] = newic.coords[3*i+1] - intic.coords[3*i+1];
    ictan[len_icp+cxyzic++] = newic.coords[3*i+2] - intic.coords[3*i+2];
  }

  //some normalization
  double norm0 = 0.;
  for (int i=0;i<size_ic;i++)
    norm0 += ictan[i]*ictan[i];
  double norm = sqrt(norm0);
  for (int i=0;i<size_ic;i++)
    ictan[i] = ictan[i] / norm;


//CPMZ clean up!
#if 0
  if (nadd==0)
    bdist = nbrk * 0.5;
  else if (bdist < nadd*DQMAG_SSM && nbrk>0) 
    bdist = nbrk * 0.5;
  if (nadd==0 && nbrk==0)
    bdist = nangle * 0.5 + ntors * 0.5;
#endif
#if 0
  bdist = nadd * 0.5 + nbrk * 0.5 + nangle * 0.25 + ntors * 0.25;
#endif
#if 0
 //bdist starts from bond from actual distances
  bdist += nbrk * 0.5 + nangle * 0.25 + ntors * 0.25;
  if (nadd == 0 && nbrk == 0)
    bdist = nangle * 0.5 + ntors * 0.5;
#endif

  bdist = norm;

 // printf(" bdist: %4.3f norm: %4.3f \n",bdist,norm);
  return bdist;
}


void GString::align_rxn()
{
  printf("\n in align_rxn \n");

  ICoord aic1,aic2;
  aic1.alloc(natoms); aic2.alloc(natoms);
  aic1.reset(natoms,anames,anumbers,coords[0]);
  aic2.reset(natoms,anames,anumbers,coords[nnmax-1]);
  aic1.isOpt = 0; aic2.isOpt = 0;
  aic1.ic_create(); aic2.ic_create();
  aic1.print_bonds();
  aic2.print_bonds();

  int nbonds1 = aic1.nbonds;
  int nbonds2 = aic2.nbonds;

  nnew_bond = 0;
  new_bond = new int[100]; 
  for (int i=0;i<100;i++)
    new_bond[i] = -1;

  int nnew_tor = 0;
  int* new_tor = new int[100];
  for (int i=0;i<100;i++)
    new_tor[i] = -1;

 //ID new bonds
  int bnum;
  for (int i=0;i<nbonds2;i++)
  {
    int found = 0;
    bnum = i;
    for (int j=0;j<nbonds1;j++)
    {
      if (aic1.bond_exists(aic2.bonds[i][0],aic2.bonds[i][1]))
      {
        found = 1; 
        break;
      }
    } //loop j over nbonds2
    if (!found)
    {
      new_bond[2*nnew_bond +0] = aic2.bonds[bnum][0];
      new_bond[2*nnew_bond +1] = aic2.bonds[bnum][1];
      nnew_bond++;
    }
  } //loop i over nbonds2

  for (int i=0;i<nnew_bond;i++)
    printf(" found new bond: %i %i \n",new_bond[2*i],new_bond[2*i+1]);

  //hack together for 2+2
  if (nnew_bond==2)
  {
    int type = 0;
    if (aic1.bond_exists(new_bond[0],new_bond[2]))
      type = 1;
    else if (aic1.bond_exists(new_bond[0],new_bond[3]))
      type = 2;
    else
    {
      printf(" could not identify cycloaddition! \n");
      exit(1);
    }
    new_tor[1] = new_bond[0];
    new_tor[2] = new_bond[1];
    new_tor[5] = new_bond[2];
    new_tor[6] = new_bond[3];

    printf(" align type: %i \n",type);
    if (type==1)
    {
      new_tor[0] = new_bond[2];
      new_tor[3] = new_bond[3];
      new_tor[4] = new_bond[0];
      new_tor[7] = new_bond[1];
    }
    else if (type==2)
    {
      new_tor[0] = new_bond[3];
      new_tor[3] = new_bond[2];
      new_tor[4] = new_bond[1];
      new_tor[7] = new_bond[0];
    }

    printf(" torsion 1: %i %i %i %i \n",new_tor[0],new_tor[1],new_tor[2],new_tor[3]);
    printf(" torsion 2: %i %i %i %i \n",new_tor[4],new_tor[5],new_tor[6],new_tor[7]);

    nnew_tor = 2;
    
  }


  //opt first structure with force
  int osteps = 200;
  string nstr = StringTools::int2str(runNum,4,"0");
  newic.reset(natoms,anames,anumbers,icoords[0].coords);
  newic.bmatp_create();
  newic.bmatp_to_U();
  newic.bmat_create();
  newic.make_Hint();

  //printf(" newic internals \n");
  //newic.print_ic();

  double energyn = newic.opt_a(nnew_bond,new_bond,nnew_tor,new_tor,"scratch/align.xyz"+nstr,osteps);

  icoords[0].reset(natoms,anames,anumbers,newic.coords);
  icoords[0].update_ic();


  delete [] new_tor;
  printf(" done with align_rxn \n");

  return;
}

///Grows the initial nodes for SE and DE. nnodes is 4 for DE and 3 for SE.
void GString::starting_string(double* dq, int nnodes)
{

  int nbonds = newic.nbonds;
  int nangles = newic.nangles;
  int ntor = newic.ntor;
  int size_ic = newic.nbonds+newic.nangles+newic.ntor+newic.nxyzic;
  int len_d = newic.nicd0;
  double* ictan = new double[size_ic];
  double* ictan0 = new double[size_ic];

//  for (int i=0;i<len_d;i++) newic.dq0[i] = 0.;

// Form the initial string
  int rp = -1;
  int iR,iP,wR,wP,iN;
  for (int n=2;n<nnodes;n++)
  {
    rp *= -1;
    iR = nnR-1;
    iP = nnmax-nnP;
    if (rp==1)
    {
      wR = nnR; 
      iN = wR;
      nnR++;
      wP = -1;
      printf(" creating R node: %i    ",wR);
    }
    else if (rp==-1)
    {
      nnP++;
      wP = nnmax-nnP; 
      wR = -1;
      iN = wP;
      printf(" creating P node: %i    ",wP);
    }
    printf(" iR,iP: %i %i wR,wP: %i %i iN: %i ",iR,iP,wR,wP,iN);

    if (rp==1)
    {
      newic.reset(natoms,anames,anumbers,icoords[iR].coords);
      intic.reset(natoms,anames,anumbers,icoords[iP].coords);
    }
    else if (rp==-1)
    {
      newic.reset(natoms,anames,anumbers,icoords[iP].coords);
      intic.reset(natoms,anames,anumbers,icoords[iR].coords);
    }
    newic.update_ic();
    intic.update_ic();

    double bdist = 0.;
    if (isSSM)
      bdist = tangent_1b(ictan);
    else
      tangent_1(ictan);
    printf(" bdist: %4.3f \n",bdist);

#if 0
    printf(" printing ictan \n");
    for (int i=0;i<nbonds;i++)
      printf(" %1.2f",ictan[i]);
    printf("\n");
    for (int i=0;i<nangles;i++)
      printf(" %1.2f",ictan[nbonds+i]);
    printf("\n");
    if (ntors>0)
    for (int i=0;i<ntor;i++)
      printf(" %1.2f",ictan[nbonds+nangles+i]);
    printf("\n");
#endif

    double dqmag = 0.;

    for (int i=0;i<size_ic;i++) ictan0[i] = ictan[i];

    newic.bmatp_create();
    newic.bmatp_to_U();
    newic.opt_constraint(ictan);
 
    if (isSSM)
    {
      dqmag = get_ssm_dqmag(bdist);
      if (tstype==2)
        dqmag = DQMAG_SSM_MAX;
    }
    else
    {
      for (int j=0;j<size_ic-ntor;j++)
        dqmag += ictan0[j]*newic.Ut[newic.nicd*size_ic+j];
      for (int j=nbonds+nangles;j<size_ic;j++)
        dqmag += ictan0[j]*newic.Ut[newic.nicd*size_ic+j]; 
    }

    printf(" dqmag: %1.2f",dqmag);

//    newic.bmatp_create();
//    newic.bmatp_to_U();
//    newic.opt_constraint(ictan);
    newic.bmat_create();
    if (nnmax-n!=1)
      newic.dq0[newic.nicd0-1] = -dqmag/(nnmax-n);
    else
      newic.dq0[newic.nicd0-1] = -dqmag/2;
    if (isSSM)
      newic.dq0[newic.nicd0-1] = -dqmag; //CPMZ check

    printf(" dq0[constraint]: %1.2f \n",newic.dq0[newic.nicd0-1]);
    int success = newic.ic_to_xyz();

    if (isSSM && !success)
    {
      newic.dq0[newic.nicd0-1] = -dqmag/2.0;
      success = newic.ic_to_xyz();
      if (!success)
      {
        newic.dq0[newic.nicd0-1] = -dqmag/5.0;
        success = newic.ic_to_xyz();
        if (!success)
        {
          printf(" ERROR: couldn't add node, dqmag: %4.3f \n",dqmag);
          exit(1);
        }
        else
          printf(" add node working third time \n");
      }
      else
        printf(" add node working second time \n");
    }
    newic.update_ic();

    icoords[iN].reset(natoms,anames,anumbers,newic.coords);
    com_rotate_move(iR,iP,iN,1.0); //operates on iN via newic

    icoords[iN].bmatp_create();
    icoords[iN].bmatp_to_U();
    icoords[iN].bmat_create();
    if (!isSSM)
    {
      icoords[iN].make_Hint();
    }
    else
    {
      printf(" copying Hessian from node %i \n",iR);
      for (int i=0;i<size_ic*size_ic;i++)
        icoords[iN].Hintp[i] = icoords[iR].Hintp[i];
      icoords[iN].newHess = 2;
    }

    V_profile[iN] = 100.;

  } //loop over interpolation

  delete [] ictan;
  delete [] ictan0;

  return;
}

/**
 * Adds a node between n1 and n3. In SE, n2 is nnmax-1
 * arbitrarily because the added node is not "between" any node. 
 */
int GString::addNode(int n1, int n2, int n3)
{
  printf(" adding node: %i between %i %i \n",n2,n1,n3);

  if (n1==n2 || n2==n3 || n1==n3)
  {
    printf(" cannot add node, exiting \n"); 
    exit(1);
  }

#if USE_MOLPRO
  if (n2>n1) icoords[n2].grad1.seedType = -1;
  if (n2<n1) icoords[n2].grad1.seedType = -2;
#endif

  int nbonds = newic.nbonds;
  int nangles = newic.nangles;
  int ntor = newic.ntor;
  int size_icp = newic.nbonds+newic.nangles+newic.ntor;
  int size_ic = size_icp + newic.nxyzic;
  int len_d = newic.nicd0;
  double* ictan = new double[size_ic];
  double* ictan0 = new double[size_ic];

  double BDISTMIN = 0.01;
  double bdist = 0.;

// Add a node
  int iR,iP,wR,wP,iN;
  for (int n=0;n<1;n++)
  {
    iR = n1;
    iP = n3;
    iN = n2;
    printf(" iR,iP: %i %i iN: %i ",iR,iP,iN);

    newic.reset(natoms,anames,anumbers,icoords[iR].coords);
    intic.reset(natoms,anames,anumbers,icoords[iP].coords);

    newic.update_ic();
    intic.update_ic();

    if (isSSM)
    {
      bdist = tangent_1b(ictan);
      printf(" bdist: %4.3f \n",bdist);
      if (bdist<BDISTMIN) break;
    }
    else
      tangent_1(ictan);

#if 0
    printf(" printing ictan \n");
    for (int i=0;i<nbonds;i++)
      printf(" %1.2f",ictan[i]);
    printf("\n");
    for (int i=0;i<nangles;i++)
      printf(" %1.2f",ictan[nbonds+i]);
    printf("\n");
    if (ntors>0)
    for (int i=0;i<ntor;i++)
      printf(" %1.2f",ictan[nbonds+nangles+i]);
    printf("\n");
#endif

    double dqmag = 0.;

    newic.bmatp_create();
    newic.bmatp_to_U();
    newic.opt_constraint(ictan);

    if (isSSM)
    {
      dqmag = get_ssm_dqmag(bdist);
      if (tstype==2)
        dqmag = DQMAG_SSM_MAX;
    }
    else
    {
      for (int i=0;i<size_ic;i++) ictan0[i] = ictan[i];

      for (int j=0;j<size_icp-ntor;j++)
        dqmag += ictan0[j]*newic.Ut[newic.nicd*size_ic+j];
      for (int j=nbonds+nangles;j<size_ic;j++)
        dqmag += ictan0[j]*newic.Ut[newic.nicd*size_ic+j]; //CPMZ check
    }

    printf(" dqmag: %1.2f",dqmag);

//    newic.bmatp_create();
//    newic.bmatp_to_U();
//    newic.opt_constraint(ictan);
    newic.bmat_create();
    if (nnmax-nn!=1)
      newic.dq0[newic.nicd0-1] = -dqmag/(nnmax-nn);
    else
      newic.dq0[newic.nicd0-1] = -dqmag/2;
    if (isSSM)
      newic.dq0[newic.nicd0-1] = -dqmag; //CPMZ check

    printf(" dq0[constraint]: %1.2f \n",newic.dq0[newic.nicd0-1]);
    int success = newic.ic_to_xyz();

    if (isSSM && !success)
    {
      newic.dq0[newic.nicd0-1] = -dqmag/2.0;
      success = newic.ic_to_xyz();
      if (!success)
      {
        printf(" ERROR: couldn't add node, dqmag: %4.3f \n",dqmag);
        exit(1);
      }
      else
        printf(" add node working second time \n");
    }
    newic.update_ic();

    icoords[iN].reset(natoms,anames,anumbers,newic.coords);
    com_rotate_move(iR,iP,iN,1.0); //operates on iN via newic

    icoords[iN].bmatp_create();
    icoords[iN].bmatp_to_U();
    icoords[iN].bmat_create();

    if (!isSSM)
    {
      icoords[iN].make_Hint();
#if HESS_TANG
      //not yet implemented
      //get_eigenv_finite(iN);
#endif
      icoords[iN].newHess = 5;
    }
    else
    {
      for (int i=0;i<size_ic*size_ic;i++)
        icoords[iN].Hintp[i] = icoords[iR].Hintp[i];
      icoords[iN].newHess = 2;
    }

    active[iN] = 1;

  } //loop over interpolation

  delete [] ictan;
  delete [] ictan0;

#if CLOSE_DIST_ADD
  if (close_dist_fix(1))
    addNode(n1,n2,n3);
  else
#endif
  int success = 1;
  if (isSSM)
  {
    if (bdist>=BDISTMIN)
      nn++;
    else
      success = 0;
  }
  else 
    nn++;

  return success;
}
///Add space node between n1-1 and n1
int GString::addCNode(int n1)
{
  if (nnmax>=nnmax0)
  {
    printf(" cannot add space node \n");
    return 0;
  }

  printf(" adding spacer node between %i %i \n",n1-1,n1);
  int n0 = n1 - 1;
  int n2 = n1 + 1;

  int nbonds = newic.nbonds;
  int nangles = newic.nangles;
  int ntor = newic.ntor;
  int nxyzic = newic.nxyzic;
  int size_icp = nbonds + nangles + ntor;
  int size_ic = size_icp + nxyzic;
  int len_d = newic.nicd0;
  double* ictan = new double[size_ic];
  double* ictan0 = new double[size_ic];

  for (int n=nnmax;n>n1;n--)
  {
    //shift node n-1 to n
    active[n] = active[n-1];
    V_profile[n] = V_profile[n-1];
    for (int i=0;i<3*natoms;i++)
      icoords[n].coords[i] = icoords[n-1].coords[i];
//    for (int i=0;i<size_ic*size_ic;i++)
//      icoords[n].Hintp[i] = icoords[n-1].Hintp[i];
    icoords[n].bmatp_create();
    icoords[n].bmatp_to_U();
    icoords[n].bmat_create();
    icoords[n].make_Hint();
  }
  V_profile[n1] = (V_profile[n0] + V_profile[n2]) / 2.0;


// Add a node
  int iR,iP,wR,wP,iN;
  for (int n=0;n<1;n++)
  {
    iR = n0;
    iP = n2;
    iN = n1;
    printf(" iR,iP: %i %i iN: %i ",iR,iP,iN);

    newic.reset(natoms,anames,anumbers,icoords[iR].coords);
    intic.reset(natoms,anames,anumbers,icoords[iP].coords);

    newic.update_ic();
    intic.update_ic();

    tangent_1(ictan);

#if 0
    printf(" printing ictan \n");
    for (int i=0;i<nbonds;i++)
      printf(" %1.2f",ictan[i]);
    printf("\n");
    for (int i=0;i<nangles;i++)
      printf(" %1.2f",ictan[nbonds+i]);
    printf("\n");
    if (ntors>0)
    for (int i=0;i<ntor;i++)
      printf(" %1.2f",ictan[nbonds+nangles+i]);
    printf("\n");
#endif

    double dqmag = 0.;

    newic.bmatp_create();
    newic.bmatp_to_U();
    newic.opt_constraint(ictan);

    for (int i=0;i<size_ic;i++) ictan0[i] = ictan[i];

    for (int j=0;j<size_ic-ntor;j++)
      dqmag += ictan0[j]*newic.Ut[newic.nicd*size_ic+j];
    for (int j=nbonds+nangles;j<size_ic;j++)
      dqmag += ictan0[j]*newic.Ut[newic.nicd*size_ic+j]; //CPMZ check

    printf(" dqmag: %1.2f",dqmag);

//    newic.bmatp_create();
//    newic.bmatp_to_U();
//    newic.opt_constraint(ictan);
    newic.bmat_create();
    newic.dq0[newic.nicd0-1] = -dqmag/2;

    printf(" dq0[constraint]: %1.2f \n",newic.dq0[newic.nicd0-1]);
    int success = newic.ic_to_xyz();

    newic.update_ic();

    icoords[iN].reset(natoms,anames,anumbers,newic.coords);
    com_rotate_move(iR,iP,iN,1.0); //operates on iN via newic

    icoords[iN].bmatp_create();
    icoords[iN].bmatp_to_U();
    icoords[iN].bmat_create();

    for (int i=0;i<size_ic*size_ic;i++)
      icoords[iN].Hintp[i] = icoords[iR].Hintp[i];
    icoords[iN].newHess = 2;

    active[iN] = 1;

  } //loop over interpolation

  delete [] ictan;
  delete [] ictan0;


  int failed = check_array(3*natoms,icoords[iN].coords);
  if (failed)
  for (int n=n1;n<nnmax;n)
  {
    printf("  WARNING: failed to insert spacer, resetting nodes \n");
    active[n] = active[n+1];
    V_profile[n] = V_profile[n+1];
    for (int i=0;i<3*natoms;i++)
      icoords[n].coords[i] = icoords[n+1].coords[i];
//    for (int i=0;i<size_ic*size_ic;i++)
//      icoords[n].Hintp[i] = icoords[n+1].Hintp[i];
    icoords[n].bmatp_create();
    icoords[n].bmatp_to_U();
    icoords[n].bmat_create();
    icoords[n].make_Hint();

    return 0;
  }
  else 
  {
    nnR++;
    nn++;
    nnmax++;
  }


  return 1;
}


void GString::com_rotate_move(int iR, int iP, int iN, double ff) {
 
//  printf(" in com_rotate_move() \n");

  newic.reset(natoms,anames,anumbers,icoords[iN].coords);

  double* xyz0 = new double[natoms*3];
  double* xyz1 = new double[natoms*3];
  double* xyz2 = new double[natoms*3];

// displace center of mass
  double mfrac = 0.5;
  if (nnmax-nn+1!=1)
    mfrac = 1./(nnmax-nn+1);
  mfrac *= ff;
  //printf(" dXYZ frac: %1.2f \n",mfrac);

  for (int i=0;i<3*natoms;i++) xyz0[i] = icoords[iR].coords[i];
  for (int i=0;i<3*natoms;i++) xyz2[i] = icoords[iP].coords[i];
  for (int i=0;i<3*natoms;i++) xyz1[i] = 0.;
  double mx0 = 0.;
  double my0 = 0.;
  double mz0 = 0.;
  double mx2 = 0.;
  double my2 = 0.;
  double mz2 = 0.;
  double mass = 0.;

  //for (int i=0;i<natoms;i++)
  //  printf(" amasses[%i]: %1.2f \n",i,amasses[i]);

    for (int i=0;i<natoms;i++)
    {
      mass += amasses[i];
      mx0 += amasses[i] * xyz0[3*i];
      my0 += amasses[i] * xyz0[3*i+1];
      mz0 += amasses[i] * xyz0[3*i+2];
    }
    mx0 = mx0 / mass; my0 = my0 / mass; mz0 = mz0 / mass;
    for (int i=0;i<natoms;i++)
    {
      mx2 += amasses[i] * xyz2[3*i];
      my2 += amasses[i] * xyz2[3*i+1];
      mz2 += amasses[i] * xyz2[3*i+2];
    }
    mx2 = mx2 / mass; my2 = my2 / mass; mz2 = mz2 / mass;

   // printf(" CoM1: %1.3f %1.3f %1.3f \n",mx0,my0,mz0);
   // printf(" CoM2: %1.3f %1.3f %1.3f \n",mx2,my2,mz2);
    double mx1 = mfrac*(mx2-mx0);
    double my1 = mfrac*(my2-my0);
    double mz1 = mfrac*(mz2-mz0);
    for (int i=0;i<natoms;i++)
    {
      newic.coords[3*i] += mx1;
      newic.coords[3*i+1] += my1;
      newic.coords[3*i+2] += mz1;
    }

 // printf("\n  doing rotation \n");
  int natomsqm = natoms;
  double* amassesqm = new double[natomsqm+1];
  int c = 0;
  for (int i=0;i<natoms;i++)
  {
    xyz1[3*c]   = newic.coords[3*i];
    xyz1[3*c+1] = newic.coords[3*i+1];
    xyz1[3*c+2] = newic.coords[3*i+2];
    xyz2[3*c]   = icoords[iP].coords[3*i];
    xyz2[3*c+1] = icoords[iP].coords[3*i+1];
    xyz2[3*c+2] = icoords[iP].coords[3*i+2];
 //   amassesqm[c] = amasses[i];
    amassesqm[c] = 1.0;
    c++;
  }  
 
#if 0
  cout << " " << natoms << endl << endl;
  for (int i=0;i<natoms;i++)
    cout << anames[i] << " " << newic.coords[3*i+0] << " "  << newic.coords[3*i+1] << " " << newic.coords[3*i+2] << endl;
#endif

  Eckart::Eckart_align(xyz2,xyz1,amassesqm,natomsqm,mfrac);
//  Eckart::Eckart_align(xyz2,xyz1,amassesqm,natomsqm);
 
  c = 0;
  for (int i=0;i<natoms;i++)
  {
    newic.coords[3*i]   = xyz1[3*c];
    newic.coords[3*i+1] = xyz1[3*c+1];
    newic.coords[3*i+2] = xyz1[3*c+2];
    c++;
  }  
#if 0
  cout << " " << natoms << endl << endl;
  for (int i=0;i<natoms;i++)
    cout << anames[i] << " " << newic.coords[3*i+0] << " "  << newic.coords[3*i+1] << " " << newic.coords[3*i+2] << endl;
#endif

  delete [] amassesqm;


  delete [] xyz0;
  delete [] xyz1;
  delete [] xyz2;

  icoords[iN].reset(natoms,anames,anumbers,newic.coords);

  return;
}



void GString::starting_string_dm(double* dq)
{

  int size_ic = newic_dm.nbonds+newic_dm.nangles+newic_dm.ntor+newic_dm.nxyzic;
  int len_d = newic_dm.nicd0;
  double* ictan_dm = new double[size_ic];

// Form the initial string
  int rp = -1;
  int iR,iP,wR,wP,iN;
  for (int n=2;n<nnmax;n++)
  {
    rp *= -1;
    iR = nnR-1;
    iP = nnmax-nnP;
    if (rp==1)
    {
      wR = nnR; 
      iN = wR;
      nnR++;
      wP = -1;
      printf(" creating R node: %i    ",wR);
    }
    else if (rp==-1)
    {
      nnP++;
      wP = nnmax-nnP; 
      wR = -1;
      iN = wP;
      printf(" creating P node: %i    ",wP);
    }
    printf(" iR,iP: %i %i wR,wP: %i %i iN: %i ",iR,iP,wR,wP,iN);

    if (rp==1)
    {
      newic_dm.reset(natoms,anames,anumbers,icoords[iR].coords);
      intic_dm.reset(natoms,anames,anumbers,icoords[iP].coords);
    }
    else if (rp==-1)
    {
      newic_dm.reset(natoms,anames,anumbers,icoords[iP].coords);
      intic_dm.reset(natoms,anames,anumbers,icoords[iR].coords);
    }
    newic_dm.update_ic();
    intic_dm.update_ic();
    //newic_dm.print_bonds();

    newic_dm.bmatp_create();
    intic_dm.bmatp_create();
    newic_dm.bmatp_to_U();
    newic_dm.bmat_create();
    for (int i=0;i<size_ic;i++)
    for (int j=0;j<size_ic;j++)
      intic_dm.Ut[i*size_ic+j] = newic_dm.Ut[i*size_ic+j];
    intic_dm.bmat_create();
    for (int j=0;j<len_d;j++)
      dq[j] = newic_dm.q[j] - intic_dm.q[j];
#if 0
    printf(" printing dq \n");
    for (int j=0;j<len_d;j++)
      printf(" %1.2f",dq[j]);
    printf("\n");
#endif

    double dqmag = 0.;
    for (int j=0;j<len_d;j++)
      dqmag += dq[j]*dq[j];
    dqmag = sqrt(dqmag);
    printf(" dqmag: %1.2f",dqmag);

    for (int i=0;i<size_ic;i++) ictan_dm[i] = 0.;
    for (int i=0;i<size_ic;i++)
    for (int j=0;j<len_d;j++)
      ictan_dm[i] += newic_dm.Ut[j*size_ic+i] * dq[j];

    newic_dm.opt_constraint(ictan_dm);
    newic_dm.bmat_create();
    newic_dm.dq0[newic_dm.nicd0-1] = -dqmag/(nnmax-n);
    printf(" dq0[constraint]: %1.2f \n",newic_dm.dq0[newic_dm.nicd0-1]);
    newic_dm.ic_to_xyz();
    newic_dm.update_ic();
    icoords[iN].reset(natoms,anames,anumbers,newic_dm.coords);


  } //loop over interpolation

  delete [] ictan_dm;

  return;
}


void GString::scan_r(int weig)
{
  printf("\n\n  *** Doing ridge scan *** \n\n");

  int wdir = sign(weig);
  int neig = abs(weig);
  printf(" following mode: %i (sign: %i) \n",weig,wdir);

  int nbonds = newic.nbonds;
  int nangles = newic.nangles;
  int ntor = newic.ntor;
  int nxyzic = newic.nxyzic;
  int size_ic = nbonds+nangles+ntor+nxyzic;
  int len_d = newic.nicd0;

  grad1.init(infile0,natoms,anumbers,anames,icoords[0].coords,runNum,runend,ncpu,3,CHARGE);

  double* C0 = new double[size_ic];
  double* C = new double[size_ic];
  double* D = new double[size_ic];
  double* grad = new double[3*natoms];

//  newic.reset(pTSnodecoords);
#if 1
 //TS towards product
  double* prodxyz = icoords[nnmax-1].coords;
  intic.reset(prodxyz);
#else
 //TS towards reactant
  intic.reset(icoords[0].coords);
#endif
  printf("\n initial node: \n");
  newic.print_xyz();
  newic.print_ic();

  intic.update_ic();
  newic.bmatp_create();
  newic.bmatp_to_U();
  newic.bmat_create();

  //newic.make_Hint();
  string nstr1 = StringTools::int2str(runNum,4,"0");
  string nstr2 = StringTools::int2str(pTSnode-1,2,"0"); //using minus one
  string hfile = "scratch/hessian"+nstr1+"."+nstr2+".icp";
  newic.read_hessp(hfile);
  for (int i=0;i<size_ic*size_ic;i++)
    icoords[pTSnode].Hintp[i] = newic.Hintp[i];
  //newic also gets updated Hint
  //note V_profile was obtained via restart in set_prima
  get_eigenv_finite(pTSnode); 

  newic.update_ic();

  double* xyz0 = new double[3*natoms];
  for (int i=0;i<3*natoms;i++)
    xyz0[i] = newic.coords[i];

  double* heigen = new double[len_d];
  for (int i=0;i<len_d;i++) heigen[i] = 0.;
  double* tmph = new double[len_d*len_d];
  for (int i=0;i<len_d*len_d;i++)
    tmph[i] = newic.Hint[i];
  Diagonalize(tmph,heigen,len_d);

#if 1
  string xyzfile_string = "ridge.xyz";
  ofstream xyzfile;
  xyzfile.open(xyzfile_string.c_str());
  xyzfile.setf(ios::fixed);
  xyzfile.setf(ios::left);
  xyzfile << setprecision(6);
#endif
  
  int MAX_ITERS = 250;
  double* energy = new double[MAX_ITERS];

  for (int i=0;i<len_d;i++)
    newic.dq0[i] = 0.;

  double ss = 0.2;
  weig = 1;
  for (int n=0;n<MAX_ITERS;n++)
  {
    for (int i=0;i<len_d;i++)
      newic.dq0[i] = tmph[weig*len_d+i] * ss * wdir;

    if (n==0)
    {
      printf(" dq0: ");
      for (int i=0;i<len_d;i++)
        printf(" %4.3f",newic.dq0[i]);
      printf("\n");
    }

    newic.ic_to_xyz();
    energy[n] = grad1.grads(newic.coords, grad, newic.Ut, 0);
    printf(" energy: %6.4f \n",energy[n]);

    if (energy[n]>energy[0]+100.)
    {
      printf(" next eigen \n");
      n = 0;
      weig++;
      newic.reset(xyz0);
      if (weig>=neig) break;
    }
#if 1
    xyzfile << " " << natoms << endl;
    xyzfile << " " << energy[n] << endl;
    for (int i=0;i<natoms;i++)
    { 
      xyzfile << "  " << anames[i];
      xyzfile << " " << newic.coords[3*i+0] << " " << newic.coords[3*i+1] << " " << newic.coords[3*i+2] << endl;
    }
#endif
  }

  xyzfile.close();

  delete [] xyz0;
  delete [] grad;
  delete [] tmph;
  delete [] heigen;
  delete [] C0;
  delete [] C;
  delete [] D;


  printf("\n\n  ending early \n");

#if USE_PRIMA
  printf("  this was a prima job \n");
#endif

  exit(1);
  return;

}

void GString::opt_tr()
{
  printf("\n\n  *** Doing ridge search *** \n\n");

  int nbonds = newic.nbonds;
  int nangles = newic.nangles;
  int ntor = newic.ntor;
  int nxyzic = newic.nxyzic;
  int size_ic = nbonds+nangles+ntor+nxyzic;
  int len_d = newic.nicd0;

  double* C0 = new double[size_ic];
  double* C = new double[size_ic];
  double* D = new double[size_ic];

  newic.grad_init(infile0,ncpu,runNum,runend-1,1,CHARGE);
  grad1.init(infile0,natoms,anumbers,anames,icoords[0].coords,runNum,runend,ncpu,3,CHARGE);

//  newic.reset(pTSnodecoords);
#if 1
 //TS towards product
  double* prodxyz = icoords[nnmax-1].coords;
  intic.reset(prodxyz);
#else
 //TS towards reactant
  intic.reset(icoords[0].coords);
#endif
  printf("\n initial node: \n");
  newic.print_xyz();
  newic.print_ic();

  intic.update_ic();
  newic.bmatp_create();
  newic.bmatp_to_U();
  newic.bmat_create();

  //newic.make_Hint();
  string nstr1 = StringTools::int2str(runNum,4,"0");
  string nstr2 = StringTools::int2str(pTSnode-1,2,"0"); //using minus one
  string hfile = "scratch/hessian"+nstr1+"."+nstr2+".icp";
  newic.read_hessp(hfile);
  for (int i=0;i<size_ic*size_ic;i++)
    icoords[pTSnode].Hintp[i] = newic.Hintp[i];
  //newic also gets updated Hint
  //note V_profile was obtained via restart in set_prima
  get_eigenv_finite(pTSnode); 
  newic.davidson_H(2);

  newic.update_ic();

  int ridge = 0;
  find = 0;
  newic.use_constraint = 0;
  newic.isTSnode = 0;
  newic.optCG = 0;
  newic.pgradrms = 10000.;
  newic.OPTTHRESH = CONV_TOL;
  newic.noptdone = 3;

 //was 5
  int osteps0 = 10; //for constrained and eigen searches
  int osteps =  10; //while moving perp.
  int max_iter = MAX_OPT_ITERS;
  for (oi=0;oi<max_iter;oi++)
  {
   //gets tangent between newic and intic
    intic.reset(prodxyz); intic.update_ic();
    tangent_1(C);
    if (ridge)
    {
      intic.reset(pTSnodecoords); intic.update_ic();
      tangent_1(D);
    }
 
    for (int i=0;i<nbonds;i++)
      C0[i] = newic.bondd[i]*C[i];
    for (int i=0;i<nangles;i++)
      C0[nbonds+i] = newic.anglev[i]*3.14159/180*C[nbonds+i];
    for (int i=0;i<ntor;i++)
      C0[nbonds+nangles+i] = newic.torv[i]*3.14159/180*C[nbonds+nangles+i];

#if 0
    printf(" tangent:");
    for (int i=0;i<size_ic;i++)
      printf(" %4.3f",C[i]);
    printf("\n");
#endif

    string nstr = StringTools::int2str(oi,2,"0");
    if (!find && !ridge)
    {
      printf("\n opting with TS constraint \n");
      newic.opt_r("orfile.xyz"+nstr,osteps0,C,C0,D,0); 
    }
    else if (find && !ridge)
    {
      printf("\n TS optimization, no constraint \n");
      newic.opt_eigen_ts("orfile.xyz"+nstr,osteps0,C,C0); 
    }
    else if (ridge==2)
    {
      printf("\n forcing along D \n");
      newic.opt_r("orfile.xyz"+nstr,1,C,C0,D,ridge); 
    }
    else if (ridge==3)
    {
      printf("\n opt/force along D, step: %i \n",oi+1);
      newic.opt_r("orfile.xyz"+nstr,osteps,C,C0,D,ridge); 
    }
    else
      printf(" error: find && ridge problem \n");
    printf(" %s",newic.printout.c_str()); 

    if (ridge==2) ridge = 3;

   //round 1: opt with constraint
   //regular climb until mostly converged
   //eigen_ts search
   //ridge search
    newic.isTSnode = 1;
    if (newic.gradrms<newic.OPTTHRESH*10.)
    {
      if (!find) // && newic.nicd0 != newic.nicd && abs(newic.gradq[newic.nicd0-1])<newic.OPTTHRESH*4.)
      {
#if 0
        newic.bmatp_create();
        newic.bmatp_to_U();
        newic.bmat_create();
        newic.Hintp_to_Hint();
#endif
        newic.use_constraint = 0;
        find = 1;
      }
      else if (find && !ridge && newic.gradrms<newic.OPTTHRESH)
      {
        printf("\n starting ridge search \n");
        ridge = 2;
      }
      //else if (find && ridge && newic.gradrms<newic.OPTTHRESH)
      //  break;
    }
  } //loop over oi

  //extra code
  //get_eigenv_finite(n,ictan);

  delete [] C0;
  delete [] C;
  delete [] D;


  printf("\n\n  ending early \n");

#if USE_PRIMA
  printf("  this was a prima job \n");
#endif

  exit(1);
  return;
}

void GString::opt_r()
{
  printf("\n\n  *** Doing ribbons *** \n\n");

  int nbonds = newic.nbonds;
  int nangles = newic.nangles;
  int ntor = newic.ntor;
  int nxyzic = newic.nxyzic;
  int size_ic = nbonds+nangles+ntor+nxyzic;
  int len_d = newic.nicd0;

  double* C0 = new double[size_ic];
  double* C = new double[size_ic];


#if 1
 //reactant towards product
  addNode(0,1,nnmax-1);
  newic.reset(icoords[1].coords);
  intic.reset(icoords[nnmax-1].coords);
#else
 //product towards reactant
  addNode(nnmax-1,nnmax-2,0);
  newic.reset(icoords[nnmax-2].coords);
  intic.reset(icoords[0].coords);
#endif

  intic.update_ic();
  newic.bmatp_create();
  newic.bmatp_to_U();
  newic.bmat_create();
  newic.make_Hint();
  newic.update_ic();
  printf("\n first node: \n");
  newic.print_xyz();
  newic.print_ic();

  find = 0;
  newic.use_constraint = 1;
  newic.isTSnode = 0; //was using 1,0
  newic.optCG = 0;
  int max_iter = MAX_OPT_ITERS;
  for (oi=0;oi<max_iter;oi++)
  {
   //gets tangent between newic and intic
    tangent_1(C);
 
    for (int i=0;i<nbonds;i++)
      C0[i] = newic.bondd[i]*C[i];
    for (int i=0;i<nangles;i++)
      C0[nbonds+i] = newic.anglev[i]*3.14159/180*C[nbonds+i];
    for (int i=0;i<ntor;i++)
      C0[nbonds+nangles+i] = newic.torv[i]*3.14159/180*C[nbonds+nangles+i];

#if 0
    printf(" tangent:");
    for (int i=0;i<size_ic;i++)
      printf(" %4.3f",C[i]);
    printf("\n");
#endif

    int osteps = 5;
    string nstr = StringTools::int2str(oi,2,"0");
    newic.opt_r("orfile.xyz"+nstr,osteps,C,C0,C,0); 
    printf(" %s",newic.printout.c_str()); 

    newic.isTSnode = 1;
    if (newic.gradrms<newic.OPTTHRESH*25. || oi > 2)
    {
      if (!find)
      {
        newic.use_constraint = 0;
        find = 1;
      }
      else if (newic.gradrms<newic.OPTTHRESH)
        break;
    }
  } //loop over oi

  //extra code
  //get_eigenv_finite(n,ictan);

  delete [] C0;
  delete [] C;


  printf(" ending early \n");

#if USE_PRIMA
  printf("  this was a prima job \n");
#endif

  exit(1);
  return;
}

/**
 * Optimizes each active node. 
 * osteps is the number of optimization steps for the nodes
 * oesteps is the number of eigenvector following steps for the TS
 */
void GString::opt_steps(double** dqa, double** ictan, int osteps, int oesteps)
{
  printf("\n"); fflush(stdout);

  int nbonds = newic.nbonds;
  int nangles = newic.nangles;
  int ntor = newic.ntor;
  int size_icp = newic.nbonds+newic.nangles+newic.ntor;
  int size_ic = size_icp + newic.nxyzic;
  int len_d = newic.nicd0;
  double* aC = new double[nnmax*size_ic]();
  double* aC0 = new double[nnmax*size_ic]();
  int* do_knnr = new int[nnmax];
  for (int n=0;n<nnmax;n++) do_knnr[n] = 0;

 
  int TSnode = 0;
  double emax = -1000;
  for (int n=n0+1;n<nnmax-1;n++)
  if (!find)
    icoords[n].isTSnode = 0;
  if (climb && !find)
  {
    for (int n=n0;n<nnmax-1;n++)
    if (V_profile[n]>emax)
    {
      TSnode = n;
      emax = V_profile[n];
    }
    icoords[TSnode].isTSnode = 1;
  }
  else if (find)
  {
    emax = V_profile[TSnode0];
    TSnode = TSnode0;
  }
  cTSnode = TSnode;

  for (int n=n0+1;n<nnmax-1;n++)
  {
    if (active[n]==-2) //skip node
      active[n] = 0;
    if (active[n]==-1) //not yet added
      active[n] = -2; //will be incremented back to -1
  }

  int fp = 0;
  if (!growing && isSSM) fp = find_peaks(1);

 //previously only for find mode
  int optlastnode = 0;
  if (isSSM && V_profile[nnmax-1] > V_profile[nnmax-2] && climb && fp>0)
    optlastnode = 1;


  if (ptsn!=TSnode && climb && !find)
  {
    printf(" slowing down climb optimization \n");
    icoords[TSnode].DMAX = icoords[TSnode].DMAX/newclimbscale;
    if (newclimbscale<5.0)
      newclimbscale += 1.0;
  }

#if USE_KNNR
  //if (!growing)
  {
    if (!climb)
    {
      for (int n=n0;n<nnmax-1;n++)
      if (V_profile[n]>emax)
        emax = V_profile[n];
    }
    double eknnr = emax - EKTS;
    printf("\n E_knnr: %3.2f \n",eknnr);
    for (int n=n0+1;n<nnmax;n++)
    if (V_profile[n] < eknnr && active[n]>0)
      do_knnr[n] = 1;
    grad1.update_knnr();

    if (oi%KNNR_INTERVAL==0)
    for (int n=n0+1;n<nnmax;n++)
      do_knnr[n] = 0;
  }
#if 1
  //printf(" growing: %i TSnode: %i \n",growing,TSnode);
  printf("\n knnr nodes:");
  for (int n=n0;n<nnmax;n++)
    printf(" %2i",do_knnr[n]);
  printf("\n");
#endif
#endif

#if 0
  printf("\n updating all kNNR's \n");
  for (int n=n0+1;n<nnmax;n++)
  if (active[n]>0)
    icoords[n].grad1.update_knnr();
#endif

#ifdef _OPENMP
#if QCHEM || QCHEMSF || USE_ORCA
 #pragma omp parallel for
#endif
#endif
  for (int n=n0+1;n<nnmax;n++)
  {
//#ifdef _OPENMP
  if (omp_get_num_threads()>1 && active[n]>0)
    printf(" tid: %i/%i node: %i status: %i \n",omp_get_thread_num()+1,omp_get_num_threads(),n,active[n]);
//#endif
    if (active[n]>0 && n!=nnmax-1)
    {
      //printf(" os1l"); fflush(stdout);
      int exsteps = 1; //multiplier for nodes near TSnode in energy
      if (find && V_profile[n]+1.5 > V_profile[TSnode] && n!=TSnode)
        exsteps = 2;

      double* C = &aC[n*size_ic];
      double* C0 = &aC0[n*size_ic];

      icoords[n].update_ic();

      if (!(find && n==TSnode))
      {
        icoords[n].bmatp_create();
        icoords[n].bmatp_to_U();
      }
      if (find && n==TSnode && icoords[n].use_constraint)
        get_eigenv_finite(n,ictan);
 
      double norm = 0.;
      for (int i=0;i<size_ic;i++)
        norm += ictan[n][i]*ictan[n][i];
      norm = sqrt(norm);
      for (int i=0;i<size_ic;i++)
        C[i] = ictan[n][i]/norm;

      for (int i=0;i<nbonds;i++)
        C0[i] = icoords[n].bondd[i]*C[i];
      for (int i=0;i<nangles;i++)
        C0[nbonds+i] = icoords[n].anglev[i]*3.14159/180*C[nbonds+i];
      for (int i=0;i<ntor;i++)
        C0[nbonds+nangles+i] = icoords[n].torv[i]*3.14159/180*C[nbonds+nangles+i];

      string nstr = StringTools::int2str(n,2,"0");
      if (!(find && n==TSnode))
      {
#if !HESS_TANG || USE_MOLPRO || QCHEMSF
        icoords[n].opt_constraint(ictan[n]);
#endif
        if (growing) icoords[n].stage1opt = 1;
        else icoords[n].stage1opt = 0;

        int do_opt = 1;
        if (do_knnr[n]) do_opt = knnr_vs_opt(n);
        if (do_opt) V_profile[n] = icoords[n].opt_c("scratch/xyzfile.xyz"+nstr,osteps*exsteps,C,C0);
      }
      else
        V_profile[n] = icoords[n].opt_eigen_ts("scratch/xyzfile.xyzt"+nstr,oesteps,C,C0);

    } //if active
    if (optlastnode && n==nnmax-1)
    {
      string nstr = StringTools::int2str(n,2,"0");
      V_profile[n] = icoords[n].opt_b("scratch/xyzfile.xyz"+nstr,osteps);
    } 
  } //loop over opt

  printf("\n");
  for (int n=n0+1;n<nnmax-1;n++)
  if (active[n]++>0)
  {
    printf(" %s",icoords[n].printout.c_str());
    if (V_profile[n]>2500.) 
    {
      gradFailCount++;
      V_profile[n] = 111.111;
      icoords[n].gradrms = 0.15;
      if (find && n==TSnode)
      {
        find = 0; 
        printf(" TS node failed to converge SCF, resetting exact TS search \n");
      }
      else if (climb && !find && n==TSnode)
      {
        climb = 0; 
        printf(" TS node failed to converge SCF, resetting climb \n");
      }
    } //if SCF failed

    gradJobCount += icoords[n].noptdone;
#if ONE_SKIP
    if (icoords[n].gradrms<icoords[n].OPTTHRESH && n!=TSnode && V_profile[n]+5.0 < V_profile[TSnode])
    {
      //printf(" node converged, inactiving for one step \n");
      active[n] = -2;
      icoords[n].noptdone = 0;
    } //if low grad
#endif
  } //loop over opt

  if (optlastnode)
  {
    printf("\n last node opt \n");
    printf(" %s",icoords[nnmax-1].printout.c_str());
    gradJobCount += icoords[nnmax-1].noptdone;
  }

  if (gradFailCount>25)
  {
    printf("\n opt_i: Exiting! Too many failed SCF's tgrads: %3i \n",gradJobCount);
    exit(-1);
  }

  if (climb)
    ptsn = TSnode;

  delete [] aC;
  delete [] aC0;

  delete [] do_knnr;

  return;
}


int GString::knnr_vs_opt(int n)
{
  printf("\n kvo: %i",n); fflush(stdout);

  int knnr_fail = 0;
  int nicd = newic.nicd;
  icoords[n].noptdone = 0;

  V_profile[n] = grad1.grads(icoords[n].coords, grads[n], icoords[n].Ut, 2) - V0; //2 is force knnR

  double grms = 0.;
  for (int i=0;i<nicd;i++)
    grms += grads[n][i]*grads[n][i];
  grms = sqrt(grms/nicd);
  if (V_profile[n] < -1000.)
  {
    V_profile[n] = 0.;
    knnr_fail = 1;
    grms = 0.;
  }

  icoords[n].gradrms = grms/1.; //parameter

  char* pchr = new char[100];
  sprintf(pchr,"kNNR E: %3.1f grms: %4.3f \n",V_profile[n],grms);
  icoords[n].printout = pchr;
  delete [] pchr;

  printf(" E(knn): %3.1f",V_profile[n]);
  printf(" grms: %5.4f",grms);
  if (grms>GKOPT) printf("x");
  printf("\n");

  if (grms>GKOPT || grms<0.000001)
    knnr_fail = 1;

//CPMZ always fail for testing
//  knnr_fail = 1;

  return knnr_fail;
}


int GString::close_dist_fix(int type)
{
  int N3 = 3*natoms;

  int* newbonds = new int[natoms*natoms];
  for (int i=0;i<natoms*natoms;i++) newbonds[i] = 0.;
 
  double* dist = new double[natoms*natoms];
  for (int i=0;i<natoms*natoms;i++) dist[i] = 0.;

  int nadd = 0;
#if 0
  for (int n=1;n<nnR;n++)
    nadd += check_close_dist(n,dist,&newbonds[2*nadd]);
  for (int n=nnmax-nnP;n<nnmax-1;n++)
    nadd += check_close_dist(n,dist,&newbonds[2*nadd]);
#endif
  for (int n=1;n<nnmax-1;n++)
  if (active[n])
    nadd += check_close_dist(n,dist,&newbonds[2*nadd]);

  if (nadd) 
  {
    add_bonds(nadd,newbonds);
    for (int n=1;n<nnR;n++)
    {
      icoords[n].bmatp_create();
      icoords[n].bmatp_to_U();
      icoords[n].bmat_create();
      icoords[n].make_Hint();
    }
    for (int n=nnmax-nnP;n<nnmax-1;n++)
    {
      icoords[n].bmatp_create();
      icoords[n].bmatp_to_U();
      icoords[n].bmat_create();
      icoords[n].make_Hint();
    }
  }

  delete [] newbonds;
  delete [] dist;

  if (nadd>0)
    printf(" close_dist type: %i nadd: %i \n",type,nadd);
  if (type==1 && nadd>0)
    return 1;

  return 0;
}

int GString::check_close_dist(int n, double* dist, int* newbonds) 
{
  int nclose = 0;
  int N3 = 3*natoms;

  for (int i=0;i<natoms;i++)
  for (int j=0;j<i;j++)
  {
    double x0 = allcoords[n][3*i+0]-allcoords[n][3*j+0];
    double y0 = allcoords[n][3*i+1]-allcoords[n][3*j+1];
    double z0 = allcoords[n][3*i+2]-allcoords[n][3*j+2];
    //printf(" xyz: %2.1f %2.1f %2.1f \n",x0,y0,z0);
    dist[i*natoms+j] = dist[j*natoms+i] = sqrt(x0*x0+y0*y0+z0*z0);
  }

  for (int i=0;i<natoms;i++)
  for (int j=0;j<i;j++)
  {
    if (dist[i*natoms+j]<(icoords[n].getR(i)+icoords[n].getR(j))/2.0 && !icoords[n].bond_exists(i,j)) 
    {
      printf(" Close dist (n: %i): %i %i: %4.3f bond: %i \n",n,i,j,dist[i*natoms+j],icoords[n].bond_exists(i,j));
      newbonds[2*nclose+0] = i;
      newbonds[2*nclose+1] = j;
      nclose++;
    } 
  }

  return nclose;  
}

void GString::add_bonds(int nadd, int* newbonds)
{
  for (int i=0;i<nadd;i++)
  {
    int a1 = newbonds[2*i];
    int a2 = newbonds[2*i+1];
    int found = 0;
    for (int j=0;j<i;j++)
    if (a1==newbonds[2*j] && a2==newbonds[2*j+1])
      found = 1;
    if (!found && !newic.bond_exists(a1,a2))
    {
      printf(" adding bond: %i %i \n",a1,a2);

      newic.bonds[newic.nbonds][0]     = a1;
      newic.bonds[newic.nbonds++][1]   = a2;
      intic.bonds[intic.nbonds][0]     = a1;
      intic.bonds[intic.nbonds++][1]   = a2;
      int2ic.bonds[int2ic.nbonds][0]   = a1;
      int2ic.bonds[int2ic.nbonds++][1] = a2;
  
      for (int n=0;n<nnmax0;n++)
        icoords[n].copy_ic(newic);

    } // if adding bond
  } //loop i over new bonds


  return;
}


void GString::add_angles(int nadd, int* newangles)
{
  for (int i=0;i<nadd;i++)
  {
    int a1 = newangles[3*i+0];
    int a2 = newangles[3*i+1];
    int a3 = newangles[3*i+2];
    int found = 0;
    for (int j=0;j<i;j++)
    if (a1==newangles[3*j] && a2==newangles[3*j+1] && a3==newangles[3*j+2])
      found = 1;
    if (!found && newic.angle_num(a1,a2,a3)==-1)
    {
      printf(" adding angle: %i %i %i \n",a1,a2,a3);

      newic.angles[newic.nangles][0]    = a1;
      newic.angles[newic.nangles][1]    = a2;
      newic.angles[newic.nangles][2]    = a3;
      intic.angles[intic.nangles][0]    = a1;
      intic.angles[intic.nangles][1]    = a2;
      intic.angles[intic.nangles][2]    = a3;
      int2ic.angles[int2ic.nangles][0]  = a1;
      int2ic.angles[int2ic.nangles][1]  = a2;
      int2ic.angles[int2ic.nangles][2]  = a3;
      newic.nangles++;
      intic.nangles++;
      int2ic.nangles++;
  
      for (int n=0;n<nnmax0;n++)
        icoords[n].copy_ic(newic);

    } // if adding angle
  } //loop i over new bonds


  return;
}

///reparametrizes the string along the constraint during the growth phase, only used by DE
void GString::ic_reparam_g(double** dqa, double* dqmaga) 
{
  if (bondfrags<2)
    close_dist_fix(0);

  int size_ic = newic.nbonds+newic.nangles+newic.ntor+newic.nxyzic;
  int len_d = newic.nicd0;
  double** ictan0 = new double*[nnmax];
  for (int i=0;i<nnmax;i++)
    ictan0[i] = new double[size_ic];
  double** ictan = new double*[nnmax];
  for (int i=0;i<nnmax;i++)
    ictan[i] = new double[size_ic];

  double* rpmove = new double[nnmax];
  for (int i=0;i<nnmax;i++) rpmove[i] = 0.;
  double* rpart = new double[nnmax];
  for (int i=0;i<nnmax;i++) rpart[i] = 0.;

  double totaldqmag,dqavg,disprms;
  double h1dqmag,h2dqmag;
  double* dE = new double[nnmax];
  double* edist = new double[nnmax];

  int TSnode = -1;
  double emax = -1000;

  for (int i=0;i<nnmax;i++) dqmaga[i] = 0.;

  for (int i=0;i<ic_reparam_steps;i++)
  {
   //tangents referenced to left or right during growing phase
    get_tangents_1g(dqa,dqmaga,ictan0); 

    totaldqmag = 0.;
    for (int n=n0;n<nnR-1;n++)
      totaldqmag += dqmaga[n];
    for (int n=nnmax-nnP+1;n<nnmax;n++)
      totaldqmag += dqmaga[n];
    //totaldqmag += dqmaga[nnR-1];
    //printf(" totaldqmag (without inner): %1.1f \n",totaldqmag);
#if 0
    printf(" printing spacings dqmaga: ");
    for (int n=0;n<nnmax;n++)
      printf(" %1.2f",dqmaga[n]);
    printf("\n");
#endif

    //using average
    if (i==0)
    {
      for (int n=n0;n<nnmax;n++) rpart[n] = 0.;
      for (int n=n0+1;n<nnR;n++) 
        rpart[n] = 1./(nn-2);
      for (int n=nnmax-nnP;n<nnmax-1;n++)
        rpart[n] = 1./(nn-2);

      printf(" rpart: ");
      for (int n=1;n<nnmax;n++)
        printf(" %1.2f",rpart[n]);
      printf("\n");
    }

   //reparam interior nodes once string is nearly grown
    int nnR0 = nnR;
    int nnP0 = nnP;
#if !REPARAM_G_INTERIOR
    if (nnmax-nn>2)
    {
      nnR0--;
      nnP0--;
    }
#endif

    double deltadq = 0.;
    for (int n=n0+1;n<nnR0;n++) //was nnR
    {
      deltadq = dqmaga[n-1] - totaldqmag * rpart[n];
//      deltadq = dqmaga[n-1] - h1dqmag * rpart[n];
      rpmove[n] = - deltadq;
    }
    for (int n=nnmax-nnP0;n<nnmax-1;n++) //was nnP
    {
      deltadq = dqmaga[n+1] - totaldqmag * rpart[n];
//      deltadq = dqmaga[n+1] - h2dqmag * rpart[n];
      rpmove[n] = - deltadq;
    }

    double MAXRE=1.1;
#if 0
    for (int n=n0+1;n<nnmax-1;n++)
      printf(" 0disp[%i]: %1.2f",n,rpmove[n]);
    printf("\n");
#endif

    for (int n=n0+1;n<nnmax-1;n++)
      if(abs(rpmove[n])>MAXRE)
        rpmove[n] = sign(rpmove[n])*MAXRE;
#if 0
    for (int n=n0+1;n<nnmax-2;n++)
      rpmove[n+1] += rpmove[n]; 
    for (int n=n0+1;n<nnmax-1;n++)
      if(abs(rpmove[n])>MAXRE)
        rpmove[n] = sign(rpmove[n])*MAXRE;
#endif
#if 0
    for (int n=n0+1;n<nnmax-1;n++)
      printf(" disp[%i]: %1.2f",n,rpmove[n]);
    printf("\n");
#endif
    disprms = 0.;
    for (int n=n0+1;n<nnmax-1;n++)
      disprms += rpmove[n]*rpmove[n];
    disprms = sqrt(disprms);
    lastdispr = disprms;
    //printf(" disprms: %1.3f \n",disprms);
 
    if (disprms<0.02) break;

    for (int n=n0+1;n<nnmax-1;n++)
    if (rpmove[n] > 0.) //tangent points inward, so don't move other direction
    {
      newic.reset(natoms,anames,anumbers,icoords[n].coords);
      newic.update_ic();

      newic.bmatp_create();
      newic.bmatp_to_U();

      for (int j=0;j<size_ic;j++)
        ictan[n][j] = ictan0[n][j];

      newic.opt_constraint(ictan[n]);
      newic.bmat_create();
      for (int j=0;j<newic.nicd0;j++) newic.dq0[j]=0.;
      newic.dq0[newic.nicd0-1] = rpmove[n];
      newic.ic_to_xyz();

      icoords[n].reset(natoms,anames,anumbers,newic.coords);
    }//loop n over nodes
    
  } // loop i over reparam steps

#if 1
  printf(" spacings (end ic_reparam, steps: %i): ",ic_reparam_steps);
  for (int n=0;n<nnmax;n++)
    printf(" %1.2f",dqmaga[n]);
  printf("  disprms: %1.3f ",disprms);
  printf("\n");
#endif
  int failed = check_array(nnmax,dqmaga);
  if (failed)
  {
    printf(" ERROR: bad spacings \n");
    exit(-1);
  }

  for (int i=0;i<nnmax;i++)
    delete [] ictan0[i];
  delete [] ictan0;
  for (int i=0;i<nnmax;i++)
    delete [] ictan[i];
  delete [] ictan;
  delete [] rpmove;
  delete [] rpart;
  delete [] dE;
  delete [] edist;
 
  return;
}

///reparametrizes the string along the constraint after the growth phase
void GString::ic_reparam(double** dqa, double* dqmaga, int rtype) 
{
  int size_ic = newic.nbonds+newic.nangles+newic.ntor+newic.nxyzic;
  int len_d = newic.nicd0;
  int ictalloc = nnmax+1;
  double** ictan0 = new double*[ictalloc];
  for (int i=0;i<ictalloc;i++)
    ictan0[i] = new double[size_ic];
  double** ictan = new double*[ictalloc];
  for (int i=0;i<ictalloc;i++)
    ictan[i] = new double[size_ic];

  double* rpmove = new double[ictalloc];
  for (int i=0;i<ictalloc;i++) rpmove[i] = 0.;
  double* rpart = new double[ictalloc];
  for (int i=0;i<ictalloc;i++) rpart[i] = 0.;

  double totaldqmag,dqavg,disprms;
  double h1dqmag,h2dqmag;
  double* dE = new double[ictalloc];
  double* edist = new double[ictalloc];


  if (rtype==1 || rtype==2 || climb)
  {
    printf(" V_profile:");
    for (int n=0;n<nnmax;n++)
      printf(" %1.1f",V_profile[n]);
    printf("\n");
  }
  print_em(nnmax);

  int TSnode = 0;
  double emax = -1000;
  if (climb && !find)
  {
    for (int n=n0;n<nnmax;n++)
    if (V_profile[n]>emax)
    {
      TSnode = n;
      emax = V_profile[n];
    } 
  }
  else if (find)
  {   
    emax = V_profile[TSnode0];
    TSnode = TSnode0;
  }
  printf(" TSn: %i",TSnode); fflush(stdout);



  for (int i=0;i<ic_reparam_steps;i++)
  {
   //tangents pairwise, returns distance magnitudes also
    get_tangents_1(dqa,dqmaga,ictan0); 

    totaldqmag = 0.;
    for (int n=n0+1;n<nnmax;n++)
      totaldqmag += dqmaga[n];
    dqavg = totaldqmag/(nnmax-1);
#if 0
    //printf(" spacing average: %1.2f ",dqavg);
    printf(" printing spacings dqmaga: ");
    for (int n=1;n<nnmax;n++)
      printf(" %1.2f",dqmaga[n]);
    printf("\n");
#endif

    if (climb || rtype==2)
    {
      h1dqmag = 0.;
      h2dqmag = 0.;

      for (int n=n0+1;n<=TSnode;n++)
        h1dqmag += dqmaga[n];
      for (int n=TSnode+1;n<nnmax;n++)
        h2dqmag += dqmaga[n];

      //printf(" h1dqmag, h2dqmag: %1.1f %1.1f \n",h1dqmag,h2dqmag);
    }

 
    ///rtype==0 using average
    if (rtype==0 && i==0)
    {
      if (!climb)
      for (int n=n0+1;n<nnmax;n++)
        rpart[n] = 1./(nnmax-1-n0);
      else
      {
        for (int n=n0+1;n<TSnode;n++)
          rpart[n] = 1./(TSnode-n0); //CPMZ just fixed
        for (int n=TSnode+1;n<nnmax;n++)
          rpart[n] = 1./(nnmax-TSnode-1);
        rpart[TSnode] = 0.;
      }
    }
    if (rtype==1 && i==0)
    {
// want to create partition, then iterate to convergence
      double dEmax = 0.;
      for (int n=n0+1;n<nnmax;n++)
        dE[n] = abs(V_profile[n]-V_profile[n-1]);
      for (int n=n0+1;n<nnmax;n++)
      if (dE[n]>dEmax)
        dEmax = dE[n];
      for (int n=n0+1;n<nnmax;n++)
        edist[n] = dE[n] * dqmaga[n];

     // printf(" computing dE's, dEmax: %1.1f \n",dEmax);
     // for (int n=1;n<nnmax-1;n++)
     //   printf(" Vn[%i]-Vn[%i] %1.1f \n",n-1,n,dE[n]);

      printf(" edist: ");
      for (int n=n0+1;n<nnmax;n++) 
        printf(" %1.1f",edist[n]);
      printf("\n");

      double edqavg;
      double totaledq = 0.;
      for (int n=n0+1;n<nnmax;n++) 
        totaledq += edist[n];
      edqavg = totaledq/(nnmax-1);

      for (int n=n0+1;n<nnmax;n++)
        rpart[n] = 1.0 + edist[n] / totaledq;
      for (int n=n0+1;n<nnmax;n++)
        rpart[n] = 1/rpart[n];
      double norm = 0.;
      for (int n=n0+1;n<nnmax;n++)
        norm += rpart[n];
      //norm = sqrt(norm);
      for (int n=n0+1;n<nnmax;n++)
        rpart[n] = rpart[n]/norm;
    }

    if (i==0)
    {
      printf(" rpart: ");
      for (int n=1;n<nnmax;n++)
        printf(" %1.2f",rpart[n]);
      printf("\n");
    }


    if (!climb && rtype!=2)
    for (int n=n0+1;n<nnmax-1;n++)
    {
      double deltadq = dqmaga[n] - totaldqmag * rpart[n];
      if (n==nnmax-2) deltadq += totaldqmag * rpart[n] - dqmaga[n+1];
      rpmove[n] = -deltadq;
    } //loop n over active nodes
    else
    {
      double deltadq = 0.;
      rpmove[TSnode] = 0.;
      for (int n=n0+1;n<TSnode;n++)
      {
        deltadq = dqmaga[n] - h1dqmag * rpart[n];
        if (n==nnmax-2) deltadq += h2dqmag * rpart[n] - dqmaga[n+1];
        rpmove[n] = - deltadq;
      }
      for (int n=TSnode+1;n<nnmax-1;n++)
      {
        deltadq = dqmaga[n] - h2dqmag * rpart[n];
        if (n==nnmax-2) deltadq += h2dqmag * rpart[n] - dqmaga[n+1];
        rpmove[n] = - deltadq;
      }
    }

    double MAXRE=0.5; //was 1.1
#if 0
    for (int n=1;n<nnmax-1;n++)
      printf(" 0disp[%i]: %1.2f",n,rpmove[n]);
    printf("\n");
#endif
    for (int n=n0+1;n<nnmax-1;n++)
      if(abs(rpmove[n])>MAXRE)
        rpmove[n] = sign(rpmove[n])*MAXRE;
    for (int n=n0+1;n<nnmax-2;n++)
    if (n+1!=TSnode || !climb)
      rpmove[n+1] += rpmove[n]; 
    for (int n=n0+1;n<nnmax-1;n++)
      if(abs(rpmove[n])>MAXRE)
        rpmove[n] = sign(rpmove[n])*MAXRE;
    if (climb || rtype==2) rpmove[TSnode] = 0.;
#if 1
    for (int n=n0+1;n<nnmax-1;n++)
      printf(" disp[%i]: %1.2f",n,rpmove[n]);
    printf("\n");
#endif
    disprms = 0.;
    for (int n=n0+1;n<nnmax-1;n++)
      disprms += rpmove[n]*rpmove[n];
    disprms = sqrt(disprms);
    lastdispr = disprms;
    //printf(" disprms: %1.3f \n",disprms);
 
    if (disprms<0.02) break;

    for (int n=n0+1;n<nnmax-1;n++)
    {
      newic.reset(natoms,anames,anumbers,icoords[n].coords);
      newic.update_ic();

      newic.bmatp_create();
      newic.bmatp_to_U();

      if (rpmove[n] < 0.)
      {
        for (int j=0;j<size_ic;j++)
          ictan[n][j] = ictan0[n][j];
      }
      else
      {
        for (int j=0;j<size_ic;j++)
          ictan[n][j] = ictan0[n+1][j];
      }

      newic.opt_constraint(ictan[n]);
      newic.bmat_create();
      for (int j=0;j<newic.nicd0;j++) newic.dq0[j]=0.;
      newic.dq0[newic.nicd0-1] = rpmove[n];
      newic.ic_to_xyz();

      icoords[n].reset(natoms,anames,anumbers,newic.coords);
    }//loop n over nodes
    
  } // loop i over reparam steps

#if 1
  printf(" spacings (end ic_reparam, steps: %i): ",ic_reparam_steps);
  for (int n=1;n<nnmax;n++)
    printf(" %1.2f",dqmaga[n]);
  printf("  disprms: %1.3f ",disprms);
  printf("\n");
#endif
  int failed = check_array(nnmax,dqmaga);
  if (failed)
  {
    printf(" ERROR: bad spacings \n");
    exit(-1);
  }

  int need_space = -1;
  if (isSSM && climb && disprms<0.1)
  {
    get_tangents_1(dqa,dqmaga,ictan0); 

    for (int n=n0+1;n<nnmax;n++)
    if (dqmaga[n]>QDISTMAX)
    {
      printf(" need to expand near node %i: %4.1f \n",n,dqmaga[n]);
      need_space = n;
      if (find)
      {
        printf(" TS search reset \n");
        find = 0;
      }
      break;
    }
    if (need_space>-1)
    {
      int success = addCNode(need_space);
      if (!success) need_space = -1;
    }
    //print_string(nnmax,allcoords,"stringfile.xyzacn");
  }

  for (int i=0;i<ictalloc;i++)
    delete [] ictan0[i];
  delete [] ictan0;
  for (int i=0;i<ictalloc;i++)
    delete [] ictan[i];
  delete [] ictan;
  delete [] rpmove;
  delete [] rpart;
  delete [] dE;
  delete [] edist;

  if (need_space>-1)
    ic_reparam(dqa,dqmaga,rtype);

  return;
}


void GString::ic_reparam_new(double** dqa, double* dqmaga, int rtype) 
{
  int size_ic = newic.nbonds+newic.nangles+newic.ntor+newic.nxyzic;
  int len_d = newic.nicd0;
  double** ictan0 = new double*[nnmax];
  for (int i=0;i<nnmax;i++)
    ictan0[i] = new double[size_ic];
  double** ictan = new double*[nnmax];
  for (int i=0;i<nnmax;i++)
    ictan[i] = new double[size_ic];

  double* rpmove = new double[nnmax];
  for (int i=0;i<nnmax;i++) rpmove[i] = 0.;
  double* rpart = new double[nnmax];
  for (int i=0;i<nnmax;i++) rpart[i] = 0.;

  double totaldqmag,dqavg,disprms;
  double h1dqmag,h2dqmag;
  double* dE = new double[nnmax];
  double* edist = new double[nnmax];

  if (rtype==1 || rtype==2 || climb)
  {
    printf(" V_profile:");
    for (int n=0;n<nnmax;n++)
      printf(" %1.1f",V_profile[n]);
    printf("\n");
  }

  int TSnode = -1;
  double emax = -1000;
  if (climb && !find)
  {
    for (int n=n0;n<nnmax;n++)
    if (V_profile[n]>emax)
    {
      TSnode = n;
      emax = V_profile[n];
    } 
  }
  else if (find)
  {   
    emax = V_profile[TSnode0];
    TSnode = TSnode0;
  }

  for (int i=0;i<ic_reparam_steps;i++)
  {
   //tangents pairwise, returns distance magnitudes also
    get_tangents_1(dqa,dqmaga,ictan0); 

    totaldqmag = 0.;
    for (int n=n0+1;n<nnmax;n++)
      totaldqmag += dqmaga[n];
    dqavg = totaldqmag/(nnmax-1);
#if 1
    //printf(" spacing average: %1.2f ",dqavg);
    printf(" printing spacings dqmaga: ");
    for (int n=1;n<nnmax;n++)
      printf(" %1.2f",dqmaga[n]);
    printf("\n");
#endif
    if (climb || rtype==2)
    {
      h1dqmag = 0.;
      h2dqmag = 0.;

      for (int n=n0+1;n<=TSnode;n++)
        h1dqmag += dqmaga[n];
      for (int n=TSnode+1;n<nnmax;n++)
        h2dqmag += dqmaga[n];
  
      printf(" h1dqmag, h2dqmag: %1.1f %1.1f \n",h1dqmag,h2dqmag);
    }

 
    //using average
    if (rtype==0 && i==0)
    {
      if (!climb)
      for (int n=n0+1;n<nnmax;n++)
        rpart[n] = 1./(nnmax-1);
      else
      {
        for (int n=n0+1;n<TSnode;n++)
          rpart[n] = 1./(TSnode-n0);
        for (int n=TSnode+1;n<nnmax;n++)
          rpart[n] = 1./(nnmax-TSnode-1);
        rpart[TSnode] = 0.;
      }
    }

    if (i==0)
    {
      printf(" rpart: ");
      for (int n=1;n<nnmax;n++)
        printf(" %1.2f",rpart[n]);
      printf("\n");
    }


    if (!climb && rtype!=2)
    for (int n=n0+1;n<nnmax-1;n++)
    {
      double deltadq = dqmaga[n] - totaldqmag * rpart[n];
      if (n==nnmax-2) deltadq += totaldqmag * rpart[n] - dqmaga[n+1];
      rpmove[n] = -deltadq;
    } //loop n over active nodes
    else
    {
      double deltadq = 0.;
      rpmove[TSnode] = 0.;
      for (int n=n0+1;n<TSnode;n++)
      {
        deltadq = dqmaga[n] - h1dqmag * rpart[n];
        if (n==nnmax-2) deltadq += h2dqmag * rpart[n] - dqmaga[n+1];
        rpmove[n] = - deltadq;
      }
      for (int n=TSnode+1;n<nnmax-1;n++)
      {
        deltadq = dqmaga[n] - h2dqmag * rpart[n];
        if (n==nnmax-2) deltadq += h2dqmag * rpart[n] - dqmaga[n+1];
        rpmove[n] = - deltadq;
      }
    }

    double MAXRE=0.5; //was 1.1
#if 0
    for (int n=1;n<nnmax-1;n++)
      printf(" 0disp[%i]: %1.2f",n,rpmove[n]);
    printf("\n");
#endif
    for (int n=n0+1;n<nnmax-1;n++)
      if(abs(rpmove[n])>MAXRE)
        rpmove[n] = sign(rpmove[n])*MAXRE;
    for (int n=n0+1;n<nnmax-2;n++)
    if (n+1!=TSnode || !climb)
      rpmove[n+1] += rpmove[n]; 
    for (int n=n0+1;n<nnmax-1;n++)
      if(abs(rpmove[n])>MAXRE)
        rpmove[n] = sign(rpmove[n])*MAXRE;
    if (climb || rtype==2) rpmove[TSnode] = 0.;
#if 1
    for (int n=1;n<nnmax-1;n++)
      printf(" disp[%i]: %1.2f",n,rpmove[n]);
    printf("\n");
#endif
    disprms = 0.;
    for (int n=n0+1;n<nnmax-1;n++)
      disprms += rpmove[n]*rpmove[n];
    disprms = sqrt(disprms);
    lastdispr = disprms;
    //printf(" disprms: %1.3f \n",disprms);
 
    if (disprms<0.02) break;

    for (int n=n0+1;n<nnmax-1;n++)
    {
      newic.reset(natoms,anames,anumbers,icoords[n].coords);
      newic.update_ic();

      newic.bmatp_create();
      newic.bmatp_to_U();

      if (rpmove[n] < 0.)
      {
        for (int j=0;j<size_ic;j++)
          ictan[n][j] = ictan0[n][j];
      }
      else
      {
        for (int j=0;j<size_ic;j++)
          ictan[n][j] = ictan0[n+1][j];
      }

      newic.opt_constraint(ictan[n]);
      newic.bmat_create();
      for (int j=0;j<newic.nicd0;j++) newic.dq0[j]=0.;
      newic.dq0[newic.nicd0-1] = rpmove[n];
      newic.ic_to_xyz();

      icoords[n].reset(natoms,anames,anumbers,newic.coords);
    }//loop n over nodes
    
  } // loop i over reparam steps

#if 1
  printf(" spacings (end ic_reparam, steps: %i): ",ic_reparam_steps);
  for (int n=1;n<nnmax;n++)
    printf(" %1.2f",dqmaga[n]);
  printf("  disprms: %1.3f ",disprms);
  printf("\n");
#endif

  for (int i=0;i<nnmax;i++)
    delete [] ictan0[i];
  delete [] ictan0;
  for (int i=0;i<nnmax;i++)
    delete [] ictan[i];
  delete [] ictan;
  delete [] rpmove;
  delete [] rpart;
  delete [] dE;
  delete [] edist;
 
  return;
}



void GString::ic_reparam_h(double** dqa, double* dqmaga, int rtype) 
{
  if (isSSM)
    printf(" NOT SET UP FOR SSM \n");

  int size_ic = newic.nbonds+newic.nangles+newic.ntor+newic.nxyzic;
  int len_d = newic.nicd0;
  double** ictan0 = new double*[nnmax];
  for (int i=0;i<nnmax;i++)
    ictan0[i] = new double[size_ic];
  double** ictan = new double*[nnmax];
  for (int i=0;i<nnmax;i++)
    ictan[i] = new double[size_ic];

  double* rpmove = new double[nnmax];
  for (int i=0;i<nnmax;i++) rpmove[i] = 0.;
  double* rpart = new double[nnmax];
  for (int i=0;i<nnmax;i++) rpart[i] = 0.;

  double totaldqmag,dqavg,disprms;
  double h1dqmag,h2dqmag;
  double* dE = new double[nnmax];
  double* edist = new double[nnmax];

  if (rtype==1 || rtype==2 || climb)
  {
    printf(" V_profile:");
    for (int n=0;n<nnmax;n++)
      printf(" %1.1f",V_profile[n]);
    printf("\n");
  }
  print_em(nnmax);

  int TSnode = -1;
  double emax = -1000;
  if (climb && !find)
  {
  for (int n=0;n<nnmax;n++)
    if (V_profile[n]>emax)
    {
      TSnode = n;
      emax = V_profile[n];
    } 
  }
  else if (find)
  {   
    emax = V_profile[TSnode0];
    TSnode = TSnode0;
  }

  printf(" overlap:");
  for (int n=0;n<nnmax;n++)
    printf(" %3.2f",icoords[n].path_overlap);
  printf("\n");

  for (int i=0;i<ic_reparam_steps;i++)
  {
   //tangents pairwise, returns distance magnitudes also
    get_tangents_1(dqa,dqmaga,ictan0); 

    totaldqmag = 0.;
    for (int n=1;n<nnmax;n++)
      totaldqmag += dqmaga[n];
    dqavg = totaldqmag/(nnmax-1);
#if 0
    //printf(" spacing average: %1.2f ",dqavg);
    printf(" printing spacings dqmaga: ");
    for (int n=1;n<nnmax;n++)
      printf(" %1.2f",dqmaga[n]);
    printf("\n");
#endif
    if (climb || rtype==2)
    {
      h1dqmag = 0.;
      h2dqmag = 0.;

      for (int n=1;n<=TSnode;n++)
        h1dqmag += dqmaga[n];
      for (int n=TSnode+1;n<nnmax;n++)
        h2dqmag += dqmaga[n];
  
      printf(" h1dqmag, h2dqmag: %1.1f %1.1f \n",h1dqmag,h2dqmag);
    }

 
    //using average
    if (rtype==0 && i==0)
    {
      if (!climb)
      for (int n=1;n<nnmax;n++)
        rpart[n] = 1./(nnmax-1);
      else
      {
        for (int n=1;n<TSnode;n++)
          rpart[n] = 1./TSnode;
        for (int n=TSnode+1;n<nnmax;n++)
          rpart[n] = 1./(nnmax-TSnode-1);
        rpart[TSnode] = 0.;
      }
    }

    if (i==0)
    {
      printf(" rpart: ");
      for (int n=1;n<nnmax;n++)
        printf(" %1.2f",rpart[n]);
      printf("\n");
    }


    if (!climb && rtype!=2)
    for (int n=1;n<nnmax-1;n++)
    {
      double deltadq = dqmaga[n] - totaldqmag * rpart[n];
      if (n==nnmax-2) deltadq += totaldqmag * rpart[n] - dqmaga[n+1];
      rpmove[n] = -deltadq;
    } //loop n over active nodes
    else
    {
      double deltadq = 0.;
      rpmove[TSnode] = 0.;
      for (int n=1;n<TSnode;n++)
      {
        deltadq = dqmaga[n] - h1dqmag * rpart[n];
        if (n==nnmax-2) deltadq += h2dqmag * rpart[n] - dqmaga[n+1];
        rpmove[n] = - deltadq;
      }
      for (int n=TSnode+1;n<nnmax-1;n++)
      {
        deltadq = dqmaga[n] - h2dqmag * rpart[n];
        if (n==nnmax-2) deltadq += h2dqmag * rpart[n] - dqmaga[n+1];
        rpmove[n] = - deltadq;
      }
    }

    double MAXRE=1.1;
#if 0
    for (int n=1;n<nnmax-1;n++)
      printf(" 0disp[%i]: %1.2f",n,rpmove[n]);
    printf("\n");
#endif
    for (int n=1;n<nnmax-1;n++)
      if(abs(rpmove[n])>MAXRE)
        rpmove[n] = sign(rpmove[n])*MAXRE;
    for (int n=1;n<nnmax-2;n++)
    if (n+1!=TSnode || !climb)
      rpmove[n+1] += rpmove[n]; 
    for (int n=1;n<nnmax-1;n++)
      if(abs(rpmove[n])>MAXRE)
        rpmove[n] = sign(rpmove[n])*MAXRE;
    if (climb || rtype==2) rpmove[TSnode] = 0.;
#if 1
    for (int n=1;n<nnmax-1;n++)
      printf(" disp[%i]: %1.2f",n,rpmove[n]);
    printf("\n");
#endif
    disprms = 0.;
    for (int n=1;n<nnmax-1;n++)
      disprms += rpmove[n]*rpmove[n];
    disprms = sqrt(disprms);
    //printf(" disprms: %1.3f \n",disprms);
 
    if (disprms<0.02) break;

    for (int n=1;n<nnmax-1;n++)
    {
      int useol = 0;
      if (icoords[n].path_overlap>0. && icoords[n].nicd0 == icoords[n].nicd)
        useol = 1;

      newic.reset(natoms,anames,anumbers,icoords[n].coords);
      newic.update_ic();

      newic.bmatp_create();
      if (useol)
      {
        for (int i=0;i<len_d*size_ic;i++)
          newic.Ut[i] = icoords[n].Ut[i];
      }
      else
        newic.bmatp_to_U();

      if (rpmove[n] < 0.)
      {
        for (int j=0;j<size_ic;j++)
          ictan[n][j] = ictan0[n][j];
      }
      else
      {
        for (int j=0;j<size_ic;j++)
          ictan[n][j] = ictan0[n+1][j];
      }

      if (!useol)
        newic.opt_constraint(ictan[n]);
      newic.bmat_create();
      for (int j=0;j<newic.nicd0;j++) newic.dq0[j]=0.;

      if (useol)
        newic.dq0[icoords[n].path_overlap_n] = rpmove[n];
      else
        newic.dq0[newic.nicd0-1] = rpmove[n];

      newic.ic_to_xyz();

      icoords[n].reset(natoms,anames,anumbers,newic.coords);
    }//loop n over nodes
    
  } // loop i over reparam steps

#if 1
  printf(" spacings (end ic_reparam, steps: %i): ",ic_reparam_steps);
  for (int n=1;n<nnmax;n++)
    printf(" %1.2f",dqmaga[n]);
  printf("  disprms: %1.3f ",disprms);
  printf("\n");
#endif

  for (int i=0;i<nnmax;i++)
    delete [] ictan0[i];
  delete [] ictan0;
  for (int i=0;i<nnmax;i++)
    delete [] ictan[i];
  delete [] ictan;
  delete [] rpmove;
  delete [] rpart;
  delete [] dE;
  delete [] edist;
 
  return;
}


void GString::ic_reparam_cut(int min, double** dqa, double* dqmaga, int rtype) 
{
  if (isSSM)
    printf(" NOT SET UP FOR SSM \n");
  if (bondfrags<2)
    close_dist_fix(0);

  printf(" found new intermediate \n");
  printf(" reparameterizing string to node: %i \n",min);
  nsplit++;
  nn = min;

  printf(" resetting climb \n");
  for (int n=1;n<nnmax-1;n++)
    icoords[n].isTSnode = 0;
  climb = 0;
  find = 0;

  printf(" V_profile (before):");
  for (int n=0;n<nnmax;n++)
    printf(" %1.1f",V_profile[n]);
  printf("\n");

 // opt intermediate to end node
  int osteps = 50;
  string nstr = StringTools::int2str(runNum,4,"0");
  newic.reset(natoms,anames,anumbers,icoords[min].coords);
  newic.bmatp_create();
  newic.bmatp_to_U();
  newic.bmat_create();
  newic.make_Hint();
  double energyn = newic.opt_b("scratch/cut.xyz"+nstr,osteps);

  icoords[nnmax-1].reset(natoms,anames,anumbers,newic.coords);
  icoords[nnmax-1].update_ic();
  V_profile[nnmax-1] = energyn;
  //icoords[nnmax-1].print_xyz();
  nn++;

  if (bondfrags<2)
    close_dist_fix(0);
  for (int n=min;n<nnmax-1;n++)
    addNode(n-1,n,nnmax-1);

 
  int size_ic = newic.nbonds+newic.nangles+newic.ntor+newic.nxyzic;
  int len_d = newic.nicd0;
  double** ictan0 = new double*[nnmax];
  for (int i=0;i<nnmax;i++)
    ictan0[i] = new double[size_ic];
  double** ictan = new double*[nnmax];
  for (int i=0;i<nnmax;i++)
    ictan[i] = new double[size_ic];

  double* rpmove = new double[nnmax];
  for (int i=0;i<nnmax;i++) rpmove[i] = 0.;
  double* rpart = new double[nnmax];
  for (int i=0;i<nnmax;i++) rpart[i] = 0.;

  double totaldqmag,dqavg,disprms;
  double h1dqmag,h2dqmag;
  double* dE = new double[nnmax];
  double* edist = new double[nnmax];


  for (int i=0;i<3*ic_reparam_steps;i++)
  {
   //tangents pairwise, returns distance magnitudes also
    get_tangents_1(dqa,dqmaga,ictan0); 

    totaldqmag = 0.;
    for (int n=1;n<nnmax;n++)
      totaldqmag += dqmaga[n];
    dqavg = totaldqmag/(nnmax-1);
#if 0
    //printf(" spacing average: %1.2f ",dqavg);
    printf(" printing spacings dqmaga: ");
    for (int n=1;n<nnmax;n++)
      printf(" %1.2f",dqmaga[n]);
    printf("\n");
#endif
 
    //using average
    if (rtype==0 && i==0)
    {
      for (int n=1;n<nnmax;n++)
        rpart[n] = 1./(nnmax-1);
    }

    if (i==0)
    {
      printf(" rpart: ");
      for (int n=1;n<nnmax;n++)
        printf(" %1.2f",rpart[n]);
      printf("\n");
    }


    for (int n=1;n<nnmax-1;n++)
    {
      double deltadq = dqmaga[n] - totaldqmag * rpart[n];
      if (n==nnmax-2) deltadq += totaldqmag * rpart[n] - dqmaga[n+1];
      rpmove[n] = -deltadq;
    } //loop n over active nodes

    double MAXRE=1.1;
#if 0
    for (int n=1;n<nnmax-1;n++)
      printf(" 0disp[%i]: %1.2f",n,rpmove[n]);
    printf("\n");
#endif
    for (int n=1;n<nnmax-1;n++)
      if(abs(rpmove[n])>MAXRE)
        rpmove[n] = sign(rpmove[n])*MAXRE;
    for (int n=1;n<nnmax-2;n++)
      rpmove[n+1] += rpmove[n]; 
    for (int n=1;n<nnmax-1;n++)
      if(abs(rpmove[n])>MAXRE)
        rpmove[n] = sign(rpmove[n])*MAXRE;
#if 1
    for (int n=1;n<nnmax-1;n++)
      printf(" disp[%i]: %1.2f",n,rpmove[n]);
    printf("\n");
#endif
    disprms = 0.;
    for (int n=1;n<nnmax-1;n++)
      disprms += rpmove[n]*rpmove[n];
    disprms = sqrt(disprms);
    //printf(" disprms: %1.3f \n",disprms);
 
    if (disprms<0.02) break;

    for (int n=1;n<nnmax-1;n++)
    {
      newic.reset(natoms,anames,anumbers,icoords[n].coords);
      newic.update_ic();

      newic.bmatp_create();
      newic.bmatp_to_U();

      if (rpmove[n] < 0.)
      {
        for (int j=0;j<size_ic;j++)
          ictan[n][j] = ictan0[n][j];
      }
      else
      {
        for (int j=0;j<size_ic;j++)
          ictan[n][j] = ictan0[n+1][j];
      }

      newic.opt_constraint(ictan[n]);
      newic.bmat_create();
      for (int j=0;j<newic.nicd0;j++) newic.dq0[j]=0.;
      newic.dq0[newic.nicd0-1] = rpmove[n];
      newic.ic_to_xyz();

      icoords[n].reset(natoms,anames,anumbers,newic.coords);
    }//loop n over nodes
    
  } // loop i over reparam steps

#if 1
  printf(" spacings (end ic_reparam, steps: %i): ",ic_reparam_steps);
  for (int n=1;n<nnmax;n++)
    printf(" %1.2f",dqmaga[n]);
  printf("  disprms: %1.3f ",disprms);
  printf("\n");
#endif

  for (int i=0;i<nnmax;i++)
    delete [] ictan0[i];
  delete [] ictan0;
  for (int i=0;i<nnmax;i++)
    delete [] ictan[i];
  delete [] ictan;
  delete [] rpmove;
  delete [] rpart;
  delete [] dE;
  delete [] edist;
 
  return;
}




void GString::ic_reparam_dm(double** dqa, double* dqmaga, int rtype) 
{
  if (isSSM)
    printf(" NOT SET UP FOR SSM \n");
 // printf(" WARNING: disabled reparam_dm \n");
 // return;
  int size_ic = newic_dm.nbonds+newic_dm.nangles+newic_dm.ntor+newic.nxyzic;
  int len_d = newic_dm.nicd0;
  double** ictan = new double*[nnmax];
  for (int i=0;i<nnmax;i++)
    ictan[i] = new double[size_ic];

  double* rpmove = new double[nnmax];
  for (int i=0;i<nnmax;i++) rpmove[i] = 0.;
  
  double totaldqmag,dqavg;
  double* dE = new double[nnmax];

  for (int i=0;i<5;i++)
  {
    get_tangents_dm(dqa,dqmaga,ictan);
    get_distances_dm(dqmaga,ictan);

#if 0
    printf(" printing spacings dqmaga: \n");
    for (int n=1;n<nnmax-1;n++)
      printf(" %1.2f",dqmaga[n]);
    printf("\n");
#endif

    totaldqmag = 0.;
    for (int n=1;n<nnmax;n++)
      totaldqmag += dqmaga[n];
    dqavg = totaldqmag/(nnmax-1);
    //printf(" spacing average: %1.2f \n",dqavg);
 
    //using average
    if (rtype==0)
    for (int n=1;n<nnmax;n++)
      rpmove[n] = dqavg-dqmaga[n];
    if (rtype==1)
    {
      double dEmax = 0.;
      for (int n=1;n<nnmax-1;n++)
        dE[n] = (abs(V_profile[n-1]-V_profile[n]) + abs(V_profile[n+1]-V_profile[n]))/2;
      for (int n=1;n<nnmax-1;n++)
      if (dE[n]>dEmax)
        dEmax = dE[n];
      printf(" computing dE's, dEmax: %1.1f \n",dEmax);
      for (int n=1;n<nnmax-1;n++)
        printf(" Vn[%i]-Vn[%i] %1.1f \n",n-1,n,dE[n]);

      for (int n=1;n<nnmax-1;n++)
      {
        rpmove[n] = dqavg*(0.7*(dEmax - dE[n])/dEmax + 0.5) - dqmaga[n];
      }
    }

    double MAXRE=0.5;
#if 0
    for (int n=1;n<nnmax-1;n++)
      printf(" 0disp[%i]: %1.2f \n",n,rpmove[n]);
#endif
    for (int n=1;n<nnmax-1;n++)
      if(abs(rpmove[n])>MAXRE)
        rpmove[n] = sign(rpmove[n])*MAXRE;
    for (int n=1;n<nnmax-2;n++)
      rpmove[n+1] += rpmove[n];
    for (int n=1;n<nnmax-1;n++)
      if(abs(rpmove[n])>MAXRE)
        rpmove[n] = sign(rpmove[n])*MAXRE;
#if 0
    for (int n=1;n<nnmax-1;n++)
      printf(" disp[%i]: %1.2f \n",n,rpmove[n]);
#endif

    for (int n=1;n<nnmax-1;n++)
    {
      newic_dm.reset(natoms,anames,anumbers,icoords[n].coords);
      newic_dm.update_ic();

      newic_dm.bmatp_create();
      newic_dm.bmatp_to_U();

#if 0
      for (int i=0;i<size_ic;i++) ictan[i] = 0.;
      for (int i=0;i<size_ic;i++)
      for (int j=0;j<len_d;j++)
        ictan[n][i] += newic_dm.Ut[j*size_ic+i] * dqa[n][j];
#endif
      
      newic_dm.opt_constraint(ictan[n]);
      newic_dm.bmat_create();
      for (int j=0;j<newic_dm.nicd0;j++) newic_dm.dq0[j]=0.;
      newic_dm.dq0[newic_dm.nicd0-1] = rpmove[n];

      newic_dm.ic_to_xyz();
      icoords[n].reset(natoms,anames,anumbers,newic_dm.coords);
    }//loop n over nodes
  }

#if 1
  printf(" printing spacings dqmaga (end ic_reparam_dm): \n");
  for (int n=1;n<nnmax-1;n++)
    printf(" %1.2f",dqmaga[n]);
  printf("\n");
#endif

  delete [] ictan;
  delete [] rpmove;
  delete [] dE;
 
  return;
}


int GString::check_for_reaction_g(int type)
{
  if (nadd+nbrk<1) return 0;

  ICoord ic2;
  ic2.alloc(natoms);
  ic2.isOpt = 0;

  int isrxn = 0;

  ic2.reset(natoms,anames,anumbers,icoords[nnR-1].coords);
  ic2.ic_create();

  int nadded = 0;
  int nbroken = 0;

  for (int i=0;i<nadd;i++)
  {
    int a1 = add[2*i+0];
    int a2 = add[2*i+1];
    if (ic2.bond_exists(a1,a2))
      nadded++;
  }

  for (int i=0;i<nbrk;i++)
  {
    int b1 = brk[2*i+0];
    int b2 = brk[2*i+1];
    if (!ic2.bond_exists(b1,b2))
      nbroken++;
  }

  if (type==1)
  {
    if (nadded+nbroken>=nadd+nbrk)
      isrxn = nadded+nbroken;
  }
  else
    isrxn = nadded+nbroken;

  ic2.freemem();

  printf(" check_for_reaction_g isrxn: %i nadd+nbrk: %i \n",isrxn,nadd+nbrk);

  return isrxn;
}

int GString::check_for_reaction(int& wts, int& wint)
{
  int nmin = 1;
  int nmax = 0;
  int* min = new int[nnmax];
  int* max = new int[nnmax];
  for (int n=0;n<nnmax;n++) min[n] = 0;
  for (int n=0;n<nnmax;n++) max[n] = 0;
  for (int n=1;n<nnmax-1;n++)
  {
    if (V_profile[n+1] > V_profile[n])
    {
      if (V_profile[n] < V_profile[n-1])
        min[nmin++] = n;
    }
    if (V_profile[n+1] < V_profile[n])
    {
      if (V_profile[n] > V_profile[n-1])
        max[nmax++] = n;
    }
  }

  ICoord ic1,ic2;
  ic1.alloc(natoms);
  ic2.alloc(natoms);
  ic1.isOpt = 0;
  ic2.isOpt = 0;
  ic1.reset(natoms,anames,anumbers,coords[0]);
  ic1.ic_create();

  int isrxn = 0;
  wts = 0;
  wint = nnmax-1;
  if (nmax==0)
  {
    ic2.reset(natoms,anames,anumbers,icoords[nnmax-1].coords);
    ic2.ic_create();

    int nnew = 0;
    int nmissing = 0;

    int nbonds = ic2.nbonds;
    for (int i=0;i<nbonds;i++)
    if (!ic1.bond_exists(ic2.bonds[i][0],ic2.bonds[i][1]))
      nnew++;

    nbonds = ic1.nbonds;
    for (int i=0;i<nbonds;i++)
    if (!ic2.bond_exists(ic1.bonds[i][0],ic1.bonds[i][1]))
      nmissing++;

    isrxn = nnew+nmissing;
    if (isrxn)
    {
      wts = 0;
      wint = nnmax-1;
    }
  }
  else
  for (int n=0;n<nmax;n++)
  {
   //n1,n2 are minimum pair
    int n1 = 0;
    int n2 = nnmax-1;
    for (int m=0;m<nmin;m++)
    if (min[m]<max[n])
      n1 = min[m];
    for (int m=0;m<nmin;m++)
    if (min[m]>max[n])
    {
      n2 = min[m];
      break;
    }

    //printf(" comparing nodes min1: %i min2: %i \n",n1,n2);

    ic2.reset(natoms,anames,anumbers,icoords[n2].coords);
    ic2.ic_create();

    int nnew = 0;
    int nmissing = 0;

    int nbonds = ic2.nbonds;
    for (int i=0;i<nbonds;i++)
    if (!ic1.bond_exists(ic2.bonds[i][0],ic2.bonds[i][1]))
      nnew++;

    nbonds = ic1.nbonds;
    for (int i=0;i<nbonds;i++)
    if (!ic2.bond_exists(ic1.bonds[i][0],ic1.bonds[i][1]))
      nmissing++;

    isrxn = nnew+nmissing;
    if (isrxn>0) //real reaction, ends at intermediate n2
    {
      wts = max[n];
      wint = nnmax-1;
     //only report wint if TS past n2
      for (int m=0;m<nmax;m++) 
      if (max[m]>n2)
        wint = n2;
      break;
    } //if isrxn
  } //loop n over nmax

  ic1.freemem();
  ic2.freemem();

  delete [] min;
  delete [] max;

  printf(" check_for_reaction wts: %i wint: %i isrxn: %i \n",wts,wint,isrxn);

  return isrxn;
}

int GString::find_ints()
{
  if (isSSM)
    printf(" NOT SET UP FOR SSM \n");

  int node = 0;
  int natoms = bondsic.natoms;
  int nbonds = bondsic.nbonds;

  int* min = new int[nnmax];
  for (int n=1;n<nnmax-1;n++) min[n] = 0;

  for (int n=1;n<nnmax-1;n++)  
  if (V_profile[n+1] > V_profile[n])
  if (V_profile[n] < V_profile[n-1])
  {
    //printf(" node is min: %i \n",n);
    min[n] = 1;
  }

  int found = 0;
  for (int n=1;n<nnmax;n++)
  if (min[n] && !found)
  {
    for (int i=0;i<natoms;i++)
    if (!found)
    for (int j=0;j<i;j++)
    if (!bondsic.bond_exists(i,j))
    {
      double d = icoords[n].distance(i,j);
      if ( d < (bondsic.getR(i)+bondsic.getR(j))/2. )
      {
        printf(" new bond formed at node %i: %i %i (%3.2f) \n",n,i,j,d);
        node = n;
        found = 1;
        break;
      }
    } //loop i,j over atom pairs
  } //loop n over nnmax

  return node;
}


int GString::twin_peaks()
{
  if (isSSM)
    printf(" NOT SET UP FOR SSM \n");

 // printf(" in twin_peaks \n");

  int npeaks1 = 0; 
  int npeaks2 = 0; 

#if 0
  printf(" V_profile: ");
  for (int n=0;n<nnmax;n++)
    printf(" %1.1f",V_profile[n]);
  printf("\n");
#endif
//  V_profile[nnmax-3] -= 6.;
//  V_profile[nnmax-2] += 2.5;

  int* min = new int[nnmax];
  int* max = new int[nnmax];
  for (int n=1;n<nnmax-1;n++) min[n] = 0;
  for (int n=1;n<nnmax-1;n++) max[n] = 0;
  for (int n=1;n<nnmax-1;n++)
  {
    if (V_profile[n+1] > V_profile[n])
    {
      if (V_profile[n] < V_profile[n-1])
        min[n] = 1;
    }
    if (V_profile[n+1] < V_profile[n])
    {
      if (V_profile[n] > V_profile[n-1])
        max[n] = 1;
    }
  }

#if 0
  printf(" min nodes: ");
  for (int n=1;n<nnmax-1;n++)
  if (min[n])
    printf(" %i",n);
  printf(" max nodes: ");
#endif
  for (int n=1;n<nnmax-1;n++)
  if (max[n])
  {
    npeaks1++;
    //printf(" %i",n);
  }
  //printf("\n");

  double ediff = 15.; //adjustable parameter
  if (npeaks1>1)
  for (int n=1;n<nnmax-1;n++)
  if (max[n])
  {
    for (int m=n+1;m<nnmax-1;m++)
    if (min[m])
    {
      if (V_profile[n] - V_profile[m] > ediff)
      {
        int found = 0;
        for (int l=m+1;l<nnmax-1;l++)
        if (max[l])
        if (V_profile[l] - V_profile[m] > ediff)
        {
          npeaks2++;
          found = 1;
          break;
        } //loop over l, if max[l]
        if (found) break;
      }
    } //loop over m, if min[m]
  } //loop over n, if max[n]

  int npeaks = npeaks2 + 1;
  if (npeaks1>0 && npeaks2==0) npeaks = 1;
  printf(" found %i significant peak(s) \n",npeaks);


  delete [] min;
  delete [] max;

  return npeaks;
}


int GString::find_uphill(double cutoff)
{
  printf(" in find_uphill \n");

  int maxn = nnR;
  int nup = 0;

  int allup = 1;
  for (int n=1;n<maxn;n++)
  if (V_profile[n]>cutoff)
  {
    nup = n;
    break;
  }
  printf(" first node above %2.1f is %i \n",cutoff,nup);

  return nup;
}
///Finds peaks, used by opt_iters to know which node to climb.
int GString::find_peaks(int type)
{
  printf(" in find_peaks (%i) \n",type);

 //type 1 --> growing
 //type 2 --> opting
 //type 3 --> not using
 //type 4 --> intermediate check

  int maxn = 0;
  if (type==1 || type==3)
    maxn = nnR;
  else if (type==2 || type==4)
    maxn = nnmax;
  else 
  {
    printf(" find_peaks, bad input type \n");
    exit(1);
  }

  //printf(" maxn: %i nnR: %i nnmax: %i \n",maxn,nnR,nnmax);
  if (type<3)
  {
    printf(" V_profile: ");
    for (int n=0;n<maxn;n++)
    if (n==n0)
      printf("  %1.1f",V_profile[n]);
    else
      printf(" %1.1f",V_profile[n]);
    printf("\n");
  }

  double alluptol = 0.1; //was 0.5
  double alluptol2 = 0.5;
  int allup = 1;
  for (int n=1;n<maxn;n++)
  if (V_profile[n]+alluptol<V_profile[n-1])
  {
    allup = 0;
    break;
  }
  //printf(" allup: %i V_profile[maxn-1]: %4.3f \n",allup,V_profile[maxn-1]);
  //printf(" allup: %i \n",allup);
  if (V_profile[maxn-1] > 15.0)
  {
    if (maxn-3>0)
    if (close_val(V_profile[maxn-1],V_profile[maxn-2],alluptol2)
     && close_val(V_profile[maxn-2],V_profile[maxn-3],alluptol2)
     && close_val(V_profile[maxn-3],V_profile[maxn-4],alluptol2))
    {
      printf(" possible dissociative profile \n");
      allup = -2;
    }
  }
  if (allup==1) //was on
  {
    //printf(" all string is uphill \n");
    //return -1;
  }


  int sn = n0; //was n0
  if (type==1 || type==3 || type==4)
    sn = 0;

  int npeaks1 = 0; 
  int npeaks2 = 0; 
  int* min = new int[maxn];
  int* max = new int[maxn];
  for (int n=0;n<maxn;n++) min[n] = 0;
  for (int n=0;n<maxn;n++) max[n] = 0;
  for (int n=sn+1;n<maxn-1;n++)
  {
    if (V_profile[n+1] > V_profile[n])
    {
      if (V_profile[n] < V_profile[n-1])
        min[n] = 1;
    }
    if (V_profile[n+1] < V_profile[n])
    {
      if (V_profile[n] > V_profile[n-1])
        max[n] = 1;
    }
  }

#if 1
  printf(" min nodes: ");
  for (int n=sn+1;n<maxn-1;n++)
  if (min[n])
    printf(" %i",n);
  printf(" max nodes: ");
  for (int n=sn+1;n<maxn-1;n++)
  if (max[n])
    printf(" %i",n);
  printf("\n");
#endif

  for (int n=sn+1;n<maxn-1;n++)
  if (max[n])
    npeaks1++;

  //printf(" npeaks1: %i npeaks2: %i \n",npeaks1,npeaks2);
  //printf(" sn: %i n0: %i maxn: %i \n",sn,n0,maxn);

  double ediff = 0.5; //adjustable parameter
  if (type==1 || type==3)
    ediff = 1.; //was 5.0
  if (type==4)
    ediff = PEAK4_EDIFF; //was 4.0

  double emax = -1000.;
  int nmax = 0;
  int found = 0;
  if (npeaks1)
  for (int n=sn+1;n<maxn-1;n++)
  if (max[n])
  {
    emax = V_profile[n];
    nmax = n;
    //printf(" at max node: %i \n",n);
    for (int m=n+1;m<maxn;m++)
    {
//      if (max[m] && V_profile[m] > emax) break;
      //printf(" V[n]: %4.3f V[m]: %4.3f \n",V_profile[n],V_profile[m]);
      if (V_profile[n] - V_profile[m] > ediff)
      {
        found = nmax;
        npeaks2++;
        break;
      }
      if (max[m]) break;
    } //loop over m
  } //loop over n, if max[n]

  //printf(" npeaks1: %i npeaks2: %i \n",npeaks1,npeaks2);
  int npeaks = npeaks2;
  printf(" found %i significant peak(s). TOL: %3.2f \n",npeaks,ediff);

 //handle dissociative case
  if (type==4 && npeaks==1)
  {
    int nextmin = 0;
    for (int n=found;n<maxn-1;n++)
    if (min[n])
    {
      nextmin = n;
      break;
    }
    if (nextmin) npeaks = 2;
  }

  if (type==3)
    return nmax;
  if (allup==-2 && npeaks==0)
    return -2;

  delete [] min;
  delete [] max;

  return npeaks;
}

///tangents pairwise, returns distance magnitudes also
void GString::get_tangents_1(double** dqa, double* dqmaga, double** ictan)
{
  int nbonds = newic.nbonds;
  int nangles = newic.nangles;
  int ntor = newic.ntor;
  int nxyzic = newic.nxyzic;
  int size_icp = nbonds+nangles+ntor;
  int size_ic = size_icp+nxyzic;
  int len_d = newic.nicd0;
  //printf(" get_tangents_1, nicd0: %i 3N-6: %i \n",len_d,3*natoms-6);
  double* ictan0 = new double[size_ic];

  for (int n=n0+1;n<nnmax;n++)
  {
    newic.reset(natoms,anames,anumbers,icoords[n].coords);
    intic.reset(natoms,anames,anumbers,icoords[n-1].coords);

    newic.update_ic();
    intic.update_ic();

// tangent in redundant coordinates
    for (int i=0;i<size_ic;i++) ictan[n][i] = 0.;
    tangent_1(ictan[n]);

    dqmaga[n] = 0.;
#if 1
    for (int i=0;i<size_ic;i++) ictan0[i] = ictan[n][i];

    newic.bmatp_create();
    newic.bmatp_to_U();
    newic.opt_constraint(ictan[n]);
    //newic.bmat_create();

#if 1
    for (int j=0;j<nbonds;j++) 
      dqmaga[n] += ictan0[j]*newic.Ut[newic.nicd*size_ic+j] *2.5; //CPMZ removed *2.5 weight
    for (int j=nbonds;j<size_icp-ntor;j++) 
      dqmaga[n] += ictan0[j]*newic.Ut[newic.nicd*size_ic+j];
    for (int j=nbonds+nangles;j<size_icp;j++)
      dqmaga[n] += ictan0[j]*newic.Ut[newic.nicd*size_ic+j]; //CPMZ check
    for (int j=size_icp;j<size_ic;j++)
      dqmaga[n] += ictan0[j]*newic.Ut[newic.nicd*size_ic+j];
#else
    for (int j=0;j<nbonds;j++) 
      dqmaga[n] += ictan0[j]*newic.Ut[newic.nicd*size_ic+j];
    for (int j=nbonds;j<size_ic-ntor;j++) 
      dqmaga[n] += ictan0[j]*newic.Ut[newic.nicd*size_ic+j]/1.5;
    for (int j=nbonds+nangles;j<size_ic;j++)
      dqmaga[n] += ictan0[j]*newic.Ut[newic.nicd*size_ic+j]/1.5; //CPMZ check
    for (int j=size_icp;j<size_ic;j++)
      dqmaga[n] += ictan0[j]*newic.Ut[newic.nicd*size_ic+j];
#endif
#else
    for (int j=0;j<size_ic-ntor;j++) 
      dqmaga[n] += ictan[n][j]*ictan[n][j];
    for (int j=nbonds+nangles;j<size_ic;j++)
      dqmaga[n] += ictan[n][j]*ictan[n][j]/50;
#endif
    dqmaga[n] = sqrt(dqmaga[n]);
    //printf(" dqmaga: %1.2f",dqmaga[n]);

  }

#if 0
  for (int n=1;n<nnmax-1;n++)
  {
    printf(" printing ictan[%i] \n",n);
    for (int i=0;i<nbonds;i++)
      printf(" %1.2f",ictan[n][i]);
    printf("\n");
    for (int i=0;i<nangles;i++)
      printf(" %1.2f",ictan[n][nbonds+i]);
    printf("\n");
    for (int i=0;i<ntor;i++)
      printf(" %1.2f",ictan[n][nbonds+nangles+i]);
    printf("\n");
  }
#endif
 
  delete [] ictan0;

  return;
}
///Finds the tangents during the growth phase. Tangents referenced to left or right during growing phase
void GString::get_tangents_1g(double** dqa, double* dqmaga, double** ictan)
{
  int nbonds = newic.nbonds;
  int nangles = newic.nangles;
  int ntor = newic.ntor;
  int size_icp = nbonds+nangles+ntor;
  int size_ic = size_icp+newic.nxyzic;
  int len_d = newic.nicd0;
  double* ictan0 = new double[size_ic];
  int* nlist = new int[2*nnmax];
  int ncurrent = 0;

  for (int n=n0+0;n<nnR-1;n++)
  {
   // printf(" pair: %i %i \n",n,n+1);
    nlist[2*ncurrent] = n;
    nlist[2*ncurrent+1] = n+1;
    ncurrent++;
  }
  for (int n=nnmax-nnP+1;n<nnmax;n++)
  {
   // printf(" pair: %i %i \n",n,n-1);
    nlist[2*ncurrent] = n;
    nlist[2*ncurrent+1] = n-1;
    ncurrent++;
  }
  //printf(" pair: %i %i \n",nnR-1,nnmax-nnP);
  nlist[2*ncurrent] = nnR-1;
  nlist[2*ncurrent+1] = nnmax-nnP;
  if (nnR==0) nlist[2*ncurrent]++;
  if (nnP==0) nlist[2*ncurrent+1]--;
  ncurrent++;
  //printf(" pair: %i %i \n",nnR-1,nnmax-nnP);
  nlist[2*ncurrent] = nnmax-nnP;
  nlist[2*ncurrent+1] = nnR-1;
  if (nnP==0) nlist[2*ncurrent]--;
  if (nnR==0) nlist[2*ncurrent+1]++;
  ncurrent++;

  for (int n=0;n<ncurrent;n++)
  {
    newic.reset(natoms,anames,anumbers,icoords[nlist[2*n+1]].coords);
    intic.reset(natoms,anames,anumbers,icoords[nlist[2*n+0]].coords);

    newic.update_ic();
    intic.update_ic();


    if (isSSM && nlist[2*n]==nnR-1)
      tangent_1b(ictan[nlist[2*n]]);
    else
      tangent_1(ictan[nlist[2*n]]);

    dqmaga[nlist[2*n]] = 0.;
#if 1
    for (int i=0;i<size_ic;i++) ictan0[i] = ictan[nlist[2*n]][i];

    newic.bmatp_create();
    newic.bmatp_to_U();
    newic.opt_constraint(ictan[nlist[2*n]]);
    //newic.bmat_create();

    for (int j=0;j<size_icp-ntor;j++) 
      dqmaga[nlist[2*n]] += ictan0[j]*newic.Ut[newic.nicd*size_ic+j];
    for (int j=nbonds+nangles;j<size_icp;j++)
      dqmaga[nlist[2*n]] += ictan0[j]*newic.Ut[newic.nicd*size_ic+j]; //CPMZ check
    for (int j=size_icp;j<size_ic;j++)
      dqmaga[nlist[2*n]] += ictan0[j]*newic.Ut[newic.nicd*size_ic+j];
#else
    for (int j=0;j<size_ic-ntor;j++) 
      dqmaga[nlist[2*n]] += ictan[nlist[2*n]][j]*ictan[nlist[2*n]][j];
    for (int j=nbonds+nangles;j<size_ic;j++)
      dqmaga[nlist[2*n]] += ictan[nlist[2*n]][j]*ictan[nlist[2*n]][j]/50; //CPMZ check
#endif
    dqmaga[nlist[2*n]] = sqrt(dqmaga[nlist[2*n]]);
    //printf(" dqmaga: %1.2f",dqmaga[nlist[2*n]]);
  }

#if 0
  for (int n=0;n<ncurrent;n++)
  {
    printf(" printing ictan[%i] \n",nlist[2*n]);
    for (int i=0;i<nbonds;i++)
      printf(" %1.2f",ictan[nlist[2*n]][i]);
    printf("\n");
    for (int i=0;i<nangles;i++)
      printf(" %1.2f",ictan[nlist[2*n]][nbonds+i]);
    printf("\n");
    for (int i=0;i<ntor;i++)
      printf(" %1.2f",ictan[nlist[2*n]][nbonds+nangles+i]);
    printf("\n");
  }
#endif
 
  delete [] ictan0;
  delete [] nlist;

  return;
}

void GString::get_tangents_1e(double** dqa, double* dqmaga, double** ictan)
{
  //printf(" gt1e"); fflush(stdout);
  int nbonds = newic.nbonds;
  int nangles = newic.nangles;
  int ntor = newic.ntor;
  int size_icp = nbonds+nangles+ntor;
  int size_ic = size_icp+newic.nxyzic;
  int len_d = newic.nicd0;
//  printf(" get_tangents_1e, nicd0: %i 3N-6: %i \n",len_d,3*natoms-6);
  double* t1 = new double[size_ic];
  double* t2 = new double[size_ic];
  double* ictan0 = new double[size_ic];

  printf(" V_profile: ");
  for (int n=0;n<nnmax;n++)
    printf(" %1.1f",V_profile[n]);
  printf("\n");

  int TSnode = TSnode0;

  //printf(" gt1el"); fflush(stdout);
  for (int n=n0+1;n<nnmax-1;n++)
  {
    int do3 = 0;
    //printf(" n: %i V[n+1],V[n],V[n-1]: %1.1f %1.1f %1.1f \n",n,V_profile[n+1],V_profile[n],V_profile[n-1]);
    if (!find)
    {
      if (V_profile[n+1] > V_profile[n] && V_profile[n] > V_profile[n-1])
      {
        newic.reset(natoms,anames,anumbers,icoords[n+1].coords);
        intic.reset(natoms,anames,anumbers,icoords[n].coords);
      }
      else if ((V_profile[n-1] > V_profile[n] && V_profile[n] > V_profile[n+1]))
      {
        newic.reset(natoms,anames,anumbers,icoords[n].coords);
        intic.reset(natoms,anames,anumbers,icoords[n-1].coords);
      }
      else
      {
        newic.reset(natoms,anames,anumbers,icoords[n].coords);
        intic.reset(natoms,anames,anumbers,icoords[n+1].coords);
        int2ic.reset(natoms,anames,anumbers,icoords[n-1].coords);
        do3 = 1;
      }
#if 0
     //final node point back tangent
      if (isSSM && n==nnmax-2)
      {
        newic.reset(natoms,anames,anumbers,icoords[n+1].coords);
        intic.reset(natoms,anames,anumbers,icoords[n].coords);
      }
#endif
    }
    else
    {
      if (n<TSnode)
      {
        newic.reset(natoms,anames,anumbers,icoords[n+1].coords);
        intic.reset(natoms,anames,anumbers,icoords[n].coords);
      }
      else if (n>TSnode)
      {
        newic.reset(natoms,anames,anumbers,icoords[n].coords);
        intic.reset(natoms,anames,anumbers,icoords[n-1].coords);
      }
      else
      {
        newic.reset(natoms,anames,anumbers,icoords[n].coords);
        intic.reset(natoms,anames,anumbers,icoords[n+1].coords);
        int2ic.reset(natoms,anames,anumbers,icoords[n-1].coords);
        do3 = 1;
      }
    }

    newic.update_ic();
    intic.update_ic();
    int2ic.update_ic();

    if (!do3)
    {
      for (int i=0;i<size_ic;i++) ictan[n][i] = 0.;

      tangent_1(ictan[n]);
    }
    else
    {
      double f1 = 0.;
      double dE1 = abs(V_profile[n+1] - V_profile[n]);
      double dE2 = abs(V_profile[n] - V_profile[n-1]);
      double dEmax = max(dE1,dE2);
      double dEmin = min(dE1,dE2);
      if (V_profile[n+1]>V_profile[n-1])
        f1 = dEmax/(dEmax+dEmin+0.00000001);
      else
        f1 = 1 - dEmax/(dEmax+dEmin+0.00000001);

      printf(" 3 way tangent (%i): f1: %1.2f \n",n,f1);

      for (int i=0;i<size_ic;i++) t1[i] = 0.;
      for (int i=0;i<size_ic;i++) t2[i] = 0.;
      for (int i=0;i<nbonds;i++)
      {
        t1[i] = intic.bondd[i] - newic.bondd[i];
        t2[i] = newic.bondd[i] - int2ic.bondd[i];
      }
      for (int i=0;i<nangles;i++)
      {
        t1[nbonds+i] = (intic.anglev[i] - newic.anglev[i])*3.14159/180;
        t2[nbonds+i] = (newic.anglev[i] - int2ic.anglev[i])*3.14159/180;
      }
      for (int i=0;i<ntor;i++)
      {
        t1[nbonds+nangles+i] = (intic.torv[i] - newic.torv[i])*3.14159/180;
        t2[nbonds+nangles+i] = (newic.torv[i] - int2ic.torv[i])*3.14159/180;
        if (t1[nbonds+nangles+i]>3.14159)
          t1[nbonds+nangles+i] = -1*(2*3.14159 - t1[nbonds+nangles+i]);
        if (t1[nbonds+nangles+i]<-3.14159)
          t1[nbonds+nangles+i] = 2*3.14159 + t1[nbonds+nangles+i];
        if (t2[nbonds+nangles+i]>3.14159)
          t2[nbonds+nangles+i] = -1*(2*3.14159 - t2[nbonds+nangles+i]);
        if (t2[nbonds+nangles+i]<-3.14159)
          t2[nbonds+nangles+i] = 2*3.14159 + t2[nbonds+nangles+i];
      }

      int cxyzic = 0;
      if (newic.nxyzic>0)
      for (int i=0;i<natoms;i++)
      if (newic.xyzic[i])
      {
        t1[size_icp+cxyzic]   = intic.coords[3*i+0] - newic.coords[3*i+0];
        t2[size_icp+cxyzic++] = newic.coords[3*i+0] - int2ic.coords[3*i+0];
        t1[size_icp+cxyzic]   = intic.coords[3*i+1] - newic.coords[3*i+1];
        t2[size_icp+cxyzic++] = newic.coords[3*i+1] - int2ic.coords[3*i+1];
        t1[size_icp+cxyzic]   = intic.coords[3*i+2] - newic.coords[3*i+2];
        t2[size_icp+cxyzic++] = newic.coords[3*i+2] - int2ic.coords[3*i+2];
      }

      for (int i=0;i<size_ic;i++)
        ictan[n][i] = f1 * t1[i] + (1-f1) * t2[i];
      for (int i=0;i<ntor;i++)
      {
        if (ictan[n][nbonds+nangles+i]>3.14159)
          ictan[n][nbonds+nangles+i] = -1*(2*3.14159 - ictan[n][nbonds+nangles+i]);
        if (ictan[n][nbonds+nangles+i]<-3.14159)
          ictan[n][nbonds+nangles+i] = 2*3.14159 + ictan[n][nbonds+nangles+i];
      }

    }

    dqmaga[n] = 0.;
#if 1
    for (int i=0;i<size_ic;i++) ictan0[i] = ictan[n][i];

    newic.bmatp_create();
    newic.bmatp_to_U();
    newic.opt_constraint(ictan[n]);

#if 1
    for (int j=0;j<nbonds;j++) 
      dqmaga[n] += ictan0[j]*newic.Ut[newic.nicd*size_ic+j] *2.5;
    for (int j=nbonds;j<size_icp-ntor;j++) 
      dqmaga[n] += ictan0[j]*newic.Ut[newic.nicd*size_ic+j];
    for (int j=nbonds+nangles;j<size_icp;j++)
      dqmaga[n] += ictan0[j]*newic.Ut[newic.nicd*size_ic+j]; //CPMZ check
    for (int j=size_icp;j<size_ic;j++)
      dqmaga[n] += ictan0[j]*newic.Ut[newic.nicd*size_ic+j];
#else
    for (int j=0;j<nbonds;j++) 
      dqmaga[n] += ictan0[j]*newic.Ut[newic.nicd*size_ic+j];
    for (int j=nbonds;j<size_ic-ntor;j++) 
      dqmaga[n] += ictan0[j]*newic.Ut[newic.nicd*size_ic+j]/1.5;
    for (int j=nbonds+nangles;j<size_ic;j++)
      dqmaga[n] += ictan0[j]*newic.Ut[newic.nicd*size_ic+j]/1.5; //CPMZ check
#endif
#else
    dqmaga[n] = 0.;
    for (int j=0;j<size_ic-ntor;j++)
      dqmaga[n] += ictan[n][j]*ictan[n][j];
    for (int j=nbonds+nangles;j<size_ic;j++) //CPMZ check
      dqmaga[n] += ictan[n][j]*ictan[n][j]/50;
    dqmaga[n] = sqrt(dqmaga[n]);
#endif
    //printf(" dqmaga: %1.2f",dqmaga[n]);
  }

#if 0
  for (int n=1;n<nnmax-1;n++)
  {
    printf(" printing ictan[%i] \n",n);
    for (int i=0;i<nbonds;i++)
      printf(" %1.2f",ictan[n][i]);
    printf("\n");
    for (int i=0;i<nangles;i++)
      printf(" %1.2f",ictan[n][nbonds+i]);
    printf("\n");
    for (int i=0;i<ntor;i++)
      printf(" %1.2f",ictan[n][nbonds+nangles+i]);
    printf("\n");
  }
#endif

  delete [] t1;
  delete [] t2;
  delete [] ictan0;

  return;
}

void GString::get_tangents(double** dqa, double* dqmaga, double** ictan)
{
  printf(" IS THIS USED? \n");

  int nbonds = newic.nbonds;
  int nangles = newic.nangles;
  int ntor = newic.ntor;
  int size_icp = newic.nbonds+newic.nangles+newic.ntor;
  int size_ic = size_icp+newic.nxyzic;
  int len_d = newic.nicd0;

  for (int n=1;n<nnmax-1;n++)
  {
    newic.reset(natoms,anames,anumbers,icoords[n].coords);
    intic.reset(natoms,anames,anumbers,icoords[n-1].coords);
    int2ic.reset(natoms,anames,anumbers,icoords[n+1].coords);

    newic.update_ic();
    intic.update_ic();
    int2ic.update_ic();

// tangent in redundant coordinates
    for (int i=0;i<size_ic;i++) ictan[n][i] = 0.;
    for (int i=0;i<nbonds;i++)
//      ictan[n][i] = newic.bondd[i] - intic.bondd[i];
      ictan[n][i] = (int2ic.bondd[i] - intic.bondd[i]);
#if 1
    for (int i=0;i<nangles;i++)
//      ictan[n][nbonds+i] = (newic.anglev[i] - intic.anglev[i])*3.14159/180;
      ictan[n][nbonds+i] = (int2ic.anglev[i] - intic.anglev[i])*3.14159/180;
    for (int i=0;i<ntor;i++)
    {
//      ictan[n][nbonds+nangles+i] = (newic.torv[i] - intic.torv[i])*3.14159/180;
      ictan[n][nbonds+nangles+i] = (int2ic.torv[i] - intic.torv[i])*3.14159/180;
      if (ictan[n][nbonds+nangles+i]>3.14159)
      {
        //printf(" WARNING: (a) %1.4f tor tangent %i %i %i %i \n",ictan[n][nbonds+nangles+i],newic.torsions[i][0],newic.torsions[i][1],newic.torsions[i][2],newic.torsions[i][3]);
        ictan[n][nbonds+nangles+i] = -1*(2*3.14159 - ictan[n][nbonds+nangles+i]);
        //printf(" new %1.4f tor tangent %i %i %i %i \n",ictan[n][nbonds+nangles+i],newic.torsions[i][0],newic.torsions[i][1],newic.torsions[i][2],newic.torsions[i][3]);
      }
      if (ictan[n][nbonds+nangles+i]<-3.14159)
      {
        //printf(" WARNING: (b) %1.4f tor tangent %i %i %i %i \n",ictan[n][nbonds+nangles+i],newic.torsions[i][0],newic.torsions[i][1],newic.torsions[i][2],newic.torsions[i][3]);
        ictan[n][nbonds+nangles+i] = 2*3.14159 + ictan[n][nbonds+nangles+i];
        //printf(" new %1.4f tor tangent %i %i %i %i \n",ictan[n][nbonds+nangles+i],newic.torsions[i][0],newic.torsions[i][1],newic.torsions[i][2],newic.torsions[i][3]);
      }
    }
    //for (int i=0;i<nxyzic;i++)
    //  ictan[n][size_icp+i] = 0.; 
#endif
    dqmaga[n] = 0.;
    for (int j=0;j<size_ic;j++)
      dqmaga[n] += ictan[n][j]*ictan[n][j];
    dqmaga[n] = sqrt(dqmaga[n]);
    //printf(" dqmaga: %1.2f",dqmaga[n]);
  }
  //printf("\n");

#if 0
  for (int n=1;n<nnmax-1;n++)
  {
    printf(" printing ictan[%i] \n",n);
    for (int i=0;i<nbonds;i++)
      printf(" %1.2f",ictan[n][i]);
    printf("\n");
    for (int i=0;i<nangles;i++)
      printf(" %1.2f",ictan[n][nbonds+i]);
    printf("\n");
    for (int i=0;i<ntor;i++)
      printf(" %1.2f",ictan[n][nbonds+nangles+i]);
    printf("\n");
  }
#endif


  return;
}

void GString::get_tangents_dm(double** dqa, double* dqmaga, double** ictan)
{
  if (isSSM)
    printf(" NOT SET UP FOR SSM \n");

  int nbonds = newic_dm.nbonds;
  int nangles = newic_dm.nangles;
  int ntor = newic_dm.ntor;
  int nxyzic = newic_dm.nxyzic;
  int size_ic_dm = nbonds+nangles+ntor+nxyzic;
  int len_d = newic_dm.nicd0;

  //printf(" get_tangents_dm, size_ic_dm: %i len_d: %i \n",size_ic_dm,len_d);

  for (int n=1;n<nnmax-1;n++)
  {
    newic_dm.reset(natoms,anames,anumbers,icoords[n].coords);
    intic_dm.reset(natoms,anames,anumbers,icoords[n-1].coords);
    int2ic_dm.reset(natoms,anames,anumbers,icoords[n+1].coords);

    newic_dm.update_ic();
    intic_dm.update_ic();
    int2ic_dm.update_ic();

#if 0
    newic_dm.bmatp_create();
    newic_dm.bmatp_to_U();
    newic_dm.bmat_create();
    for (int i=0;i<size_ic_dm;i++)
    for (int j=0;j<size_ic_dm;j++)
      intic_dm.Ut[i*size_ic_dm+j] = newic_dm.Ut[i*size_ic_dm+j];
    intic_dm.bmat_create();
    for (int i=0;i<size_ic_dm;i++)
    for (int j=0;j<size_ic_dm;j++)
      int2ic_dm.Ut[i*size_ic_dm+j] = newic_dm.Ut[i*size_ic_dm+j];
    int2ic_dm.bmat_create();
    for (int j=0;j<len_d;j++)
      dqa[n][j] = int2ic_dm.q[j] - intic_dm.q[j];
#if 0
    printf(" printing dqa for node %i \n",n);
    for (int j=0;j<len_d;j++)
      printf(" %1.2f",dqa[n][j]);
    printf("\n");
#endif
    dqmaga[n] = 0.;
    for (int j=0;j<len_d;j++)
      dqmaga[n] += dqa[n][j]*dqa[n][j];
    dqmaga[n] = sqrt(dqvmaga[n]);
    //printf(" dqmaga: %1.2f",dqmaga[n]);

#else

// tangent in redundant coordinates
    for (int i=0;i<nbonds;i++)
      ictan[n][i] = int2ic_dm.bondd[i] - intic_dm.bondd[i];
    for (int i=0;i<nangles;i++)
      ictan[n][nbonds+i] = (int2ic_dm.anglev[i] - intic_dm.anglev[i])*3.14159/180;

    dqmaga[n] = 0.;
    for (int j=0;j<size_ic_dm;j++)
      dqmaga[n] += ictan[n][j]*ictan[n][j];
    dqmaga[n] = sqrt(dqmaga[n]);
    //printf(" dqmaga: %1.2f",dqmaga[n]);
#endif
  }
  //printf("\n");
  //printf(" done with get_tangents_dm \n");

  return;
}


 //not being used!
void GString::get_distances(double* dqmaga, double** ictan)
{
  if (isSSM)
    printf(" NOT SET UP FOR SSM \n");

  int nbonds = newic.nbonds;
  int nangles = newic.nangles;
  int ntor = newic.ntor;
  int size_ic = newic.nbonds+newic.nangles+newic.ntor+newic.nxyzic;
  int len_d = newic.nicd0;

  double** dqa = new double*[nnmax];
  for (int n=0;n<nnmax;n++)
    dqa[n] = new double[len_d];

#if 0
  for (int n=1;n<nnmax-1;n++)
  {
    newic.reset(natoms,anames,anumbers,icoords[n].coords);
    intic.reset(natoms,anames,anumbers,icoords[n-1].coords);

    newic.update_ic();
    intic.update_ic();

    newic.bmatp_create();
    newic.bmatp_to_U();
    newic.bmat_create();
    for (int i=0;i<size_ic;i++)
    for (int j=0;j<size_ic;j++)
      intic.Ut[i*size_ic+j] = newic.Ut[i*size_ic+j];
    intic.bmat_create();
    for (int j=0;j<len_d;j++)
      dqa[n][j] = newic.q[j] - intic.q[j];
#if 0
    printf(" printing dqa for node %i \n",n);
    for (int j=0;j<len_d;j++)
      printf(" %1.2f",dqa[n][j]);
    printf("\n");
#endif
    dqmaga[n] = 0.;
    for (int j=0;j<len_d;j++)
      dqmaga[n] += dqa[n][j]*dqa[n][j];
    dqmaga[n] = sqrt(dqmaga[n]);
   // printf(" dqmaga[%i]: %1.2f \n",n,dqmaga[n]);

  }
#else
  for (int n=1;n<nnmax-1;n++)
  {
    newic.reset(natoms,anames,anumbers,icoords[n].coords);
    intic.reset(natoms,anames,anumbers,icoords[n-1].coords);

    newic.update_ic();
    intic.update_ic();

    dqmaga[n] = 0.;
    for (int j=0;j<size_ic;j++) //CPMZ: - ntor?
      dqmaga[n] += ictan[n][j]*ictan[n][j];
    dqmaga[n] = sqrt(dqmaga[n]);
#endif
  } //loop n over nnmax
  //printf("\n");

  for (int n=0;n<nnmax;n++)
    delete [] dqa[n];
  delete [] dqa;

  return;
}


void GString::get_distances_dm(double* dqmaga, double** ictan)
{
  if (isSSM)
    printf(" NOT SET UP FOR SSM \n");

  int size_ic_dm = newic_dm.nbonds+newic_dm.nangles+newic_dm.ntor+newic_dm.nxyzic;
  int len_d = newic_dm.nicd0;

  double** dqa = new double*[nnmax];
  for (int n=0;n<nnmax;n++)
    dqa[n] = new double[len_d];

#if 1
  for (int n=1;n<nnmax;n++)
  {
    newic_dm.reset(natoms,anames,anumbers,icoords[n].coords);
    intic_dm.reset(natoms,anames,anumbers,icoords[n-1].coords);

    newic_dm.update_ic();
    intic_dm.update_ic();

    newic_dm.bmatp_create();
    newic_dm.bmatp_to_U();
    newic_dm.bmat_create();
    for (int i=0;i<size_ic_dm;i++)
    for (int j=0;j<size_ic_dm;j++)
      intic_dm.Ut[i*size_ic_dm+j] = newic_dm.Ut[i*size_ic_dm+j];
    intic_dm.bmat_create();
    for (int j=0;j<len_d;j++)
      dqa[n][j] = newic_dm.q[j] - intic_dm.q[j];
#if 0
    printf(" printing dqa for node %i \n",n);
    for (int j=0;j<len_d;j++)
      printf(" %1.2f",dqa[n][j]);
    printf("\n");
#endif
    dqmaga[n] = 0.;
    for (int j=0;j<len_d;j++)
      dqmaga[n] += dqa[n][j]*dqa[n][j];
    dqmaga[n] = sqrt(dqmaga[n]);
   // printf(" dqmaga[%i]: %1.2f \n",n,dqmaga[n]);

  }
#else
  for (int n=1;n<nnmax;n++)
  {
    dqmaga[n] = 0.;
    for (int j=0;j<size_ic_dm;j++)
      dqmaga[n] += ictan[n][j]*ictan[n][j];
    dqmaga[n] = sqrt(dqmaga[n]);
   // printf(" dqmaga[%i]: %1.2f \n",n,dqmaga[n]);
  }
#endif

  for (int n=0;n<nnmax;n++)
    delete [] dqa[n];
  delete [] dqa;

  return;
}
 
void GString::print_string(int nodes, double** allcoords0, string xyzstring)
{
  if (nodes>nnmax0)
    nodes = nnmax0;

  ofstream xyzfilec;
//  string xyzstring = "xyzfile.xyzc";
  xyzfilec.open(xyzstring.c_str());
  xyzfilec.setf(ios::fixed);
  xyzfilec.setf(ios::left);
  xyzfilec << setprecision(6);

  for (int n=0;n<nodes;n++)
  if (active[n]>-1 || active[n]==-2 || isFSM || isSSM)
  {
    xyzfilec << " " << natoms << endl << " " << V_profile[n] << endl;
    for (int i=0;i<natoms;i++)
    {
      xyzfilec << "  " << anames[i];
      xyzfilec << " " << allcoords0[n][3*i+0] << " " << allcoords0[n][3*i+1] << " " << allcoords0[n][3*i+2];
      xyzfilec << endl;
    }
  }

  xyzfilec.close();

  return;
}

void GString::print_string_clump(int nodes, double tgrad, double** allcoords0, string xyzstring)
{

  ofstream xyzfilec;
//  string xyzstring = "xyzfile.xyzc";
  xyzfilec.open(xyzstring.c_str());
  xyzfilec.setf(ios::fixed);
  xyzfilec.setf(ios::left);
  xyzfilec << setprecision(6);

  xyzfilec << " " << natoms*nodes << endl << tgrad << endl;

  for (int n=0;n<nodes;n++)
  if (active[n]>-1 || active[n]==-2)
  {
    for (int i=0;i<natoms;i++)
    {
      xyzfilec << "  " << anames[i];
      xyzfilec << " " << allcoords0[n][3*i+0]+n*xdist << " " << allcoords0[n][3*i+1] << " " << allcoords0[n][3*i+2];
      xyzfilec << endl;
    }
  }
  else
  {
    for (int i=0;i<natoms;i++)
    {
      xyzfilec << "  " << anames[i];
      xyzfilec << " " << allcoords0[0][3*i+0]+n*xdist << " " << allcoords0[0][3*i+1] << " " << allcoords0[0][3*i+2];
      xyzfilec << endl;
    }
  }

  xyzfilec.close();

  return;
}

void GString::print_string_clump_p(int nodes, double tgrad, double** allcoords0, string xyzstring)
{

  ofstream xyzfilec;
//  string xyzstring = "xyzfile.xyzc";
  xyzfilec.open(xyzstring.c_str());
  xyzfilec.setf(ios::fixed);
  xyzfilec.setf(ios::left);
  xyzfilec << setprecision(6);

  xyzfilec << " " << 2*natoms*nodes << endl << tgrad << endl;

  for (int n=0;n<nodes;n++)
  if (active[n]>-1 || active[n]==-2)
  {
    for (int i=0;i<natoms;i++)
    {
      xyzfilec << "  " << anames[i];
      xyzfilec << " " << allcoords0[n][3*i+0]+n*xdist << " " << allcoords0[n][3*i+1] << " " << allcoords0[n][3*i+2];
      xyzfilec << endl;
    }
    for (int i=0;i<natoms;i++)
    {
      xyzfilec << "  X";
      xyzfilec << " " << pTSnodecoords[3*i+0]+n*xdist << " " << pTSnodecoords[3*i+1] << " " << pTSnodecoords[3*i+2];
      xyzfilec << endl;
    }
  }
  else
  {
    for (int i=0;i<natoms;i++)
    {
      xyzfilec << "  " << anames[i];
      xyzfilec << " " << allcoords0[0][3*i+0]+n*xdist << " " << allcoords0[0][3*i+1] << " " << allcoords0[0][3*i+2];
      xyzfilec << endl;
    }
    for (int i=0;i<natoms;i++)
    {
      xyzfilec << "  X";
      xyzfilec << " " << pTSnodecoords[3*i+0]+n*xdist << " " << pTSnodecoords[3*i+1] << " " << pTSnodecoords[3*i+2];
      xyzfilec << endl;
    }
  }

  xyzfilec.close();

  return;
}

void GString::align_string(ICoord ic1, ICoord ic2)
{ 
  printf(" aligning string via 3 stationary atoms \n");

  int natoms = ic1.natoms;
  int nbonds1 = ic1.nbonds;
  int nbonds2 = ic2.nbonds;

  printf(" reactant ic \n");
  //ic1.print_ic();
  for (int i=0;i<natoms;i++)
    printf(" %i: %i coord \n",i,ic1.coordn[i]);

  printf(" product ic \n");
  //ic2.print_ic();
  for (int i=0;i<natoms;i++)
    printf(" %i: %i coord \n",i,ic2.coordn[i]);


  int* atomsf = new int[2*natoms];
  for (int i=0;i<2*natoms;i++) atomsf[i] = 0;
  for (int i=0;i<nbonds1;i++)
  {
    int b1 = ic1.bonds[i][0];
    int b2 = ic1.bonds[i][1];

    if (ic2.bond_exists(b1,b2))
    {
      atomsf[b1]++;
      atomsf[b2]++;
    }
  }

  int* a = new int[3];
  int found = 0;
  for (int i=0;i<natoms;i++)
  if (found<3)
  {
    if (atomsf[i]==ic1.coordn[i])
    {
      //printf(" this atom doesn't move: %i \n",i);
      a[found++] = i;
    }
  }
  printf(" plane of %i %i %i \n",a[0],a[1],a[2]);

  for (int n=0;n<nnmax;n++)
  if (active[n]>-1)
    rotate_structure(icoords[n].coords,a);


  delete [] atomsf;


  return;
}

void GString::rotate_structure(double* xyz0, int* a)
{
  printf("\n in rotate_structure \n");

  double* xyz1 = new double[3*natoms];
  for (int i=0;i<natoms;i++)
  {
    xyz1[3*i+0] = xyz0[3*i+0] - xyz0[3*a[0]+0];
    xyz1[3*i+1] = xyz0[3*i+1] - xyz0[3*a[0]+1];
    xyz1[3*i+2] = xyz0[3*i+2] - xyz0[3*a[0]+2];
  }
  print_xyz_gen(natoms,anames,xyz1);


  double** rotm = new double*[3];
  rotm[0] = new double[3]; 
  rotm[1] = new double[3]; 
  rotm[2] = new double[3]; 

  double* angles = new double[3];
  double* xyzn = new double[3*natoms];
  double* u1 = new double[3];


 //align second atom to x axis
  u1[0] = xyz1[3*a[1]+0];
  u1[1] = xyz1[3*a[1]+1];
  u1[2] = xyz1[3*a[1]+2];
  double norm = sqrt(u1[0]*u1[0]+u1[1]*u1[1]);
  u1[0] = u1[0]/norm; u1[1] = u1[1]/norm; u1[2] = u1[2]/norm;
  //printf(" u1: %4.3f %4.3f %4.3f \n",u1[0],u1[1],u1[2]);

  angles[0] = angles[1] = angles[2] = 0.;
  angles[2] = acos(u1[0]);
  printf(" angle of %i %i to x axis: %4.3f \n",a[0],a[1],angles[2]);

  Utils::get_rotation_matrix(rotm,angles);

  for (int i=0;i<3*natoms;i++) xyzn[i] = 0.;
  for (int i=0;i<natoms;i++)
  for (int j=0;j<3;j++)
  for (int k=0;k<3;k++)
    xyzn[3*i+j] += rotm[j][k]*xyz1[3*i+k];
  for (int i=0;i<3*natoms;i++)
    xyz1[i] = xyzn[i];
  print_xyz_gen(natoms,anames,xyz1);

  u1[0] = xyz1[3*a[1]+0];
  u1[1] = xyz1[3*a[1]+1];
  u1[2] = xyz1[3*a[1]+2];
  norm = sqrt(u1[0]*u1[0]+u1[2]*u1[2]);
  u1[0] = u1[0]/norm; u1[1] = u1[1]/norm; u1[2] = u1[2]/norm;

  angles[0] = angles[1] = angles[2] = 0.;
  angles[1] = acos(u1[0]);
  printf(" angle of %i %i to x axis: %4.3f \n",a[0],a[1],angles[1]);

  Utils::get_rotation_matrix(rotm,angles);
  
  for (int i=0;i<3*natoms;i++) xyzn[i] = 0.;
  for (int i=0;i<natoms;i++)
  for (int j=0;j<3;j++)
  for (int k=0;k<3;k++)
    xyzn[3*i+j] += rotm[j][k]*xyz1[3*i+k];
  for (int i=0;i<3*natoms;i++)
    xyz1[i] = xyzn[i];
  print_xyz_gen(natoms,anames,xyz1);

 
 //third atom
  u1[0] = xyz1[3*a[2]+0] - xyz1[3*a[1]+0];
  u1[1] = xyz1[3*a[2]+1] - xyz1[3*a[1]+1];
  u1[2] = xyz1[3*a[2]+2] - xyz1[3*a[1]+2];
  norm = sqrt(u1[1]*u1[1]+u1[2]*u1[2]);
  u1[0] = u1[0]/norm; u1[1] = u1[1]/norm; u1[2] = u1[2]/norm;

  angles[0] = angles[1] = angles[2] = 0.;
  angles[0] = acos(u1[1]);
  printf(" angle of %i %i to z axis: %4.3f \n",a[1],a[2],angles[0]);

  Utils::get_rotation_matrix(rotm,angles);
  
  for (int i=0;i<3*natoms;i++) xyzn[i] = 0.;
  for (int i=0;i<natoms;i++)
  for (int j=0;j<3;j++)
  for (int k=0;k<3;k++)
    xyzn[3*i+j] += rotm[j][k]*xyz1[3*i+k];
  for (int i=0;i<3*natoms;i++)
    xyz1[i] = xyzn[i];

  print_xyz_gen(natoms,anames,xyz1);


  return;
}

int GString::read_string(string stringfile, double** coordsn, double* energies)
{
  printf("  in read_string \n");

  ifstream infile;
  infile.open(stringfile.c_str());
  if (!infile){
    printf("  Error opening string file: %s \n",stringfile.c_str());
    exit(-1);
  }

  string line;
  bool success=true;
  success=getline(infile, line);
  if (success)
  {
    int length=StringTools::cleanstring(line);
    int natoms0=atoi(line.c_str());
    if (natoms0!=natoms)
    {
      printf(" xyz size mismatch! %i %i \n",natoms0,natoms);
      exit(1);
    }
  }

  int nfound = 0;
  for (int i=0;i<nnmax;i++)
  {
    if (i>0)
      success=getline(infile, line);
    success=getline(infile, line);
    if (infile.eof())
    {
      printf("   end of restart.xyz reached \n");
      break;
    }
    int length1=StringTools::cleanstring(line);
    vector<string> tok_line1 = StringTools::tokenize(line, " \t");
    energies[i]=atof(tok_line1[0].c_str());

    for (int j=0;j<natoms;j++)
    {
      success=getline(infile, line);
      int length=StringTools::cleanstring(line);
      vector<string> tok_line = StringTools::tokenize(line, " \t");
//      cout << " i: " << i << " string: " << line << endl;
      coordsn[i][3*j+0]=atof(tok_line[1].c_str());
      coordsn[i][3*j+1]=atof(tok_line[2].c_str());
      coordsn[i][3*j+2]=atof(tok_line[3].c_str());
    }
    nfound++;
    if (infile.eof())
    {
      printf(" end of restart.xyz reached \n");
      break;
    }
  }

  infile.close();

  //printf("  done reading \n");

#if 0
  printf(" printing string back \n");
  for (int i=0;i<nnmax;i++)
  {
    printf(" %i \n\n",natoms);
    for (int j=0;j<natoms;j++)
      printf(" %4.4f %4.4f %4.4f \n",coordsn[i][3*j+0],coordsn[i][3*j+1],coordsn[i][3*j+2]);
  }
#endif

  return nfound;
}


//CPMZ here
void GString::restart_string(string pstring)
{
 //Note: isRestart==2 keeps initial.xyz structures at endpoints
  int natoms = newic.natoms;
  int N3 = 3*natoms;

  //read in xyz file
  double** coordsn = new double*[nnmax];
  for (int i=0;i<nnmax;i++)
    coordsn[i] = new double[N3];
  double* energies = new double[nnmax];
  int nrnodes = read_string(pstring,coordsn,energies);
  printf("  found %i nodes \n",nrnodes);

  printf(" restart energies:");
  for (int i=0;i<nrnodes;i++)
    printf(" E[%i]: %3.2f",i,energies[i]);
  printf("\n");

  printf(" copying structures to coords \n");
  if (isRestart==2)
    printf(" not modifying starting point \n");
  else
  for (int i=0;i<3*natoms;i++)
    coords[0][i] = coordsn[0][i];
  
  for (int n=1;n<nrnodes-1;n++)
  for (int i=0;i<3*natoms;i++)
    coords[n][i] = coordsn[n][i];

  if (isRestart!=2)
  for (int i=0;i<3*natoms;i++)
    coords[nrnodes-1][i] = coordsn[nrnodes-1][i];

  printf(" updating icoords \n");
  for (int n=0;n<nrnodes;n++)
    icoords[n].reset(natoms,anames,anumbers,coords[n]);

  printf(" resetting hessians \n");
  for (int n=1;n<nrnodes;n++)
  {
    icoords[n].bmatp_create();
    icoords[n].bmatp_to_U();
    icoords[n].bmat_create();
    icoords[n].update_ic();
    icoords[n].make_Hint();
    icoords[n].gradrms = 0.;
    icoords[n].DMAX = 0.05; //half of default value
  }

  for (int n=1;n<nrnodes-1;n++)
  {
    active[n] = 1;
    V_profile[n] = energies[n];
  }
  V_profile[nrnodes-1] = energies[nrnodes-1];
  active[nrnodes-1] = 0;

  for (int i=0;i<nnmax;i++)
    delete [] coordsn[i];
  delete [] coordsn;
  delete [] energies;

  nn = nrnodes;
  nnR = nrnodes;
  nnmax = nrnodes;
  n0 = 0;

#if 0
  printf("  active nodes:");
  for (int i=0;i<nrnodes;i++)
  if (active[i])
    printf(" %i",i);
  printf("\n");
#endif

  double emax = -1000.;
  int nmax = 0;
  for (int i=1;i<nnmax;i++)
  if (V_profile[i]>emax)
  {
    nmax = i;
    emax = V_profile[i];
  }
  if (isRestart==3)
  {
    printf(" RESTART==3, reading Hessian for node %2i \n",nmax);
    string nstr = StringTools::int2str(runNum,4,"0");
    string hfilename = "scratch/hess"+nstr+".xyz";
    icoords[nmax].read_hessxyz(hfilename,0);
    icoords[nmax].useExactH = 1;
    climb = 1;
    find = 1;
    TSnode0 = nmax;
    for (int n=0;n<nnmax;n++)
      icoords[n].isTSnode = 0;
    icoords[nmax].isTSnode = 1;
    icoords[nmax].newHess = 1;
    icoords[nmax].OPTTHRESH = CONV_TOL;
    icoords[nmax].use_constraint = 0;
    icoords[nmax].pgradrms = 10000.;
  }

  return;
}


void GString::set_prima(string pstring)
{
  printf("\n in set_prima for: %s \n",pstring.c_str());

  int nbonds = newic.nbonds;
  int nangles = newic.nangles;
  int ntor = newic.ntor;
  int len = nbonds + nangles + ntor;
  int N3 = 3*natoms;

  pTSnodecoords = new double[N3];
  for (int i=0;i<N3;i++) pTSnodecoords[i] = 0.;

  //read in xyz file
  double** coordsn = new double*[nnmax];
  for (int i=0;i<nnmax;i++)
    coordsn[i] = new double[N3];
  double* energies = new double[nnmax];
  int nrnodes = read_string(pstring,coordsn,energies);

#if 1
  printf(" restart energies:");
  for (int i=0;i<nnmax;i++)
    printf(" E[%i]: %3.2f",i,energies[i]);
  printf("\n");
#endif

  //create ic's for all (use newic coordinate system)
  //save primitive ic values

  double** prima = new double*[nrnodes];
  for (int i=0;i<nrnodes;i++)
    prima[i] = new double[len];

  int* unodes = new int[nrnodes];
  for (int i=0;i<nrnodes;i++) unodes[i] = 0;

  int nmin = 0;
  int nmax = 0;
  double emin = 1000.;
  double emax = -1000.;
  for (int i=1;i<nrnodes-1;i++)
  {
    if (energies[i]<emin)
    {
      emin = energies[i];
      nmin = i;
    }
    if (energies[i]>emax)
    {
      emax = energies[i];
      nmax = i;
    }
  }
  //shift if first or last node
  if (nmax==1) nmax = 2;
  if (nmax==nrnodes-2) nmax = nrnodes-3;

  printf(" nmax: %i nmin: %i \n",nmax,nmin);
  pTSnode = nmax;
  for (int i=0;i<N3;i++)
    pTSnodecoords[i] = coordsn[pTSnode][i];

#if 1
  int npused = 3;
  for (int i=-1;i<2;i++)
  {
    int n = nmax+i;
    newic.reset(coordsn[n]);
    newic.update_ic();

    for (int j=0;j<nbonds;j++)
      prima[i+1][j] = newic.bondd[j];
    for (int j=0;j<nangles;j++)
      prima[i+1][nbonds+j] = newic.anglev[j];
    for (int j=0;j<ntor;j++)
      prima[i+1][nbonds+nangles+j] = newic.torv[j];
  }
#else
  //use all nodes for prima
  int npused = nrnodes;
  for (int i=0;i<nrnodes;i++)
  {
    newic.reset(coordsn[i]);
    newic.update_ic();

    for (int j=0;j<nbonds;j++)
      prima[i][j] = newic.bondd[j];
    for (int j=0;j<nangles;j++)
      prima[i][nbonds+j] = newic.anglev[j];
    for (int j=0;j<ntor;j++)
      prima[i][nbonds+nangles+j] = newic.torv[j];
  }
#endif

 //note: corrected torsion tangent 
  double* tan1 = new double[len];
  double* tan2 = new double[len];
  for (int i=0;i<len;i++) tan1[i] = 0.;
  for (int i=0;i<len;i++) tan2[i] = 0.;
  for (int i=0;i<nbonds;i++)
    tan1[i] = prima[0][i] - prima[1][i];
  for (int i=0;i<nbonds;i++)
    tan2[i] = prima[1][i] - prima[2][i];
  for (int i=nbonds;i<nbonds+nangles;i++)
    tan1[i] = (prima[0][i] - prima[1][i])*3.14/180.;
  for (int i=nbonds;i<nbonds+nangles;i++)
    tan2[i] = (prima[1][i] - prima[2][i])*3.14/180.;
#if 1
  for (int i=nbonds+nangles;i<len;i++)
  {
    tan1[i] = (prima[0][i] - prima[1][i])*3.14/180.;
    if (tan1[i]>3.14159)
      tan1[i] = -1*(2*3.14159 - tan1[i]);
    if (tan1[i]<-3.14159)
      tan1[i] = 2*3.14159 + tan1[i];
  }
  for (int i=nbonds+nangles;i<len;i++)
  {
    tan2[i] = (prima[0][1] - prima[2][i])*3.14/180.;
    if (tan2[i]>3.14159)
      tan2[i] = -1*(2*3.14159 - tan2[i]);
    if (tan2[i]<-3.14159)
      tan2[i] = 2*3.14159 + tan2[i];
  }
#endif

#if 1
  printf(" printing tangents:");
  for (int i=0;i<len;i++)
    printf(" %3.2f",tan1[i]);
  printf("\n");
  for (int i=0;i<len;i++)
    printf(" %3.2f",tan2[i]);
  printf("\n\n"); 
#endif

#if 0
  //eliminate coordinates that don't move
  double THRESH = 0.001;
  for (int i=0;i<len;i++)
  if (abs(tan1[i])+abs(tan2[i])<THRESH)
    prima[0][i] = prima[1][i] = prima[2][i] = -999.;
#endif

#if 0
  //just TS node
  printf(" only using TS node for prima \n\n");
  for (int i=0;i<len;i++)
    prima[0][i] = prima[2][i] = prima[1][i];
#endif

  //put prim ic's into node files
  for (int i=0;i<nnmax;i++)
    icoords[i].create_prima(npused,nbonds,nangles,ntor,prima);

#if RIBBONS
  if (nnmax==nrnodes)
  {
    for (int n=0;n<nnmax;n++)
      V_profile[n] = energies[n];
    for (int n=1;n<nnmax-1;n++)
      icoords[n].reset(coordsn[n]);
  }
  else
  {
    printf(" error: nrnodes != nnmax \n");
    exit(1);
  }

  newic.create_prima(npused,nbonds,nangles,ntor,prima);
  newic.reset(pTSnodecoords);
//  newic.reset(coordsn[pTSnode-1]); //using minus one
#endif

  delete [] tan1;
  delete [] tan2;

  for (int i=0;i<nnmax;i++)
    delete [] coordsn[i];
  delete [] coordsn;

  return;
}

void GString::set_fsm_active(int nnR, int nnP)
{
   if (nnR!=nnP)
     printf(" setting active nodes to %i and %i \n",nnR,nnP);
   else
     printf(" setting active node to %i \n",nnR);

   for (int i=0;i<nnmax;i++)
   {
     active[i] = -1;
     icoords[i].OPTTHRESH = CONV_TOL*2.;
   }
   active[nnR] = 1;
   active[nnP] = 1;

   if (isSSM)
   {
     icoords[nnR].OPTTHRESH = CONV_TOL*10.;
     icoords[nnP].OPTTHRESH = CONV_TOL*10.;
//     icoords[nnR].OPTTHRESH = icoords[nnP].OPTTHRESH = CONV_TOL*15.;
   }


   return;
}


//CPMZ note: here we could just add the torsion or angle?
void GString::set_ssm_bonds(ICoord &ic1)
{
  printf("\n setting SSM bonds \n");

  printf(" nbonds: %2i nangles: %2i ntor: %2i \n",ic1.nbonds,ic1.nangles,ic1.ntor);
  ic1.isOpt = 0;
  if (bondfrags==2)
    ic1.isOpt = 2;
  for (int i=0;i<nbond;i++)
  {
    int a1 = bond[2*i+0];
    int a2 = bond[2*i+1];
    if (!ic1.bond_exists(a1,a2))
    {
      ic1.bonds[ic1.nbonds][0] = a1;
      ic1.bonds[ic1.nbonds][1] = a2;
      ic1.nbonds++;
      printf(" added bond for coordinates only: %i %i \n",a1+1,a2+1);
    }
  } //loop i over nbond

  if (!isSSM) 
  {
    ic1.ic_create_nobonds();
    return;
  }

  for (int i=0;i<nadd;i++)
  {
    int a1 = add[2*i+0];
    int a2 = add[2*i+1];
    if (!ic1.bond_exists(a1,a2))
    {
      ic1.bonds[ic1.nbonds][0] = a1;
      ic1.bonds[ic1.nbonds][1] = a2;
      ic1.nbonds++;
      printf(" added add bond: %i %i \n",a1+1,a2+1);
    }
  } //loop i over nadd
  for (int i=0;i<nbrk;i++)
  {
    int b1 = brk[2*i+0];
    int b2 = brk[2*i+1];
    if (!ic1.bond_exists(b1,b2))
    {
      ic1.bonds[ic1.nbonds][0] = b1;
      ic1.bonds[ic1.nbonds][1] = b2;
      ic1.nbonds++;
      printf(" added brk bond: %i %i \n",b1+1,b2+1);
    }
  } //loop i over nbrk

  for (int i=0;i<nangle;i++)
  {
    int b1 = angles[3*i+0];
    int b2 = angles[3*i+1];
    int b3 = angles[3*i+2];

    if (!ic1.bond_exists(b1,b2))
    {
      ic1.bonds[ic1.nbonds][0] = b1;
      ic1.bonds[ic1.nbonds][1] = b2;
      ic1.nbonds++;
      printf(" added angle bond: %i %i \n",b1+1,b2+1);
    }
    if (!ic1.bond_exists(b2,b3))
    {
      ic1.bonds[ic1.nbonds][0] = b2;
      ic1.bonds[ic1.nbonds][1] = b3;
      ic1.nbonds++;
      printf(" added angle bond: %i %i \n",b2+1,b3+1);
    }
  } //loop i over angles

  for (int i=0;i<ntors;i++)
  {
    int b1 = tors[4*i+0];
    int b2 = tors[4*i+1];
    int b3 = tors[4*i+2];
    int b4 = tors[4*i+3];

    if (!ic1.bond_exists(b1,b2))
    {
      ic1.bonds[ic1.nbonds][0] = b1;
      ic1.bonds[ic1.nbonds][1] = b2;
      ic1.nbonds++;
      printf(" added tor bond: %i %i \n",b1+1,b2+1);
    }
    if (!ic1.bond_exists(b2,b3))
    {
      ic1.bonds[ic1.nbonds][0] = b2;
      ic1.bonds[ic1.nbonds][1] = b3;
      ic1.nbonds++;
      printf(" added tor bond: %i %i \n",b2+1,b3+1);
    }
    if (!ic1.bond_exists(b3,b4))
    {
      ic1.bonds[ic1.nbonds][0] = b3;
      ic1.bonds[ic1.nbonds][1] = b4;
      ic1.nbonds++;
      printf(" added tor bond: %i %i \n",b3+1,b4+1);
    }
  } //loop i over angles

  ic1.ic_create_nobonds();

  return;
} 

///Checks if string is past_ts. Used by SSM.
int GString::past_ts()
{
  int ispast = 0; //return value
  int ispast1 = 0;
  int ispast2 = 0;
  int ispast3 = 0;
  double THRESH1 = 5.0; //was 1.0
  double THRESH2 = 3.0;
  double THRESH3 = -1.0;
  double THRESHB = 0.05;
  double CTHRESH = 0.005; //constraint gradient 
  //old// double OTHRESH = -0.05; //cgrad for over the hill
  double OTHRESH = -0.15; //cgrad for over the hill

  double emax = -100.;
  int nodemax = 1; //changed to start at node 1
 //CPMZ changed to n0
  int ns = n0 - 1;
  if (ns<nodemax) ns = nodemax;
  for (int n=ns;n<nnR;n++)
  {
    //printf(" %4.3f",V_profile[n]);
    if (V_profile[n]>emax)
    {
      nodemax = n;
      emax = V_profile[n];
    }
  }

  //printf(" nnmax: %i nn: %i \n",nnmax,nn);
  for (int n=nodemax;n<nnR;n++)
  {
    if (V_profile[n]<emax-THRESH1)
      ispast1++;
    if (V_profile[n]<emax-THRESH2)
      ispast2++;
    if (V_profile[n]<emax-THRESH3)
      ispast3++;
//    if (V_profile[n]>V_profile[n-1]+THRESHB) gup++;
    if (ispast1>1)
      break;
  }
#if 0
  int nodemax1 = nodemax;
  if (nodemax1==nnR-1) nodemax1--;
  for (int n=nodemax1;n<nnR;n++)
  {
    if (V_profile[n]<emax-THRESH3)
      ispast3++;
  }
#endif

  printf("\n");

  double cgrad = icoords[nnR-1].gradq[icoords[nnR-1].nicd0-1];
  printf(" cgrad: %4.3f nodemax: %i nnR: %i \n",cgrad,nodemax,nnR);
  if (cgrad>CTHRESH)
  {
    printf(" constraint gradient positive! \n");
    ispast = 2;
  }
  else if (ispast1>0 && cgrad>OTHRESH) 
  {
    printf(" over the hill(1)! \n");
    ispast = 1;
  }
  else if (ispast2>1) 
  {
    printf(" over the hill(2)! \n");
    ispast = 1;
  }
  else
    ispast = 0;

  if (!ispast)
  {
    int bch = check_for_reaction_g(1);
    if (ispast3>1 && bch)
    {
      printf(" over the hill(3)! %i connections changed \n",bch);
      ispast = 3;
    }
  }

  return ispast;
}

/**
 * Optimization and add node driver for SE and DE during growth phase.
 * max_iter is the max number of iterations for the entire calculation (growth and
 * optimiazation)
 */
void GString::growth_iters(int max_iter, double& totalgrad, double& gradrms, double endenergy, string strfileg, int& tscontinue, double gaddmax, int osteps, int oesteps, double** dqa, double* dqmaga, double** ictan)
{
  double rn3m6 = sqrt(3*natoms-6);
  int nmax;
  double emax;
  double emaxp;
  double emin = 0.;
  string nstr = StringTools::int2str(runNum,4,"0");
  string endg = StringTools::int2str(ngrowth+1,1,"0");
  tscontinue = 1;
  growing = 1;
  TSnode0 = 0;
  climb = find = 0;
  int pastts = 0;
  int addednode = 0;
  endearly = 0;

  if (isSSM)
    osteps = STEP_OPT_ITERS;

  ngrowth++;
  if ((ngrowth>4 && tstype!=2) || (ngrowth>14 && tstype==2))
  {
    printf("\n at limit of growth (%i growth attempts) \n",ngrowth);
    exit(1);
  }

//  for (int n=0;n<nnmax0;n++) icoords[n].grad1.knnr_active = 3;

  for (;oi<max_iter;oi++)
  {
    printf("\n growing iter: %i \n",oi+1);

    if (oi>0 && isFSM)
      set_fsm_active(nnR,nnmax-nnP-1);
    if (oi>0 && isSSM)
      set_fsm_active(nnR,nnR);
    if((icoords[nnR-1].gradrms<gaddmax && GROWD!=2) || isFSM || isSSM)
    {
      if (oi>0 && nn < nnmax)
      {
        addednode = addNode(nnR-1,nnR,nnmax-nnP);
        if (addednode) nnR++;
        else
        {
          set_fsm_active(nnR-1,nnR-1); 
          icoords[nnR-1].OPTTHRESH = CONV_TOL;
        }
      }
    } 
    //printf(" icoords[nnmax-nnP].gradrms: %8.5f GROWD: %i \n",icoords[nnmax-nnP].gradrms,GROWD);
    if((icoords[nnmax-nnP].gradrms<gaddmax && GROWD!=1) || isFSM)
    {
      if (oi>0 && nn < nnmax && !isSSM)
      {
        addNode(nnmax-nnP,nnmax-nnP-1,nnR-1);
        nnP++;
      }
    }
    if (nn==nnmax) 
    {
      if (isFSM)
      {
        get_tangents_1g(dqa,dqmaga,ictan);
        opt_steps(dqa,ictan,osteps,oesteps);
      }
      else if (!isSSM)
      {
        printf(" gopt_iter: string done growing \n");
        break;
      }
    }
    if (!isFSM && !isSSM)
      ic_reparam_g(dqa,dqmaga);

    get_tangents_1g(dqa,dqmaga,ictan);
    opt_steps(dqa,ictan,osteps,oesteps);

    if (isSSM)
    {
      pastts = past_ts();
      if (pastts && using_break_planes)
      {
        int bch = check_for_reaction_g(0);
        if (bch==0)
        {
          pastts = 0;
          printf("  break_planes in use and no bond changes: \n");
          printf("   not yet past TS \n");
        }
      }
      if ((pastts && nn>3) || (addednode==0 && nn>2)) //checks for TS starting with n0-1 (was 0, previously n0)
      {
        printf(" gopt_iter: SSM string done growing \n\n");
        if (tstype==2)
          n0 = 0;
        else
          n0 = find_uphill(3.0) - 1; //go to first node X kcal/mol above start
        if (n0<0 || nnR < 6) n0 = 0;
        printf(" new n0: %i \n",n0);
        break;
      }
      int fp = find_peaks(1);
      if (fp==-1 && V_profile[nnR-1]>200.) //CPMZ parameter
      {
        printf(" gopt_iters over: all uphill and high energy \n");
        endearly = 2;
        tscontinue = 0;
        nnmax = nnR;
        break;
      }
      if (fp==-2)
      {
        printf(" gopt_iters over: all uphill and flattening out \n");
        endearly = 2;
        tscontinue = 0;
        nnmax = nnR;
        break;
      }
#if 0
      if (fp>0) //for TS starting from first node
      {
        printf(" gopt_iter: SSM string grew past TS \n");
//        n0 = find_peaks(3) - 3; //put 2 free nodes prior to TS
        n0 = find_uphill(3.0); //go to first node X kcal/mol above start
        if (n0<0) n0 = 0;
        printf(" new n0: %i \n",n0);
        break;
      }
#endif
    } //SSM growth terminations

    totalgrad = 0.;
    gradrms = 0.;
    for (int i=1;i<nnmax-1;i++)
    if (active[i]>-1 || isSSM || isFSM)
    {
      if (icoords[i].gradrms<1000.)
      {
        totalgrad += icoords[i].gradrms*rn3m6;
        gradrms += icoords[i].gradrms*icoords[i].gradrms;
      }
    }
    if (isSSM)
      gradrms = sqrt(gradrms/(nnR-1));
    else
      gradrms = sqrt(gradrms/(nn-2));
    emaxp = emax;
    emax = -10000;
    nmax = 1;
    for (int i=0;i<nnmax;i++)
    if (active[i]>-1 || isFSM || isSSM)
    if (V_profile[i]>emax)
    {
      emax = V_profile[i];
      nmax = i;
    }
    printf("\n");
    printf(" gopt_iter: %2i totalgrad: %4.3f gradrms: %5.4f tgrads: %3i",oi+1,totalgrad,gradrms,gradJobCount);
    printf(" max E: %5.1f",emax-emin);
    if (isSSM)
    {
      double cgrad = icoords[nnR-1].gradq[icoords[nnR-1].nicd0-1];
      printf(" cgr: %6.3f",cgrad);
    }
    printf(" \n");

    if ((emax>3*endenergy && oi>25) || (emax>10*endenergy && oi>15) || oi>150) 
    {
      printf(" Growth is poor, emax/limit: %1.1f %1.1f oi/limit: %2i %2i \n",emax,endenergy,oi,25);
      printf(" Exiting early (Growth Phase)! \n");
      endearly = 1;
      tscontinue = 0;
      break;
    }

    printf(" printing string to %s%s \n",strfileg.c_str(),endg.c_str());
    if (isSSM)
      print_string(nnR,allcoords,strfileg+endg);
    else
      print_string(nnmax,allcoords,strfileg+endg);
#if 1
    string ois = StringTools::int2str(oi,2,"0");
    string nstr = StringTools::int2str(runNum,4,"0");
    string strfile = "stringfile.xyz"+nstr+"_"+ois+".xyz";
#if USE_PRIMA
    print_string_clump_p(nnmax,totalgrad,allcoords,strfile);
#else
//    print_string_clump(nnmax,totalgrad,allcoords,strfile);
#endif
#endif

  } // growth iters

  if (tscontinue==1 && isSSM)
  {
    if (pastts==1)
    {
      int addedn = addNode(nnR-1,nnR,nnmax-nnP);
      if (addedn)
        add_last_node(2);
    }
    else if (pastts==2) //when constraint grad is positive
    {
      add_last_node(1);
      if (icoords[nnR-1].gradrms>5*CONV_TOL)
        add_last_node(1);
    }
    else if (pastts==3) //product detected by bonding
    {
      add_last_node(1);
    }

    printf("\n SSM run, growth phase over \n");
    if (V_profile[nnR-1]>prodelim)
    {
      printf("\n gopt_iter: Last node high energy %3.1f / %3.1f tgrads: %3i  -exit early- \n",V_profile[nnR-1],prodelim,gradJobCount);
      printf(" V_profile:");
      for (int i=0;i<nnR;i++)
        printf(" %2.1f",V_profile[i]);
      printf("\n");

      print_em(nnR); 
      printf("\n");

      printf(" creating final string file  \n");
      nstr = StringTools::int2str(runNum,4,"0");
      string strfile = "stringfile.xyz"+nstr;
      print_string(nnR,allcoords,strfile);
      exit(1);
    }
    else
    {
      printf(" running string up to node: %i from %i \n",nnR,n0);
      nnmax = nnR;
      for (int n=1;n<nnmax-1;n++)
        active[n] = 1;
      for (int n=1;n<nnmax-1;n++)
        icoords[n].OPTTHRESH = CONV_TOL;
    }
  }


  return;
}

void GString::print_em(int nmaxp)
{
  int nstates = icoords[0].grad1.nstates;
  double E0 = icoords[0].grad1.E[0];
  if (nstates>1)
  for (int j=0;j<nstates;j++)
  {
    printf(" Em[%i]:",j+1);
    for (int i=0;i<nmaxp;i++)
      printf(" %2.1f",icoords[i].grad1.E[j]-E0);
    printf("\n");
  }

  return;
}
/**Optimization driver for DE and SE during the optimization phase.
 * climber=1 uses climbing image, 
 *  finder=1 finds the exact TS
 */
void GString::opt_iters(int max_iter, double& totalgrad, double& gradrms, double endenergy, string strfileg, int& tscontinue, double gaddmax, int osteps, int oesteps, double** dqa, double* dqmaga, double** ictan, int finder, int climber, int do_tp, int& tp)
{
  double rn3m6 = sqrt(3*natoms-6);
  int nmax;
  int nnmaxp = nnmax;
  double emax;
  double emaxp = -10000.;
  double emin = 0.;
  double overlap;
  int overlapn;
  string nstr;
  nstr = StringTools::int2str(runNum,4,"0");
  int flat = 0;
  int nhessreset = 0;
  int hessrcount = 0;
  int nclimb = 0;
  int peakc = 0;
  growing = 0;
  int isDiss = 0;
  int dfailed = 0;
  endearly = 0;

//  for (int i=0;i<nnmax;i++) icoords[i].grad1.always_do_exact = 1;
//  for (int n=0;n<nnmax0;n++) icoords[n].grad1.knnr_active = 1;

  for (;oi<max_iter;oi++)
  {
    get_tangents_1e(dqa,dqmaga,ictan);
    opt_steps(dqa,ictan,osteps,oesteps);

    totalgrad = 0.;
    gradrms = 0.;
    for (int i=n0+1;i<nnmax-1;i++)
    {
      totalgrad += icoords[i].gradrms*rn3m6;
      gradrms += icoords[i].gradrms*icoords[i].gradrms;
    }
    gradrms = sqrt(gradrms/(nnmax-2-n0));
    emaxp = emax;
    emax = -10000;
    nmax = 1;
    for (int i=n0+1;i<nnmax-1;i++)
    {
      if (V_profile[i]>emax)
      {
        emax = V_profile[i];
        nmax = i;
      }
    }
    overlapn = icoords[TSnode0].path_overlap_n;
    overlap = icoords[TSnode0].path_overlap;
    printf("\n");
    if (climb && !find) printf("c");
    if (find) printf("x");
    printf(" opt_iter: %2i totalgrad: %1.3f gradrms: %1.4f tgrads: %i",oi+1,totalgrad,gradrms,gradJobCount);
    printf(" ol(%i): %1.2f max E: %1.1f",overlapn,overlap,emax-emin);
    if (nsplit) printf(" s");
    printf(" \n");
    if (emax > endenergy && emax!=111.1) //don't end due to scf failure
    {
      printf(" considering early termination, emax/limit: %1.1f %1.1f \n",emax,endenergy);
      if (oi>50 || (oi>25 && totalgrad > 1.5) || (oi>25 && emax > 2*endenergy))
      {
        printf(" Exiting early (Opt Phase! \n");
        tscontinue = 0;
        endearly = 1;
        tp = 0;
        break;
      }
    }

    int fp = find_peaks(2);

    int added = 0;
    if (isSSM && nmax==nnmax-2 && (find || totalgrad < 0.2) && fp==1) //was fp>0
    {
      printf("\n TS node is second to last node, adding one more node \n");
      add_last_node(1);
      nnmax = nnR;
      active[nnmax-2] = active[nnmax-1] = 1;
      icoords[nnmax-1].OPTTHRESH = CONV_TOL;
      added = 1;
      printf("\n");
    }
    if (isSSM && (V_profile[n0]>V_profile[n0+1] || V_profile[n0]>emax) && n0>0)
    {
      printf(" first opt'd node is high energy, unfreezing \n");
      added = 1;
      active[n0] = 1;
      n0--;
    }

//CPMZ opt settings
    if (totalgrad < 0.3 && fp>0 && !added)
    {
//previous settings: tg 0.2 and tg 0.1
      //printf(" totalgrad<0.3 climber: %i climb: %i \n",climber,climb);
#if 0
      if (climb && !find && finder && fabs(emax-emaxp)<4. && nclimb<1 &&
          ((totalgrad < 0.25 && icoords[nmax].gradrms<CONV_TOL*10. && fabs(icoords[nmax].gradq[icoords[nmax].nicd0-1])<0.01)
        || (totalgrad < 0.15 && icoords[nmax].gradrms<CONV_TOL*10. && fabs(icoords[nmax].gradq[icoords[nmax].nicd0-1])<0.02)
        || (icoords[nmax].gradrms<CONV_TOL*2.)))
#endif
      if (climb && !find && finder && fabs(emax-emaxp)<4. && nclimb<1 &&
          ((totalgrad < 0.2 && icoords[nmax].gradrms<CONV_TOL*10. && fabs(icoords[nmax].gradq[icoords[nmax].nicd0-1])<0.01)
        || (totalgrad < 0.1 && icoords[nmax].gradrms<CONV_TOL*10. && fabs(icoords[nmax].gradq[icoords[nmax].nicd0-1])<0.02) //was 0.03
        || (icoords[nmax].gradrms<CONV_TOL*5.)))
      {
        printf(" ** starting exact TS search at node %i ** \n",nmax);
        printf(" totalgrad: %5.4f gradrms: %5.4f gts: %5.4f \n",totalgrad,icoords[nmax].gradrms,icoords[nmax].gradq[icoords[nmax].nicd0-1]);
        TSnode0 = nmax;
        for (int n=1;n<nnmax-1;n++)
          icoords[n].isTSnode = 0;
        icoords[nmax].isTSnode = 1;
        //icoords[nmax].make_Hint(); //new
        get_eigenv_finite(nmax,ictan); //also resets Ut
#if USE_DAVID
        if (!dfailed)
        {
          icoords[nmax].davidson_H(2);
          icoords[nmax].newHess = 3; 
          if (icoords[nmax].nneg==-1) dfailed = 1;
        }
#endif
        nhessreset = 10;
        hessrcount = 0;
        gradJobCount += icoords[nmax].noptdone;
        //get_eigenv_bofill();
        find = 1;
        //osteps = 0;
      }
      if (climb) nclimb--;
      if (!climb && climber)
      {
        printf(" ** starting climb ** \n");
        climb = 1;
      }
      for (int n1=1;n1<nnmax-1;n1++) icoords[n1].OPTTHRESH=CONV_TOL*2;
      icoords[TSnode0].OPTTHRESH = CONV_TOL;
    } //if totalg < 0.3
    if (find && icoords[TSnode0].nneg > 3 && icoords[TSnode0].gradrms > CONV_TOL && isRestart!=3)
    {
      if (hessrcount < 1)
      {
       //should make sure TSnode is still the max node
        printf(" resetting TS node coords Ut (and Hessian) \n");
        //icoords[TSnode0].make_Hint(); //CPMZ new
        get_eigenv_finite(TSnode0,ictan); //also resets Ut
#if USE_DAVID
        if (!dfailed)
        {
          icoords[TSnode0].davidson_H(2);
          //icoords[TSnode0].newHess = 3; 
          if (icoords[TSnode0].nneg==-1) dfailed = 1;
        }
#endif
        nhessreset = 10;
        hessrcount = 1;
        gradJobCount += icoords[nmax].noptdone;
      }
      else
      {
        printf(" Hessian consistently bad, going back to climb \n");
        find = 0;
        nclimb = 3;
      }
    }
    else if (find && icoords[TSnode0].nneg <= 3)
      hessrcount--;
    if (find && nhessreset <= 0 && !dfailed)
    {
#if USE_DAVID
      printf(" updating Hessian after 10 cycles \n");
      icoords[TSnode0].davidson_H(2);
      icoords[TSnode0].newHess = 3; 
      if (icoords[TSnode0].nneg==-1) dfailed = 1;
      nhessreset = 10;
      gradJobCount += icoords[TSnode0].noptdone;
#endif
    }
    if (find && nhessreset <= 0 && dfailed) dfailed = 0;
    nhessreset--;


    string strfile = "stringfile.xyz"+nstr;
    printf(" printing string to %s \n",strfile.c_str());
    print_string(nnmax,allcoords,strfile);
#if 0
    string ois = StringTools::int2str(oi,2,"0");
    strfile = "stringfile.xyz"+nstr+"_"+ois+".xyz";
#if USE_PRIMA
    print_string_clump_p(nnmax,totalgrad,allcoords,strfile);
#else
    print_string_clump(nnmax,totalgrad,allcoords,strfile);
#endif
#endif

   //standard GSM convergence criteria
    if (!isSSM)
    {
      if (icoords[TSnode0].gradrms < CONV_TOL && emax<V_profile[TSnode0]+0.01) { tscontinue = 0; break; } //adjustable parameter
      if (totalgrad < 0.1 && icoords[TSnode0].gradrms < 2.5*CONV_TOL && emaxp + 0.02 > emax && emaxp - 0.02 < emax) { tscontinue = 0; break; } //adjustable parameter
      if (!climber && !finder && totalgrad<0.025) { tscontinue = 0; break; } //end even if not TS search
    }
    else if (isSSM && !added)
    {
      if (fp==-1) //total string is uphill
      {
        printf(" fp == -1, check V_profile \n");
        fp = 0;
      }
      if (fp==-2)
      {
        printf(" terminating due to dissociation \n");
        tscontinue = 0;
        endearly = 1;
        isDiss = 1;
        break;
      }
      if (fp==0) //segment of string is uphill
      {
        int done = 0;
        if (tstype==2)
        {
          printf("  checking for total dissociation \n");
          double dist1 = icoords[nnmax-1].distance(brk[0],brk[1]);
          printf("   current distance: %8.5f \n",dist1);
          if (dist1>10.) //CPMZ parameter
            done = 1;
          if (done && gradrms < CONV_TOL)
          {
            printf(" TS_FINAL_TYPE 2 convergence reached \n");
            tscontinue = 0;
            endearly = 1;
            isDiss = 1;
            break;
          }
        }
        if (!done || tstype!=2) 
        {
          printf(" flatland! setting new start node to: %i \n",nnR-1);
          flat = 1;
          find = 0; climb = 0;
          tscontinue = 2;
          n0 = nnR-1; //CPMZ adjust me
          if (n0<0) n0 = 0;
          nnmax = nnmax0;
          break;
        }
      }
      if (climb && fp>0)
      {
        fp = find_peaks(4);
        if (fp>1) peakc++;
        else peakc = 0;
        int wts,wint;
        int rxnocc = 0;
        if (peakc>1)
          rxnocc = check_for_reaction(wts,wint);
        if (peakc>1 && rxnocc && wint<nnmax-1)
        {
          printf("\n more than one TS! truncating string \n");
          //trim_string(); //finds int after first TS
          trim_string(wint);
          n0 = find_uphill(3.0) - 1; //go to first node X kcal/mol above start
          if (n0<0 || nnR < 6) n0 = 0;
          climb = 0;
          find = 0;
          printf(" new n0: %i \n",n0);
        }
      }
      if (find && tstype!=2 && icoords[TSnode0].gradrms < CONV_TOL && emax<V_profile[TSnode0]+0.01) { tscontinue = 0; break; } //adjustable parameter
      if (find && tstype!=2 && totalgrad < 0.1 && icoords[TSnode0].gradrms < 2.5*CONV_TOL && emaxp + 0.02 > emax && emaxp - 0.02 < emax) { tscontinue = 0; break; } //adjustable parameter
      if (!climber && !finder && tstype!=2 && totalgrad<0.05) { tscontinue = 0; break; } //end even if not TS search
    }
    

    int min = 0;
#if 0
#if SPLIT_STRING
    tp = 0;
    min = find_ints();
    if (!min)
#endif
    tp = twin_peaks();
    if (tp>1 && do_tp)
    {
      printf(" WARNING: double step detected \n");
      if (gradrms<0.01)
      {
        printf(" Exiting early (Twin Peaks)! \n");
        tscontinue = 0;
        endearly = 1;
        break;
      }
    }
    else if (tp<1)
      printf(" WARNING: no TS detected \n");
#endif

    if (oi!=max_iter-1)
    {
      if (min && nsplit<1)
        ic_reparam_cut(min,dqa,dqmaga,0);
      else
        ic_reparam(dqa,dqmaga,0);
    }
    //printf(" oile"); fflush(stdout);
  } //loop over string iterations

  if (oi>=max_iter) tscontinue = 0;

#if 0
  if (oi<max_iter)
  {
    printf("\n\n computing exact energies \n");
    for (int n=1;n<nnmaxp;n++)
    {
      icoords[n].grad1.write_on = 0;
      V_profile[n] = icoords[n].grad1.grads(icoords[n].coords,grads[n],icoords[n].Ut,3) - V0;
      icoords[n].grad1.write_on = 1;
      gradJobCount++;
    }
  }
#endif

  if (tscontinue==0 && oi<max_iter && !isDiss && !endearly)
  {   
    int wts,wint;
    int rxnocc = check_for_reaction(wts,wint);
    if (!rxnocc && tstype==1 && isSSM)
    {
      printf("\n WARNING: no bond changes in reaction \n");
      climb = find = 0;
      n0 = nnR - 1;
      if (n0<0) n0 = 0;
      tscontinue = 2;
      nnmax = nnmax0;
      printf("  will restart growth at node: %i \n",n0);
     //CPMZ might want to check active nodes
    }
    if (rxnocc)
    {
      double emin1 = 10000.;
      int nmin1 = wts;
      for (int i=wts;i<nnmax;i++)
      {
        if (V_profile[i]<emin1)
        {
          emin1 = V_profile[i];
          nmin1 = i;
        }
      }
      if (nmin1<nnmax-1 && isSSM)
      {
        printf(" WARNING: string goes up in energy at end \n");
        printf("  setting final node to %i \n",nmin1);
        nnmax = nmin1+1;
      }
    }
  }
  if (tscontinue==0 && oi<max_iter && !isDiss && !endearly)
  {
    double emax1 = -10000;
    int nmax1 = 1;
    for (int i=1;i<nnmax-1;i++)
    {
      if (V_profile[i]>emax1)
      {
        emax1 = V_profile[i];
        nmax1 = i;
      }
    }
    if (nmax1!=TSnode0 && climber && finder)
    {
      printf("\n need to restart opt iters, TS node is not max E \n");
      tscontinue = 1;
      find = 0;
      if (isSSM)
      {
        if (nmax1>TSnode0)
          n0 = TSnode0;
        else
        {
          n0 = 0;
          //n0 = find_uphill(3.);
          if (n0<0) n0 = 0;
          nnmax = TSnode0;
        }
        printf(" opting node range: %i to %i \n",n0,nnmax);
      }
    } // if mismatched TS node
  } //if converged

  if (tscontinue!=0)
    printf("\n opt_iters over: totalgrad: %5.3f gradrms: %5.4f tgrads: %4i  ol(%i): %3.2f max E: %4.1f Erxn: %4.1f nmax: %i TSnode: %i ",totalgrad,gradrms,gradJobCount,overlapn,overlap,emax-emin,V_profile[nnmax-1],nmax,TSnode0);
  if (flat) printf(" -FL-");
  if (tscontinue) printf("\n");


  killcounter++;
  if (killcounter>1000) exit(1);

  return;
}


double GString::get_ssm_dqmag(double bdist)
{
  double dqmag = 0.;

  double minmax = (DQMAG_SSM_MAX - DQMAG_SSM_MIN);
  double a = bdist/DQMAG_SSM_SCALE;
  if (a > 1.)
    a = 1.;
  dqmag = DQMAG_SSM_MIN + minmax*a;

  if (dqmag < DQMAG_SSM_MIN)
    dqmag = DQMAG_SSM_MIN;

#if 0
   dqmag = DQMAG_SSM * (nadd+nbrk+nangle+ntors);
   if (dqmag > DQMAG_SSM_MAX)
     dqmag = DQMAG_SSM_MAX;
#endif

  printf(" dqmag: %4.3f from bdist: %4.3f \n",dqmag,bdist);

  return dqmag;
}

int GString::break_planes_ssm(ICoord ic1)
{
#if !DRIVE_ADD_TETRA
  printf("  not using break_planes \n");
  return 0;
#endif

  printf("\n in break_planes_ssm for nadd: %i \n",nadd);

  int newtor = 0;
  using_break_planes = 0;

  double TORFLAT = 15.;
  double TORFLATC = 180 - TORFLAT;
  double TORTARGET = 30.; //40. caused some issues
  double TORTARGETC = 180. - TORTARGET;

  int nb = 0;
  int* b1 = new int[12];
  int* b2 = new int[12];
  for (int i=0;i<nadd;i++)
  {
    int a1 = add[2*i+0];
    int a2 = add[2*i+1];

    printf("  add: %i %i \n",a1+1,a2+1);

    int nbf1 = 0;
    for (int j=0;j<natoms;j++)
    if (j!=a1 && j!=a2)
    if (ic1.bond_exists(a1,j))
      b1[nbf1++] = j;
    int nbf2 = 0;
    for (int j=0;j<natoms;j++)
    if (j!=a1 && j!=a2)
    if (ic1.bond_exists(a2,j))
      b2[nbf2++] = j;

#if 1
    printf("   printing bonds to a1(%i):",a1+1);
    for (int i=0;i<nbf1;i++)
      printf(" %i",b1[i]+1);
    printf("\n");
    printf("   printing bonds to a2(%i):",a2+1);
    for (int i=0;i<nbf2;i++)
      printf(" %i",b2[i]+1);
    printf("\n");
#endif

#if 0
    double b1tor = 0;
    if (nbf1>2)
      b1tor = ic1.torsion_val(b1[0],a1,b1[1],b1[2]);
    if (nbf1>2 && (fabs(b1tor)>TORFLATC || fabs(b1tor)<TORFLAT))
    {
      printf("  breaking planarity at atom %i \n",a1+1);
      tors[4*ntors+0] = b1[0];
      tors[4*ntors+1] = a1; 
      tors[4*ntors+2] = b1[1];
      tors[4*ntors+3] = b1[2];
      if (fabs(b1tor)>TORFLATC)
        tort[ntors++] = sign(b1tor)*TORTARGETC;
      else
        tort[ntors++] = sign(b1tor)*TORTARGET;

      newtor++;
    }
    double b2tor = 0;
    if (nbf2>2)
      b2tor = ic1.torsion_val(b2[0],a2,b2[1],b2[2]);
    if (nbf2>2 && (fabs(b2tor)>TORFLATC || fabs(b2tor)<TORFLAT))
    {
      printf("  breaking planarity at atom %i \n",a2+1);
      tors[4*ntors+0] = b2[0];
      tors[4*ntors+1] = a2; 
      tors[4*ntors+2] = b2[1];
      tors[4*ntors+3] = b2[2];
      if (fabs(b2tor)>TORFLATC)
        tort[ntors++] = sign(b2tor)*TORTARGETC;
      else
        tort[ntors++] = sign(b2tor)*TORTARGET;

      newtor++;
    }
#endif


//CPMZ note: need to determine b2[0] b2[1] ordering via
// angle furthest from 180.
    if (nbf2>1)
    {
      double b12tor = ic1.torsion_val(a1,a2,b2[0],b2[1]);
      if (fabs(b12tor)>TORFLATC || fabs(b12tor)<TORFLAT)
      {
        printf("  breaking planarity from %i to %i \n",a1+1,a2+1);
        tors[4*ntors+0] = a1;
        tors[4*ntors+1] = a2; 
        tors[4*ntors+2] = b2[0];
        tors[4*ntors+3] = b2[1];
        if (fabs(b12tor)>TORFLATC)
          tort[ntors++] = sign(b12tor)*TORTARGETC;
        else
          tort[ntors++] = sign(b12tor)*TORTARGET;

        newtor++;
      }
    }
    if (nbf1>1)
    {
      double b21tor = ic1.torsion_val(a2,a1,b1[0],b1[1]);
      if (fabs(b21tor)>TORFLATC || fabs(b21tor)<TORFLAT)
      {
        printf("  breaking planarity from %i to %i \n",a2+1,a1+1);
        tors[4*ntors+0] = a2;
        tors[4*ntors+1] = a1; 
        tors[4*ntors+2] = b1[0];
        tors[4*ntors+3] = b1[1];
        if (fabs(b21tor)>TORFLATC)
          tort[ntors++] = sign(b21tor)*TORTARGETC;
        else
          tort[ntors++] = sign(b21tor)*TORTARGET;

        newtor++;
      }
    }

  }

  delete [] b1;
  delete [] b2;

  if (newtor) using_break_planes = 1;

  return 0;
}


int GString::add_linear()
{
  printf("\n in add_linear \n");

  int nnewangle = 0;
  int nbonds = newic.nbonds;
  int nangles = newic.nangles;
  int* addangles = new int[3];

  for (int i=0;i<nadd;i++)
  {
    int a1 = add[2*i+0];
    int a2 = add[2*i+1];
    int wbond = newic.bond_num(a1,a2);

    int found = 0;
    int an1 = -1;
    int mid;
    for (int j=0;j<natoms;j++)
    if (j!=a1 && j!=a2)
    if (newic.angle_val(a1,j,a2)>165.)
    {
      //printf(" anglev>165. %i %i %i \n",newic.angles[j][0],newic.angles[j][1],newic.angles[j][2]);
      printf(" angle_val>165. %i %i %i \n",a1,j,a2);
      found = 1;
      mid = j;
      break;
    }
    if (found)
    {
      printf(" found possible linear collapse angle: %i %i %i (%4.3f) \n",a1,mid,a2,newic.angle_val(a1,mid,a2));
      found = 0;
      for (int j=0;j<nangle;j++)
      {
        if (angles[3*j+0] == a1 && angles[3*j+1] == mid && angles[3*j+2] == a2) 
        { found = 1; break; }
        if (angles[3*j+0] == a2 && angles[3*j+1] == mid && angles[3*j+2] == a1) 
        { found = 1; break; }
      }
      if (!found)
      {
        angles[3*nangle+0] = a1;
        angles[3*nangle+1] = mid;
        angles[3*nangle+2] = a2;
        anglet[nangle] = 100.;
        nangle++;

        printf(" found linear collapse angle: %i %i %i (%4.3f) \n",a1,mid,a2,newic.angle_val(a1,mid,a2));

        if (newic.angle_num(a1,mid,a2)==-1)
        {
          printf(" angle not in coordinates! \n");
          addangles[0] = a1;
          addangles[1] = mid;
          addangles[2] = a2;
          add_angles(1,addangles);
          nnewangle++;
        }
      }
    } //if found
 
  } //loop i over nadd

  delete [] addangles;

  printf("\n");

  return 0;
}

void GString::add_last_node(int type)
{
  if (nnR>=nnmax0) 
  {
    printf(" too many nodes, cannot add (%i/%i) \n",nnR,nnmax0);
    return;
  }
  if (tstype==2 && type==1)
  {
    printf(" TS_FINAL_TYPE is 2, not adding last node \n");
    return;
  }
  int noptsteps = 15;
  int size_ic = icoords[nnR-1].nbonds + icoords[nnR-1].nangles + icoords[nnR-1].ntor + icoords[nnR-1].nxyzic;
  icoords[nnR].OPTTHRESH = CONV_TOL;
  if (type==1)
  {
    printf(" copying last node, opting \n");
    icoords[nnR].reset(natoms,anames,anumbers,icoords[nnR-1].coords);
    icoords[nnR].bmatp_create();
    icoords[nnR].bmatp_to_U();
    icoords[nnR].bmat_create();
    for (int i=0;i<size_ic*size_ic;i++)
      icoords[nnR].Hintp[i] = icoords[nnR-1].Hintp[i];
  }
  else if (type==2)
  {
    printf(" already created node, opting \n");
  }
  string nstr = StringTools::int2str(runNum,4,"0");
  icoords[nnR].grad1.update_knnr();
  V_profile[nnR] = icoords[nnR].opt_b("scratch/intopt"+nstr+".xyz",noptsteps);
  gradJobCount += icoords[nnR].noptdone;
  
  printf(" %s",icoords[nnR].printout.c_str()); 
  
  int samegeom = 1;
  for (int i=0;i<3*natoms;i++)
  if (icoords[nnR].coords[i] != icoords[nnR-1].coords[i])
  {
    samegeom = 0;
    break;
  }

  if (samegeom)
  {
    printf(" opt did not produce new geometry \n");
  }
  else
    nnR++;


  return;
}


void GString::trim_string()
{
  int maxn = nnmax;
  int npeaks1 = 0; 
  int* min = new int[maxn];
  int* max = new int[maxn];
  for (int n=0;n<maxn;n++) min[n] = 0;
  for (int n=0;n<maxn;n++) max[n] = 0;
  for (int n=1;n<maxn-1;n++)
  {
    if (V_profile[n+1] > V_profile[n])
    {
      if (V_profile[n] < V_profile[n-1])
        min[n] = 1;
    }
    if (V_profile[n+1] < V_profile[n])
    {
      if (V_profile[n] > V_profile[n-1])
        max[n] = 1;
    }
  }

#if 1
  printf(" min nodes: ");
  for (int n=1;n<maxn-1;n++)
  if (min[n])
    printf(" %i",n);
  printf(" max nodes: ");
  for (int n=1;n<maxn-1;n++)
  if (max[n])
    printf(" %i",n);
  printf("\n");
#endif

  for (int n=1;n<maxn-1;n++)
  if (max[n])
    npeaks1++;

  double ediff = PEAK4_EDIFF; //same as in find_peaks(4)
  double emax = -1000.;
  int nmax = 0;
  int found = 0;
  if (npeaks1)
  for (int n=1;n<maxn-1;n++)
  if (max[n] && !found)
  {
    emax = V_profile[n];
    nmax = n;
    printf(" at max node: %i \n",n);
    for (int m=n+1;m<maxn;m++)
    {
      printf(" V[n]: %4.3f V[m]: %4.3f \n",V_profile[n],V_profile[m]);
      if (emax - V_profile[m] > ediff)
      {
        found = m;
        break;
      }
//      if (max[m] && V_profile[m] > emax) break;
      if (max[m]) break;
    } //loop over m
  } //loop over n, if max[n]

  int nextmin = nmax;
  for (int n=found;n<maxn;n++)
  if (min[n])
  {
    nextmin = n;
    break;
  }

  if (nmax==nextmin)
  {
    printf(" couldn't find next min mode, something is wrong \n");
    printf(" nmax: %i nextmin: %i found: %i \n",nmax,nextmin,found);
    //exit(1);
  }
  else
    trim_string(nextmin);

  delete [] min;
  delete [] max;

  return;
}

void GString::trim_string(int nextmin)
{
  printf(" running string up to nnmax: %i \n",nextmin+1);
  nnmax = nextmin+1;
  nnR = nnmax;
  TSnode0 = 0;
  find = 0;

  return;
}
