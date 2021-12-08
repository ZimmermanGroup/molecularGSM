#include "icoord.h"
#include "utils.h"
#include <mkl.h>
//#include "/export/apps/Intel/Compiler/11.1/075/mkl/include/mkl.h"
//#include "/opt/acml5.3.1/gfortran64/include/acml.h"

using namespace std;

#define HESS_TANG 1
//0.7 0.5 works fine
#define HESS_TANG_TOL 0.75
#define HESS_TANG_TOL_TS 0.35
#define NEARCONVTS 0
#define RIBBONS 0
#define RIDGE_STEP_SIZE 0.15
#define MIN_NEG_STEP 0.00005
#define USE_PRIMA 0
#define USE_NOTBONDS 0
#define WRITE_HESS 0

#define STEPCONTROL 1
#define STEPCONTROLG 0
#define THRESH 1E-3

int ICoord::bmat_alloc() {

  int size_ic = nbonds+nangles+ntor+150; //buffer of 150 for new primitives
  int size_xyz = 3*natoms;
  //printf(" in bmat_alloc, size_ic: %i size_xyz: %i \n",size_ic-150,size_xyz);
  //printf(" max_bonds: %i max_angles: %i max_torsions: %i \n",max_bonds,max_angles,max_torsions);
  bmat = new double[size_ic*size_xyz+100];
  bmatp = new double[size_ic*size_xyz+1000];
  bmatti = new double[size_ic*size_xyz+100];
  torv0 = new double[max_torsions+100];
  for (int i=0;i<max_torsions;i++) torv0[i] = 0;
  torfix = new double[max_torsions+100];
  Ut = new double[size_ic*size_ic+100];
  Ut0 = new double[size_ic*size_ic+100];
  q = new double[size_ic+100];
  Hint = new double[size_ic*size_ic+100];
  Hintp = new double[size_ic*size_ic+100];
  Hinv = new double[size_ic*size_ic+100];
  Gmh = new double[size_xyz*size_xyz];
  Gmih = new double[size_xyz*size_xyz];
  dq0 = new double[size_ic+100];
  dqm1 = new double[size_ic+100];
  dqprim = new double[size_ic+100];
  pgradq = new double[size_ic+100];
  gradq = new double[size_ic+100];
  for (int i=0;i<size_ic+100;i++) pgradq[i]=0.;
  for (int i=0;i<size_ic+100;i++) gradq[i]=0.;
  pgradqprim = new double[size_ic+100];
  gradqprim = new double[size_ic+100];
  for (int i=0;i<size_ic+100;i++) pgradqprim[i]=0.;
  for (int i=0;i<size_ic+100;i++) gradqprim[i]=0.;
  OPTTHRESH = 0.0005;
  revertOpt = 1;
  useExactH = 0;
 
  MAXAD = 0.075; //max along one coordinate (was using 0.1)
  DMAX = 0.1; //max of step magnitude (was using 0.125)
  DMIN0 = DMAX/5.; //was 5.
#if USE_PRIMA
  DMAX = 0.025;
#endif

  SCALEQN0 = 1.0;
  SCALEQN = SCALEQN0;
  ixflag = 0;
  isTSnode = 0;
  optCG = 1;
  do_bfgs = 0;
  ridge = 0;
  path_overlap = 0.;
  path_overlap_n = 0;
  path_overlap_e_g = 0.;
  noptdone = 0;
  nneg = 0;
  isDavid = 0;

  FMAG = 0.015; //was 0.015

  use_constraint = 1;
  stage1opt = 0;
  sbuff = new char[350];
  g_inited = 0;
  V0 = 0.;

#if USE_NOTBONDS
  mm_init();
#endif


  return 0;
}

int ICoord::grad_init(string infilename, int ncpu, int run, int rune, int knnr_level, int q1) 
{
  runends = StringTools::int2str(run,4,"0");
  runend2 = StringTools::int2str(run,4,"0")+"."+StringTools::int2str(rune,4,"0");
  grad1.init(infilename,natoms,anumbers,anames,coords,run,rune,ncpu,knnr_level,q1);
  g_inited = 1;
  //printf(" knnr_level: %i \n",knnr_level);

  return 0;
}



int ICoord::bmat_free() {

  //int size_ic = nbonds+nangles+ntor;
  //int size_xyz = 3*natoms;
  //printf(" in bmat_free, size_ic: %i size_xyz: %i \n",size_ic,size_xyz);
  delete [] bmat;
  delete [] bmatp;
  delete [] bmatti;
  delete [] torv0;
  delete [] torfix;
  delete [] Ut;
  delete [] Ut0;
  delete [] q;
  delete [] Hint;
  delete [] Hinv;
  delete [] dq0;
  delete [] dqm1;
  delete [] sbuff;

  return 0;
}

///forms the original B matrix
int ICoord::bmatp_create() {

 // printf(" in bmatp_create \n");

  int len = nbonds+nangles+ntor;
  int N3 = 3*natoms;
  int max_size_ic = len;
  int size_xyz = N3;

  for (int i=0;i<max_size_ic*N3;i++)
    bmatp[i] = 0.;

  double* dqbdx = new double[6];
  for (int i=0;i<nbonds;i++)
  {
    for (int j=0;j<6;j++) dqbdx[j] = 0.;
    int a1=bonds[i][0];
    int a2=bonds[i][1];
    bmatp_dqbdx(a1,a2,dqbdx);
    double FACTOR = 1;
    bmatp[i*N3+3*a1+0] = dqbdx[0]*FACTOR;
    bmatp[i*N3+3*a1+1] = dqbdx[1]*FACTOR;
    bmatp[i*N3+3*a1+2] = dqbdx[2]*FACTOR;
    bmatp[i*N3+3*a2+0] = dqbdx[3]*FACTOR;
    bmatp[i*N3+3*a2+1] = dqbdx[4]*FACTOR;
    bmatp[i*N3+3*a2+2] = dqbdx[5]*FACTOR;
  }

  double* dqadx = new double[9];
  for (int i=nbonds;i<nbonds+nangles;i++)
  {
    for (int j=0;j<9;j++) dqadx[j] = 0.;
    int a1=angles[i-nbonds][0];
    int a2=angles[i-nbonds][1];
    int a3=angles[i-nbonds][2];
    bmatp_dqadx(a1,a2,a3,dqadx);
    bmatp[i*N3+3*a1+0] = dqadx[0];
    bmatp[i*N3+3*a1+1] = dqadx[1];
    bmatp[i*N3+3*a1+2] = dqadx[2];
    bmatp[i*N3+3*a2+0] = dqadx[3];
    bmatp[i*N3+3*a2+1] = dqadx[4];
    bmatp[i*N3+3*a2+2] = dqadx[5];
    bmatp[i*N3+3*a3+0] = dqadx[6];
    bmatp[i*N3+3*a3+1] = dqadx[7];
    bmatp[i*N3+3*a3+2] = dqadx[8];
  }
  double* dqtdx = new double[12];
  for (int i=nbonds+nangles;i<nbonds+nangles+ntor;i++)
  {
    for (int j=0;j<12;j++) dqtdx[j] = 0.;
    int a1=torsions[i-nbonds-nangles][0];
    int a2=torsions[i-nbonds-nangles][1];
    int a3=torsions[i-nbonds-nangles][2];
    int a4=torsions[i-nbonds-nangles][3];
    bmatp_dqtdx(a1,a2,a3,a4,dqtdx);
    bmatp[i*N3+3*a1+0] = dqtdx[0]; //*1.8897
    bmatp[i*N3+3*a1+1] = dqtdx[1];
    bmatp[i*N3+3*a1+2] = dqtdx[2];
    bmatp[i*N3+3*a2+0] = dqtdx[3];
    bmatp[i*N3+3*a2+1] = dqtdx[4];
    bmatp[i*N3+3*a2+2] = dqtdx[5];
    bmatp[i*N3+3*a3+0] = dqtdx[6];
    bmatp[i*N3+3*a3+1] = dqtdx[7];
    bmatp[i*N3+3*a3+2] = dqtdx[8];
    bmatp[i*N3+3*a4+0] = dqtdx[9];
    bmatp[i*N3+3*a4+1] = dqtdx[10];
    bmatp[i*N3+3*a4+2] = dqtdx[11];
  }

  //printf(" \n after creating bmatp \n");

#if 0
  printf(" printing bond contributions \n");
  for (int i=0;i<nbonds;i++)
  {
    for (int j=0;j<natoms;j++)
    {
      for (int k=0;k<3;k++)
        printf(" %1.3f",bmatp[i*size_xyz+3*j+k]);
    }
    printf(" \n");
  }
#endif
#if 0
  printf(" printing angle contributions \n");
  for (int i=nbonds;i<nbonds+nangles;i++)
  {
    for (int j=0;j<natoms;j++)
    {
      for (int k=0;k<3;k++)
        printf(" %1.3f",bmatp[i*size_xyz+3*j+k]);
    }
    printf(" \n");
  }
#endif

#if 0
  int nztor = 0;
  double* x = new double[3];
  printf(" printing torsion contributions \n");
  for (int i=nbonds+nangles;i<nbonds+nangles+ntor;i++)
  {
    x[0] = x[1] = x[2] = 0.;
    for (int j=0;j<natoms;j++)
    {
//      for (int k=0;k<3;k++)
//        printf(" %7.3f",bmatp[i*size_xyz+3*j+k]);
      for (int k=0;k<3;k++) 
        x[k] += bmatp[i*N3+3*j+k]*bmatp[i*N3+3*j+k];
    }
    printf("   mag: %8.4f %8.4f %8.4f ",x[0],x[1],x[2]);
    printf(" \n");
    if (x[0]*x[0]+x[1]*x[1]+x[2]*x[2]>0.001)
      nztor++;
  }
  printf("  non-zero torsions: %2i \n",nztor);
  delete [] x;
#endif

#if 0
  double* x = new double[3];
  x[0] = x[1] = x[2] = 0.;
  for (int i=0;i<len;i++)
  {
    for (int j=0;j<natoms;j++)
    for (int k=0;k<3;k++)
      x[k] += bmatp[i*N3+3*j+k];
    printf(" xyz components for IC %i: %8.5f %8.5f %8.5f \n",i,x[0],x[1],x[2]);
    x[0]=x[1]=x[2] = 0.;
  }
#endif
#if 0
  double* x1 = new double[N3];
  for (int j=0;j<N3;j++) x1[j] = 0.;
  for (int i=0;i<len;i++)
  for (int j=0;j<N3;j++)
    x1[j] += bmatp[i*N3+j]*bmatp[i*N3+j];
  printf("\n printing bmatp magnitudes over xyz: \n");
  for (int j=0;j<natoms;j++)
    printf(" %2s %8.4f %8.4f %8.4f \n",anames[j].c_str(),x1[3*j+0],x1[3*j+1],x1[3*j+2]);
  printf("\n");
  delete [] x1;
#endif


  delete [] dqbdx;
  delete [] dqadx;
  delete [] dqtdx;

  return 0;
}

///Diagonalizes G to form U
int ICoord::bmatp_to_U()
{
 // printf(" in bmatp_to_U \n");
 // fflush(stdout);
  int len = nbonds+nangles+ntor;
  int N3 = 3*natoms;
  int max_size_ic = len;
  int size_xyz = N3;
  //printf(" bmatp_to_U. nbonds: %2i nangles: %2i ntorsions: %2i  total: %3i \n",nbonds,nangles,ntor,len);

  int len_d;
  double* e = new double[len];
//  double* U = new double[len*len];
  double* tmp = new double[len*N3];
  for (int i=0;i<len*N3;i++)
    tmp[i] = bmatp[i];

  double* G = new double[len*len];
#if 1
  mat_times_mat_bt(G,bmatp,bmatp,len,len,N3);
#else
  for (int i=0;i<len*len;i++) G[i] = 0.;
  for (int i=0;i<len;i++) 
  for (int j=0;j<len;j++)
  for (int k=0;k<N3;k++)
    G[i*len+j] += bmatp[i*N3+k]*bmatp[j*N3+k];
#endif

#if 0
  printf(" G: \n");
  for (int i=0;i<len;i++)
  {
    for (int j=0;j<len;j++)
      printf(" %1.2f",G[i*len+j]);
    printf("\n");
  }
#endif

//  printf("\n using diagonalize(G) \n");
//  printf(" before diagonalize: mkl_threads: %i \n",mkl_get_max_threads());
//  fflush(stdout);
  Diagonalize(G,e,len);
  len_d = N3-6;
//  printf(" after diagonalize \n");
//  fflush(stdout);

#if 0
  printf(" eigenvalues:");
  for (int i=0;i<len;i++)
    printf(" %10.8f",e[i]);
  printf("\n");
#endif
#if 1
  int lowev = 0;
  for (int i=0;i<len_d;i++)
  if (e[len-1-i]<0.001)
  {
#if 0
    printf(" small ev: %10.8f \n",e[len-1-i]);
    int i1 = len-1-i;
  //  for (int j=0;j<len;j++)
  //    printf(" %8.5f",G[i1*len+j]);
  //  printf("\n");
#endif
    lowev++;
  }
  if (lowev>0)
    printf(" lowev: %i",lowev);
  len_d -= lowev;
  if (lowev>3)
  {
    printf("\n\n ERROR: optimization space less than 3N-6 DOF \n");
    printf("  probably need more IC's \n");
    printf("  check fragmentation or linear angles \n");
    exit(-1);
  }
#endif

  int redset = len - len_d;
  for (int i=0;i<len_d;i++)
  for (int j=0;j<len;j++)
    Ut[i*len+j] = G[(i+redset)*len+j];
  for (int i=0;i<redset;i++)
  for (int j=0;j<len;j++)
    Ut[(len_d+i)*len+j] = G[i*len+j];

#if 0
  printf(" Ut eigenvalues:");
  for (int i=0;i<len_d;i++)
    printf(" %1.4f",e[len-1-i]);
  printf(" \n");
#endif
#if 0
  if (lowev)
  {
    for (int i=0;i<nangles;i++)
    if (anglev[i]>175.)
      printf(" angle %i: %i %i %i: %1.1f \n",i+1,angles[i][0],angles[i][1],angles[i][2],anglev[i]);
    printf(" \n");
  }
#endif

#if 0
  //printf(" checking orthonormality of U vectors \n");
  trans(U,Ut,len,len);
  double dot;
  for (int i=0;i<len;i++)
  for (int j=0;j<len;j++)
  {
    dot = 0.;
    for (int k=0;k<len;k++)
      dot += U[k*len+i]*U[k*len+j];
    if (i!=j && abs(dot)>0.0001)
      printf(" WARNING: dot of %i %i: %1.3f \n",i,j,dot);
  }
#endif

#if 0
  printf(" Printing %i nonredundant (column) vectors of U \n",len_d);
  for (int i=0;i<len;i++)
  {
    for (int j=0;j<len_d;j++)
      printf(" %2.3f",U[i*len+j]);
    printf("\n");
  }
#endif

#if 0
  double* weights = new double[len];
  for (int i=0;i<len;i++) weights[i] = 0.;
  for (int i=0;i<len;i++)
  for (int j=0;j<nbonds;j++)
    weights[i] += Ut[i*len+j]*Ut[i*len+j];
  for (int i=0;i<len;i++)
    printf(" coord %i has bond weight %1.1f \n",i,weights[i]*100);
  double tweight = 0.;
  for (int i=0;i<len_d;i++)
    tweight += weights[i];
  printf(" total in nonred set: %1.3f \n",tweight*100);
 
  delete [] weights;

#endif

#if 0
//CPMZ previous
  printf("\n using SVD \n");
  //double* tmp2 = new double[len*N3];
  //trans(tmp2,tmp,N3,len);
  //SVD(tmp2,U,e,N3,len);
  SVD(tmp,U,e,len,N3);
  trans(Ut,U,len,len);
#endif

//  if (len_d>N3-6 && len_d>1) len_d = N3-6;
//  else if (len_d<N3-6 && len_d>1) len_d = N3-6;
  nicd = len_d;

  for (int i=0;i<ntor;i++)
    torv0[i] = torv[i];

  for (int i=0;i<len*len;i++)
    Ut0[i] = Ut[i];
  nicd0 = nicd;

  delete [] tmp;
  delete [] G;
  delete [] e;
  //delete [] U;

  return 0;
}


///Determines q the components of the molecular geometry in the DI space. Also forms the active B matrix.

int ICoord::bmat_create() 
{
 // printf(" in bmat_create() \n");
 // fflush(stdout);

  int len = nbonds+nangles+ntor;
  int N3 = 3*natoms;
  int max_size_ic = len;
  int size_xyz = N3;
  
  int len_d = nicd0;

  //printf(" determining q in delocalized internals \n");
  //printf(" nicd: %i \n",nicd);
  update_ic();
  for (int i=0;i<len_d;i++)
    q[i] = 0.;

#if 0
  for (int i=0;i<len_d;i++)
    for (int j=0;j<nbonds;j++)
      q[i] += Ut[len*i+j]*bondd[j];
  for (int i=0;i<len_d;i++)
    for (int j=0;j<nangles;j++)
      q[i] += Ut[len*i+nbonds+j]*anglev[j]*3.14159/180;
  for (int i=0;i<len_d;i++)
    for (int j=0;j<ntor;j++)
      q[i] += Ut[len*i+nbonds+nangles+j]*torv[j]*3.14159/180;
#endif

#if 1
  for (int i=0;i<len_d;i++)
    for (int j=0;j<nbonds;j++)
      q[i] += Ut[len*i+j]*distance(bonds[j][0],bonds[j][1]);
  for (int i=0;i<len_d;i++)
    for (int j=0;j<nangles;j++)
      q[i] += Ut[len*i+nbonds+j]*angle_val(angles[j][0],angles[j][1],angles[j][2])*3.14159/180;
  for (int j=0;j<ntor;j++) torfix[j] = 0.;
  for (int j=0;j<ntor;j++)
  {
    double tordiff = torv0[j] - torsion_val(torsions[j][0],torsions[j][1],torsions[j][2],torsions[j][3]);
    if (tordiff>180.)
      torfix[j] = 360.;
    else if (tordiff<-180)
      torfix[j] = -360.;
    else torfix[j] = 0;
//    if (abs(tordiff)>180)
//      printf(" tordiff: %1.1f, effective tor_val: %1.1f ",tordiff,torsion_val(torsions[j][0],torsions[j][1],torsions[j][2],torsions[j][3])+torfix[j]);
  }
  //printf(" torfix: "); for (int j=0;j<ntor;j++) printf(" %1.1f",torfix[j]); printf("\n");
  for (int i=0;i<len_d;i++)
    for (int j=0;j<ntor;j++)
      q[i] += Ut[len*i+nbonds+nangles+j]*(torfix[j]+torsion_val(torsions[j][0],torsions[j][1],torsions[j][2],torsions[j][3]))*3.14159/180;
#endif

#if 0
  printf(" printing q: \n");
  for (int i=0;i<len_d;i++)
    printf(" %1.4f",q[i]);
  printf(" \n");
#endif
 
#if 0
  printf(" verifying q \n");
  double* q0 = new double[len];
  for (int i=0;i<len;i++)
    q0[i] = 0.;
  for (int i=0;i<len;i++)
    for (int j=0;j<len_d;j++)
      q0[i] += Ut[j*len+i]*q[j];
#if 0
  printf(" printing q0: \n");
  for (int i=0;i<len;i++)
    printf(" %1.2f",q0[i]);
  printf(" \n");
#endif
  printf(" printing q0 vs. bonds: \n");
  for (int i=0;i<nbonds;i++)
    printf(" %1.2f",q0[i]-bondd[i]);
  printf(" \n");

  delete [] q0;
#endif

  //printf(" now making bmat in delocalized internals (len: %i len_d: %i) \n",len,len_d);
#if 1
  mat_times_mat(bmat,Ut,bmatp,len_d,N3,len);
#else
  for (int i=0;i<len_d*N3;i++) bmat[i] = 0.;
  for (int i=0;i<len_d;i++)
  for (int j=0;j<N3;j++)
  for (int k=0;k<len;k++)
    bmat[i*N3+j] += Ut[i*len+k]*bmatp[k*N3+j];
#endif

#if 0
  printf(" printing bmat in coordinates U \n");
  for (int i=0;i<len_d;i++)
  {
    for (int j=0;j<natoms;j++)
    {
      for (int k=0;k<3;k++)
        printf(" %1.3f",bmat[i*N3+3*j+k]);
    }
    printf(" \n");
  }
#endif

  double* bbt = new double[len_d*len_d];
  double* bbti = new double[len_d*len_d];

#if 1
  mat_times_mat_bt(bbt,bmat,bmat,len_d,len_d,N3);
#else
  for (int i=0;i<len_d*len_d;i++) bbt[i] = 0.;
  for (int i=0;i<len_d;i++)
  for (int j=0;j<len_d;j++)
  for (int k=0;k<N3;k++)
    bbt[i*len_d+j] += bmat[i*N3+k]*bmat[j*N3+k];
#endif

  for (int i=0;i<len_d*len_d;i++)
    bbti[i] = bbt[i];

  //need to invert bbt, then bbt-1 * bmat = bt-1
 // printf(" before invert bbti \n");
 // fflush(stdout);
  Invert(bbti,len_d);

#if 0
  //Checked inverse, it is okay only when 3N-6 vectors are present

  double* tmp2 = new double[len_d*len_d];
  for (int i=0;i<len_d*len_d;i++)
    tmp2[i] = 0.;
  for (int i=0;i<len_d;i++)
  for (int j=0;j<len_d;j++)
  for (int k=0;k<len_d;k++)
    tmp2[i*len_d+j] += bbti[i*len_d+k]*bbt[k*len_d+j];

  printf(" debug: bbti*bbt diagonals \n");
  for (int i=0;i<len_d;i++)
    printf(" %1.2f",tmp2[i*len_d+i]);
#if 0
  printf(" debug: bbti*bbt \n");
  for (int i=0;i<len_d;i++)
  {
    for (int j=0;j<len_d;j++)
      printf(" %1.3f",tmp2[i*len_d+j]);
    printf("\n");
  }
#endif
  printf("\n");
  delete [] tmp2;
#endif

 //printf(" bmatti formation \n");
#if 1
  mat_times_mat(bmatti,bbti,bmat,len_d,N3,len_d);
#else
  for (int i=0;i<len_d*N3;i++)
    bmatti[i] = 0.;
  for (int i=0;i<len_d;i++)
  for (int j=0;j<N3;j++)
  for (int k=0;k<len_d;k++)
    bmatti[i*N3+j] += bbti[i*len_d+k]*bmat[k*N3+j];
#endif

// printf(" dealloc \n");
  delete [] bbt;
  delete [] bbti;

  return 0;
}



int ICoord::bmatp_finite() {

  printf(" in bmatp_finite() \n");

  int len = nbonds+nangles+ntor;
  int N3 = 3*natoms;
  int max_size_ic = len;
  int size_xyz = N3;

  for (int i=0;i<max_size_ic*N3;i++)
    bmatp[i] = 0.;

  double fstep = 0.001;

  double* dqbdx = new double[6];
  for (int i=0;i<6;i++) dqbdx[i] = 0.;
  for (int i=0;i<nbonds;i++)
  {
    int a1=bonds[i][0];
    int a2=bonds[i][1];
    //bmatp_dqbdx(a1,a2,dqbdx);
    double b0,b1,b2;

    b0 = distance(a1,a2);
    for (int j=0;j<3;j++)
    {
      coords[3*a1+j]+=fstep;
      b1 = distance(a1,a2);
      coords[3*a1+j]-=fstep;
      coords[3*a2+j]+=fstep;
      b2 = distance(a1,a2);
      coords[3*a2+j]-=fstep;

      dqbdx[3*0+j] = (b1-b0)/fstep;
      dqbdx[3*1+j] = (b2-b0)/fstep;
    }

    bmatp[i*N3+3*a1+0] = dqbdx[0];
    bmatp[i*N3+3*a1+1] = dqbdx[1];
    bmatp[i*N3+3*a1+2] = dqbdx[2];
    bmatp[i*N3+3*a2+0] = dqbdx[3];
    bmatp[i*N3+3*a2+1] = dqbdx[4];
    bmatp[i*N3+3*a2+2] = dqbdx[5];
  }
//  delete [] dqbdx;

#if 1
  double* dqadx = new double[9];
  for (int i=0;i<9;i++) dqadx[i] = 0.;
  for (int i=nbonds;i<nbonds+nangles;i++)
  {
    int a1=angles[i-nbonds][0];
    int a2=angles[i-nbonds][1];
    int a3=angles[i-nbonds][2];

    //printf(" doing angle %i, %i %i %i \n",i-nbonds,a1,a2,a3);

    double b0,b1,b2,b3;

    b0 = angle_val(a1,a2,a3)*3.14159/180;
    for (int j=0;j<3;j++)
    {
      coords[3*a1+j]+=fstep;
      b1 = angle_val(a1,a2,a3)*3.14159/180;
      coords[3*a1+j]-=fstep;
      coords[3*a2+j]+=fstep;
      b2 = angle_val(a1,a2,a3)*3.14159/180;
      coords[3*a2+j]-=fstep;
      coords[3*a3+j]+=fstep;
      b3 = angle_val(a1,a2,a3)*3.14159/180;
      coords[3*a3+j]-=fstep;

      dqadx[3*0+j] = (b1-b0)/fstep;
      dqadx[3*1+j] = (b2-b0)/fstep;
      dqadx[3*2+j] = (b3-b0)/fstep;
    }

    bmatp[i*N3+3*a1+0] = dqadx[0];
    bmatp[i*N3+3*a1+1] = dqadx[1];
    bmatp[i*N3+3*a1+2] = dqadx[2];
    bmatp[i*N3+3*a2+0] = dqadx[3];
    bmatp[i*N3+3*a2+1] = dqadx[4];
    bmatp[i*N3+3*a2+2] = dqadx[5];
    bmatp[i*N3+3*a3+0] = dqadx[6];
    bmatp[i*N3+3*a3+1] = dqadx[7];
    bmatp[i*N3+3*a3+2] = dqadx[8];
  }
  delete [] dqadx;
#endif

#if 1
  double* dqtdx = new double[12];
  for (int i=0;i<12;i++) dqtdx[i] = 0.;
  for (int i=nbonds+nangles;i<nbonds+nangles+ntor;i++)
  {
    int a1=torsions[i-nbonds-nangles][0];
    int a2=torsions[i-nbonds-nangles][1];
    int a3=torsions[i-nbonds-nangles][2];
    int a4=torsions[i-nbonds-nangles][3];

    //printf(" doing tor angle %i, %i %i %i \n",i-nbonds-nangles,a1,a2,a3,a4);

    double b0,b1,b2,b3,b4;

    b0 = torsion_val(a1,a2,a3,a4)*3.14159/180;
    for (int j=0;j<3;j++)
    {
      coords[3*a1+j]+=fstep;
      b1 = torsion_val(a1,a2,a3,a4)*3.14159/180;
      coords[3*a1+j]-=fstep;
      coords[3*a2+j]+=fstep;
      b2 = torsion_val(a1,a2,a3,a4)*3.14159/180;
      coords[3*a2+j]-=fstep;
      coords[3*a3+j]+=fstep;
      b3 = torsion_val(a1,a2,a3,a4)*3.14159/180;
      coords[3*a3+j]-=fstep;
      coords[3*a4+j]+=fstep;
      b4 = torsion_val(a1,a2,a3,a4)*3.14159/180;
      coords[3*a4+j]-=fstep;

      //printf(" b0,b1,b2,b3,b4: %1.6f %1.6f %1.6f %1.6f %1.6f \n",b0,b1,b2,b3,b4);

      dqtdx[3*0+j] = (b1-b0)/fstep;
      dqtdx[3*1+j] = (b2-b0)/fstep;
      dqtdx[3*2+j] = (b3-b0)/fstep;
      dqtdx[3*3+j] = (b4-b0)/fstep;
    }
 
    //printf(" dqtdx: %1.2f %1.2f %1.2f %1.2f \n",dqtdx[0],dqtdx[3],dqtdx[6],dqtdx[9]);

    bmatp[i*N3+3*a1+0] = dqtdx[0];
    bmatp[i*N3+3*a1+1] = dqtdx[1];
    bmatp[i*N3+3*a1+2] = dqtdx[2];
    bmatp[i*N3+3*a2+0] = dqtdx[3];
    bmatp[i*N3+3*a2+1] = dqtdx[4];
    bmatp[i*N3+3*a2+2] = dqtdx[5];
    bmatp[i*N3+3*a3+0] = dqtdx[6];
    bmatp[i*N3+3*a3+1] = dqtdx[7];
    bmatp[i*N3+3*a3+2] = dqtdx[8];
    bmatp[i*N3+3*a4+0] = dqtdx[9];
    bmatp[i*N3+3*a4+1] = dqtdx[10];
    bmatp[i*N3+3*a4+2] = dqtdx[11];
  }
  delete [] dqtdx;
#endif

  //printf(" \n after creating bmatp \n");

#if 0
  printf(" printing bond contributions \n");
  for (int i=0;i<nbonds;i++)
  {
    for (int j=0;j<natoms;j++)
    {
      for (int k=0;k<3;k++)
        printf(" %1.3f",bmatp[i*size_xyz+3*j+k]);
    }
    printf(" \n");
  }
#endif
#if 0
  printf(" printing angle contributions \n");
  for (int i=nbonds;i<nbonds+nangles;i++)
  {
    for (int j=0;j<natoms;j++)
    {
      for (int k=0;k<3;k++)
        printf(" %1.3f",bmatp[i*size_xyz+3*j+k]);
    }
    printf(" \n");
  }
#endif

#if 0
  printf(" printing torsion contributions \n");
  for (int i=nbonds+nangles;i<nbonds+nangles+ntor;i++)
  {
    for (int j=0;j<natoms;j++)
    {
      for (int k=0;k<3;k++)
        printf(" %1.3f",bmatp[i*size_xyz+3*j+k]);
    }
    printf(" \n");
  }
#endif

  return 0;
}



void ICoord::bmatp_dqbdx(int i, int j, double* dqbdx) {

  double* u = new double[3];  
  u[0] = coords[3*i+0]-coords[3*j+0];
  u[1] = coords[3*i+1]-coords[3*j+1];
  u[2] = coords[3*i+2]-coords[3*j+2];

  double norm = distance(i,j);
  //double norm = sqrt(u[0]*u[0]+u[1]*u[1]+u[2]*u[2]);

  u[0] = u[0]/norm;
  u[1] = u[1]/norm;
  u[2] = u[2]/norm;

  dqbdx[0] = u[0];
  dqbdx[1] = u[1];
  dqbdx[2] = u[2];
  dqbdx[3] = -u[0];
  dqbdx[4] = -u[1];
  dqbdx[5] = -u[2];

  delete [] u;

  return;
}



void ICoord::bmatp_dqadx(int i, int j, int k, double* dqadx) {

  double angle = angle_val(i,j,k) *3.14159/180; // in radians
  //printf(" angle_val: %1.2f \n",angle);
#if 0
  if (angle>3.0)
  {
   // printf(" near-linear angle, using finite difference \n");
    double fstep=0.0001;
    int a1=i;
    int a2=j;
    int a3=k;

    double b0,b1,b2,b3;

    b0 = angle_val(a1,a2,a3)*3.14159/180;
    for (int j=0;j<3;j++)
    {
      coords[3*a1+j]+=fstep;
      b1 = angle_val(a1,a2,a3)*3.14159/180;
      coords[3*a1+j]-=fstep;
      coords[3*a2+j]+=fstep;
      b2 = angle_val(a1,a2,a3)*3.14159/180;
      coords[3*a2+j]-=fstep;
      coords[3*a3+j]+=fstep;
      b3 = angle_val(a1,a2,a3)*3.14159/180;
      coords[3*a3+j]-=fstep;

      dqadx[3*0+j] = (b1-b0)/fstep;
      dqadx[3*1+j] = (b2-b0)/fstep;
      dqadx[3*2+j] = (b3-b0)/fstep;
    }
    return;
  }
#endif

  double* u = new double[3];
  double* v = new double[3];
  u[0] = coords[3*i+0]-coords[3*j+0];
  u[1] = coords[3*i+1]-coords[3*j+1];
  u[2] = coords[3*i+2]-coords[3*j+2];
  v[0] = coords[3*k+0]-coords[3*j+0];
  v[1] = coords[3*k+1]-coords[3*j+1];
  v[2] = coords[3*k+2]-coords[3*j+2];

  double n1 = distance(i,j);
  double n2 = distance(j,k);

  u[0] = u[0]/n1;  
  u[1] = u[1]/n1;  
  u[2] = u[2]/n1;  
  v[0] = v[0]/n2;  
  v[1] = v[1]/n2;  
  v[2] = v[2]/n2;  

  double* w = new double[3];
  w[0]=w[1]=w[2] = 0.;

  cross(w,u,v);
  double mag = (w[0]*w[0]+w[1]*w[1]+w[2]*w[2]);
  if (mag<THRESH)
  { 
//    printf(" Linear angle detected, w: %1.6f %1.6f %1.6f \n",w[0],w[1],w[2]);
    double* vn = new double[3];
    vn[0]=0.; vn[1]=0.; vn[2]=1.;
    cross(w,u,vn);
    mag = sqrt(w[0]*w[0]+w[1]*w[1]+w[2]*w[2]);
    if (mag<THRESH)
    {
//      printf(" Linear angle(b) detected, w: %1.6f %1.6f %1.6f \n",w[0],w[1],w[2]);
      vn[0]=0.; vn[1]=1.; vn[2]=0.;
      cross(w,u,vn);
    }
    delete [] vn;
  }
//  if (angle>3.0)
//    printf(" w: %1.3f %1.3f %1.3f \n",w[0],w[1],w[2]);

  double n3 = sqrt(w[0]*w[0]+w[1]*w[1]+w[2]*w[2]);
  //printf(" n3: %1.3f \n",n3);
  w[0] = w[0]/n3;
  w[1] = w[1]/n3;
  w[2] = w[2]/n3;
 
  double* uw = new double[3]; 
  double* wv = new double[3]; 
  cross(uw,u,w);
  cross(wv,w,v);

  dqadx[0] = uw[0]/n1;
  dqadx[1] = uw[1]/n1;
  dqadx[2] = uw[2]/n1;
  dqadx[3] = -uw[0]/n1 + -wv[0]/n2;
  dqadx[4] = -uw[1]/n1 + -wv[1]/n2;
  dqadx[5] = -uw[2]/n1 + -wv[2]/n2;
  dqadx[6] = wv[0]/n2;
  dqadx[7] = wv[1]/n2;
  dqadx[8] = wv[2]/n2;

#if 0
  if (angle>3.0)
  {
    printf(" uw: %1.3f %1.3f %1.3f, n1: %1.3f \n",uw[0],uw[1],uw[2],n1);
    printf(" wv: %1.3f %1.3f %1.3f, n2: %1.3f \n",wv[0],wv[1],wv[2],n2);
    printf(" dqadx: %1.3f %1.3f %1.3f %1.3f %1.3f %1.3f %1.3f %1.3f %1.3f \n",dqadx[0],dqadx[1],dqadx[2],dqadx[3],dqadx[4],dqadx[5],dqadx[6],dqadx[7],dqadx[8]);
  }
#endif

  delete [] u;
  delete [] v;
  delete [] w;
  delete [] uw;
  delete [] wv;

  return;
}


void ICoord::bmatp_dqtdx(int i, int j, int k, int l, double* dqtdx) {

  //printf(" \n beginning torsion bmat: %i %i %i %i \n",i,j,k,l);

  double angle1 = angle_val(i,j,k) *3.14159/180; // in radians
  double angle2 = angle_val(j,k,l) *3.14159/180; // in radians
  if (angle1>3.0 || angle2>3.0)
  {
    //printf(" near-linear angle, skipping bmat element \n");
    return;
  }
  //printf(" angle1,2: %1.2f %1.2f (%1.1f %1.1f) \n",angle1,angle2,angle1*180/3.14159,angle2*180/3.14159);

//u is between first and second atoms
//w is between third and second atoms
//v is between fourth and third atoms

  double* u = new double[3];
  double* w = new double[3];
  double* v = new double[3];
  u[0] = coords[3*i+0]-coords[3*j+0];
  u[1] = coords[3*i+1]-coords[3*j+1];
  u[2] = coords[3*i+2]-coords[3*j+2];
  w[0] = coords[3*k+0]-coords[3*j+0];
  w[1] = coords[3*k+1]-coords[3*j+1];
  w[2] = coords[3*k+2]-coords[3*j+2];
  v[0] = coords[3*l+0]-coords[3*k+0];
  v[1] = coords[3*l+1]-coords[3*k+1];
  v[2] = coords[3*l+2]-coords[3*k+2];

  double n1 = distance(i,j);
  double n2 = distance(j,k);
  double n3 = distance(k,l);
 
  //printf(" n1,n2,n3: %1.3f %1.3f %1.3f \n",n1,n2,n3);

  u[0] = u[0]/n1;  
  u[1] = u[1]/n1;  
  u[2] = u[2]/n1;  
  w[0] = w[0]/n2;  
  w[1] = w[1]/n2;  
  w[2] = w[2]/n2;  
  v[0] = v[0]/n3;  
  v[1] = v[1]/n3;  
  v[2] = v[2]/n3;  

  double* uw = new double[3]; 
  double* vw = new double[3]; 
  cross(uw,u,w);
  cross(vw,v,w);

  //double n4 = sqrt(uw[0]*uw[0]+uw[1]*uw[1]+uw[2]*uw[2]);
  //double n5 = sqrt(vw[0]*vw[0]+vw[1]*vw[1]+vw[2]*vw[2]);
  //do not normalize uw and vw

  double cosphiu = u[0]*w[0] + u[1]*w[1] + u[2]*w[2];
  double cosphiv = -v[0]*w[0] - v[1]*w[1] - v[2]*w[2];
//  double sin2phiu = sqrt(1-cosphiu*cosphiu);
//  double sin2phiv = sqrt(1-cosphiv*cosphiv);
  double sin2phiu = 1-cosphiu*cosphiu;
  double sin2phiv = 1-cosphiv*cosphiv;

  //printf(" cos's: %1.4f %1.4f vs %1.4f %1.4f \n",cosphiu,cosphiv,cos(angle1),cos(angle2));
  //printf(" sin2's: %1.4f %1.4f vs %1.4f %1.4f \n",sin2phiu,sin2phiv,sin(angle1)*sin(angle1),sin(angle2)*sin(angle2));

  if (sin2phiu<THRESH || sin2phiv<THRESH)
  { 
  //  printf(" sin2phi too small, not creating element \n");
    return;
  }

  //printf(" angle1,2: %1.2f %1.2f \n",angle1,angle2);
  //printf(" n1,n2,n3: %1.2f %1.2f %1.2f sin2phiu,v: %1.2f %1.2f cosphiu,v: %1.2f %1.2f \n",n1,n2,n3,sin2phiu,sin2phiv,cosphiu,cosphiv);


//CPMZ possible error in uw calc
  dqtdx[0]  = uw[0]/(n1*sin2phiu);
  dqtdx[1]  = uw[1]/(n1*sin2phiu);
  dqtdx[2]  = uw[2]/(n1*sin2phiu);
#if 0
//according to Helgaker, but doesn't work
  dqtdx[3]   = -uw[0]/(n1*sin2phiu) + ( uw[0]*cosphiu/(n2*sin2phiu) - vw[0]*cosphiv/(n2*sin2phiv) );
  dqtdx[4]   = -uw[1]/(n1*sin2phiu) + ( uw[1]*cosphiu/(n2*sin2phiu) - vw[1]*cosphiv/(n2*sin2phiv) );
  dqtdx[5]   = -uw[2]/(n1*sin2phiu) + ( uw[2]*cosphiu/(n2*sin2phiu) - vw[2]*cosphiv/(n2*sin2phiv) );
  dqtdx[6]   =  vw[0]/(n3*sin2phiv) - ( uw[0]*cosphiu/(n2*sin2phiu) - vw[0]*cosphiv/(n2*sin2phiv) );
  dqtdx[7]   =  vw[1]/(n3*sin2phiv) - ( uw[1]*cosphiu/(n2*sin2phiu) - vw[1]*cosphiv/(n2*sin2phiv) );
  dqtdx[8]   =  vw[2]/(n3*sin2phiv) - ( uw[2]*cosphiu/(n2*sin2phiu) - vw[2]*cosphiv/(n2*sin2phiv) );
#endif
#if 1
  dqtdx[3]   = -uw[0]/(n1*sin2phiu) + ( uw[0]*cosphiu/(n2*sin2phiu) + vw[0]*cosphiv/(n2*sin2phiv) );
  dqtdx[4]   = -uw[1]/(n1*sin2phiu) + ( uw[1]*cosphiu/(n2*sin2phiu) + vw[1]*cosphiv/(n2*sin2phiv) );
  dqtdx[5]   = -uw[2]/(n1*sin2phiu) + ( uw[2]*cosphiu/(n2*sin2phiu) + vw[2]*cosphiv/(n2*sin2phiv) );
  dqtdx[6]   =  vw[0]/(n3*sin2phiv) - ( uw[0]*cosphiu/(n2*sin2phiu) + vw[0]*cosphiv/(n2*sin2phiv) );
  dqtdx[7]   =  vw[1]/(n3*sin2phiv) - ( uw[1]*cosphiu/(n2*sin2phiu) + vw[1]*cosphiv/(n2*sin2phiv) );
  dqtdx[8]   =  vw[2]/(n3*sin2phiv) - ( uw[2]*cosphiu/(n2*sin2phiu) + vw[2]*cosphiv/(n2*sin2phiv) );
#endif
  dqtdx[9]   = -vw[0]/(n3*sin2phiv);
  dqtdx[10]  = -vw[1]/(n3*sin2phiv);
  dqtdx[11]  = -vw[2]/(n3*sin2phiv);

//  for (int i=0;i<12;i++)
//    dqtdx[i] = dqtdx[i]/10;
//  for (int i=0;i<12;i++)
//    printf(" dqtdx[%i]: %1.4f \n",i,dqtdx[i]);

  delete [] u;
  delete [] v;
  delete [] w;
  delete [] uw;
  delete [] vw;

  return;
}

void ICoord::update_bfgs()
{
// updates Hinv using BFGS
//  return;
//  return update_bofill();

  newHess--;

  int len0 = nicd0;
  int len = nicd;

  double* dg = new double[len0];
  double* dx = new double[len0];
  double* G = new double[len0*len0];
  double* dxdx = new double[len0*len0];
  double* dxdg = new double[len0*len0];
  double* dxdgG = new double[len0*len0];
  double* Gdgdx = new double[len0*len0];
  double* Gdg = new double[len0];

  double dxtdg = 0.;
  double dgGdg = 0.;

//  printf(" in update_bfgs, nicd, nicd0: %i %i \n",nicd,nicd0);

  //dx[len0-1] = 0.;
  //dg[len0-1] = 0.;
  for (int i=0;i<len0;i++) //CPMZ check
    dx[i] = dq0[i];
  for (int i=0;i<len0;i++)
    dg[i] = gradq[i] - pgradq[i];

#if 0
  printf(" dg:");
  for (int i=0;i<len0;i++)
    printf(" %1.3f",dg[i]);
  printf("\n");
#endif

  for (int i=0;i<len0;i++)
  for (int j=0;j<len0;j++)
    G[i*len0+j] = Hinv[i*len0+j];

  for (int i=0;i<len0;i++) Gdg[i]=0.;
  for (int i=0;i<len0;i++)
  for (int j=0;j<len0;j++)
    Gdg[i] += G[i*len0+j]*dg[j];
  for (int i=0;i<len0;i++)
    dgGdg += dg[i]*Gdg[i];

  for (int i=0;i<len0;i++)
  for (int j=0;j<len0;j++)
    dxdx[i*len0+j] = dx[i]*dx[j];

  for (int i=0;i<len0;i++)
  for (int j=0;j<len0;j++)
    dxdg[i*len0+j] = dx[i]*dg[j];

#if 1
  mat_times_mat_bt(dxdgG,dxdg,G,len0);
#else
  for (int i=0;i<len0*len0;i++) dxdgG[i] = 0.;
  for (int i=0;i<len0;i++)
  for (int j=0;j<len0;j++)
  for (int k=0;k<len0;k++)
    dxdgG[i*len0+j] += dxdg[i*len0+k]*G[k*len0+j];
#endif

#if 1
  mat_times_mat_at_bt(Gdgdx,dxdg,G,len0);
#else
  for (int i=0;i<len0*len0;i++) Gdgdx[i] = 0.;
  for (int i=0;i<len0;i++)
  for (int j=0;j<len0;j++)
  for (int k=0;k<len0;k++)
    Gdgdx[i*len0+j] += dxdg[k*len0+i]*G[k*len0+j];
#endif
 
  for (int i=0;i<len0;i++)
    dxtdg += dx[i]*dg[i];

  //printf(" dgGdg: %1.3f dxtdg: %1.3f \n",dgGdg,dxtdg);

  for (int i=0;i<len0;i++)
  for (int j=0;j<len0;j++)
    Hinv[i*len0+j] += (1+dgGdg/dxtdg) * dxdx[i*len0+j]/dxtdg;

  for (int i=0;i<len0;i++)
  for (int j=0;j<len0;j++)
    Hinv[i*len0+j] -= dxdgG[i*len0+j]/dxtdg + dxdgG[j*len0+i]/dxtdg;
//    Hinv[i*len0+j] -= dxdgG[i*len0+j]/dxtdg + Gdgdx[i*len0+j]/dxtdg;

#if 0
  printf(" Hinv elements: \n");
  for (int i=0;i<len0;i++)
  {
    for (int j=0;j<len0;j++)
      printf(" %1.3f",Hinv[i*len0+j]);
    printf("\n");
  }
#endif

#if 0
  if (!optCG)
  {
    double* tmph = new double[len0*len0];
    for (int i=0;i<len;i++)
    for (int j=0;j<len;j++)
      tmph[i*len+j] = Hinv[i*len0+j];
    Invert(tmph,len);
    double* eigen = new double[len0];
    Diagonalize(tmph,eigen,len);
    printf(" Hint ev:");
//    for (int i=0;i<len;i++)
    for (int i=0;i<4;i++)
      printf(" %1.3f",eigen[i]);
    //printf("\n");
    delete [] tmph;
    delete [] eigen;
  }
#endif

  delete [] dg;
  delete [] dx;
  delete [] dxdx;
  delete [] dxdg;
  delete [] G;
  delete [] dxdgG;
  delete [] Gdgdx;

  return;
}



void ICoord::update_bfgsp(int makeHint)
{
// updates Hintp using BFGS

  newHess--;

  int len0 = nbonds+nangles+ntor;
  int len_d = nicd0;

  double* dg = new double[len0];
  double* dx = new double[len0];
  double* Hdx = new double[len0*len0];
  double* dgdg = new double[len0*len0];

  double dgtdx = 0.;
  double dxHdx = 0.;


  //printf(" in update_bfgsp, nicd, nicd0: %i %i \n",nicd,nicd0);


//  printf(" WARNING: constraining dqprim! \n");
//  for (int i=0;i<len0;i++) dx[i] = 0.;
//  for (int i=0;i<nbonds+nangles;i++) 
  for (int i=0;i<len0;i++) 
    dx[i] = dqprim[i];
  for (int i=0;i<len0;i++)
    dg[i] = gradqprim[i] - pgradqprim[i];

#if 0
  printf(" dg:");
  for (int i=0;i<len0;i++)
    printf(" %1.3f",dg[i]);
  printf("\n");
#endif
#if 0
  printf(" dx:");
  for (int i=0;i<len0;i++)
    printf(" %1.3f",dx[i]);
  printf("\n");
#endif
#if 0
  printf(" Hint elements (before): \n");
  for (int i=0;i<len_d;i++)
  {
    for (int j=0;j<len_d;j++)
      printf(" %1.3f",Hintp[i*len_d+j]);
    printf("\n");
  }
#endif


  for (int i=0;i<len0;i++) Hdx[i]=0.;
  for (int i=0;i<len0;i++)
  for (int j=0;j<len0;j++)
    Hdx[i] += Hintp[i*len0+j]*dx[j];

  for (int i=0;i<len0;i++)
    dxHdx += dx[i]*Hdx[i];

  for (int i=0;i<len0;i++)
  for (int j=0;j<len0;j++)
    dgdg[i*len0+j] = dg[i]*dg[j];

  for (int i=0;i<len0;i++)
    dgtdx += dg[i]*dx[i];

//  printf(" dgtdx: %1.3f dxHdx: %1.3f ",dgtdx,dxHdx);

  if (dgtdx>0.)
  {
    if (dgtdx<0.001) dgtdx = 0.001;
    for (int i=0;i<len0;i++)
    for (int j=0;j<len0;j++)
      Hintp[i*len0+j] += dgdg[i*len0+j]/dgtdx;
  }
  if (dxHdx>0.)
  {
    if (dxHdx<0.001) dxHdx = 0.001;
    for (int i=0;i<len0;i++)
    for (int j=0;j<len0;j++)
      Hintp[i*len0+j] += - Hdx[i]*Hdx[j]/dxHdx;
  }

#if 0
  printf(" Hintp elements: \n");
  for (int i=0;i<len0;i++)
  {
    for (int j=0;j<len0;j++)
      printf(" %1.3f",Hintp[i*len0+j]);
    printf("\n");
  }
#endif

#if 0
  if (!optCG || 1)
  {
    double* tmph = new double[len0*len0];
    for (int i=0;i<len0;i++)
    for (int j=0;j<len0;j++)
      tmph[i*len0+j] = Hintp[i*len0+j];
    double* eigen = new double[len0];
    Diagonalize(tmph,eigen,len0);
    printf(" Hintp ev:");
//    for (int i=0;i<len0;i++)
    for (int i=0;i<4;i++)
      printf(" %1.3f",eigen[i]);
    printf("\n");
    delete [] tmph;
    delete [] eigen;
  }
#endif

  delete [] dg;
  delete [] dx;
  delete [] Hdx;
  delete [] dgdg;

  if (makeHint)
    Hintp_to_Hint();

  return;
}

void ICoord::Hintp_to_Hint()
{

  int len0 = nbonds+nangles+ntor;
  int len_d = nicd0;

  double* tmp = new double[len0*len0];

#if 1
  mat_times_mat_bt(tmp,Ut,Hintp,len_d,len0,len0);
#else
  for (int i=0;i<len0*len_d;i++) tmp[i] = 0.;
  for (int i=0;i<len_d;i++)
  for (int j=0;j<len0;j++)
  for (int k=0;k<len0;k++)
    tmp[i*len0+j] += Ut[i*len0+k]*Hintp[k*len0+j];
#endif
    
#if 1
  mat_times_mat_bt(Hint,tmp,Ut,len_d,len_d,len0);
#else
  for (int i=0;i<len_d*len_d;i++) Hint[i] = 0.;
  for (int i=0;i<len_d;i++)
  for (int j=0;j<len_d;j++)
  for (int k=0;k<len0;k++)
    Hint[i*len_d+j] += tmp[i*len0+k]*Ut[j*len0+k];
#endif

#if 0
  printf(" Hint elements: \n");
  for (int i=0;i<len_d;i++)
  {
    for (int j=0;j<len_d;j++)
      printf(" %1.3f",Hint[i*len_d+j]);
    printf("\n");
  }
#endif

#if 0
  if (1)
  {
    double* tmph = new double[len_d*len_d];
    for (int i=0;i<len_d;i++)
    for (int j=0;j<len_d;j++)
      tmph[i*len_d+j] = Hint[i*len_d+j];
    double* eigen = new double[len_d];
    Diagonalize(tmph,eigen,len_d);
    printf(" Hint ev:");
    for (int i=0;i<len_d;i++)
//    for (int i=0;i<4;i++)
      printf(" %1.4f",eigen[i]);
    printf("\n");
    delete [] tmph;
    delete [] eigen;
  }
#endif

  delete [] tmp;

  return;
}



void ICoord::update_bofill()
{
// updates Hint, Hinv using Bofill

//  newHess--;
  //Maintain BFGS primitive Hessian
  update_bfgsp(0);

  int len0 = nicd0;
  int len = nicd;

  double* dg = new double[len0];
  double* dx = new double[len0];
  double* G = new double[len0*len0];
  double* Gms = new double[len0*len0];
  double* Gpsb = new double[len0*len0];
  double* dxdx = new double[len0*len0];
  double* dgmGdx = new double[len0];
  double* dgmGdxdx = new double[len0*len0];
  double* dxdgmGdx = new double[len0*len0];
  double* Gdx = new double[len0];

  double dxtdx = 0.;
  double dxtdg = 0.;
  double dgmGdxtdx = 0.;
  double dxtGdx = 0.;

//  printf(" in update_bofill, nicd, nicd0: %i %i \n",nicd,nicd0);

  for (int i=0;i<len0;i++)
    dx[i] = dq0[i];
  for (int i=0;i<len0;i++)
    dg[i] = gradq[i] - pgradq[i];

#if 0
  printf(" dg:");
  for (int i=0;i<len0;i++)
    printf(" %1.3f",dg[i]);
  printf("\n");
#endif

  for (int i=0;i<len0;i++)
  for (int j=0;j<len0;j++)
    G[i*len0+j] = Hint[i*len0+j];

  for (int i=0;i<len0;i++) Gdx[i]=0.;
  for (int i=0;i<len0;i++)
  for (int j=0;j<len0;j++)
    Gdx[i] += G[i*len0+j]*dx[j];
  for (int j=0;j<len0;j++)
    dgmGdx[j] = dg[j] - Gdx[j];

//MS
  for (int j=0;j<len0;j++)
    dgmGdxtdx += dgmGdx[j]*dx[j];

  for (int i=0;i<len0*len0;i++) Gms[i] = 0.;
  for (int i=0;i<len0;i++)
  for (int j=0;j<len0;j++)
    Gms[i*len0+j] += dgmGdx[i]*dgmGdx[j]/dgmGdxtdx;

//PSB
  for (int i=0;i<len0;i++)
  for (int j=0;j<len0;j++)
    dxdx[i*len0+j] = dx[i]*dx[j];

  for (int j=0;j<len0;j++)
    dxtdx += dx[j]*dx[j];

  for (int j=0;j<len0;j++)
    dxtdg += dx[j]*dg[j];

  for (int j=0;j<len0;j++)
    dxtGdx += dx[j]*Gdx[j];

  for (int i=0;i<len0*len0;i++) Gpsb[i] = 0.;
  for (int i=0;i<len0;i++)
  for (int j=0;j<len0;j++)
    Gpsb[i*len0+j] += dgmGdx[i]*dx[j]/dxtdx+dx[i]*dgmGdx[j]/dxtdx;

  double dxtdx2 = dxtdx*dxtdx;
  double xtdgmxtGdx = dxtdg - dxtGdx;
  for (int i=0;i<len0;i++)
  for (int j=0;j<len0;j++)
    Gpsb[i*len0+j] -= (xtdgmxtGdx)*dxdx[i*len0+j]/dxtdx2;

// for Bofill mixing
  double EtE = 0.;
  double dxtE = 0.; //E is dgmGdx
  for (int i=0;i<len0;i++)
    dxtE += dx[i]*dgmGdx[i];
  for (int i=0;i<len0;i++)
    EtE += dgmGdx[i]*dgmGdx[i];

  double phi = 1 - dxtE*dxtE/(dxtdx*EtE);

  //printf(" phi: %1.3f",phi);

  for (int i=0;i<len0;i++)
  for (int j=0;j<len0;j++)
    Hint[i*len0+j] += (1-phi)*Gms[i*len0+j] + phi*Gpsb[i*len0+j];

  for (int i=0;i<len0*len0;i++)
    Hinv[i] = Hint[i];

  Invert(Hinv,len0);
#if 0
  printf(" Hint elements: \n");
  for (int i=0;i<len0;i++)
  {
    for (int j=0;j<len0;j++)
      printf(" %1.3f",Hint[i*len0+j]);
    printf("\n");
  }
#endif

#if 0
  if (!optCG)
  {
    double* tmph = new double[len0*len0];
    double* eigen = new double[len0];
    for (int i=0;i<len;i++)
    for (int j=0;j<len;j++)
      tmph[i*len+j] = Hint[i*len0+j];
    Diagonalize(tmph,eigen,len);
    printf(" Hint ev:");
//    for (int i=0;i<len;i++)
    for (int i=0;i<4;i++)
      printf(" %1.3f",eigen[i]);
    //printf("\n");
    delete [] tmph;
    delete [] eigen;
  }
#endif

  delete [] dg;
  delete [] dx;
  delete [] G;
  delete [] Gms;
  delete [] Gpsb;
  delete [] dxdx;
  delete [] dgmGdx;
  delete [] dgmGdxdx;
  delete [] dxdgmGdx;
  delete [] Gdx;

  return;
}

void ICoord::make_Hint()
{
  newHess = 5;

 // printf(" in make_Hint() \n");

  int size_ic = nbonds+nangles+ntor;
  int len0 = nicd0;
  int len = nicd;
  
  double* tmp = new double[len0*size_ic];

  double* Hdiagp = new double[size_ic];
#if 0
  for (int i=0;i<nbonds;i++)
    Hdiagp[i] = 0.5*close_bond(i);
  for (int i=nbonds;i<nbonds+nangles;i++)
    Hdiagp[i] = 0.2*close_angle(i-nbonds);
  for (int i=nbonds+nangles;i<nbonds+nangles+ntor;i++)
    Hdiagp[i] = 0.05*close_tor(i-nbonds-nangles);
#else
  for (int i=0;i<nbonds;i++)
    Hdiagp[i] = 0.35*close_bond(i);
  for (int i=nbonds;i<nbonds+nangles;i++)
    Hdiagp[i] = 0.2;
  for (int i=nbonds+nangles;i<nbonds+nangles+ntor;i++)
    Hdiagp[i] = 0.035;
#endif
#if 0
  printf(" Hdiagp elements: \n");
  for (int i=0;i<size_ic;i++)
    printf(" %1.3f \n",Hdiagp[i]);
#endif

  for (int i=0;i<size_ic*size_ic;i++) Hintp[i] = 0.;
  for (int i=0;i<size_ic;i++) Hintp[i*size_ic+i] = Hdiagp[i];
  

  for (int i=0;i<len0;i++)
  for (int k=0;k<size_ic;k++)
    tmp[i*size_ic+k] = Ut[i*size_ic+k]*Hdiagp[k];

#if 1
  mat_times_mat_bt(Hint,tmp,Ut,len0,len0,size_ic);
#else
  for (int i=0;i<len0*len0;i++) Hint[i] = 0.;
  for (int i=0;i<len0;i++)
  for (int j=0;j<len0;j++)
  for (int k=0;k<size_ic;k++)
    Hint[i*len0+j] += tmp[i*size_ic+k]*Ut[j*size_ic+k];
#endif

#if 0
  printf(" Hint elements: \n");
  for (int i=0;i<len0;i++)
  {
    for (int j=0;j<len0;j++)
      printf(" %1.3f",Hint[i*len0+j]);
    printf("\n");
  }
#endif

  for (int i=0;i<len0*len0;i++)
    Hinv[i] = Hint[i];
  Invert(Hinv,len0);

#if 0
  printf(" WARNING: overriding Hinv \n");
  for (int i=0;i<len0*len0;i++) Hinv[i] = 0.;
  for (int i=0;i<len0;i++)
    Hinv[i*len0+i] = 1.0;
#endif

#if 0
  double* tmp2 = new double[len0*len0];
  for (int i=0;i<len0*len0;i++)
    tmp2[i] = 0.;
  for (int i=0;i<len0;i++)
  for (int j=0;j<len0;j++)
  for (int k=0;k<len0;k++)
    tmp2[i*len0+j] += Hinv[i*len0+k]*Hint[k*len0+j];

  printf(" debug: Hinv*Hint diagonals \n");
  for (int i=0;i<len0;i++)
    printf(" %1.2f",tmp2[i*len0+i]);
  printf("\n");
  delete [] tmp2;
#endif

#if 0
  printf(" Initial Hinv elements: \n");
  for (int i=0;i<len0;i++)
  {
    for (int j=0;j<len0;j++)
      printf(" %1.3f",Hinv[i*len0+j]);
    printf("\n");
  }
#endif

#if 1
  if (!optCG || isTSnode)
  {
    double* tmph = new double[len0*len0];
    double* eigen = new double[len0];
    for (int i=0;i<len0*len0;i++)
      tmph[i] = Hint[i];
    Diagonalize(tmph,eigen,len0);
    printf(" initial Hint ev:");
//    for (int i=0;i<len0;i++)
    for (int i=0;i<4;i++)
      printf(" %1.3f",eigen[i]);
    printf("\n");
    delete [] tmph;
    delete [] eigen;
  }
#endif

  delete [] Hdiagp;
  delete [] tmp;

  return;
}




void ICoord::opt_constraint(double* C) 
{
  int len = nbonds+nangles+ntor;

#if 0
  printf(" constraint: ");
  for (int i=0;i<len;i++)
    printf(" %1.3f",C[i]);
  printf("\n");
#endif

  //for (int i=0;i<nicd0*nicd0;i++)
  //  Ut[i] = Ut0[i];

  nicd = nicd0;
  nicd--;
  //printf(" nicd: %i \n",nicd);

  /** take constraint vector, project it out of all Ut
  * orthonormalize vectors
  * last vector becomes C (projection onto space)
	*/

  double norm = 0.;
  for (int i=0;i<len;i++)
    norm += C[i]*C[i];
  norm = sqrt(norm);
  for (int j=0;j<len;j++)
    C[j] = C[j]/norm;

  double* dots = new double[len];
  for (int i=0;i<len;i++) dots[i] =0.;

  double* Cn = new double[len];
  for (int i=0;i<len;i++) Cn[i] =0.;

  for (int i=0;i<len;i++)
  for (int j=0;j<len;j++)
    dots[i] += C[j]*Ut[i*len+j];
 
  for (int i=0;i<nicd0;i++) //CPMZ fix? right now subspace projection
  for (int j=0;j<len;j++)
    Cn[j] += dots[i]*Ut[i*len+j];

  norm = 0.;
  for (int i=0;i<len;i++)
    norm += Cn[i]*Cn[i];
  norm = sqrt(norm);
  //printf(" Cn norm: %1.2f \n",norm);
  for (int j=0;j<len;j++)
    Cn[j] = Cn[j]/norm;

#if 0
  norm = 0.;
  for (int j=0;j<len;j++)
    norm += C[j] * Cn[j];
  printf(" C dot Cn: %1.3f \n",norm);
#endif
#if 0
  printf(" Printing Cn: \n");
  for (int j=0;j<nbonds;j++)
    printf(" %1.2f",Cn[j]);
  printf("\n");
  for (int j=0;j<nangles;j++)
    printf(" %1.2f",Cn[nbonds+j]);
  printf("\n");
  for (int j=0;j<ntor;j++)
    printf(" %1.2f",Cn[nbonds+nangles+j]);
  printf("\n");
#endif

  for (int i=0;i<len;i++) dots[i] =0.;
  for (int i=0;i<len;i++) 
  for (int j=0;j<len;j++)
    dots[i] += Cn[j]*Ut[i*len+j];

//  for (int i=0;i<nicd0;i++)
//    printf(" dots[%i]: %1.2f \n",i,dots[i]);

  for (int i=0;i<nicd0;i++)
  {
    if (i!=nicd0-1)
    for (int j=0;j<len;j++)
      Ut[i*len+j] -= dots[i] * Cn[j];

    for (int k=0;k<i;k++)
    {
      double dot2 = 0.;
      for (int j=0;j<len;j++)
        dot2 += Ut[i*len+j] * Ut[k*len+j];

      for (int j=0;j<len;j++)
        Ut[i*len+j] -= dot2 * Ut[k*len+j];
    } // loop k over previously formed vectors
 
    double norm = 0.;
    for (int j=0;j<len;j++)
      norm += Ut[i*len+j] * Ut[i*len+j];
    norm = sqrt(norm);
    if (abs(norm)<0.00001) norm = 1;
    if (abs(norm)<0.00001) printf(" WARNING: small norm: %1.7f \n",norm);
    for (int j=0;j<len;j++)
      Ut[i*len+j] = Ut[i*len+j]/norm;
  }

  for (int j=0;j<len;j++)
    Ut[nicd*len+j] = Cn[j];
#if 0
  printf(" printing Cn vs. Ut[nicd*len]\n");
  for (int j=0;j<len;j++)
    printf(" %1.2f/%1.2f\n",Cn[j],Ut[nicd*len+j]);
#endif
#if 0
  printf(" printing orthonormalized vectors \n");
  for (int i=0;i<nicd0;i++)
  {
    for (int j=0;j<len;j++)
      printf(" %1.3f",Ut[i*len+j]);
    printf("\n");
  }
#endif

  delete [] dots;
  delete [] Cn;

  return;
}


double ICoord::opt_a(int nnewb, int* newb, int nnewt, int* newt, string xyzfile_string, int nsteps){

  int OPTSTEPS = nsteps;
  //printf("  \n"); 
  
  int len = nbonds+nangles+ntor;
  for (int i=0;i<len;i++)
    dq0[i] = 0.;
  n_nonbond = make_nonbond();

  printf("\n internals at start of opt_a \n");
  print_ic();

  mm_init();
  Hintp_to_Hint();
  double energyp;
  double energyl;
  double gradrmsl;
  double* xyzl = new double[3*natoms];
  for (int j=0;j<3*natoms;j++) xyzl[j] = coords[j];

  pgradrms = 10000;
  SCALEQN = SCALEQN0;
  ixflag = 0;

  int rflag = 0;
  int nrflag = 0;
  noptdone = 1;

  do_bfgs = 0; //resets at each step

  double energy = grad1.grads(coords, grad, Ut, 1) - V0;
  if (energy > 1000.) return 10000.;

  energyp = energy;
  energyl = energy;
#if 1
  ofstream xyzfile;
  xyzfile.open(xyzfile_string.c_str());
  xyzfile.setf(ios::fixed);
  xyzfile.setf(ios::left);
  xyzfile << setprecision(6);
#endif

  //printf(" \n beginning opt \n\n");

  for (int n=0;n<OPTSTEPS;n++)
  {
    //if (n==0)
    printf(" Opt step: %2i ",n+1);

//#if USE_PRIMA
//  energy += prima_force();
//#endif

    bmatp_create();
    bmat_create();

    grad_to_q();
    //print_gradq();
    if (n==0) gradrmsl = gradrms;
    if ( (gradrms<gradrmsl && energy<energyl) ||
          energy<energyl-5.)
    {
      gradrmsl = gradrms;
      energyl = energy;
      for (int j=0;j<3*natoms;j++) xyzl[j] = coords[j];
    }

    if (do_bfgs) update_bfgsp(1);
    save_hess();
    do_bfgs = 1;

#if 1
    xyzfile << " " << natoms << endl;
    xyzfile << " " << energy << endl;
    for (int i=0;i<natoms;i++) 
    {
      xyzfile << "  " << anames[i];
      xyzfile << " " << coords[3*i+0] << " " << coords[3*i+1] << " " << coords[3*i+2];
      xyzfile << endl;
    }
#endif

    update_ic_eigen();

    rflag = ic_to_xyz_opt();
    update_ic();
    //print_ic();
    //print_xyz();

    if (rflag)
    {
      nrflag++;
      DMAX = DMAX/1.6;
      //MAXAD = MAXAD/1.1;
    //  SCALEQN *= 1.5;
      printf(" updating SCALE "); 
      update_ic_eigen();
      ic_to_xyz_opt();
      update_ic();
      do_bfgs = 0;
    }

    if (ixflag>2)
    {
      printf(" bc problem, r Ut ");
      bmatp_create();
      bmatp_to_U();
      make_Hint();
      do_bfgs = 0;
      ixflag = 0;
    }

    if (n<OPTSTEPS-1)
    {
      noptdone++;
      energy = grad1.grads(coords, grad, Ut, 1) - V0;
      if (energy > 1000.) break;

#if STEPCONTROL && 0
      double dE = energy - energyp;
      energyp = energy;
      if (abs(dEpre)<0.05) dEpre = sign(dEpre)*0.05; 
      double ratio = dE/dEpre;
      printf(" ratio: %2.3f ",ratio);
      if (ratio < 0.25)
      {
        printf(" decreasing DMAX ");
        if (smag<DMAX)
          DMAX = smag / 1.1;
        else
          DMAX = DMAX / 1.2;
      }
      else if (ratio > 0.75 && ratio < 1.5 && smag>DMAX && gradrms<pgradrms*1.35)
      {
        printf(" increasing DMAX ");
        DMAX = DMAX * 1.1 + 0.001;
        if (DMAX > 0.25)
          DMAX = 0.25;
      }
      if (DMAX<DMIN0) DMAX=DMIN0;
#endif
#if STEPCONTROL && 1
      if (gradrms>pgradrms*1.1) 
      {
        printf(" decreasing DMAX ");
        if (smag<DMAX)
          DMAX = smag / 1.1;
        else
          DMAX = DMAX / 1.2;
      }
      if (gradrms<pgradrms/1.15) 
      {
        printf(" increasing DMAX ");
        DMAX = DMAX * 1.1 + 0.001;
        if (DMAX > 0.25)
          DMAX = 0.25;
      }
      if (DMAX<DMIN0) DMAX=DMIN0;
#endif
      printf(" E(M): %1.1f",energy);
      printf(" gRMS: %1.4f ",gradrms);
      printf(" \n");
    }
    pgradrms = gradrms;
    if (gradrms<OPTTHRESH) break;

  } //loop over opt steps

#if 1
  printf(" E(M): %1.1f",energy);
  printf(" final IC grad RMS: %1.4f ",gradrms);
  if (gradrms<OPTTHRESH) printf(" *");
  printf("\n");
#endif

  delete [] xyzl;

  return energy;
}
/// optimizes the node to the minimum without a constraint
double ICoord::opt_b(string xyzfile_string, int nsteps){

  printout = "";

  int OPTSTEPS = nsteps;
  //printf("  \n"); 
  
  int len = nbonds+nangles+ntor;
  for (int i=0;i<len;i++)
    dq0[i] = 0.;

  Hintp_to_Hint();
  double energyp;
  double energyl;
  double gradrmsl;
  int stepl = 0;
  double* xyzl = new double[3*natoms];
  for (int j=0;j<3*natoms;j++) xyzl[j] = coords[j];

  pgradrms = 10000;
  SCALEQN = SCALEQN0;
  ixflag = 0;

  int rflag = 0;
  int nrflag = 0;
  noptdone = 1;

  do_bfgs = 0; //resets at each step

  double energy = grad1.grads(coords, grad, Ut, 1) - V0;
  if (energy > 1000.) { sprintf(sbuff,"SCF failed \n"); printout+=sbuff; return 10000.; }

  energyp = energy;
  energyl = energy;
#if 1
  ofstream xyzfile;
  xyzfile.open(xyzfile_string.c_str());
  xyzfile.setf(ios::fixed);
  xyzfile.setf(ios::left);
  xyzfile << setprecision(6);
#endif

  //printf(" \n beginning opt \n\n");

  sprintf(sbuff,"\n"); printout += sbuff;
  for (int n=0;n<OPTSTEPS;n++)
  {
    //if (n==0)
    sprintf(sbuff," Opt step: %2i ",n+1); printout += sbuff;

#if USE_PRIMA
    energy += prima_force();
#endif

    bmatp_create();
    bmat_create();

    grad_to_q();
    //print_gradq();
    if (n==0) gradrmsl = gradrms;
    if ( (gradrms<gradrmsl && energy<energyl) ||
          energy<energyl-5.)
    {
      gradrmsl = gradrms;
      energyl = energy;
      stepl = n;
      for (int j=0;j<3*natoms;j++) xyzl[j] = coords[j];
    }

    if (do_bfgs) update_bfgsp(1);
    save_hess();
    do_bfgs = 1;

#if 1
    xyzfile << " " << natoms << endl;
    xyzfile << " " << energy << endl;
    for (int i=0;i<natoms;i++) 
    {
      xyzfile << "  " << anames[i];
      xyzfile << " " << coords[3*i+0] << " " << coords[3*i+1] << " " << coords[3*i+2];
      xyzfile << endl;
    }
#endif

    if (gradrms<OPTTHRESH) break;

    update_ic_eigen();

    rflag = ic_to_xyz_opt();
    update_ic();
    //print_ic();
    //print_xyz();

    if (rflag)
    {
      nrflag++;
      DMAX = DMAX/1.6;
      //MAXAD = MAXAD/1.1;
    //  SCALEQN *= 1.5;
      sprintf(sbuff," updating SCALE "); printout += sbuff; 
      update_ic_eigen();
      ic_to_xyz_opt();
      update_ic();
      do_bfgs = 0;
    }

    if (ixflag>2)
    {
      sprintf(sbuff," bc problem, r Ut "); printout += sbuff;
      bmatp_create();
      bmatp_to_U();
      make_Hint();
      do_bfgs = 0;
      ixflag = 0;
    }

    sprintf(sbuff," E(M): %1.1f gRMS: %1.4f",energy,gradrms); printout += sbuff;
    if (n<OPTSTEPS-1)
    {
      noptdone++;
      energy = grad1.grads(coords, grad, Ut, 1) - V0;
      if (energy > 1000.) { sprintf(sbuff,"SCF failed \n"); printout += sbuff; gradrms = 1.; break; }

#if STEPCONTROL
      double dE = energy - energyp;
      energyp = energy;
      if (abs(dEpre)<0.05) dEpre = sign(dEpre)*0.05; 
      double ratio = dE/dEpre;
      sprintf(sbuff," ratio: %2.3f ",ratio); printout += sbuff;
      if (dE > 0. && !isTSnode)
      {
        sprintf(sbuff," dE>0, decreasing DMAX "); printout += sbuff;
        if (smag<DMAX)
          DMAX = smag / 1.5;
        else
          DMAX = DMAX / 1.5;
        if (dE > 2.0 && revertOpt)
        {
          sprintf(sbuff," resetting structure \n"); printout += sbuff;
          for (int j=0;j<3*natoms;j++) coords[j] = xyzl[j];
          bmatp_create();
          bmatp_to_U();
          Hintp_to_Hint();
          do_bfgs = 0;
          OPTSTEPS++;
          energy = grad1.grads(coords, grad, Ut, 1) - V0;
        }
      }
      else if (ratio < 0.25)
      {
        sprintf(sbuff," decreasing DMAX "); printout += sbuff;
        if (smag<DMAX)
          DMAX = smag / 1.1;
        else
          DMAX = DMAX / 1.2;
      }
      else if (ratio > 0.75 && ratio < 1.5 && smag>DMAX && gradrms<pgradrms*1.35)
      {
        sprintf(sbuff," increasing DMAX "); printout += sbuff;
        DMAX = DMAX * 1.1 + 0.001;
        if (DMAX > 0.25)
          DMAX = 0.25;
      }
      if (DMAX<DMIN0) DMAX=DMIN0;
#endif
    }
    pgradrms = gradrms;
    sprintf(sbuff,"\n"); printout += sbuff;

  } //loop over opt steps

#if 1
  sprintf(sbuff," E(M): %1.1f",energy); printout += sbuff;
  sprintf(sbuff," final IC grad RMS: %1.4f ",gradrms); printout += sbuff;
  if (gradrms<OPTTHRESH) { sprintf(sbuff," *"); printout += sbuff; }
  sprintf(sbuff,"\n"); printout += sbuff;
#endif

#if 1
  if (gradrms>gradrmsl*3.0 && energy>energyl && revertOpt)
  {
    if (DMAX>smag)
      DMAX = smag/1.5;
    else
      DMAX = DMAX/1.5;

    sprintf(sbuff,"r%i",stepl); printout += sbuff;
    for (int j=0;j<3*natoms;j++)
      coords[j] = xyzl[j];
    energy = energyl;
    gradrms = gradrmsl;
    for (int j=0;j<nicd0;j++) gradq[j] = 0.0;
//    if (stepl==0)
//      nneg = -4;
  }
#endif

  delete [] xyzl;

  return energy;
}



///Optimizes the node subject to a constraint, nsteps times.
double ICoord::opt_c(string xyzfile_string, int nsteps, double* C, double* C0)
{
  //printf(" oc"); fflush(stdout);
  printout = "";

#if HESS_TANG && !USE_MOLPRO && !QCHEMSF
  use_constraint = 0; //use overlap if possible

  if (isTSnode)
  {
    use_constraint = 1;
    opt_constraint(C);
  }
#endif

  if (stage1opt && !use_constraint)
  {
    use_constraint = 1;
    opt_constraint(C);
  }

  int OPTSTEPS = nsteps;
  //printf("  \n"); 
  
  int len0 = nbonds+nangles+ntor;
  int len = nicd0;
  for (int i=0;i<len;i++)
    dq0[i] = 0.;

//Creating Tangent Vector in Delocalized coordinates

  bmatp_create();
  bmat_create();

  double* dots = new double[len];
  double* Cn = new double[len0];
  for (int i=0;i<len;i++) dots[i] = 0.;
  for (int i=0;i<len0;i++) Cn[i] = 0.;
  for (int i=0;i<len;i++)
  for (int j=0;j<len0;j++)
    dots[i] += Ut[i*len0+j]*C[j];
  for (int i=0;i<len;i++)
  for (int j=0;j<len0;j++)
    Cn[j] += dots[i]*Ut[i*len0+j];
  double norm = 0.;
  for (int i=0;i<len0;i++)
    norm += Cn[i]*Cn[i];
  norm = sqrt(norm);
  for (int j=0;j<len0;j++)
    Cn[j] = Cn[j]/norm;
#if 0
  printf(" Cn norm: %1.2f \n",norm);
  printf(" C:");
  for (int j=0;j<len0;j++)
    printf(" %1.2f",C[j]);
  printf("\n");
  printf(" Cn:");
  for (int j=0;j<len0;j++)
    printf(" %1.2f",Cn[j]);
  printf("\n");
#endif



  pgradrms = 10000;
  Hintp_to_Hint();

  double energyp;
//revert to lowest energy found
  double energyl;
  double gradrmsl;
  double* xyzl = new double[3*natoms];
  for (int j=0;j<3*natoms;j++) xyzl[j] = coords[j];

  if (SCALEQN>=SCALEQN0) SCALEQN = SCALEQN/1.2;
  if (SCALEQN<SCALEQN0) SCALEQN = SCALEQN0;
  if (DMAX<DMIN0) DMAX = DMIN0;
//  if (isTSnode) SCALEQN = 2.;
  ixflag = 0;

  do_bfgs = 0; //resets at each step
  int rflag = 0; //did backconvert work?
  int nrflag = 0;
  int bcp = 0;
  noptdone = 1;

  double energy = grad1.grads(coords, grad, Ut, 1) - V0;

  energyp = energy;
  energyl = energy;
#if 0
  ofstream xyzfile;
  xyzfile.open(xyzfile_string.c_str());
  xyzfile.setf(ios::fixed);
  xyzfile.setf(ios::left);
  xyzfile << setprecision(6);
#endif


  sprintf(sbuff,"\n"); printout += sbuff;
  for (int n=0;n<OPTSTEPS;n++)
  {
    //printf(" ocl"); fflush(stdout);
    //printf(" Opt step(n%2i): %i ",n+1);
    //printf(" Opt step(TS:%i): %i ",isTSnode,n+1);
    sprintf(sbuff," Opt step: %2i ",n+1); printout += sbuff;

#if USE_PRIMA
    energy += prima_force();
#endif

   //might want to move
    bmatp_create();
    bmat_create();

    grad_to_q();
    //print_gradq();
    if (use_constraint)
    {
      sprintf(sbuff," gqc: %4.3f",gradq[nicd0-1]); printout += sbuff;
    }

    if (do_bfgs) update_bfgsp(1);
    save_hess();
    do_bfgs = 1;

#if 0
    xyzfile << " " << natoms << endl;
    xyzfile << " " << energy << endl;
    for (int i=0;i<natoms;i++) 
    {
      xyzfile << "  " << anames[i];
      xyzfile << " " << coords[3*i+0] << " " << coords[3*i+1] << " " << coords[3*i+2];
      xyzfile << endl;
    }
#endif

  //  print_grad();
  //  print_gradq();
#if HESS_TANG && !USE_MOLPRO && !QCHEMSF
    update_ic_eigen_h(Cn,Cn);  
#else
    update_ic_eigen();

    if (isTSnode) walk_up();
#endif


 
    if (n==0) gradrmsl = gradrms;
    if ( (gradrms<gradrmsl && energy<energyl) ||
          energy<energyl-5.)
    {
      gradrmsl = gradrms;
      energyl = energy;
      for (int j=0;j<3*natoms;j++) xyzl[j] = coords[j];
    }

    rflag = ic_to_xyz_opt();
    update_ic();
    //print_xyz();
    if (rflag)
    {
      nrflag++;
      DMAX = DMAX/1.6;
      //MAXAD = MAXAD/1.1;
    //  SCALEQN *= 1.5;
      sprintf(sbuff," updating SCALE "); printout += sbuff;
#if HESS_TANG && !USE_MOLPRO && !QCHEMSF
      update_ic_eigen_h(Cn,Cn); 
#else
      update_ic_eigen();
#endif
      rflag = ic_to_xyz_opt();
      update_ic();
      do_bfgs = 0;
    }
    if (nrflag > 4) break;

    if (ixflag>2)
    {
      DMAX = DMAX/1.5;
      //MAXAD = MAXAD/1.1;
      sprintf(sbuff," bc problem, r Ut "); printout += sbuff;
      if (nicd!=nicd0)
      { 
        bmatp_create();
        bmatp_to_U();
        opt_constraint(C);
      }
      else 
      {
        bmatp_create();
        bmatp_to_U();
      }
      make_Hint();
      do_bfgs = 0;
      ixflag = 0;
      bcp = 1;
    }
    else bcp = 0;
    //printf(" oc3"); fflush(stdout);

    sprintf(sbuff," E(M): %1.2f gRMS: %1.4f",energy,gradrms); printout += sbuff;
    if (gradrms<OPTTHRESH && !bcp) 
    {
      sprintf(sbuff," * \n"); printout += sbuff;
      break;
    }
    if (n<OPTSTEPS-1)
    {
      noptdone++;
      energy = grad1.grads(coords, grad, Ut, 1) - V0;
      if (energy > 1000.) { gradrms = 1.; break; }

      double dE = energy - energyp;
      energyp = energy;
      //if (abs(dEpre)<0.05) dEpre = sign(dEpre)*0.05; 
      double ratio = dE/dEpre;
      sprintf(sbuff," ratio: %2.3f ",ratio); printout += sbuff;
#if STEPCONTROLG
      if (gradrms>pgradrms)
      {
        sprintf(sbuff," decreasing DMAX "); printout += sbuff;
        if (smag<DMAX)
          DMAX = smag / 1.1;
        else
          DMAX = DMAX / 1.2;
      }
      if (gradrms<pgradrms/1.25)
      {
        sprintf(sbuff," increasing DMAX "); printout += sbuff;
        DMAX = DMAX * 1.05;
        if (DMAX > 0.25)
          DMAX = 0.25;
      }
#endif
#if STEPCONTROL
      if (dE > 0.001 && !isTSnode)
      {
        sprintf(sbuff," dE>0, decreasing DMAX "); printout += sbuff;
        if (smag<DMAX)
          DMAX = smag / 1.5;
        else
          DMAX = DMAX / 1.5;
      }
      else if ((ratio < 0.25 || ratio > 1.5) && abs(dEpre)>0.05)
      {
        sprintf(sbuff," decreasing DMAX "); printout += sbuff;
        if (smag<DMAX)
          DMAX = smag / 1.1;
        else
          DMAX = DMAX / 1.2;
      }
      else if (ratio > 0.75 && ratio < 1.25 && smag>DMAX && gradrms<pgradrms*1.35)
      {
        sprintf(sbuff," increasing DMAX "); printout += sbuff;
        DMAX = DMAX * 1.1;
        if (DMAX > 0.25)
          DMAX = 0.25;
      }
      if (DMAX<DMIN0) DMAX = DMIN0;
#endif
      //printf(" oc4"); fflush(stdout);
    }
    sprintf(sbuff,"\n"); printout += sbuff;
    pgradrms = gradrms;
  } //loop n over OPTSTEPS
  //printf(" ocle"); fflush(stdout);

#if 1
//  if ( (gradrms>gradrmsl && energy>energyl) 
//    || gradrms>gradrmsl*2)
//  if (gradrms>gradrmsl*1.5 && energy>energyl+1.0
//       && !isTSnode)
  if ((gradrms>gradrmsl*1.75 && !isTSnode && revertOpt)
   || (gradrms>gradrmsl*3.0 && revertOpt))
  {
    //SCALEQN *= 1.85; //was 1.5
    if (DMAX>smag)
      DMAX = smag/1.5;
    else
      DMAX = DMAX/1.5;

    sprintf(sbuff,"r"); printout += sbuff;
    for (int j=0;j<3*natoms;j++)
      coords[j] = xyzl[j];
    energy = energyl;
    gradrms = gradrmsl;
    for (int j=0;j<nicd0;j++) gradq[j] = 0.0;
//    make_Hint();
  }
  else if (gradrms>gradrmsl*1.5 && !isTSnode)
  {
    sprintf(sbuff,"S"); printout += sbuff;
    //SCALEQN *= 1.35; //was 1.25 
    if (DMAX>smag)
      DMAX = smag/1.25;
    else
      DMAX = DMAX/1.25;
  }
#endif


  //printf(" ocd"); fflush(stdout);
  delete [] dots;
  delete [] Cn;
  delete [] xyzl;
  //printf(" ocd"); fflush(stdout);

  return energy;
}




double ICoord::opt_r(string xyzfile_string, int nsteps, double* C, double* C0, double* D, int rtype)
{
  printout = "";

  ridge = rtype;

  if (use_constraint && nicd==nicd0)
    opt_constraint(C);

  int OPTSTEPS = nsteps;
  //printf("  \n"); 
  
  int len0 = nbonds+nangles+ntor;
  int len = nicd0;
  for (int i=0;i<len;i++)
    dq0[i] = 0.;

//Creating Tangent Vector in Delocalized coordinates

  bmatp_create();
  bmat_create();

  double* dots = new double[len];
  double* Cn = new double[len0];
  for (int i=0;i<len;i++) dots[i] = 0.;
  for (int i=0;i<len0;i++) Cn[i] = 0.;
  for (int i=0;i<len;i++)
  for (int j=0;j<len0;j++)
    dots[i] += Ut[i*len0+j]*C[j];
  for (int i=0;i<len;i++)
  for (int j=0;j<len0;j++)
    Cn[j] += dots[i]*Ut[i*len0+j];
  double norm = 0.;
  for (int i=0;i<len0;i++)
    norm += Cn[i]*Cn[i];
  norm = sqrt(norm);
  for (int j=0;j<len0;j++)
    Cn[j] = Cn[j]/norm;

  double* Dn = new double[len0];
  for (int i=0;i<len;i++) dots[i] = 0.;
  for (int i=0;i<len0;i++) Dn[i] = 0.;
  for (int i=0;i<len;i++)
  for (int j=0;j<len0;j++)
    dots[i] += Ut[i*len0+j]*D[j];
  for (int i=0;i<len;i++)
  for (int j=0;j<len0;j++)
    Dn[j] += dots[i]*Ut[i*len0+j];
  norm = 0.;
  for (int i=0;i<len0;i++)
    norm += Dn[i]*Dn[i];
  norm = sqrt(norm);
  for (int j=0;j<len0;j++)
    Dn[j] = Dn[j]/norm;

#if 0
  printf(" Cn norm: %1.2f \n",norm);
  printf(" C:");
  for (int j=0;j<len0;j++)
    printf(" %1.2f",C[j]);
  printf("\n");
  printf(" Cn:");
  for (int j=0;j<len0;j++)
    printf(" %1.2f",Cn[j]);
  printf("\n");
#endif
#if 0
  printf(" Dn norm: %1.2f \n",norm);
  printf(" D:");
  for (int j=0;j<len0;j++)
    printf(" %1.2f",D[j]);
  printf("\n");
  printf(" Dn:");
  for (int j=0;j<len0;j++)
    printf(" %1.2f",Dn[j]);
  printf("\n");
#endif


  //pgradrms = 10000;
  //Hintp_to_Hint();

  double energyp;
//revert to lowest energy found
  double energyl;
  double gradrmsl;
  double* xyzl = new double[3*natoms];
  for (int j=0;j<3*natoms;j++) xyzl[j] = coords[j];

  if (SCALEQN>=SCALEQN0) SCALEQN = SCALEQN/1.2;
  if (SCALEQN<SCALEQN0) SCALEQN = SCALEQN0;
  if (DMAX<DMIN0) DMAX = DMIN0;
  ixflag = 0;

  do_bfgs = 0; //resets at each step
  int rflag = 0; //did backconvert work?
  int nrflag = 0;

  double energy = grad1.grads(coords, grad, Ut, 1) - V0;
  if (energy > 1000.) return 10000.;

  energyp = energy;
  energyl = energy;
#if 1
  ofstream xyzfile;
  xyzfile.open(xyzfile_string.c_str());
  xyzfile.setf(ios::fixed);
  xyzfile.setf(ios::left);
  xyzfile << setprecision(6);
#endif


  sprintf(sbuff,"\n"); printout += sbuff;
  for (int n=0;n<OPTSTEPS;n++)
  {
    //printf(" Opt step(n%2i): %i ",id+1,n+1);
    //printf(" Opt step(TS:%i): %i ",isTSnode,n+1);
    sprintf(sbuff," step: %2i",noptdone+1); printout += sbuff;

#if USE_PRIMA
    energy += prima_force();
#endif

   //might want to move
    bmatp_create();
    bmat_create();

    grad_to_q();
    //print_gradq();

    if (do_bfgs)
    {
//      if (use_constraint)
//        update_bfgsp(1);
//      else
        update_bofill();
    }
    do_bfgs = 1;
    save_hess();

#if 0
    xyzfile << " " << natoms << endl;
    xyzfile << " " << energy << endl;
    for (int i=0;i<natoms;i++) 
    {
      xyzfile << "  " << anames[i];
      xyzfile << " " << coords[3*i+0] << " " << coords[3*i+1] << " " << coords[3*i+2];
      xyzfile << endl;
    }
#endif

    //print_grad();
    //print_gradq();
    if (use_constraint && !isTSnode)
      update_ic_eigen();
    else
      update_ic_eigen_h(Cn,Dn);
    //if (isTSnode && use_constraint) walk_up();
 //CPMZ here
    if (ridge==3)
      ridge = 4;

 
    if (n==0) gradrmsl = gradrms;
    if ( (gradrms<gradrmsl && energy<energyl) ||
          energy<energyl-5.)
    {
      gradrmsl = gradrms;
      energyl = energy;
      for (int j=0;j<3*natoms;j++) xyzl[j] = coords[j];
    }

    rflag = ic_to_xyz_opt();
    update_ic();
    //print_xyz();
    if (rflag)
    {
      nrflag++;
      DMAX = DMAX/1.6;
      //MAXAD = MAXAD/1.1;
    //  SCALEQN *= 1.5;
      sprintf(sbuff," updating SCALE "); printout += sbuff;
      if (use_constraint && !isTSnode)
        update_ic_eigen();
      else
        update_ic_eigen_h(Cn,Dn);
      //update_ic_eigen();        
      ic_to_xyz_opt();
      update_ic();
      do_bfgs = 0;
    }
    if (nrflag > 4) break;

    if (ixflag>2)
    {
      DMAX = DMAX/1.5;
      //MAXAD = MAXAD/1.1;
      sprintf(sbuff," bc problem, r Ut "); printout += sbuff;
      if (nicd!=nicd0)
      { 
        bmatp_create();
        bmatp_to_U();
        opt_constraint(C);
      }
      else 
      {
        bmatp_create();
        bmatp_to_U();
      }
      make_Hint();
      do_bfgs = 0;
      ixflag = 0;
    }

    if (n<OPTSTEPS-1)
    {
      noptdone++;
      energy = grad1.grads(coords, grad, Ut, 1) - V0;
      if (energy > 1000.) { gradrms = 1.; break; }

      double dE = energy - energyp;
      energyp = energy;
      //if (abs(dEpre)<0.05) dEpre = sign(dEpre)*0.05; 
      double ratio = dE/dEpre;
      sprintf(sbuff," r: %2.2f ",ratio); printout += sbuff;
#if STEPCONTROLG
      if (gradrms>pgradrms)
      {
        sprintf(sbuff," decreasing DMAX "); printout += sbuff;
        if (smag<DMAX)
          DMAX = smag / 1.1;
        else
          DMAX = DMAX / 1.2;
      }
      if (gradrms<pgradrms/1.25)
      {
        sprintf(sbuff," increasing DMAX "); printout += sbuff;
        DMAX = DMAX * 1.05;
        if (DMAX > 0.25)
          DMAX = 0.25;
      }
#endif
#if STEPCONTROL
      if (dE > 0.001 && !isTSnode)
      {
        sprintf(sbuff," dE>0, decreasing DMAX "); printout += sbuff;
        if (smag<DMAX)
          DMAX = smag / 1.5;
        else
          DMAX = DMAX / 1.5;
      }
      else if (ratio < 0.25 && abs(dEpre)>0.05)
      {
        sprintf(sbuff," decreasing DMAX "); printout += sbuff;
        if (smag<DMAX)
          DMAX = smag / 1.1;
        else
          DMAX = DMAX / 1.2;
      }
      else if (ratio > 0.75 && ratio < 1.5 && smag>DMAX && gradrms<pgradrms*1.35)
      {
        sprintf(sbuff,sbuff," increasing DMAX "); printout += sbuff;
        DMAX = DMAX * 1.1;
        if (DMAX > 0.25)
          DMAX = 0.25;
      }
#endif
    }
    sprintf(sbuff," E(M): %1.2f gRMS: %1.4f",energy,gradrms); printout += sbuff;

//    if ( (gradrms<OPTTHRESH && nicd==nicd0 && ridge==0) 
    if ( (gradrms<OPTTHRESH && nicd==nicd0 && abs(path_overlap_e_g)<OPTTHRESH) 
      || (nicd!=nicd0 && abs(gradq[nicd0-1])<OPTTHRESH) )
    {
      sprintf(sbuff," * \n"); printout += sbuff;
      break;
    }

    sprintf(sbuff,"\n"); printout += sbuff;
    pgradrms = gradrms;
  }

#if 0
//  if ( (gradrms>gradrmsl && energy>energyl) 
//    || gradrms>gradrmsl*2)
//  if (gradrms>gradrmsl*1.5 && energy>energyl+1.0
//       && !isTSnode)
  if ((gradrms>gradrmsl*1.75 && !isTSnode) 
   || (gradrms>gradrmsl*3.0))
  {
    //SCALEQN *= 1.85; //was 1.5
    if (DMAX>smag)
      DMAX = smag/1.5;
    else
      DMAX = DMAX/1.5;

    sprintf(sbuff,"r"); printout += sbuff;
    for (int j=0;j<3*natoms;j++)
      coords[j] = xyzl[j];
    energy = energyl;
    gradrms = gradrmsl;
//    make_Hint();
  }
  else if (gradrms>gradrmsl*1.5 && !isTSnode)
  {
    sprintf(sbuff,"S"); printout += sbuff;
    //SCALEQN *= 1.35; //was 1.25 
    if (DMAX>smag)
      DMAX = smag/1.25;
    else
      DMAX = DMAX/1.25;
  }
#endif

#if 1
    xyzfile << " " << natoms << endl;
    xyzfile << " " << energy << endl;
    for (int i=0;i<natoms;i++) 
    {
      xyzfile << "  " << anames[i];
      xyzfile << " " << coords[3*i+0] << " " << coords[3*i+1] << " " << coords[3*i+2];
      xyzfile << endl;
    }
#endif

  delete [] dots;
  delete [] Cn;
  delete [] xyzl;

  return energy;
}



double ICoord::opt_eigen_ts(string xyzfile_string, int nsteps, double* C, double* C0)
{
  //printf("\n\n in opt_eigen_ts \n\n");
  printout = "";

  use_constraint = 0; //use overlap if possible

  int OPTSTEPS = nsteps; 
  int stepinc = 1;
  //printf("  \n"); 
  
  int len0 = nbonds+nangles+ntor;
  int len = nicd0;

//Creating Tangent Vector in Delocalized coordinates
  bmatp_create();
  bmat_create();

  double* dots = new double[len];
  double* Cn = new double[len0];
  for (int i=0;i<len;i++) dots[i] = 0.;
  for (int i=0;i<len0;i++) Cn[i] = 0.;
  for (int i=0;i<len;i++)
  for (int j=0;j<len0;j++)
    dots[i] += Ut[i*len0+j]*C[j];
  for (int i=0;i<len;i++)
  for (int j=0;j<len0;j++)
    Cn[j] += dots[i]*Ut[i*len0+j];
  double norm = 0.;
  for (int i=0;i<len0;i++)
    norm += Cn[i]*Cn[i];
  norm = sqrt(norm);
  for (int j=0;j<len0;j++)
    Cn[j] = Cn[j]/norm;
#if 0
  printf(" Cn norm: %1.2f \n",norm);
  printf(" C:");
  for (int j=0;j<len0;j++)
    printf(" %1.2f",C[j]);
  printf("\n");
  printf(" Cn:");
  for (int j=0;j<len0;j++)
    printf(" %1.2f",Cn[j]);
  printf("\n");
#endif

  double energyp;
//revert to lowest energy found
  double energyl;
  double gradrmsl = 0.;
  double gradrms1 = 0.;
  double* xyzl = new double[3*natoms];
  for (int j=0;j<3*natoms;j++) xyzl[j] = coords[j];

  int update_hess = 1;
  if (pgradrms==10000.) update_hess = 0;
//  update_hess = 0;

  //printf(" SCALEQN: %14.10f SCALEQN0: %14.10f \n",SCALEQN,SCALEQN0);
  if (SCALEQN>SCALEQN0) SCALEQN = SCALEQN/1.2;
  if (SCALEQN<SCALEQN0) SCALEQN = SCALEQN0;
//  DMAX *= 1.005;
  if (DMAX<DMIN0) DMAX = DMIN0;
  ixflag = 0;

  int rflag = 0; //did backconvert work?
  int nrflag = 0;
  noptdone = 1;

  double energy = grad1.grads(coords, grad, Ut, 3) - V0;
  if (energy > 1000.) return 10000.;

  energyp = energy;
  energyl = energy;
#if 0
  ofstream xyzfile;
  xyzfile.open(xyzfile_string.c_str());
  xyzfile.setf(ios::fixed);
  xyzfile.setf(ios::left);
  xyzfile << setprecision(6);
#endif

  //printf(" \n beginning opt \n\n");

  sprintf(sbuff,"\n"); printout += sbuff;
  for (int n=0;n<OPTSTEPS;n++)
  {
    sprintf(sbuff," Opt step: %2i ",n+1); printout += sbuff;

#if USE_PRIMA
    energy += prima_force();
#endif

    grad_to_q();
    //print_gradq();

    if (update_hess) update_bofill();
    save_hess();
    update_hess = 1;

#if 0
    xyzfile << " " << natoms << endl;
    xyzfile << " " << energy << endl;
    for (int i=0;i<natoms;i++) 
    {
      xyzfile << "  " << anames[i];
      xyzfile << " " << coords[3*i+0] << " " << coords[3*i+1] << " " << coords[3*i+2];
      xyzfile << endl;
    }
#endif

    if (n==0) gradrmsl = gradrms;
    if (gradrms<gradrmsl)
    {
      gradrmsl = gradrms;
      energyl = energy;
      for (int j=0;j<3*natoms;j++) xyzl[j] = coords[j];
    }
    if (gradrms<OPTTHRESH) break;

    update_ic_eigen_ts(Cn);
    rflag = ic_to_xyz_opt();
    update_ic();
    bmatp_create();
    bmat_create();

    //print_ic();
    //print_xyz();
 
    if (rflag)
    {
      nrflag++;
      DMAX = DMAX/1.5;
    //  MAXAD = MAXAD/1.1;
    //  SCALEQN *= 1.5;
      sprintf(sbuff," updating SCALE "); printout += sbuff;
      update_ic();
      update_hess = 0;
    }

    if (ixflag>2)
    {
      DMAX = DMAX/1.5;
      //MAXAD = MAXAD/1.1;
      sprintf(sbuff," bc problem, r Ut "); printout += sbuff;
    //  bmatp_create();
    //  bmatp_to_U();
    //  make_Hint();
      ixflag = 0;
      update_hess = 0;
      nneg = 4;
    }
    if (nneg>3 && !useExactH) break;

    if (n==0) gradrms1 = gradrms;
    if (n<OPTSTEPS-1)
    {
      noptdone++;
      energy = grad1.grads(coords, grad, Ut, 3) - V0;
      if (energy > 1000.) { gradrms = 1.; break; }

#if STEPCONTROL
      double dE = energy - energyp;
      energyp = energy;
      //if (abs(dEpre)<0.05) dEpre = sign(dEpre)*0.05; 
      double ratio = dE/dEpre;
      sprintf(sbuff," ratio: %2.3f ",ratio); printout += sbuff;
      if (ratio < 0. && abs(dEpre)>0.05)
      {
        sprintf(sbuff," sign problem, decreasing DMAX "); printout += sbuff;
        DMAX = DMAX / 1.35;
      }
      else if ((ratio < 0.75 || ratio > 1.5) && abs(dEpre)>0.05)
      {
        sprintf(sbuff," decreasing DMAX "); printout += sbuff;
        if (smag<DMAX)
          DMAX = smag / 1.1;
        else
          DMAX = DMAX / 1.2;
      }
      else if (ratio > 0.85 && ratio < 1.3 && smag>DMAX && gradrms<pgradrms*1.35)
      {
        sprintf(sbuff," increasing DMAX "); printout += sbuff;
        DMAX = DMAX * 1.1;
        if (DMAX > 0.15)
          DMAX = 0.15;
      }
#endif
      sprintf(sbuff," E(M): %1.2f gRMS: %1.4f \n",energy,gradrms); printout += sbuff;
    } //if not last opt step
    pgradrms = gradrms;

#if NEARCONVTS
    if (n+1==OPTSTEPS && stepinc && gradrms < OPTTHRESH*5.)
    {
      stepinc = 0; 
      OPTSTEPS *= 2;
    }
#endif
  } // loop over n

  sprintf(sbuff," E(M): %1.2f",energy); printout += sbuff;
  sprintf(sbuff," gRMS: %1.4f ",gradrms); printout += sbuff;
  if (gradrms<OPTTHRESH)
  {
    sprintf(sbuff," *"); printout += sbuff;
  }
  sprintf(sbuff,"\n"); printout += sbuff;

#if 1
  if (gradrms>gradrms1*1.75) //was 1.5
  {
    //printf(" grms: %1.4f grms1: %1.4f ",gradrms,gradrms1);
    sprintf(sbuff,"S"); printout += sbuff;
    //SCALEQN *= 1.25;
    if (DMAX>smag)
      DMAX = smag/1.5;
    else
      DMAX = DMAX/1.5;
  }
  if ((nneg>3 && !useExactH) || gradrms>=1.0 || energy>energyl+50.)
  {
    sprintf(sbuff,"tsr"); printout += sbuff;
    for (int j=0;j<3*natoms;j++)
      coords[j] = xyzl[j];
    energy = energyl;
    gradrms = gradrmsl;
  }
#endif


  delete [] dots;
  delete [] Cn;
  delete [] xyzl;

  return energy;
}


//here
void ICoord::force_notbonds()
{
  //printf(" pushing back on nearby unbonded atom pairs \n");

  int N3 = natoms*3;
  double* dqbdx = new double[6];
  double gradrms1 = 0.;
  for (int i=0;i<N3;i++)
    gradrms1+=grad[i]*grad[i];
  gradrms1 = sqrt(gradrms1/N3);
  //printf(" gradrms1: %4.3f \n",gradrms1);

  if (gradrms1>0.01) gradrms1 = 0.01;
  double pushscale = 30.*gradrms1;


//  int i = 27;
//  int j = 26;

  //for pairs where !bond_exists
  for (int i=0;i<natoms;i++)
  for (int j=0;j<i;j++)
  if (!bond_exists(i,j))
  {

    double d = distance(i,j);
//    if (d<getR(i)+getR(j))
    if (d < ffR[i] + ffR[j])
    {
      bmatp_dqbdx(i,j,dqbdx);

      double f0;
      double f1;
      f0  = dqbdx[0]*grad[3*i+0];
      f0 += dqbdx[1]*grad[3*i+1];
      f0 += dqbdx[2]*grad[3*i+2];
      f1  = dqbdx[3]*grad[3*j+0];
      f1 += dqbdx[4]*grad[3*j+1];
      f1 += dqbdx[5]*grad[3*j+2];

      if (i==27 && j==26)
      {
        printf("  close notbond: %2i %2i: %4.3f scale: %4.3f \n",i,j,d,pushscale);
        printf(" forces0: %4.3f",f0+f1);
        //printf(" forces0: %4.3f %4.3f total: %4.3f",f0,f1,f0+f1);
      }

      vdw_grad_1(i,j,pushscale);

#if 1
      if (i==27 && j==26)
      {
        f0  = dqbdx[0]*grad[3*i+0];
        f0 += dqbdx[1]*grad[3*i+1];
        f0 += dqbdx[2]*grad[3*i+2];
        f1  = dqbdx[3]*grad[3*j+0];
        f1 += dqbdx[4]*grad[3*j+1];
        f1 += dqbdx[5]*grad[3*j+2];
        printf(" forces1: %4.3f \n",f0+f1);
        //printf(" forces1: %4.3f %4.3f total: %4.3f \n",f0,f1,f0+f1);
      }
#endif


    }
  } //loop over nonbonded pairs

  delete [] dqbdx;

  return;
}

void ICoord::save_hess()
{
#if WRITE_HESS
  //if (g_inited) printf(" wrote_grad: %i \n",grad1.wrote_grad);
  if (g_inited && grad1.wrote_grad)
  {
    string nstrh = StringTools::int2str(grad1.gradcalls+grad1.res_t,4,"0");
    string filename = "scratch/qcsave"+runend2+"."+nstrh+".icp";
    //printf(" saving hess file %s \n",filename.c_str());
    save_hesspu(filename);
  }
#endif

  return;
}
///Transforms the gradient in Cartesian representation to delocalized internal coordinate representation
int ICoord::grad_to_q() {

#if USE_NOTBONDS
    force_notbonds();
#endif

  //printf(" in grad_to_q \n"); print_grad();

  int N3 = 3*natoms;
  int len_d = nicd0;
  int len0 = nbonds+nangles+ntor;
//  for (int i=0;i<N3;i++)
//    pgrad[i] = grad[i];
  for (int i=0;i<len_d;i++)
    pgradq[i] = gradq[i];
  for (int i=0;i<len_d;i++)
    gradq[i] = 0.0;

  //printf(" gtype: %i %i \n",grad1.xyz_grad,g_inited);

  if (frozen!=NULL)
  for (int i=0;i<natoms;i++)
  if (frozen[i])
    grad[3*i+0] = grad[3*i+1] = grad[3*i+2] = 0.;

  if (grad1.xyz_grad==1 || !g_inited)
  for (int i=0;i<len_d;i++)
  for (int j=0;j<N3;j++)
    gradq[i] += bmatti[i*N3+j] * grad[j];

  if (grad1.xyz_grad==0 && g_inited)
  for (int i=0;i<len_d;i++)
    gradq[i] = grad[i];
 
  gradrms = 0.;
  for (int i=0;i<nicd;i++)
    gradrms+=gradq[i]*gradq[i];
  gradrms = sqrt(gradrms/nicd);
  //print_gradq();

#if 1
// for Hessian update
  for (int i=0;i<len0;i++) pgradqprim[i] = gradqprim[i];
  for (int i=0;i<len0;i++) gradqprim[i] = 0.;
  for (int i=0;i<len0;i++)
  for (int j=0;j<len_d;j++)
    gradqprim[i] += Ut[j*len0+i]*gradq[j];
#endif

#if 0
  if (g_inited) print_gradq();
#elif 0
  print_gradq();
#endif


  return 0;
}



// steepest ascent
void ICoord::walk_up()
{

//  printf(" walking up! ");
  sprintf(sbuff," gts: %1.4f ",gradq[nicd0-1]); printout += sbuff;
//  if(abs(gradq[nicd0-1])>MAXAD)
//    gradq[nicd0-1]=sign(gradq[nicd0-1])*MAXAD;  

  double SCALEW = 1.0;
  double SCALE = SCALEQN*1.0;
  dq0[nicd0-1] = gradq[nicd0-1]/SCALE;
  if (fabs(dq0[nicd0-1])>MAXAD/SCALEW) dq0[nicd0-1] = sign(dq0[nicd0-1])*MAXAD/SCALEW;
//  if (fabs(dq0[nicd0-1])>DMAX/SCALEW) dq0[nicd0-1] = sign(dq0[nicd0-1])*DMAX/SCALEW;

#if STEPCONTROL
  dEpre += dq0[nicd0-1] * gradq[nicd0-1] * 627.5;
#endif
  sprintf(sbuff," predE: %5.2f ",dEpre); printout += sbuff;

  return;
}


#if 0
// steepest descent
void ICoord::update_ic_sd()
{
  int len = nicd;
  for (int i=0;i<len;i++)
    if(abs(gradq[i])>MAXAD)
      gradq[i]=sign(gradq[i])*MAXAD;  

  double SCALE = SCALESD;
  for (int i=0;i<len;i++)
    dq0[i] = -gradq[i]/SCALE;
  for (int i=0;i<len;i++)
    dqm1[i] = dq0[i];


  return;
}


void ICoord::update_ic_cg()
{
  int len = nicd;
  for (int i=0;i<len;i++)
    if(abs(gradq[i])>MAXAD)
      gradq[i]=sign(gradq[i])*MAXAD;

//  if (gradrms<pgradrms) SCALESD = SCALESD/1.05;

  double SCALE = SCALESD;
  double SCALE2 = gradrms*gradrms/(pgradrms*pgradrms);
  if (SCALE2 > 1) SCALE2 = 1;
  SCALE2 = SCALE2 / SCALECG;
//  printf(" GRADRMS: %1.4f SCALE: %1.2f SCALE2: %1.2f \n",gradrms,SCALE,SCALE2);
  for (int i=0;i<len;i++)
    dq0[i] = -gradq[i]/SCALE + dqm1[i]*SCALE2;
  for (int i=0;i<len;i++)
    dqm1[i] = dq0[i];

  return;
}
#endif

void ICoord::update_ic_qn()
{
  int len0 = nicd0;
  int len = nicd;

  //print_gradq();
#if 0
  for (int i=0;i<len;i++)
    if(abs(gradq[i])>MAXAD)
      gradq[i]=sign(gradq[i])*MAXAD;
#endif

  //if (pgradrms*1.05<gradrms) SCALEQN *= 1.5;
  //if (pgradrms>gradrms) SCALEQN /= 1.1;
  double SCALE = SCALEQN;
  if (SCALE>10.0) SCALE = 10.;
  //printf(" GRADRMS: %1.3f PGRADRMS: %1.3f SCALE: %1.3f \n",gradrms,pgradrms,SCALE);

  for (int i=0;i<len0;i++) dq0[i] = 0.;
  for (int i=0;i<len;i++)
  for (int j=0;j<len;j++)
    dq0[i] += -Hinv[i*len0+j] * gradq[j] / SCALE;

  for (int i=0;i<len;i++)
    if(abs(dq0[i])>MAXAD)
      dq0[i]=sign(dq0[i])*MAXAD;

#if 0
  printf(" dq0: ");
  for (int i=0;i<len0;i++)
    printf(" %1.4f",dq0[i]);
  printf("\n");
#endif

  return;
}



void ICoord::update_ic_eigen()
{
  //printf(" in update_ic_eigen, use_constraint: %i isTSnode: %i \n",use_constraint,isTSnode);

  path_overlap_n = 0;
  path_overlap = 0.;

  int len0 = nicd0;
  int len = nicd;
  double* tmph = new double[len*len];
  double* eigen = new double[len];
  double* gqe = new double[len];
  double* dqe0 = new double[len];
  double lambda1 = 0.;

  //print_gradq();
#if 0
  for (int i=0;i<len;i++)
    if(abs(gradq[i])>MAXAD)
      gradq[i]=sign(gradq[i])*MAXAD;
#endif

  double SCALE = SCALEQN;
  if (newHess>0) SCALE = SCALEQN*newHess;
  if (SCALE>10.0) SCALE = 10.;
//  printf(" GRADRMS: %1.3f PGRADRMS: %1.3f SCALE: %1.3f \n",gradrms,pgradrms,SCALE);
//  printf(" SCALE: %1.3f \n",SCALE);

  for (int i=0;i<len;i++)
  for (int j=0;j<len;j++)
    tmph[i*len+j] = Hint[i*len0+j];
  Diagonalize(tmph,eigen,len);

  //replace Hessian eigenvectors with MW-Hessian eigenvectors
//  create_mwHint_EV(tmph,eigen); 
//exit(1);
  double leig = eigen[0];

  if (leig < 0.)
    lambda1 = -leig + 0.015;
  else
    lambda1 = 0.005;
//  if (abs(lambda0)<0.005) lambda0 = 0.005;
  if (abs(lambda1)<0.005) lambda1 = 0.005;

 // lambda1 = 0.0;

  for (int i=0;i<len;i++) gqe[i] = 0.;
  for (int i=0;i<len;i++)
  for (int j=0;j<len;j++)
    gqe[i] += tmph[i*len+j]*gradq[j];

  for (int i=0;i<len;i++)
    dqe0[i] = -gqe[i] / (eigen[i]+lambda1) / SCALE;

#if 1
  for (int i=0;i<len;i++)
    if(abs(dqe0[i])>MAXAD)
      dqe0[i]=sign(dqe0[i])*MAXAD;
#endif

//convert grad to new basis
//take step uphill on first vector
//downhill elsewhere
//convert back to q 

  for (int i=0;i<len;i++) dq0[i] = 0.;
  for (int i=0;i<len;i++)
  for (int j=0;j<len;j++)
    dq0[i] += tmph[j*len+i] * dqe0[j];

  for (int i=0;i<len;i++)
    if(abs(dq0[i])>MAXAD)
      dq0[i]=sign(dq0[i])*MAXAD;


//regulate max overall step
  smag = 0.;
  for (int i=0;i<len;i++)
    smag += dq0[i]*dq0[i];
  smag = sqrt(smag);
  sprintf(sbuff," ss: %1.3f (DMAX: %1.3f)",smag,DMAX); printout += sbuff;
  if (smag > DMAX)
  {
    for (int i=0;i<len;i++)
      dq0[i] = dq0[i]*DMAX/smag;
  }

//Compute predicted change in energy
  double* dEtmp = new double[len];
  for (int i=0;i<len;i++) dEtmp[i] = 0.;
  dEpre = 0.;
  //first order
  for (int i=0;i<len;i++)
    dEpre += dq0[i] * gradq[i];
  //second order
  for (int i=0;i<len;i++)
  for (int j=0;j<len;j++)
    dEtmp[i] += Hint[i*len0+j]*dq0[j];
  for (int i=0;i<len;i++)
    dEpre += 0.5*dEtmp[i]*dq0[i];
  dEpre = dEpre * 627.5;

  if (!isTSnode)
  {
    sprintf(sbuff," predE: %5.2f ",dEpre); printout += sbuff;
  }
  delete [] dEtmp;


#if 0
  printf(" tmph: \n");
  for (int i=0;i<len;i++)
  {
    for (int j=0;j<len;j++)
      printf(" %1.4f",tmph[i*len+j]);
    printf("\n");
  }
#endif
#if 0
  printf(" eigen opt Hint ev:");
//  for (int i=0;i<len;i++)
  for (int i=0;i<4;i++)
    printf(" %1.3f",eigen[i]);
//  printf("\n");
#endif
#if 0
  printf(" gradq: ");
  for (int i=0;i<len;i++)
    printf(" %1.4f",gradq[i]);
  printf("\n");
#endif
#if 0
  printf(" gqe: ");
  for (int i=0;i<len;i++)
    printf(" %1.4f",gqe[i]);
  printf("\n");
#endif
#if 0
  printf(" dqe0: ");
  for (int i=0;i<len;i++)
    printf(" %1.4f",dqe0[i]);
  printf("\n");
#endif
#if 0
  printf(" dq0: ");
  for (int i=0;i<len0;i++)
    printf(" %1.4f",dq0[i]);
  printf("\n");
#endif

  delete [] tmph;
  delete [] eigen;
  delete [] gqe;
  delete [] dqe0;

  return;
}




void ICoord::update_ic_eigen_h(double* Cn, double* Dn)
{

  //printf(" in update_ic_eigen_h, use_constraint: %i isTSnode: %i \n",use_constraint,isTSnode);
  if (use_constraint) 
  {
    update_ic_eigen();
    if (isTSnode)
      walk_up();
    return;
  }


  int len = nicd0;
  int len0 = nbonds+nangles+ntor;
  double* tmph = new double[len*len];
  double* eigen = new double[len];
  double* gqe = new double[len];
  double* dqe0 = new double[len];
  double lambda1 = 0.;

  //print_gradq();
#if 0
  for (int i=0;i<len;i++)
    if(abs(gradq[i])>MAXAD)
      gradq[i]=sign(gradq[i])*MAXAD;
#endif

  double SCALE = SCALEQN;
  if (newHess>0) SCALE = SCALEQN*newHess;
  if (SCALE>10.0) SCALE = 10.;
  //printf(" GRADRMS: %1.3f PGRADRMS: %1.3f SCALE: %1.3f \n",gradrms,pgradrms,SCALE);
  //printf(" S: %1.1f ",SCALE);

  for (int i=0;i<len;i++)
  for (int j=0;j<len;j++)
    tmph[i*len+j] = Hint[i*len+j];
  Diagonalize(tmph,eigen,len);

  nneg = 0;
  for (int i=0;i<len;i++)
  if (eigen[i]<-0.01)
    nneg++;
//  if (nneg>1)
//    gradrms += OPTTHRESH*(nneg-1);


//Overlap metric
  double* Cd = new double[len0];
  double* overlap = new double[len];
  for (int i=0;i<len;i++) overlap[i] = 0.;
  //printf(" t/ol:");
  for (int n=0;n<len;n++)
  {
    for (int i=0;i<len0;i++) Cd[i] = 0.;
    for (int i=0;i<len;i++)
    for (int j=0;j<len0;j++)
      Cd[j] += tmph[n*len+i]*Ut[i*len0+j];

    for (int j=0;j<len0;j++)
      overlap[n] += Cd[j]*Cn[j];
#if 0
    printf(" Cd:");
    for (int j=0;j<len0;j++)
      printf(" %1.2f",Cd[j]);
    printf("\n");
#endif
#if 0
    if (n==0) overlap[n] = 1.;
    else overlap[n] = 0.;
#endif
    //printf(" %1.2f",abs(overlap[n]));
  }
  //printf("\n");

  double maxol = 0.;
  double maxols = 0.;
  int maxoln = 0;
#if USE_PRIMA
  double maxolp = 0.;
  double maxolps = 0.;
  int maxolnp = 0;

  double* overlapp = new double[len];
  for (int n=0;n<len;n++) overlapp[n] = 0.;
  for (int n=0;n<len;n++)
  {
    for (int i=0;i<len0;i++) Cd[i] = 0.;
    for (int i=0;i<len;i++)
    for (int j=0;j<len0;j++)
      Cd[j] += tmph[n*len+i]*Ut[i*len0+j];

    for (int j=0;j<len0;j++)
      overlapp[n] += Cd[j]*Cp[j];
  }
  for (int n=0;n<len;n++)
  if (abs(overlapp[n])>maxolp)
  {
    maxolp = abs(overlapp[n]);
    maxolps = overlapp[n];
    maxolnp = n;
  }
  sprintf(sbuff," p/ol: %i",maxolnp); printout += sbuff;
  sprintf(sbuff," (%3.2f)",maxolp); printout += sbuff;

//CPMZ here
  int found = 0;
  if (isTSnode && maxolp > 0.001)
  for (int n=0;n<len;n++)
#if !RIBBONS
  if (abs(overlap[n])>maxol)
#elif 0
  if (abs(overlap[n])>maxol && n!=maxolnp)
#else
  if (abs(overlap[n])>HESS_TANG_TOL_TS && n!=maxolnp)
#endif
  {
    maxol = abs(overlap[n]);
    maxols = overlap[n];
    maxoln = n;
    found = 1;
    break;
  }
  if (!isTSnode || !found)
  for (int n=0;n<len;n++)
  if (abs(overlap[n])>maxol)
  {
    maxol = abs(overlap[n]);
    maxols = overlap[n];
    maxoln = n;
  }
  delete [] overlapp;
#else
  for (int n=0;n<len;n++)
  if (abs(overlap[n])>maxol)
  {
    maxol = abs(overlap[n]);
    maxols = overlap[n];
    maxoln = n;
  }
#endif
  path_overlap = maxol;
  path_overlap_n = maxoln;
  sprintf(sbuff," t/ol: %i",maxoln); printout += sbuff;
  sprintf(sbuff," (%3.2f)",maxol); printout += sbuff;
  //printf(" (f%i)",maxoln);

  delete [] Cd;
  delete [] overlap;


//if overlap is small, use Cn constraint
#if USE_PRIMA && 0
  if (maxol<HESS_TANG_TOL)
#elif !RIBBONS && 1
  if (maxol<HESS_TANG_TOL || gradrms>OPTTHRESH*20.) //was 10. 
#else
  if (maxol<HESS_TANG_TOL_TS)
#endif
  {
    //printf(" maxol: %4.3f/%4.3f gradrms: %4.3f/%4.3f \n",maxol,HESS_TANG_TOL,gradrms,OPTTHRESH*10.);
    delete [] tmph;
    delete [] eigen;
    delete [] gqe;
    delete [] dqe0;

    opt_constraint(Cn);
    bmatp_create();
    bmat_create();
    Hintp_to_Hint();
   //must get gradient again for KNNR
    if (grad1.xyz_grad==0)
      double energy = grad1.grads(coords, grad, Ut, 1) - V0;
    grad_to_q();
    //save_hess();
    use_constraint = 1;
    update_ic_eigen();
    if (isTSnode)
      walk_up();
    return;
  }
 

  double leig = eigen[1];
  if (maxoln!=0) leig = eigen[0];

  if (leig<0.0)
    lambda1 = -leig + 0.015;
  else
    lambda1 = 0.005;
  if (abs(lambda1)<0.005) lambda1 = 0.005;
  //sprintf(sbuff," l1: %4.3f",lambda1); printout += sbuff;

  for (int i=0;i<len;i++) gqe[i] = 0.;
  for (int i=0;i<len;i++)
  for (int j=0;j<len;j++)
    gqe[i] += tmph[i*len+j]*gradq[j];


 //fixed along max overlap direction
  if (!isTSnode)
    dqe0[maxoln] = 0.;
  else
  {
    dqe0[maxoln] = gqe[maxoln] / (abs(eigen[maxoln]) + lambda1) / SCALE;
//    dqe0[maxoln] = gqe[maxoln] / SCALE;
    path_overlap_e_g = gqe[maxoln];
    sprintf(sbuff," gtse: %1.4f ",gqe[maxoln]); printout += sbuff;
  }
  for (int i=0;i<len;i++)
  if (i!=maxoln)
    dqe0[i] = -gqe[i] / (abs(eigen[i])+lambda1) / SCALE;


  //default move is "forward"
  if (ridge==2)
  {
    printf(" 1st ridge step \n");
//negative -0.15 allowed TS finding at Silane
    if (maxoln==0)
      dqe0[1] = -0.15;
    else
      dqe0[0] = -0.15;
  }

  //first step move along
  int maxolnd = 0;
  if (ridge>2)
  {
    double maxold = 0.;
    double maxolds = 0.;
    double* overlapd = new double[len];
    Cd = new double[len0];
    for (int n=0;n<len;n++) overlapd[n] = 0.;
    for (int n=0;n<len;n++)
    {
      for (int i=0;i<len0;i++) Cd[i] = 0.;
      for (int i=0;i<len;i++)
      for (int j=0;j<len0;j++)
        Cd[j] += tmph[n*len+i]*Ut[i*len0+j];

      for (int j=0;j<len0;j++)
        overlapd[n] += Cd[j]*Dn[j];
    }
#if 0
    printf(" overlapd:");
    for (int n=0;n<len;n++) 
      printf(" %4.3f",overlapd[n]);
    printf("\n");
#endif
    for (int n=0;n<len;n++)
    if (abs(overlapd[n])>maxold && n!=maxoln)
    {
      maxold = abs(overlapd[n]);
      maxolds = overlapd[n];
      maxolnd = n;
    }

    if (ridge==3)
      dqe0[maxolnd] = sign(maxolds)*RIDGE_STEP_SIZE;
    else if (ridge==4)
      dqe0[maxolnd] = 0.;
    sprintf(sbuff," max r/ol: %i",maxolnd); printout += sbuff;
    sprintf(sbuff," (%3.2f)",maxold); printout += sbuff;
    sprintf(sbuff," gtser: %1.4f ",gqe[maxolnd]); printout += sbuff;

    if (maxolds*gqe[maxolnd]>0.)
    {
      sprintf(sbuff," sign+"); printout += sbuff;
    }
    else
    {
      sprintf(sbuff," sign-"); printout += sbuff;
    }

    delete [] overlapd; 
    delete [] Cd;
  } //if climbing the ridge




#if 0
  for (int i=0;i<len;i++)
    if(abs(dqe0[i])>MAXAD)
      dqe0[i]=sign(dqe0[i])*MAXAD;
#endif

//convert grad to new basis
//take step uphill on first vector
//downhill elsewhere
//convert back to q 

  for (int i=0;i<len;i++) dq0[i] = 0.;
  for (int i=0;i<len;i++)
  for (int j=0;j<len;j++)
    dq0[i] += tmph[j*len+i] * dqe0[j];

  for (int i=0;i<len;i++)
    if(abs(dq0[i])>MAXAD)
      dq0[i]=sign(dq0[i])*MAXAD;

//regulate max overall step
  smag = 0.;
  for (int i=0;i<len;i++)
    smag += dq0[i]*dq0[i];
  smag = sqrt(smag);
  sprintf(sbuff," ss: %1.3f (%1.3f)",smag,DMAX); printout += sbuff;
//  printf(" ss: %1.3f",smag);
  if (smag > DMAX)
  {
    for (int i=0;i<len;i++)
      dq0[i] = dq0[i]*DMAX/smag;
  //  printf(" scaled to %1.3f",DMAX);
  }


//Compute predicted change in energy
  double* dEtmp = new double[len];
  for (int i=0;i<len;i++) dEtmp[i] = 0.;
  dEpre = 0.;
  //first order
  for (int i=0;i<len;i++)
    dEpre += dq0[i] * gradq[i];
  //second order
  for (int i=0;i<len;i++)
  for (int j=0;j<len;j++)
    dEtmp[i] += Hint[i*len+j]*dq0[j];
  for (int i=0;i<len;i++)
    dEpre += 0.5*dEtmp[i]*dq0[i];
  dEpre = dEpre * 627.5;

  sprintf(sbuff," predE: %5.2f ",dEpre); printout += sbuff;
  delete [] dEtmp;


#if !RIBBONS
  //adjust gradrms to account for eigenvector constraint
  gradrms = gradrms*gradrms*nicd0-gqe[maxoln]*gqe[maxoln];
  gradrms = sqrt(gradrms/nicd0);
#endif
#if RIBBONS
  //recalc gradrms to account for perpendicular constraint
  gradrms = gradrms*gradrms*nicd0-gqe[maxolnd]*gqe[maxolnd];
  gradrms = sqrt(gradrms/nicd0);

//  gradrms = 0.;
//  for (int i=0;i<len;i++)
//  if (i!=maxolnd)
//    gradrms += gqe[i]*gqe[i];
//  gradrms = sqrt(gradrms/nicd0);
#endif


#if 0
  printf(" tmph: \n");
  for (int i=0;i<len;i++)
  {
    for (int j=0;j<len;j++)
      printf(" %1.4f",tmph[i*len+j]);
    printf("\n");
  }
#endif
#if 1
  sprintf(sbuff," Hint ev:"); printout += sbuff;
//  for (int i=0;i<len;i++)
  for (int i=0;i<2;i++)
  {
    sprintf(sbuff," %1.3f",eigen[i]); printout += sbuff;
  }
//  printf("\n");
#endif
#if 0
  printf(" gradq: ");
  for (int i=0;i<len;i++)
    printf(" %1.4f",gradq[i]);
  printf("\n");
#endif
#if 0
  printf(" gqe: ");
  for (int i=0;i<len;i++)
    printf(" %1.4f",gqe[i]);
  printf("\n");
#endif
#if 0
  printf(" dqe0: ");
  for (int i=0;i<len;i++)
    printf(" %1.4f",dqe0[i]);
  printf("\n");
#endif
#if 0
  printf(" dq0: ");
  for (int i=0;i<len;i++)
    printf(" %1.4f",dq0[i]);
  printf("\n");
#endif

  delete [] tmph;
  delete [] eigen;
  delete [] gqe;
  delete [] dqe0;

  return;
}



void ICoord::update_ic_eigen_ts(double* Cn)
{

  if (use_constraint) 
  {
    update_ic_eigen();
    if (isTSnode)
      walk_up();
    return;
  }

  int len = nicd;
  int len0 = nbonds+nangles+ntor;
  double* tmph = new double[len*len];
  double* eigen = new double[len];
  double* gqe = new double[len];
  double* dqe0 = new double[len];
  double lambda0 = 0.;
  double lambda1 = 0.;

  //print_gradq();
#if 0
  for (int i=0;i<len;i++)
    if(abs(gradq[i])>MAXAD)
      gradq[i]=sign(gradq[i])*MAXAD;
#endif

  double SCALE = SCALEQN;
  if (newHess>0) SCALE = SCALEQN*newHess;
  if (SCALE>10.0) SCALE = 10.;
  //printf(" GRADRMS: %1.3f PGRADRMS: %1.3f SCALE: %1.3f \n",gradrms,pgradrms,SCALE);
  sprintf(sbuff," S: %1.1f ",SCALE); printout += sbuff;

#if 0
  printf("  in update_ic_eigen_ts (use_contraint: %i), printing Hessian \n",use_constraint);
  for (int i=0;i<len;i++)
  {  
    for (int j=0;j<len;j++)
      printf(" %8.5f",Hint[i*len+j]);
    printf("\n");
  }
#endif


  for (int i=0;i<len;i++)
  for (int j=0;j<len;j++)
    tmph[i*len+j] = Hint[i*len+j];
  Diagonalize(tmph,eigen,len);


//Overlap metric
  double* Cd = new double[len0];
  double* overlap = new double[len];
  for (int i=0;i<len;i++) overlap[i] = 0.;
  sprintf(sbuff," ol:"); printout += sbuff;
  for (int n=0;n<4;n++)
  {
    for (int i=0;i<len0;i++) Cd[i] = 0.;
    for (int i=0;i<len;i++)
    for (int j=0;j<len0;j++)
      Cd[j] += tmph[n*len+i]*Ut[i*len0+j];

    for (int j=0;j<len0;j++)
      overlap[n] += Cd[j]*Cn[j];
#if 0
    printf(" Cd:");
    for (int j=0;j<len0;j++)
      printf(" %1.2f",Cd[j]);
    printf("\n");
#endif
#if 0
    if (n==0) overlap[n] = 1.;
    else overlap[n] = 0.;
#endif
    sprintf(sbuff," %1.2f",abs(overlap[n])); printout += sbuff;
  }
  sprintf(sbuff,"\n"); printout += sbuff;

  double maxol = 0;
  int maxoln = 0;
  for (int n=0;n<len;n++)
  if (abs(overlap[n])>maxol)
  {
    maxol = abs(overlap[n]);
    maxoln = n;
  }
  path_overlap = maxol;
  path_overlap_n = maxoln;
  sprintf(sbuff," max ol: %i",maxoln); printout += sbuff;

//  if (isDavid)
//  if (isDavid && maxol < HESS_TANG_TOL_TS)
//    path_overlap_n = maxoln = 0;

  nneg = 0;

  sprintf(sbuff," (f%i)",maxoln); printout += sbuff;
  if (maxol<0.5 && !isDavid) nneg = 4; //trigger Hessian reset
  if (maxol<0.45 && isDavid) nneg = 4; //trigger Hessian reset
#if QCHEM
  //mopac often has weird eigenvalue structure
  if (eigen[0] < -1.0) nneg = 4;
#endif

  for (int i=0;i<len;i++)
//  if ((eigen[i]<-0.005 && maxoln!=0) || eigen[i]<-0.005) 
  if (eigen[i]<-0.01 || (useExactH && eigen[i]<0.))
    nneg++;
//  if (nneg>1)
//    gradrms += OPTTHRESH*(nneg-1);


  delete [] Cd;
  delete [] overlap;

#if HESS_TANG
//if overlap is small, use Cn constraint
  if (maxol<HESS_TANG_TOL_TS && !isDavid && !useExactH) // || gradrms>OPTTHRESH*40.) 
  {
    delete [] tmph;
    delete [] eigen;
    delete [] gqe;
    delete [] dqe0;

    nneg = 4;
    return;

    opt_constraint(Cn);
    bmatp_create();
    bmat_create();
    Hintp_to_Hint();
   //must get gradient again for KNNR
    if (grad1.xyz_grad==0)
      double energy = grad1.grads(coords, grad, Ut, 1) - V0;
    grad_to_q();
    //save_hess();
    use_constraint = 1;
    update_ic_eigen();
    if (isTSnode)
      walk_up();
    return;
  }
#endif


  double leig = eigen[1];
  if (maxoln!=0) leig = eigen[0];

  lambda0 = 0.;
  if (leig<0.0 && maxoln==0)
    lambda1 = -leig;
  else
    lambda1 = 0.01;
  if (abs(lambda0)<0.0025) lambda0 = 0.0025;
  if (abs(lambda1)<0.005) lambda1 = 0.005;

  for (int i=0;i<len;i++) gqe[i] = 0.;
  for (int i=0;i<len;i++)
  for (int j=0;j<len;j++)
    gqe[i] += tmph[i*len+j]*gradq[j];

  dqe0[maxoln] = gqe[maxoln] / (abs(eigen[maxoln]) + lambda0) / SCALE;
  for (int i=0;i<len;i++)
  if (i!=maxoln)
    dqe0[i] = -gqe[i] / (abs(eigen[i])+lambda1) / SCALE;

#if 0
  printf(" dqe0:");
  for (int i=0;i<len;i++)
    printf(" %8.5f",dqe0[i]);
  printf("\n");
#endif

  double negstep = MIN_NEG_STEP;
  if (useExactH)
  for (int i=0;i<nneg;i++)
  if (fabs(dqe0[i])<negstep && i!=maxoln)
  {
    printf(" dqe0: %8.5f-->%8.5f \n",dqe0[i],sign(dqe0[i])*negstep);
    dqe0[i] = sign(dqe0[i])*negstep;
  }
 
#if 0
  printf(" DEBUG: moving oddly! \n");
  dqe0[1] = 0.0025;
  dqe0[2] = 0.01;
#endif

#if 0
  for (int i=0;i<len;i++)
    if(abs(dqe0[i])>MAXAD)
      dqe0[i]=sign(dqe0[i])*MAXAD;
#endif

//convert grad to new basis
//take step uphill on first vector
//downhill elsewhere
//convert back to q 

  for (int i=0;i<len;i++) dq0[i] = 0.;
  for (int i=0;i<len;i++)
  for (int j=0;j<len;j++)
    dq0[i] += tmph[j*len+i] * dqe0[j];

  for (int i=0;i<len;i++)
    if(abs(dq0[i])>MAXAD)
      dq0[i]=sign(dq0[i])*MAXAD;

//regulate max overall step
  smag = 0.;
  for (int i=0;i<len;i++)
    smag += dq0[i]*dq0[i];
  smag = sqrt(smag);
  sprintf(sbuff," ss: %1.3f (DMAX: %1.3f)",smag,DMAX); printout += sbuff;
//  printf(" ss: %1.3f",smag);
  if (smag > DMAX)
  {
    for (int i=0;i<len;i++)
      dq0[i] = dq0[i]*DMAX/smag;
  //  printf(" scaled to %1.3f",DMAX);
  }


//Compute predicted change in energy
  double* dEtmp = new double[len];
  for (int i=0;i<len;i++) dEtmp[i] = 0.;
  dEpre = 0.;
  //first order
  for (int i=0;i<len;i++)
    dEpre += dq0[i] * gradq[i];
  //second order
  for (int i=0;i<len;i++)
  for (int j=0;j<len;j++)
    dEtmp[i] += Hint[i*len+j]*dq0[j];
  for (int i=0;i<len;i++)
    dEpre += 0.5*dEtmp[i]*dq0[i];
  dEpre = dEpre * 627.5;

  sprintf(sbuff," predE: %5.2f ",dEpre); printout += sbuff;
  delete [] dEtmp;


#if 0
  printf(" tmph: \n");
  for (int i=0;i<len;i++)
  {
    for (int j=0;j<len;j++)
      printf(" %1.4f",tmph[i*len+j]);
    printf("\n");
  }
#endif
#if 1
  sprintf(sbuff," Hint ev:"); printout += sbuff;
//  for (int i=0;i<len;i++)
  for (int i=0;i<4;i++)
  {
    sprintf(sbuff," %1.3f",eigen[i]); printout += sbuff;
  }
//  printf("\n");
#endif
#if 0
  printf(" gradq: ");
  for (int i=0;i<len;i++)
    printf(" %1.4f",gradq[i]);
  printf("\n");
#endif
#if 0
  printf(" gqe: ");
  for (int i=0;i<len;i++)
    printf(" %1.4f",gqe[i]);
  printf("\n");
#endif
#if 0
  printf(" dqe0: ");
  for (int i=0;i<len;i++)
    printf(" %1.4f",dqe0[i]);
  printf("\n");
#endif
#if 0
  printf(" dq0: ");
  for (int i=0;i<len;i++)
    printf(" %1.4f",dq0[i]);
  printf("\n");
#endif


  delete [] tmph;
  delete [] eigen;
  delete [] gqe;
  delete [] dqe0;

  return;
}




void ICoord::print_gradq(){

  //printf(" Gradient in delocalized IC:\n");
  printf(" gradq(curr):");
  int len = nicd0;
  for (int i=0;i<len;i++)
    printf(" %12.10f",gradq[i]);
  printf("\n");

  return;
}

void ICoord::print_q(){

  printf(" q in delocalized IC:\n");
  int len_d = nicd;
  printf(" printing q: \n");
  for (int i=0;i<len_d;i++)
    printf(" %1.2f",q[i]);
  printf(" \n");

  return;
}

///back transforms from delocalized IC representation to Cartesian represention
int ICoord::ic_to_xyz() {

  int MAX_STEPS = 10; //was 6

  int success = 1;

  int N3 = 3*natoms;
  //int len0 = min(N3,nbonds+nangles+ntor);
  int len = nicd0;
  double** xyzall = new double*[MAX_STEPS+2];
  for (int i=0;i<MAX_STEPS+2;i++)
    xyzall[i] = new double[N3];
  double* magall = new double[MAX_STEPS+2];
  for (int i=0;i<MAX_STEPS+2;i++)
    magall[i] = 100.;
  double* xyz1 = new double[N3];
  double* xyzd = new double[N3];
  //double* xyzd0 = new double[N3];
  double* btit = new double[N3*len];
  double* dq = new double[len];
  double* qn = new double[len]; //target IC values

  for (int i=0;i<N3;i++)
    xyzall[0][i] = coords[i];

  update_ic();
  bmatp_create();
  bmat_create();

  for (int i=0;i<len;i++)
    dq[i] = dq0[i];
  for (int i=0;i<len;i++)
    qn[i] = q[i] + dq[i];

#if 0
    printf(" qn:");
    for (int i=0;i<len;i++)
      printf(" %1.4f",qn[i]);
    printf("\n");
#endif
#if 0
  printf(" dq at start: \n");
  for (int i=0;i<len;i++)
    printf(" %1.4f",dq[i]);
  printf("\n");
#endif

  double MAX_MOVE = 0.75;
  double mag = 0.;
  double magp = 100.;
  double SCALEBT = 1.5;
  for (int n=0;n<MAX_STEPS;n++) 
  {
    trans(btit,bmatti,N3,len);

    for (int i=0;i<N3;i++) xyzd[i] = 0.;
    for (int i=0;i<N3;i++)
    for (int j=0;j<len;j++) 
      xyzd[i] += btit[len*i+j] * dq[j];

    if (frozen!=NULL)
    for (int i=0;i<natoms;i++)
    if (frozen[i])
      xyzd[3*i+0] = xyzd[3*i+1] = xyzd[3*i+2] = 0.;

    mag = 0.;
    for (int i=0;i<N3;i++)
      mag += xyzd[i]*xyzd[i];

#if 0
   //was on, why does it exist?
    if (mag>1.*natoms)
    { 
      printf(" rsc");
      double rsc=1.*natoms/mag;
      for (int i=0;i<N3;i++)
        xyzd[i] = xyzd[i]*rsc;
    }
#endif
#if 0
    for (int i=0;i<N3;i++)
      if (abs(xyzd[i])>MAX_MOVE)
        xyzd[i] = sign(xyzd[i])*MAX_MOVE;
#endif

    for (int i=0;i<N3;i++)
      xyz1[i] = coords[i] + xyzd[i]/SCALEBT;

    //if(n==0)
    //  printf(" diff in xyz mag (start) is: %1.6f \n",sqrt(mag));
    for (int i=0;i<N3;i++)
      xyzall[n+1][i] = xyz1[i];
    magall[n] = mag;

    if (mag>magp)
      SCALEBT *= 1.5;
    magp = mag;

    for (int i=0;i<N3;i++)
      coords[i] = xyz1[i];
    update_ic();
    bmatp_create();
    bmat_create();

    for (int i=0;i<len;i++)
      dq[i] = qn[i] - q[i];

#if 0
    printf(" dq: \n");
    for (int i=0;i<len;i++)
      printf(" %1.4f",dq[i]);
    printf("\n");
#endif

    if (mag<0.00005) break;
  }

#if 0
  for (int i=0;i<natoms;i++)
    printf(" dX: %1.3f %1.3f %1.3f \n",xyzd[3*i+0],xyzd[3*i+1],xyzd[3*i+2]);
  printf("\n");
#endif

  //printf(" diff in xyz mag (end) is: %1.4f \n",sqrt(mag)); 
  double MAXMAG = 0.025*natoms;
  if (sqrt(mag)>MAXMAG)
  {
    //printf(" WARNING: diff in xyz mag (end) is: %1.4f, using first step, mag: %1.4f \n",sqrt(mag),sqrt(mag0));
    ixflag++;
    double maglow = 100.;
    int nlow = -1;
    for (int n=0;n<MAX_STEPS+2;n++)
    if (magall[n]<maglow)
    {
      nlow = n+1;
      maglow = magall[n];
    }
    if (maglow<MAXMAG)
    {
      for (int i=0;i<N3;i++)
        coords[i] = xyzall[nlow][i];
      printf("W(%6.5f/%i)",maglow,nlow);
    }
    else
    {
      for (int i=0;i<N3;i++)
        coords[i] = xyzall[0][i];
      printf("Wf(%6.5f/%i)",maglow,nlow);
      success = 0;
    }
  }
  else if (ixflag>0)
    ixflag = 0;

  delete [] btit;
  delete [] xyz1;
  delete [] xyzd;
  for (int i=0;i<MAX_STEPS+2;i++)
    delete [] xyzall[i];
  delete [] magall;
  delete [] xyzall;
  delete [] dq;
  delete [] qn;

  return success;
}



int ICoord::ic_to_xyz_opt() {

  int MAX_STEPS = 8; //was 6

  int rflag = 0;
  int retry = 0;

  int N3 = 3*natoms;
  //int len0 = min(N3,nbonds+nangles+ntor);
  int len = nicd0;
  double** xyzall = new double*[MAX_STEPS+2];
  for (int i=0;i<MAX_STEPS+2;i++)
    xyzall[i] = new double[N3];
  double* magall = new double[MAX_STEPS+2];
  for (int i=0;i<MAX_STEPS+2;i++)
    magall[i] = 100.;
  double* xyz1 = new double[N3];
  double* xyzp = new double[N3];
  double* xyzd = new double[N3];
  double* btit = new double[N3*len];
  double* dq = new double[len];
  double* qn = new double[len]; //target IC values

  update_ic();
  double* qprim = new double[nbonds+nangles+ntor+1];
  for (int i=0;i<nbonds;i++)
    qprim[i] = bondd[i];
  for (int i=0;i<nangles;i++)
    qprim[nbonds+i] = anglev[i];
  for (int i=0;i<ntor;i++)
    qprim[nbonds+nangles+i] = torv[i];
#if 0
  printf(" printing qprim: ");
  for (int i=0;i<nbonds+nangles+ntor;i++)
    printf(" %1.1f",qprim[i]);
  printf("\n");
#endif

  for (int i=0;i<N3;i++)
    xyzall[0][i] = coords[i];

//  trans(btit,bmatti,N3,len);

#if 0
  printf(" btit \n");
  for (int i=0;i<N3;i++)
  {
    for (int j=0;j<len;j++)
      printf(" %1.4f",btit[len*i+j]);
    printf("\n");
  }
  printf("\n");
#endif

  for (int i=0;i<len;i++)
    dq[i] = dq0[i];
  for (int i=0;i<len;i++)
    qn[i] = q[i] + dq[i];

#if 0
    printf(" qn:");
    for (int i=0;i<len;i++)
      printf(" %1.4f",qn[i]);
    printf("\n");
#endif
#if 0
  printf(" dq at start: \n");
  for (int i=0;i<len;i++)
    printf(" %1.4f",dq[i]);
  printf("\n");
#endif

//print_ic();
  double MAX_MOVE = 0.75; 
  double mag = 0.;
  double mag0;
  double magp = 100.;
  double dqmag;
  double dqmagp = 100.;

  double SCALEBT = 1.5;
  for (int n=0;n<MAX_STEPS;n++)
  {
    trans(btit,bmatti,N3,len);

    for (int i=0;i<N3;i++) xyzd[i] = 0.;
    for (int i=0;i<N3;i++)
    for (int j=0;j<len;j++) 
      xyzd[i] += btit[len*i+j] * dq[j];

    if (frozen!=NULL)
    for (int i=0;i<natoms;i++)
    if (frozen[i])
      xyzd[3*i+0] = xyzd[3*i+1] = xyzd[3*i+2] = 0.;

#if 0
    for (int i=0;i<N3;i++)
      if (abs(xyzd[i])>MAX_MOVE)
        xyzd[i] = sign(xyzd[i])*MAX_MOVE;
#endif

    for (int i=0;i<N3;i++)
      xyz1[i] = coords[i] + xyzd[i]/SCALEBT;

    mag = 0.;
    for (int i=0;i<N3;i++)
      mag += xyzd[i]*xyzd[i];
    //if(n==0)
    //  printf(" diff in xyz mag (start) is: %1.6f \n",sqrt(mag));
    for (int i=0;i<N3;i++)
      xyzall[n+1][i] = xyz1[i];
    magall[n] = mag;

    for (int i=0;i<N3;i++)
      xyzp[i] = coords[i];

    for (int i=0;i<N3;i++)
      coords[i] = xyz1[i];
    update_ic();
    bmatp_create();
    bmat_create();

    for (int i=0;i<len;i++)
      dq[i] = qn[i] - q[i];

    dqmag = 0.;
    for (int i=0;i<len;i++)
      dqmag += dq[i]*dq[i];
    dqmag = sqrt(dqmag);
    //printf(" dqmag: %1.3f",dqmag);
    if (dqmag<0.0001) break;

//    if (mag>mag)
    if (dqmag>dqmagp*10)
    {
      printf("Q%i",n);

      SCALEBT *= 2.0;
      for (int i=0;i<N3;i++)
        coords[i] = xyzp[i];
      update_ic();
      bmatp_create();
      bmat_create();

      for (int i=0;i<len;i++)
        dq[i] = qn[i] - q[i];
    }
    magp = mag;
    dqmagp = dqmag;

#if 0
    printf(" dq: \n");
    for (int i=0;i<len;i++)
      printf(" %1.4f",dq[i]);
    printf("\n");
#endif

    if (mag<0.00005) break;
    
  } //loop over back convert

#if 0
  for (int i=0;i<natoms;i++)
    printf(" dX: %1.3f %1.3f %1.3f \n",xyzd[3*i+0],xyzd[3*i+1],xyzd[3*i+2]);
  printf("\n");
#endif

  //printf(" diff in xyz mag (end) is: %1.4f \n",sqrt(mag)); 
  //printf(" dq[nicd0-1]: %1.2f ",dq[nicd0-1]);
  double MAXMAG = 0.025*natoms;
  if (sqrt(mag)>MAXMAG)
  {
   // printf(" WARNING: diff in xyz mag (end) is: %1.4f, not stepping, mag0: %1.4f \n",sqrt(mag),sqrt(mag0));
    ixflag++;
    double maglow = 100.;
    int nlow = -1;
    for (int n=0;n<MAX_STEPS+2;n++)
    if (magall[n]<maglow)
    {
      nlow = n+1;
      maglow = magall[n];
    }
    if (maglow<MAXMAG)
    {
      for (int i=0;i<N3;i++)
        coords[i] = xyzall[nlow][i];
      printf("Wb(%6.5f/%i)",maglow,nlow);
    }
    else
    {
      for (int i=0;i<N3;i++)
        coords[i] = xyzall[0][i];
      rflag = 1;
      printf("Wr(%6.5f/%i)",maglow,nlow);
      for (int i=0;i<nicd;i++)
        dq0[i] = dq0[i] / 2.0;
      retry = 1;
    }
  }
  else if (ixflag>0)
    ixflag = 0;

  if (!retry)
  {
    update_ic();
    for (int i=0;i<nbonds;i++)
      dqprim[i] = bondd[i] - qprim[i];
    for (int i=0;i<nangles;i++)
      dqprim[nbonds+i] = (anglev[i] - qprim[nbonds+i])*3.1415926/180.;
    for (int i=0;i<ntor;i++)
    {
      double torfix1;
      double tordiff = qprim[nbonds+nangles+i] - torv[i];
      if (tordiff>180)
        torfix1 = -360.;
      else if (tordiff<-180)
        torfix1 = 360.;
      else torfix1 = 0;

      //printf(" tordiff: %1.3f torfix: %1.3f \n",tordiff,torfix);
      dqprim[nbonds+nangles+i] = (tordiff + torfix1)*3.1415926/180.;
    }

#if 0
    printf(" printing dqprim: ");
    for (int i=0;i<nbonds+nangles+ntor;i++)
      printf(" %1.3f",dqprim[i]);
    printf("\n");
#endif
  }

  delete [] btit;
  for (int i=0;i<MAX_STEPS+2;i++)
    delete [] xyzall[i];
  delete [] magall;
  delete [] xyz1;
  delete [] xyzp;
  delete [] xyzd;
  delete [] dq;
  delete [] qn;

  delete [] qprim;

  if (!retry)
    return rflag;
  else
    return ic_to_xyz_opt();
}

void ICoord::read_hessxyz(string filename, int write)
{
  printf(" reading in Hxyz from file \n");
  int len = nbonds+nangles+ntor;
  int len_d = nicd0;
  int len0 = nicd0;
  int N3 = natoms*3;
  int size = len;
  if (N3>size) size = N3;

  ifstream hessfile;
  hessfile.open(filename.c_str());
  if (!hessfile){
    cout << " Error opening Hess file: " << filename << endl;
    exit(-1);
  }

  double* Hxyz = new double[N3*N3];
  for (int i=0;i<N3*N3;i++) Hxyz[i] = 0.;

  vector<string> tok_line;
  string line;
  bool success=true;

  int nf = 0;
  int found = 0;
  while(!hessfile.eof())
  {
    success = (bool)getline(hessfile,line);
    if (line.find("Hessian of the SCF Energy")!=string::npos)
    {
      found = 1;
      break;
    }
  }
  if (!found)
  {
    printf(" couldn't find Hessian! \n");
    exit(1);
  }
  while(!hessfile.eof() && found)
  {
    nf++;
    success = (bool)getline(hessfile,line);
    //cout << " RR0: " << line << endl;
    for (int j=0;j<N3;j++)
    {
      getline(hessfile,line);
      //cout << " RR: " << line << endl;
      int length=StringTools::cleanstring(line);
      vector<string> tok_line = StringTools::tokenize(line, " \t");
      for (int k=1;k<tok_line.size();k++)
        Hxyz[N3*j+6*(nf-1)+k-1]=atof(tok_line[k].c_str());
    }
    if (nf*6>=N3) break;
  } // while !eof

  make_Hint();
  if (!found)
  { 
    printf("  couldn't find Hessian, using default IC Hessian \n");
    return;
  }

 //now converting Hxyz to Hint
  double* tmp = new double[size*size];
  for (int i=0;i<size*size;i++) tmp[i] = 0.;

  for (int i=0;i<len_d;i++)
  for (int j=0;j<N3;j++)
  for (int k=0;k<N3;k++)
    tmp[i*N3+k] += bmatti[i*N3+j] * Hxyz[j*N3+k];

  for (int i=0;i<len_d*len_d;i++) Hint[i] = 0.;
  for (int i=0;i<len_d;i++)
  for (int j=0;j<N3;j++)
  for (int k=0;k<len_d;k++)
    Hint[i*len_d+k] += tmp[i*N3+j] * bmatti[k*N3+j];

  //update Hintp with Hint
  for (int i=0;i<size*size;i++) tmp[i] = 0.;
  for (int i=0;i<len;i++)
  for (int j=0;j<len_d;j++)
  for (int k=0;k<len_d;k++)
    tmp[i*len_d+k] += Ut[j*len+i]*Hint[j*len_d+k];

  for (int i=0;i<len*len;i++) Hintp[i] = 0.;
  for (int i=0;i<len;i++)
  for (int j=0;j<len;j++)
  for (int k=0;k<len_d;k++)
    Hintp[i*len+j] += tmp[i*len_d+k]*Ut[k*len+j];


  delete [] tmp;


#if 1
  if (N3<50)
  {
    printf(" found Hxyz: \n");
    for (int i=0;i<N3;i++)
    {
      for (int j=0;j<N3;j++)
        printf(" %6.3f",Hxyz[N3*i+j]);
      printf("\n");
    }
    printf("\n");
  }
#endif

#if 0
  printf(" Hintp from Hxyz: \n");
  for (int i=0;i<len;i++)
  {
    for (int j=0;j<len;j++)
      printf(" %6.3f",Hintp[len*i+j]);
    printf("\n");
  }
  printf("\n");
#endif

#if 0
  printf(" printing Hint from Hxyz \n");
  for (int i=0;i<len0;i++)
  {  
    for (int j=0;j<len0;j++)
      printf(" %8.5f",Hint[i*len0+j]);
    printf("\n");
  }
#endif


 //for debug
  //Hintp_to_Hint();

#if 1
  double* tmph = new double[len0*len0];
  double* eigen = new double[len0];
  for (int i=0;i<len0;i++) eigen[i] = 0.;
  for (int i=0;i<len0*len0;i++)
    tmph[i] = Hint[i];
  Diagonalize(tmph,eigen,len0);
  printf(" initial (read-in) Hint ev:");
  for (int i=0;i<len0;i++)
    printf(" %1.3f",eigen[i]);
  printf("\n");
  delete [] tmph;
  delete [] eigen;
#endif

  delete [] Hxyz;

#if WRITE_HESS
  if (write)
    save_hess();
#endif

  return;
}


void ICoord::read_hessp(string filename)
{
  printf(" reading in Hintp from file \n");
  int len = nbonds+nangles+ntor;

  ifstream hesspfile;
  hesspfile.open(filename.c_str());
  if (!hesspfile){
    cout << " Error opening Hessp file: " << filename << endl;
    exit(-1);
  }

  int natomsf = 0;
  int lenf = 0;
  vector<string> tok_line;
  string line;
  bool success=true;

  success=(bool)getline(hesspfile, line);
  //cout << " RR: " << line << endl;
  int length=StringTools::cleanstring(line);  
  tok_line = StringTools::tokenize(line, " \t");
  natomsf = atoi(tok_line[1].c_str());

  success=(bool)getline(hesspfile, line);
  //cout << " RR: " << line << endl;
  length=StringTools::cleanstring(line);  
  tok_line = StringTools::tokenize(line, " \t");
  lenf = atoi(tok_line[1].c_str());

  printf(" found %i atoms and %i coordinates \n",natomsf,lenf);
  if (natomsf!=natoms || lenf!=len)
  {
    printf(" mismatched size of Hessian matrix (atoms: %i/%i ic's: %i/%i) \n",natomsf,natoms,lenf,len);
    exit(-1);
  }

  for (int i=0;i<len;i++)
  {
    success=(bool)getline(hesspfile, line);
    length=StringTools::cleanstring(line);
    if (length<1) break;
    tok_line = StringTools::tokenize(line, " \t");

    for (int j=0;j<len;j++)
    {
      Hintp[i*len+j] = atof(tok_line[j].c_str());
    }
  }

#if 0
  printf(" printing Hintp \n");
  for (int i=0;i<len;i++)
  {
    for (int j=0;j<len;j++)
      printf(" %4.3f",Hintp[i*len+j]);
    printf("\n");
  }
#endif
#if 1
  Hintp_to_Hint();
  int len0 = nicd0;
  double* tmph = new double[len0*len0];
  double* eigen = new double[len0];
  for (int i=0;i<len0;i++) eigen[i] = 0.;
  for (int i=0;i<len0*len0;i++)
    tmph[i] = Hint[i];
  Diagonalize(tmph,eigen,len0);
  printf(" initial Hint ev:");
  for (int i=0;i<len0;i++)
    printf(" %1.3f",eigen[i]);
  printf("\n");
  delete [] tmph;
  delete [] eigen;
#endif

  return;
}

void ICoord::save_hessp(string filename)
{
  int len = nbonds+nangles+ntor;

  ofstream hesspfile;
  hesspfile.open(filename.c_str());
  hesspfile.setf(ios::fixed);
  hesspfile.setf(ios::left);
  hesspfile << setprecision(15);

  hesspfile << " natoms: " << natoms << endl;
  hesspfile << " pICs: " << len << " newHess: " << newHess << endl;

  for (int i=0;i<len;i++)
  {
    for (int j=0;j<len;j++)
      hesspfile << " " << Hintp[i*len+j];
    hesspfile << endl;
  }
 
  return;
}


void ICoord::save_hesspu(string filename)
{
  int len = nbonds+nangles+ntor;
  int len_d = nicd0;

  double* Hintpu = new double[len*len];
  for (int i=0;i<len*len;i++) Hintpu[i] = 0.;

  //create Hintp from Hint(U)
  double* tmp = new double[len*len];
  for (int i=0;i<len*len;i++) tmp[i] = 0.;
  for (int i=0;i<len;i++)
  for (int j=0;j<len_d;j++)
  for (int k=0;k<len_d;k++)
    tmp[i*len_d+k] += Ut[j*len+i]*Hint[j*len_d+k];

  for (int i=0;i<len*len;i++) Hintpu[i] = 0.;
  for (int i=0;i<len;i++)
  for (int j=0;j<len;j++)
  for (int k=0;k<len_d;k++)
    Hintpu[i*len+j] += tmp[i*len_d+k]*Ut[k*len+j];

  ofstream hesspfile;
  hesspfile.open(filename.c_str());
  hesspfile.setf(ios::fixed);
  hesspfile.setf(ios::left);
  hesspfile << setprecision(15);

  hesspfile << " natoms: " << natoms << endl;
  hesspfile << " pICs: " << len << " newHess: " << newHess << endl;

  for (int i=0;i<len;i++)
  {
    for (int j=0;j<len;j++)
      hesspfile << " " << Hintpu[i*len+j];
    hesspfile << endl;
  }

  delete [] tmp; 
  delete [] Hintpu;

  return;
}


int ICoord::create_prima(int nnodes0, int nbonds1, int nangles1, int ntor1, double** prima0)
{
  int len = nbonds+nangles+ntor;
  nnodes = nnodes0;

  mm_init();

  if (nbonds1!=nbonds)
    printf(" nbonds not equal: %i %i \n",nbonds1,nbonds);
  if (nangles1!=nangles)
    printf(" nangles not equal: %i %i \n",nangles1,nangles);
  if (ntor1!=ntor)
    printf(" ntor not equal: %i %i \n",ntor1,ntor);


  printf(" in create_prima, nnodes: %i \n",nnodes); fflush(stdout);

  aprima = new int[len];
  for (int j=0;j<len;j++)
    aprima[j] = 1;
  prima = new double[nnodes*len];
  for (int i=0;i<nnodes;i++)
  for (int j=0;j<len;j++)
    prima[i*len+j] = 0.;

  for (int i=0;i<nnodes;i++)
  for (int j=0;j<len;j++)
    prima[i*len+j] = prima0[i][j];

  for (int j=0;j<len;j++)
  if (prima0[0][j]==-999.)
  {
    aprima[j] = 0;
    for (int i=0;i<nnodes;i++)
      prima[i*len+j] = 0.;
  }

 //tangent arises to TS
  int n1 = 0;
  int n2 = 1;
 //prima tangent "overpasses" TS
//  n1 = 0;
//  n2 = 2;

  Cp = new double[len];
  for (int j=0;j<len;j++)
    Cp[j] = 0.;
  for (int j=0;j<nbonds;j++)
    Cp[j] = prima[n1*len+j] - prima[n2*len+j];
  for (int j=nbonds;j<nbonds+nangles;j++)
    Cp[j] = prima[n1*len+j] - prima[n2*len+j];
  for (int j=nbonds+nangles;j<len;j++)
  {
    Cp[j] = (prima[n1*len+j] - prima[n2*len+j])*3.14/180.;
    if (Cp[j]>3.14159)
      Cp[j] = -1*(2*3.14159 - Cp[j]);
    if (Cp[j]<-3.14159)
      Cp[j] = 2*3.14159 + Cp[j];
  }

  double norm = 0.;
  for (int j=0;j<len;j++)
    norm += Cp[j]*Cp[j];
  norm = sqrt(norm);
  for (int j=0;j<len;j++)
    Cp[j] = Cp[j]/norm;

#if 0 
  printf(" printing prima tangent Cp: \n");
  for (int j=0;j<len;j++)
    printf(" %4.3f",Cp[j]);
  printf("\n\n");
#endif

  return 0;
}

double ICoord::prima_force()
{
  //printf(" adding prima force \n"); fflush(stdout);
  //return 0.;

  int len = nbonds+nangles+ntor;
  int N3 = 3*natoms;
  update_ic();

  double energya = 0.;

  double* dist = new double[nnodes];
  for (int i=0;i<nnodes;i++) dist[i] = 0.;

  double* diff = new double[len];
  for (int i=0;i<len;i++) diff[i] = 0.;


  //here add force
  //print_grad();
  sprintf(sbuff," dist: "); printout += sbuff;
  double f1,f2;

  mdist = 0.;

  //for (int i=0;i<nnodes;i++)
 //just using restart TS node
  for (int i=1;i<2;i++)
  {
    double d;
    dist[i] = 0.;
    for (int j=0;j<nbonds;j++)
    if (aprima[j])
    {
      d = bondd[j] - prima[i*len+j];
      diff[j] = d;
      dist[i] += d*d;
    }
#if 1
    for (int j=0;j<nangles;j++)
    if (aprima[nbonds+j])
    {
      d = (anglev[j] - prima[i*len+nbonds+j])*3.14/180.;
      diff[nbonds+j] = d;
      dist[i] += d*d;
    }
#endif
#if 0
    for (int j=0;j<ntor;j++)
    if (aprima[nbonds+nangles+j])
    {
      d = (torv[j] - prima[i*len+nbonds+nangles+j])*3.14/180.;
      diff[nbonds+nangles+j] = d;
      dist[i] += d*d;
    }
#endif

#if 0
    double norm = 0.;
    for (int j=0;j<len;j++)
      norm += diff[j]*diff[j];
    norm = sqrt(norm);
    if (norm<0.0000001) norm = 1.;
    for (int j=0;j<len;j++)
      diff[j] = diff[j] / norm;
#endif

   //
    //double FMAG = 0.015; //was 0.05
    double sigma = 1.5; //was 1.25
    double f0 = 1.25;  //TS node extra force
    if (i!=1) f0 = 1.0; 
    f1 = FMAG * f0 * exp(-dist[i]/sigma);
    sprintf(sbuff," %3.2f (%3.2f)",dist[i],f1); printout += sbuff;

    if (f1>0.0001)
    {
      for (int j=0;j<nbonds;j++)
      if (aprima[j])
      {
        f2 = exp(-(diff[j]*diff[j])/sigma);
        lin_grad_1(bonds[j][0],bonds[j][1],sign(diff[j])*f1*f2);
      }
      for (int j=0;j<nangles;j++)
      if (aprima[nbonds+j])
      {
        f2 = exp(-(diff[nbonds+j]*diff[nbonds+j])/sigma);
       //using lin_grad in place of angle grad
        lin_grad_1(angles[j][0],angles[j][2],sign(diff[nbonds+j])*f1*f2);
      }
    } //if applying forces

    if (dist[i]<mdist)
      mdist = dist[i];

  } //loop i over nnodes
//  sprintf(sbuff,"\n"); printout += sbuff;

 
  //print_grad();

  delete [] diff;
  delete [] dist;

  return energya;
}


int ICoord::davidson_H(int nval)
{
  double DETHRESH = 0.0005;
  double DTHRESH = 0.00005;

  printf("\n creating %i lowest eigenvectors \n",nval);
  if (nval<=0) return 0;
  //if (nval>2) return 0;

  int nnegfound = 0;
  int noptdone = 0;

  grad1.write_on = 0;

 //get initial vectors
  int len = nicd0;
  int len0 = nbonds+nangles+ntor;
  double* eigen = new double[len];
  double* tmph = new double[len*len];
  for (int i=0;i<len;i++)
  for (int j=0;j<len;j++)
    tmph[i*len+j] = Hint[i*len+j];
  Diagonalize(tmph,eigen,len);

#if 1
  printf(" Hint eigenvalues:");
  for (int i=0;i<5;i++)
    printf(" %4.3f",eigen[i]);
  printf("\n");
#endif

  int MAX_VECS = 45*nval;
  double** vecs = new double*[MAX_VECS];
  for (int i=0;i<MAX_VECS;i++)
    vecs[i] = new double[len];
  for (int i=0;i<MAX_VECS;i++)
  for (int j=0;j<len;j++)
    vecs[i][j] = 0.;
 
  for (int i=0;i<nval;i++)
  for (int j=0;j<len;j++)
    vecs[i][j] = tmph[i*len+j];

  double* xyz0 = new double[3*natoms];
  for (int i=0;i<3*natoms;i++)
    xyz0[i] = coords[i];
  double* g1 = new double[len];
  double* g2 = new double[len];
  double** y = new double*[MAX_VECS];
  for (int i=0;i<MAX_VECS;i++)
    y[i] = new double[len];
  double* q1 = new double[len];
  double* G = new double[len*len];
  double* lamb = new double[len];     
  double* lambp = new double[len];     
  for (int i=0;i<len;i++) lamb[i] = 0.;
  for (int i=0;i<len;i++) lambp[i] = 0.;
  double** b = new double*[MAX_VECS];
  for (int i=0;i<MAX_VECS;i++)
    b[i] = new double[len];
  double** yn = new double*[MAX_VECS];
  for (int i=0;i<MAX_VECS;i++)
    yn[i] = new double[len];

  double E1,E2;

  int nvec = 0;
  int nnew = nval;
  int dcontinue = 1;
  double DIST = 0.025; //was 0.05
  int MAX_ITER = 10;
  if (len/nval>MAX_ITER)
    MAX_ITER = len/nval;
  printf(" maximum number of iterations: %i \n",MAX_ITER);
  for (int n=0;n<MAX_ITER;n++)
  {
    printf("\n Davidson iteration %i \n",n+1);

   //evaluate "sigma"
    //printf(" sigma -- nvec: %i nnew: %i \n",nvec,nnew);
    for (int i=0;i<nnew;i++)
    {
      int wv = nvec+i;

     //move forward along vector
      for (int j=0;j<len;j++)
        dq0[j] = vecs[wv][j] * DIST;
      ic_to_xyz();
      //print_xyz();

      E1 = grad1.grads(coords, grad, Ut, 3) - V0;

      for (int j=0;j<3*natoms;j++)
        coords[j] = xyz0[j];
      grad_to_q();
      for (int j=0;j<len;j++)
        g1[j] = gradq[j];

     //move reverse along vector
      for (int j=0;j<len;j++)
        dq0[j] = - vecs[wv][j] * DIST;
      //print_xyz();
      ic_to_xyz();
      //print_xyz();

      E2 = grad1.grads(coords, grad, Ut, 3) - V0;

      for (int j=0;j<3*natoms;j++)
        coords[j] = xyz0[j];
      grad_to_q();
      for (int j=0;j<len;j++)
        g2[j] = gradq[j];


      for (int j=0;j<len;j++)
        y[wv][j] = (g1[j] - g2[j]) / (2*DIST);

#if 0
      printf(" E1/E2: %4.3f %4.3f \n",E1,E2);
      printf(" printing vecs:");
      for (int j=0;j<len;j++)
        printf(" %4.3f",vecs[wv][j]);
      printf("\n");
      printf(" printing y:");
      for (int j=0;j<len;j++)
        printf(" %4.3f",y[wv][j]);
      printf("\n");
#endif

      noptdone += 2;
    } //loop i over eigenvectors
    nvec += nnew;

    for (int i=0;i<len*len;i++) G[i] = 0.;
    for (int i=0;i<nvec;i++)
    for (int j=0;j<nvec;j++)
    for (int k=0;k<len;k++)
      G[i*nvec+j] += y[i][k]*vecs[j][k];

#if 0
    printf(" printing G:\n");
    for (int i=0;i<nvec;i++)
    {
      for (int j=0;j<nvec;j++)
        printf(" %4.3f",G[i*nvec+j]);
      printf("\n");
    }
    printf("\n");
#endif

    Diagonalize(G,lamb,nvec);
#if 0
    printf(" printing G eigen:");
    for (int i=0;i<nvec;i++)
      printf(" %4.3f",lamb[i]);
    printf("\n");
#endif
    for (int i=0;i<nval;i++)
    if (fabs(lamb[i])>1.5)
    {
      printf(" large eigenvalue: %4.3f \n",lamb[i]);
      dcontinue = 0;
    }
    if (dcontinue==0)
      break;

    for (int i=0;i<nvec;i++)
    for (int j=0;j<len;j++)
      b[i][j] = yn[i][j] = 0.;

    for (int i=0;i<nval;i++)
    for (int j=0;j<nvec;j++)
    for (int k=0;k<len;k++)
      yn[i][k] += y[j][k]*G[i*nvec+j];

    for (int i=0;i<nval;i++)
    for (int j=0;j<nvec;j++)
    for (int k=0;k<len;k++)
      b[i][k] += vecs[j][k]*G[i*nvec+j];

#if 0
      printf(" printing yn[0]:");
      for (int j=0;j<len;j++)
        printf(" %4.3f",yn[0][j]);
      printf("\n");
      printf(" printing b[0]:");
      for (int j=0;j<len;j++)
        printf(" %4.3f",b[0][j]);
      printf("\n");
#endif

    //printf(" create -- nvec: %i nnew: %i \n",nvec,nnew);
    printf("\n");
    int nnew0 = nnew;
    nnew = 0;
    int done = 0;
    for (int i=0;i<nval;i++)
    {
      int wv = nvec + i;
      for (int j=0;j<len;j++)
        q1[j] = yn[i][j] - lamb[i]*b[i][j];

      for (int j=0;j<len;j++)
        vecs[wv][j] = q1[j] / (lamb[i] - eigen[j]);

      for (int j=0;j<wv;j++)
      {
        double dot = 0.;
        for (int k=0;k<len;k++)
          dot += vecs[j][k]*vecs[wv][k];
        for (int k=0;k<len;k++)
          vecs[wv][k] -= dot * vecs[j][k];
        //printf(" %i %i dot: %4.3f \n",j,wv,dot);
      }

      double mag = 0.;
      for (int j=0;j<len;j++)
        mag += vecs[wv][j]*vecs[wv][j];
      double norm = sqrt(mag);
      for (int j=0;j<len;j++)
        vecs[wv][j] = vecs[wv][j] / norm;
      //printf(" %i mag: %4.3f \n",i,mag);

#if 0
      printf(" new vector:");
      for (int j=0;j<len;j++)
        printf(" %4.3f",vecs[wv][j]);
      printf("\n");
      printf(" updated eigen vector:");
      for (int j=0;j<len;j++)
        printf(" %4.3f",b[i][j]);
      printf("\n");
#endif

      printf(" mag/DTHRESH: %8.6f %8.6f lamb/lambp: %4.3f %4.3f \n",mag,DTHRESH,lamb[i],lambp[i]);
      if (mag<DTHRESH || (fabs(lamb[i]-lambp[i])<DETHRESH && n>0))
      {
        printf(" vector %i converged \n",i);
        done++;
      }
      else
        nnew++;

    } //loop i over new vector creation
    for (int i=0;i<nvec;i++)
      lambp[i] = lamb[i];

    if (done==nval && n>1)
    {
      printf(" Davidson converged \n");
      break;
    }

  } //loop n over iters
  if (dcontinue==0)
    printf(" Davidson failed \n");


  if (dcontinue)
  {
    string savestr = "stringfile.xyz"+runends+"fr";
    string tstr;
    printf("\n now saving vibrations to %s \n",savestr.c_str());
    DIST = 0.3;
    int nsave = 0;
    for (int i=0;i<nval;i++)
    {
      for (int j=0;j<3*natoms;j++)
        coords[j] = xyz0[j];

     //move forward along vector
      for (int j=0;j<len;j++)
        dq0[j] = b[i][j] * DIST;
      ic_to_xyz();
      //print_xyz();
      tstr = StringTools::int2str(nsave++,2,"0");
      print_xyz_save(savestr+"t"+tstr);

      for (int j=0;j<3*natoms;j++)
        coords[j] = xyz0[j];
      //print_xyz();
      tstr = StringTools::int2str(nsave++,2,"0");
      print_xyz_save(savestr+"t"+tstr);

     //move reverse along vector
      for (int j=0;j<len;j++)
        dq0[j] = - b[i][j] * DIST;
      ic_to_xyz();
      //print_xyz();
      tstr = StringTools::int2str(nsave++,2,"0");
      print_xyz_save(savestr+"t"+tstr);

    } //loop i, printing xyz freq

    string cmd = "cat "+savestr+"t* > "+savestr;
    system(cmd.c_str());
    cmd = "rm "+savestr+"t*";
    system(cmd.c_str());
  }//if dcontinue

  for (int j=0;j<3*natoms;j++)
    coords[j] = xyz0[j];

  printf(" Hessian eigenvalues:");
  for (int i=0;i<nval;i++)
    printf(" %4.3f",lamb[i]);
  printf("\n");

  for (int i=0;i<nval;i++)
  if (lamb[i]<0.)
    nnegfound++;


  double* Ht = new double[len];
  double* ttt = new double[len*len];

  printf("\n Updating Hint with new vectors \n");
  //if (lamb[0]<0.0)
  if (dcontinue)
  for (int n=0;n<nval;n++)
  {
    double C = lamb[n];
    printf(" updating vector: %i with curvature: %6.5f \n",n+1,C);

   //modified from get_eigen_finite
    double tHt = 0.;
    for (int i=0;i<len;i++) Ht[i] = 0.;
    for (int i=0;i<len;i++)
    for (int j=0;j<len;j++)
      Ht[i] += Hint[i*len+j]*b[n][j]; //b is eigenvector
    for (int i=0;i<len;i++)
      tHt += b[n][i]*Ht[i];

    for (int i=0;i<len;i++)
    for (int j=0;j<len;j++)
      ttt[i*len+j] = b[n][i]*b[n][j];

    for (int i=0;i<len;i++)
    for (int j=0;j<len;j++)
      Hint[i*len+j] += (C-tHt)*ttt[i*len+j]; //C is curvature
  }
  if (dcontinue)
    nneg = nnegfound;
  else
    nneg = -1;

  delete [] Ht;
  delete [] ttt;

#if 0
  if (lamb[0]>0.0)
    printf(" did not update Hessian due to wrong curvature \n");
#endif
  if (dcontinue==0)
    printf(" did not update Hessian");
  if (fabs(lamb[0])>1.5)
    printf(" due to excessive curvature \n");
  printf("\n");

  printf(" Davidson required %i gradients \n",noptdone);
  printf("\n");

  grad1.write_on = 1;

  delete [] eigen;
  delete [] tmph;
  for (int i=0;i<MAX_VECS;i++)
    delete [] vecs[i];
  delete [] vecs;
  delete [] xyz0;
  delete [] g1;
  delete [] g2;
  for (int i=0;i<MAX_VECS;i++)
    delete [] y[i];
  delete [] y;
  delete [] q1;
  delete [] G;
  delete [] lamb;
  delete [] lambp;
  for (int i=0;i<MAX_VECS;i++)
    delete [] b[i];
  delete [] b;
  for (int i=0;i<MAX_VECS;i++)
    delete [] yn[i];
  delete [] yn;

  use_constraint = 0;
  if (dcontinue)
    isDavid = 1;

  return nnegfound;
}




void ICoord::get_gm() 
{
  int pr = 0;

  int N3 = natoms*3; // int N3M6 = N3-6;
  int dim = nicd;

#if 0
  printf(" printing masses \n");
  for (int i=0;i<natoms;i++)
  {
    printf(" %7.6f",amasses3[3*i]);
  }
  printf("\n"); fflush(stdout);
#endif

  double* tmpg = new double[N3*N3];
  for (int i=0;i<dim*dim;i++) Gmih[i] = 0.;
  for (int i=0;i<N3*N3;i++) tmpg[i] = 0.;

  for (int i=0;i<N3;i++)
  for (int j=0;j<dim;j++)
    tmpg[i*dim+j] = 1/amasses3[i] * bmat[j*N3+i] / 1822.888;
  for (int i=0;i<dim;i++)
  for (int j=0;j<dim;j++)
  for (int k=0;k<N3;k++)
    Gmih[i*dim+j] += bmat[i*N3+k] * tmpg[k*dim+j];

  for (int i=0;i<dim*dim;i++)
    Gmh[i] = Gmih[i];

  if (pr)
  {
    printf(" \n printing Gmh \n"); //symmetric matrix
    for (int i=0;i<dim;i++)
    {
      for (int j=0;j<dim;j++)
        printf(" %9.6f",Gmh[i*dim+j]);
      printf("\n");
    }
    printf(" Gm diagonals:");
    for (int i=0;i<dim;i++)
      printf(" %10.7f",Gmh[i*(dim+1)]);;
    printf("\n");
  }
  if (pr)
  {
    double* tmp = new double[dim*dim];
    double* tmpe = new double[dim];
    for (int i=0;i<dim*dim;i++)
      tmp[i] = Gmh[i];
    Diagonalize(tmp,tmpe,dim);
    printf(" Gmh eigenvalues:");
    for (int i=0;i<dim;i++)
      printf(" %10.7f",tmpe[i]);
    printf("\n");
    delete [] tmp;
    delete [] tmpe;
  }

  //conversion matrices for MW-ic's
  mat_root(Gmh,dim);
  mat_root_inv(Gmih,dim);


  delete [] tmpg;

  return;
}


void ICoord::create_mwHint_EV(double* Lm, double* Lme) 
{ //see F. Jensen, D. S. Palmer JCTC 2011, 7, 223-230
  int pr = 2;

  int N3 = natoms*3; // int N3M6 = N3-6;
  int N3M6 = nicd;
  int nic = nbonds+nangles+ntor;


  if (pr>1)
  {
    printf("\n creating Hintm, Gm, and IC vibrations \n");
    printf(" nicd: %i N3M6: %i \n",nicd,N3M6);
  }

#if 0
  printf(" \n printing bmatti \n");
  for (int i=0;i<N3M6;i++)
  {
    for (int j=0;j<N3;j++)
      printf(" %3.1f",bmatti[i*N3+j]);
    printf("\n");
  }
#endif

  double* tmph = new double[N3M6*N3M6];
  for (int i=0;i<N3M6*N3M6;i++) tmph[i] = 0.;
  for (int i=0;i<nicd;i++) Lme[i] = 0.;

#if 1
  if (pr>1 && nicd < 30)
  {
    printf(" \n printing Hint \n");
    for (int i=0;i<N3M6;i++)
    {
      for (int j=0;j<N3M6;j++)
        printf(" %6.3f",Hint[i*N3M6+j]);
      printf("\n");
    }
  }
#endif

  for (int i=0;i<N3M6*N3M6;i++) Gmh[i] = 0.;

  get_gm();

  double* tmpg = new double[N3*N3];
  double* GHG = new double[N3M6*N3M6];
  for (int i=0;i<N3M6*N3M6;i++) GHG[i] = 0.;

  for (int i=0;i<N3*N3;i++) tmpg[i] = 0.;
  for (int i=0;i<N3M6;i++)
  for (int j=0;j<N3M6;j++)
  for (int k=0;k<N3M6;k++)
    tmpg[i*N3M6+j] += Hint[i*N3M6+k] * Gmh[k*N3M6+j];
  for (int i=0;i<N3M6;i++)
  for (int j=0;j<N3M6;j++)
  for (int k=0;k<N3M6;k++)
    GHG[i*N3M6+j] += Gmh[i*N3M6+k] * tmpg[k*N3M6+j];

#if 0
  printf(" printing amasses: ");
  for (int i=0;i<natoms;i++)
    printf(" %2.1f",amasses[i]);
  printf(" \n printing Gm^1/2 \n");
  for (int i=0;i<N3M6;i++)
  {
    for (int j=0;j<N3M6;j++)
      printf(" %4.3f",Gmh[i*N3M6+j]);
    printf("\n");
  }
#endif 
#if 0
  double* tmpgg = new double[nicd*nicd];
  for (int i=0;i<nicd*nicd;i++)
    tmpgg[i] = 0.;
  for (int i=0;i<nicd;i++)
  for (int j=0;j<nicd;j++)
  for (int k=0;k<nicd;k++)
    tmpgg[i*nicd+j] += Gmh[i*N3M6+k] * Gmh[k*N3M6+j];
  printf(" \n printing Gm^1/2 * Gm^1/2 \n");
  for (int i=0;i<N3M6;i++)
  {
    for (int j=0;j<N3M6;j++)
      printf(" %10.7f",tmpgg[i*N3M6+j]);
    printf("\n");
  }
  delete [] tmpgg;
#endif 
#if 0
  printf(" \n printing GHG \n");
  for (int i=0;i<N3M6;i++)
  {
    for (int j=0;j<N3M6;j++)
      printf(" %10.7f",GHG[i*N3M6+j]);
    printf("\n");
  }
#endif

  Diagonalize(GHG,Lme,nicd);

  if (pr>0)
  {
    printf("\n printing IC vibrational eigenvalues:");
    for (int i=0;i<nicd;i++)
      printf(" %6.5f",Lme[i]);
    printf("\n printing IC vibrational frequencies:");
    for (int i=0;i<nicd;i++)
      printf(" %4.1f",sqrt(fabs(Lme[i]))*219474.6);
    printf("\n");
  }


#if 1
  if (pr>1 && nicd < 30)
  {
    printf(" \n printing eigenvectors (U) (MW-IC) \n");
    for (int i=0;i<nicd;i++)
    {
      for (int j=0;j<nicd;j++)
        printf(" %6.3f",GHG[i*nicd+j]);
      printf("\n");
    }
  }
#endif

#if 0
  printf("\n printing Ut vectors \n");
  for (int i=0;i<nicd;i++)
  {
    for (int j=0;j<nic;j++)
      printf(" %6.3f",Ut[i*nic+j]);
    printf("\n");
  }
#endif

 //copying mass-weighted vectors to Lm
  for (int i=0;i<nicd*nicd;i++)
    Lm[i] = GHG[i];

  delete [] GHG;
  delete [] amasses3;
  delete [] tmpg;
  delete [] tmph;

  return;
} 

