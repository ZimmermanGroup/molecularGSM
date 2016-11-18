#include "icoord.h"
#include "utils.h"
#include "omp.h"
//#include <Accelerate/Accelerate.h>
//#include <vecLib/clapack.h>
#include <mkl.h>
//#include "/export/apps/Intel/Compiler/11.1/075/mkl/include/mkl.h"
//#include "/export/apps/Intel/Compiler/11.1/075/mkl/include/mkl_lapack.h"
//#include "/opt/acml5.3.1/gfortran64/include/acml.h"

#define USE_ACML 0

using namespace std;

void trans(double* Bt, double* B, int m, int n) {

  for (int i=0;i<m;i++)
  for (int j=0;j<n;j++)
    Bt[i*n+j] = B[j*m+i];

  return;
}

int close_val(double x1, double	x2, double diff)
{
  int close = 0;
  if (fabs(x1-x2)<diff)
    close = 1;  
  return close;
}

int check_array(int size, double* A)
{
  int bad = 0;
  for (int i=0;i<size;i++)
  if (A[i]!=A[i])
  {
    bad = 1;
    break;
  }

  return bad;
}


int mat_times_mat(double* C, double* A, double* B, int M, int N, int K)
{
  //printf(" in mat_times_mat M/N/K: %i %i %i  \n",M,N,K);
  char TA = 'N';
  char TB = 'N';

#if 1
  int LDA = K; //rules in LAPACK documentation opposite due to RowMajor (CBlas only)
  int LDB = N;
  int LDC = N;
#else
  int LDA = M; 
  int LDB = N;
  int LDC = M;
#endif

  double ALPHA = 1.0;
  double BETA = 0.0;
 
 //C := alpha*op( A )*op( B ) + beta*C (op means A or B, possibly transposed)
 //CBlas version
  cblas_dgemm(CblasRowMajor,CblasNoTrans,CblasNoTrans,M,N,K,ALPHA,A,LDA,B,LDB,BETA,C,LDC);
 //untested LIBLAS version
  //dgemm(&TA,&TB,&M,&N,&K,&ALPHA,A,&LDA,B,&LDB,&BETA,C,&LDC);

#if 0
  printf(" printing C \n");
  for (int i=0;i<N;i++)
  {
    for (int j=0;j<M;j++)
      printf(" %6.3f",C[i*M+j]);
    printf("\n");
  }
#endif

  return 0;
}

int mat_times_mat_bt(double* C, double* A, double* B, int M, int N, int K)
{
  //printf(" in mat_times_mat_bt M/N/K: %i %i %i  \n",M,N,K);
  char TA = 'N';
  char TB = 'Y';

#if 1
//rules in LAPACK documentation opposite due to RowMajor (only CBlas)
  int LDA = K; 
  int LDB = K;
  int LDC = N;
#else
  int LDA = M; 
  int LDB = N;
  int LDC = M;
#endif

  double ALPHA = 1.0;
  double BETA = 0.0;
 
 //C := alpha*op( A )*op( B ) + beta*C (op means A or B, possibly transposed)
 //CBlas version
  cblas_dgemm(CblasRowMajor,CblasNoTrans,CblasTrans,M,N,K,ALPHA,A,LDA,B,LDB,BETA,C,LDC);
 //untested LIBLAS version
 // dgemm(&TA,&TB,&M,&N,&K,&ALPHA,A,&LDA,B,&LDB,&BETA,C,&LDC);

#if 0
  printf(" printing C \n");
  for (int i=0;i<N;i++)
  {
    for (int j=0;j<M;j++)
      printf(" %6.3f",C[i*M+j]);
    printf("\n");
  }
#endif

  return 0;
}

int mat_times_mat(double* C, double* A, double* B, int size)
{
  char TA = 'N';
  char TB = 'N';

  int M = size;
  int N = size;
  int K = size;

  int LDA = size;
  int LDB = size;
  int LDC = size;

  double ALPHA = 1.0;
  double BETA = 0.0;
 
 //C := alpha*op( A )*op( B ) + beta*C (op means A or B, possibly transposed)
 //CBlas version
  cblas_dgemm(CblasRowMajor,CblasNoTrans,CblasNoTrans,M,N,K,ALPHA,A,LDA,B,LDB,BETA,C,LDC);
 //untested LIBLAS version
 // dgemm(&TA,&TB,&M,&N,&K,&ALPHA,A,&LDA,B,&LDB,&BETA,C,&LDC);

#if 0
  printf(" printing C \n");
  for (int i=0;i<size;i++)
  {
    for (int j=0;j<size;j++)
      printf(" %6.3f",C[i*size+j]);
    printf("\n");
  }
#endif

  return 0;
}

int mat_times_mat_bt(double* C, double* A, double* B, int size)
{
  char TA = 'N';
  char TB = 'Y';

  int M = size;
  int N = size;
  int K = size;

  int LDA = size;
  int LDB = size;
  int LDC = size;

  double ALPHA = 1.0;
  double BETA = 0.0;
 
 //C := alpha*op( A )*op( B ) + beta*C (op means A or B, possibly transposed)
 //CBlas version
  cblas_dgemm(CblasRowMajor,CblasNoTrans,CblasNoTrans,M,N,K,ALPHA,A,LDA,B,LDB,BETA,C,LDC);
 //untested LIBLAS version
 // dgemm(&TA,&TB,&M,&N,&K,&ALPHA,A,&LDA,B,&LDB,&BETA,C,&LDC);

#if 0
  printf(" printing C \n");
  for (int i=0;i<size;i++)
  {
    for (int j=0;j<size;j++)
      printf(" %6.3f",C[i*size+j]);
    printf("\n");
  }
#endif

  return 0;
}

int mat_times_mat_at_bt(double* C, double* A, double* B, int size)
{
  char TA = 'Y';
  char TB = 'Y';

  int M = size;
  int N = size;
  int K = size;

  int LDA = size;
  int LDB = size;
  int LDC = size;

  double ALPHA = 1.0;
  double BETA = 0.0;
 
 //C := alpha*op( A )*op( B ) + beta*C (op means A or B, possibly transposed)
 //CBlas version
  cblas_dgemm(CblasRowMajor,CblasNoTrans,CblasNoTrans,M,N,K,ALPHA,A,LDA,B,LDB,BETA,C,LDC);
 //untested LIBLAS version
 // dgemm(&TA,&TB,&M,&N,&K,&ALPHA,A,&LDA,B,&LDB,&BETA,C,&LDC);

#if 0
  printf(" printing C \n");
  for (int i=0;i<size;i++)
  {
    for (int j=0;j<size;j++)
      printf(" %6.3f",C[i*size+j]);
    printf("\n");
  }
#endif

  return 0;
}

int mat_root(double* A, int size) {

  //printf(" in mat_root with size: %i \n",size); fflush(stdout);

#if 0
  printf(" printing A \n");
  for (int i=0;i<size;i++)
  {
    for (int j=0;j<size;j++)
      printf(" %6.5f",A[i*size+j]);
    printf("\n");
  }
#endif

  double* B = new double[size*size];
  for (int i=0;i<size*size;i++) B[i] = A[i]; 
  double* Beigen = new double[size];
  for (int i=0;i<size;i++) Beigen[i] = 0.;

  Diagonalize(B,Beigen,size);

#if 0
  printf(" printing eigenvectors of A \n");
  for (int i=0;i<size;i++)
  {
    for (int j=0;j<size;j++)
      printf(" %1.1f",B[i*size+j]);
    printf("\n");
  }
  printf(" eigenvalues: ");
  for (int i=0;i<size;i++)
    printf(" %1.1f",Beigen[i]);
  printf("\n");
  fflush(stdout);
#endif

  double* Bi = new double[size*size];
  //for (int i=0;i<size*size;i++) Bi[i] = B[i];

  trans(Bi,B,size,size);
 // Invert(Bi,size);
#if 0
  printf(" printing inverse of eigenvectors \n");
  for (int i=0;i<size;i++)
  {
    for (int j=0;j<size;j++)
      printf(" %1.1f",Bi[i*size+j]);
    printf("\n");
  }
#endif

#if 0
//checking inversion
  double* bbi = new double[size*size];
  for (int i=0;i<size*size;i++) bbi[i] = 0.;
  for (int i=0;i<size;i++)
  for (int j=0;j<size;j++)
  for (int k=0;k<size;k++)
    bbi[i*size+j] += B[i*size+k] * Bi[k*size+j];

  printf(" B*Bi \n");
  for (int i=0;i<size;i++)
  {
    for (int j=0;j<size;j++)
      printf(" %6.3f",bbi[i*size+j]);
    printf("\n");
  }
  delete [] bbi;
#endif

  double* tmp = new double[size*size];
  for (int i=0;i<size*size;i++) tmp[i] = 0.;
  for (int i=0;i<size*size;i++) A[i] = 0.; 

  for (int i=0;i<size;i++)
  for (int j=0;j<size;j++)
    tmp[i*size+j] += Bi[i*size+j] * sqrt(Beigen[j]);
  for (int i=0;i<size;i++)
  for (int j=0;j<size;j++)
  for (int k=0;k<size;k++)
    A[i*size+j] += tmp[i*size+k] * B[k*size+j];


  delete [] B;
  delete [] Beigen;
  delete [] Bi;
  delete [] tmp;
  
  return 0;
}

int mat_root_inv(double* A, int size) {

  //printf(" in mat_root_inv with size: %i \n",size); fflush(stdout);

#if 0
  printf(" printing A \n");
  for (int i=0;i<size;i++)
  {
    for (int j=0;j<size;j++)
      printf(" %1.1f",A[i*size+j]);
    printf("\n");
  }
#endif

  double* B = new double[size*size];
  for (int i=0;i<size*size;i++) B[i] = A[i]; 
  double* Beigen = new double[size];
  for (int i=0;i<size;i++) Beigen[i] = 0.;

  Diagonalize(B,Beigen,size);

#if 0
  printf(" printing eigenvectors of A \n");
  for (int i=0;i<size;i++)
  {
    for (int j=0;j<size;j++)
      printf(" %1.1f",B[i*size+j]);
    printf("\n");
  }
  printf(" eigenvalues: ");
  for (int i=0;i<size;i++)
    printf(" %1.1f",Beigen[i]);
  printf("\n");
#endif

  double* Bi = new double[size*size];
  //for (int i=0;i<size*size;i++) Bi[i] = B[i];

  trans(Bi,B,size,size);
  //Invert(Bi,size);
#if 0
  printf(" printing inverse of eigenvectors \n");
  for (int i=0;i<size;i++)
  {
    for (int j=0;j<size;j++)
      printf(" %1.1f",Bi[i*size+j]);
    printf("\n");
  }
#endif

#if 0
//checking inversion
  double* bbi = new double[size*size];
  for (int i=0;i<size*size;i++) bbi[i] = 0.;
  for (int i=0;i<size;i++)
  for (int j=0;j<size;j++)
  for (int k=0;k<size;k++)
    bbi[i*size+j] += B[i*size+k] * Bi[k*size+j];

  printf(" B*Bi \n");
  for (int i=0;i<size;i++)
  {
    for (int j=0;j<size;j++)
      printf(" %4.3f",bbi[i*size+j]);
    printf("\n");
  }
  delete [] bbi;
#endif

  double* tmp = new double[size*size];
  for (int i=0;i<size*size;i++) tmp[i] = 0.;
  for (int i=0;i<size*size;i++) A[i] = 0.; 

  for (int i=0;i<size;i++)
  for (int j=0;j<size;j++)
    tmp[i*size+j] += Bi[i*size+j] / sqrt(Beigen[j]);
  for (int i=0;i<size;i++)
  for (int j=0;j<size;j++)
  for (int k=0;k<size;k++)
    A[i*size+j] += tmp[i*size+k] * B[k*size+j];


  delete [] B;
  delete [] Beigen;
  delete [] Bi;
  delete [] tmp;
  
  return 0;
}

int SVD(double* A, double* V, double* eigen, int m, int n){

  //printf(" in SVD call, m,n: %i %i \n",m,n);

#if USE_ACML
 //disabled this function
  printf(" SVD not set up for ACML \n");
  return -1;
#endif

#if 0
  for (int i=0;i<m;i++)
  for (int j=0;j<n;j++)
    printf(" %1.3f",A[n*i+j]);
  printf("\n");
#endif

  double* B=new double[m*n];
  trans(B,A,n,m);

  int NSV,LV; // # singular values
  int LDA = m;
  if (m>n){
    NSV = n;
    LV=m;
  }
  else{
    NSV = m;
    LV=n;
  }

  double* Vt = new double[n*n];
  double* U = new double[m*m];
  double* S = new double[LV];
  int* IWork = new int[8*NSV];

  int Info = 0;

  //printf(" LenWork: %i \n",LenWork);

// "A" refers to full SVD, returning all vectors

  int LenWork = 320*NSV;
  double* Work = new double[LenWork]();
  char JOBU='A';
  char JOBVT='A';

#if !USE_ACML
  dgesvd_(&JOBU, &JOBVT, &m, &n, B, &LDA, S, U, &m, Vt, &n, Work, &LenWork, &Info);

//vs dgesdd (divide and conquer version)
//  dgesdd_((char*)"A", &m, &n, A, &LDA, S, U, &m, Vt, &n, Work0, &LenWork, IWork, &Info);
#endif

  if (Info!=0)
    printf(" after SVD, Info error is: %i \n",Info);

  trans(V,Vt,n,n);

  int n_nonzero = 0;
  for (int i=0;i<LV;i++)
  {
    //printf(" eigenvalue %i: %1.5f \n",i,S[i]);
    eigen[i]=S[i];
    if (abs(S[i])>0.0001) n_nonzero++;
  }

  delete [] S;
  delete [] Vt;
  delete [] U;
  delete [] Work;
  delete [] IWork;
  delete [] B;

//  printf(" found %i singular vectors \n",n_nonzero);
  return n_nonzero;


}

int Invert(double* A, int m){

  if (m<1)
  {
    printf("  WARNING: cannot invert, size: %i \n",m);
    return 1;
  }

#if 0
  printf(" in Invert call, m: %i \n",m);
  for (int i=0;i<m;i++)
  {
    for (int j=0;j<m;j++)
      printf(" %1.3f",A[m*i+j]);
    printf("\n");
  }
#endif

  int LenWork = 4*m;
  double* Work = new double[LenWork];

  int Info = 0;

  //printf(" LenWork: %i \n",LenWork);

  int* IPiv = new int[m];

//printf("\n");
//  dgesdd_((char*)"A", &m, &n, A, &LDA, S, U, &m, Vt, &n, Work, &LenWork, IWork, &Info);

  dgetrf_(&m,&m,A,&m,IPiv,&Info);
  if (Info!=0)
  {
    printf(" after dgetrf, Info error is: %i \n",Info);
    delete [] IPiv;
    delete [] Work;

    for (int i=0;i<m*m;i++) A[i] = 0.;
    for (int i=0;i<m;i++)
       A[i*m+i] = 1.;
 
    return 1;
  }

  dgetri_(&m,A,&m,IPiv,Work,&LenWork,&Info);
  if (Info!=0)
  {
    printf(" after invert, Info error is: %i \n",Info);
    printf(" A-1: \n");
    for (int i=0;i<m;i++)
    {
      for (int j=0;j<m;j++)
        printf(" %4.3f",A[i*m+j]);
      printf("\n");
    }
  }

  delete [] IPiv;
  delete [] Work;


  return 0;
}


int Diagonalize(double* A, double* eigen, int size){

  if (size<1)
  {
    printf("  WARNING: cannot Diagonalize, size: %i \n",size);
    return 1;
  }

#ifdef _OPENMP
//  mkl_set_num_threads(1);
//  int nthreads = omp_get_num_threads();
//  omp_set_num_threads(1);
#endif

#define DSYEVX 1
 // printf(" in diagonalize call, size: %i \n",size);
 // printf(" in diagonalize: mkl_threads: %i \n",mkl_get_max_threads());

  int N = size;
  int LDA = size;
  double* EVal = eigen;

//borrowed from qchem liblas/diagon.C


    char JobZ = 'V', Range = 'A', UpLo = 'U';
    int IL = 1, IU = N;
    double AbsTol = 0.0, VL = 1.0, VU = -1.0;
    int NEValFound;

    double* EVec = new double[LDA*N];

    // Give dsyevx more work space than the minimum 8*N to improve performance
#if DSYEVX
    int LenWork = 32*N; //8*N min for dsyevx (was 32)
#else
    int LenWork = 1+6*N+2*N*N; //1+6*N+2*N*N min for dsyevd
#endif
    double* Work = new double[LenWork];

#if DSYEVX
    int LenIWork = 5*N; //5*N for dsyevx (was 5)
#else
    int LenIWork = 10*N; //3+5*N min for dsyevd
#endif
    int* IWork = new int[LenIWork];
    int* IFail = new int[N];

    int Info = 0;

#if USE_ACML
   dsyevx(JobZ, Range, UpLo, N, A, LDA, VL,
          VU, IL, IU, AbsTol, &NEValFound, EVal, EVec, LDA,
          IFail, &Info);
#else
#if DSYEVX
    dsyevx_(&JobZ, &Range, &UpLo, &N, A, &LDA, &VL, &VU, &IL, &IU, &AbsTol,
           &NEValFound, EVal, EVec, &LDA, Work, &LenWork, IWork, IFail, &Info);
#else
    dsyevd_(&JobZ, &UpLo, &N, A, &LDA, EVal, Work, &LenWork, IWork, &LenIWork, &Info);
#endif
#endif

#if 0
    if (Info != 0 && KillJob) {
      printf(" Info = %d\n",Info);
      QCrash("Call to dsyevx failed in Diagonalize");
    }
#endif

  int n_nonzero = 0;
  for (int i=0;i<size;i++)
  {
    //printf(" eigenvalue %i: %1.5f \n",i,eigen[i]);
    if (abs(eigen[i])>0.0001) n_nonzero++;
  }
  //printf(" found %i independent vectors \n",n_nonzero);

#if DSYEVX
  for (int i=0;i<size;i++)
  for (int j=0;j<size;j++)
    A[i*size+j] = EVec[i*size+j];
#endif

#ifdef _OPENMP
//  omp_set_num_threads(nthreads);
#endif

    delete [] EVec;
    delete [] Work;
    delete [] IWork;
    delete [] IFail;

  return 0;

}

int Diagonalize(double* A, double* eigenvecs, double* eigen, int size){

  printf(" in diagonalize call, size: %i \n",size);
  

  int N = size;
  int LDA = size;
  double* EVal = eigen;

//borrowed from qchem liblas/diagon.C


    char JobZ = 'V', Range = 'A', UpLo = 'U';
    int IL = 1, IU = N;
    double AbsTol = 0.0, VL = 1.0, VU = -1.0;
    int NEValFound;

    double* EVec = new double[LDA*N];

    // Give dsyevx more work space than the minimum 8*N to improve performance
    int LenWork = 32*N;
    double* Work = new double[LenWork];

    int LenIWork = 5*N;
    int* IWork = new int[LenIWork];
    int* IFail = new int[N];

    int Info = 0;

#if USE_ACML
   dsyevx(JobZ, Range, UpLo, N, A, LDA, VL,
          VU, IL, IU, AbsTol, &NEValFound, EVal, EVec, LDA,
          IFail, &Info);
#else
    dsyevx_(&JobZ, &Range, &UpLo, &N, A, &LDA, &VL, &VU, &IL, &IU, &AbsTol,
           &NEValFound, EVal, EVec, &LDA, Work, &LenWork, IWork, IFail, &Info);
#endif

#if 0
    if (Info != 0 && KillJob) {
      printf(" Info = %d\n",Info);
      QCrash("Call to dsyevx failed in Diagonalize");
    }
#endif

  for (int i=0;i<size;i++)
  for (int j=0;j<size;j++)
    eigenvecs[i*size+j] = EVec[i*size+j];

    delete [] EVec;
    delete [] IWork;
    delete [] IFail;

  return 0;

}


int sign(double x){
    
  if (x>0) return 1;
  else if (x<=0) return -1;
 
  return 0;
}

void cross(double* m, double* r1, double* r2){

  m[0] = r1[1]*r2[2]-r2[1]*r1[2];
  m[1] = -r1[0]*r2[2]+r2[0]*r1[2];
  m[2] = r1[0]*r2[1]-r2[0]*r1[1];

  return;
}


void Utils::matrix_times_matrix(double** A, double** B, double** C, int LEN){

  for (int i=0;i<LEN;i++){
    for (int j=0;j<LEN;j++){
      C[i][j] = 0;
      for (int k=0;k<LEN;k++){
	C[i][j] += A[i][k]*B[k][j];
      }
    }
  }

}


void Utils::display_structure(double* a, int natoms, string* anames){

  cout.setf(ios::left);
  cout.setf(ios::fixed);

  for (int i=0;i<natoms;i++){

    cout << setw(2) << anames[i];
    cout.unsetf(ios::left);

    cout.setf(ios::right);
    for (int j=0;j<3;j++){
      cout << setw(15) << setprecision(10) << a[3*i+j];
    }
    cout.unsetf(ios::right);
    cout << endl;
    cout.setf(ios::left);

  }
  cout.unsetf(ios::left);
}

void Utils::display_structure_nonames(double* a, int natoms){
 
  cout.setf(ios::left);
  cout.setf(ios::fixed);

  for (int i=0;i<natoms;i++){

    cout.unsetf(ios::left);

    cout.setf(ios::right);
    for (int j=0;j<3;j++){
      cout << setw(15) << setprecision(10) << a[3*i+j];
    }
    cout.unsetf(ios::right);
    cout << endl;
    cout.setf(ios::left);

  }
  cout.unsetf(ios::left);
}


void Utils::S_straight_line_in_angs(double** string_angs, double* S, int nstring, int natoms){

  S[1] = 0;
  double* diff = new double[1+natoms*3];
  for (int i=1;i<nstring;i++){
    for (int j=0;j<natoms*3;j++){
      diff[j] = string_angs[i][j] - string_angs[i-1][j];
    }
    S[i] = S[i-1]+ Utils::vecMag(diff, natoms*3);
  }

  delete [] diff;
}


void Utils::subtract_arrays(double* a, double* b, double* diff, int LEN){
  for (int i=0;i<LEN;i++){
    diff[i] = a[i]-b[i];
  }
}


void Utils::vector_outer_prod(double* vec1, double* vec2, int LEN, double** output){
  for (int i=0;i<LEN;i++){
    for (int j=0;j<LEN;j++){
      output[i][j] = vec1[i]*vec2[j];
    }
  }
}

void Utils::copy_structure(double* structure1, double* structure2, int natoms){
  for (int i=0;i<natoms*3;i++){
    structure2[i] = structure1[i];
  }
}

void Utils::copy_2D_array(double** pointer1, double** pointer2, int LEN1, int LEN2){
  for (int i=0;i<LEN1;i++){
    for (int j=0;j<LEN2;j++){
      pointer2[i][j] = pointer1[i][j];
    }
  }
}


void Utils::get_rotation_matrix(double** rotMat, double* thetas){
  double x=thetas[0]; double y=thetas[1]; double z=thetas[2];
  rotMat[0][0] = cos(y)*cos(z);
  rotMat[0][1] = -cos(y)*sin(z);
  rotMat[0][2] = sin(y);
  rotMat[1][0] = sin(x)*sin(y)*cos(z)+cos(x)*sin(z);
  rotMat[1][1] = -sin(x)*sin(y)*sin(z)+cos(x)*cos(z);
  rotMat[1][2] = -sin(x)*cos(y);
  rotMat[2][0] = -cos(x)*sin(y)*cos(z)+sin(x)*sin(z);
  rotMat[2][1] = cos(x)*sin(y)*sin(z)+sin(x)*cos(z);
  rotMat[2][2] = cos(x)*cos(y);
}

void Utils::Rotate_structure(double** RotMat, double* structure, int natoms){

  double* temp = new double[1+natoms*3];
  for (int i=0;i<natoms*3;i++){
    temp[i]=0.0;
  }

  for (int i=0;i<natoms;i++){
    for (int j=0;j<3;j++){
      for (int k=0;k<3;k++){
        temp[3*i+j] += RotMat[j][k]*structure[3*i+k];
      }
    }
  }

  for (int i=0;i<natoms*3;i++){
    structure[i]=temp[i];
  }
  delete [] temp;
}



void Utils::Rot_around_vec(double* vec, double* structure, int natoms) {

  double** RotMat = new double*[1+3];
  for (int i=0;i<3;i++){
    RotMat[i] = new double[1+3];
  }

  RotMat[0][0]=2*vec[0]*vec[0]-1;
  RotMat[1][1]=2*vec[1]*vec[1]-1;
  RotMat[2][2]=2*vec[2]*vec[2]-1;
  RotMat[1][0]=2*vec[0]*vec[1];
  RotMat[2][0]=2*vec[0]*vec[2];
  RotMat[2][1]=2*vec[1]*vec[2];
  RotMat[0][1]=RotMat[1][0];
  RotMat[0][2]=RotMat[2][0];
  RotMat[1][2]=RotMat[2][1];

  Utils::Rotate_structure(RotMat, structure, natoms);

  for (int i=0;i<3;i++){
    delete [] RotMat[i];
  }
  delete [] RotMat;

}


void Utils::mwc_to_ang(double** angs, double** mwc, int nstring, int natoms, double* amasses){

  for (int i=0;i<nstring;i++){
    for (int j=1;j<=natoms;j++){
      for (int k=1;k<=3;k++){
	angs[i][3*(j-1)+k]=mwc[i][3*(j-1)+k]*(1/ANGtoBOHR)*(1/sqrt(amasses[j]));
      }
    }
  }
}

void Utils::mwc_to_ang(double* angs, double* mwc, int natoms, double* amasses){

  for (int j=0;j<natoms;j++){
    for (int k=0;k<3;k++){
      angs[3*j+k]=mwc[3*j+k]*(1/ANGtoBOHR)*(1/sqrt(amasses[j]));
    }
  }
}


void Utils::ang_to_mwc(double** mwc, double** ang, int nstring, int natoms, double* amasses){

  for (int i=0;i<nstring;i++){
    for (int j=0;j<natoms;j++){
      for (int k=0;k<3;k++){
        mwc[i][3*j+k]=ang[i][3*j+k]*(ANGtoBOHR)*(sqrt(amasses[j]));
      }
    }
  }
}

void Utils::ang_to_mwc(double* mwc, double* ang, int natoms, double* amasses){

  for (int j=0;j<natoms;j++){
    for (int k=0;k<3;k++){
      mwc[3*j+k]=ang[3*j+k]*(ANGtoBOHR)*(sqrt(amasses[j]));
    }
  }

}



void Utils::mwcgrad_to_anggrad(double** ang_grad, double** mwc_grad, int nstring, int natoms, double* amasses){

  for (int i=0;i<nstring;i++){
    for (int j=0;j<natoms;j++){
      for (int k=0;k<3;k++){
        ang_grad[i][3*j+k]=mwc_grad[i][3*j+k]*(ANGtoBOHR)*(sqrt(amasses[j]));
      }
    }
  }
}

void Utils::mwcgrad_to_anggrad(double* ang_grad, double* mwc_grad, int natoms, double* amasses){

  for (int j=0;j<natoms;j++){
    for (int k=0;k<3;k++){
      ang_grad[3*j+k]=mwc_grad[3*j+k]*(ANGtoBOHR)*(sqrt(amasses[j]));
    }
  }
  
}


void Utils::anggrad_to_mwcgrad(double** mwc_grad, double** ang_grad, int nstring, int natoms, double* amasses){

  for (int i=0;i<nstring;i++){
    for (int j=0;j<natoms;j++){
      for (int k=0;k<3;k++){
        mwc_grad[i][3*j+k]=ang_grad[i][3*j+k]*(1/ANGtoBOHR)*(1/sqrt(amasses[j]));
      }
    }
  }
}

void Utils::anggrad_to_mwcgrad(double* mwc_grad, double* ang_grad, int natoms, double* amasses){

  for (int j=0;j<natoms;j++){
    for (int k=0;k<3;k++){
      mwc_grad[3*j+k]=ang_grad[3*j+k]*(1/ANGtoBOHR)*(1/sqrt(amasses[j]));
    }
  }
}


void Utils::diagonalize3x3(double** hmwc, double** smwc,
			   double* w2, int ndiag){
			   
  int test;
  double temp;
  double diff, tolerance, relerror, evalue;

#if 0
  double** x = new double*[3];
  for (int i=0;i<3;i++)
    x[i] = new double[1+3];
#else
  double x[3][1+3];
#endif

  double** K= new double*[1+3];
  for (int i=0;i<3;i++){
    K[i] = new double[1+3];
  } 

  for(int j=0;j<3;j++){
    for(int k=0;k<3;k++){
      K[j][k] = hmwc[j][k];
    }
  }
  tolerance = .000001;

  for(int i=0;i<3;i++){
    for(int j=0;j<3;j++){
//      x[0][j]=randomf(-1.0,1.0);
      x[0][j] = 0.5; //hard coded to prevent irreproducibility 
      x[1][j] = 0.0;
      x[2][j] = 0.0;
    }
    Utils::normalize(x[0],3);
       
    test = 0;
 //CPMZ warning, check below...?
    for(int k=1;test == 0;k++){  
      for(int j=0;j<3;j++){ 
	x[k%3][j]=Utils::dotProd(K[j],x[(k-1)%3],3);                 
      }
      Utils::normalize(x[k%3],3);  

      relerror = 0.0;
      for(int j=0;j<3;j++){
	diff = x[k%3][j]-x[(k-2)%3][j];      
	relerror = relerror + fabs(diff);          
      }
      
      if((relerror < tolerance && k > 100) || (k == ndiag)){

	for(int j=0;j<3;j++){
	  x[(k+1)%3][j] = Utils::dotProd(K[j],x[k%3],3);
	}

	w2[i] = Utils::dotProd(x[k%3],x[(k+1)%3],3);
	for(int j=0;j<3;j++){           
	  smwc[i][j] = x[k%3][j];  
	}

	Utils::projectfrommatrix3x3(smwc[i], K);
	test = 1;                                      
      }                      
    }
  }   

  for (int i=0;i<3;i++){
    delete [] K[i];
  }
  delete [] K;
#if 0
//CPMZ some weird memory bug
  for (int i=0;i<3;i++){
    delete [] x[i];
  }
  delete [] x;
#endif

}

double Utils::randomf(double a, double b){
  
  timeval t1;
  gettimeofday(&t1, NULL);
  srand(t1.tv_usec*t1.tv_sec);

  double range = b-a;
  double randn = a + double(range*rand()/(RAND_MAX));
  return randn;
}

/// normalize vector
void Utils::normalize(double* u, int LEN){
  double dp=Utils::dotProd(u,u,LEN); // dot product
  double vm=sqrt(dp); // magnitude
  for (int i=0; i<LEN; i++){
    u[i]=u[i]/vm;
  }   
}


void Utils::projectfrommatrix3x3(double* vector, double** hmwc){

  double** p = new double*[1+3];
  double** K = new double*[1+3];
  for (int i=0;i<3;i++){
    p[i] = new double[1+3];
    K[i] = new double[1+3];
  }
 
  for(int i=0;i<3;i++){
    for(int j=0;j<3;j++){
      if(i == j){
	p[i][j] = 1.0 - vector[i]*vector[j];
      }
      else{
	p[i][j] = - vector[i]*vector[j];
      }
    }
  }
  for(int i=0;i<3;i++){
    for(int j=0;j<3;j++){
      K[i][j] = 0.0;
      for(int k=0;k<3;k++){
	K[i][j] = K[i][j] + hmwc[i][k]*p[k][j];
      }
    }
  }
  for(int i=0;i<3;i++){
    for(int j=0;j<3;j++){
      hmwc[i][j] = 0.0;
      for(int k=0;k<3;k++){
	hmwc[i][j] = hmwc[i][j] + p[i][k]*K[k][j];
      }
    }
  }

  for (int i=0;i<3;i++){
    delete [] p[i];
    delete [] K[i];
  }
  delete [] p;
  delete [] K;

}


void Utils::Mat_times_vec(double** d2S_1, double* dS, double* prod, int LEN){
  for (int i=0;i<LEN;i++){
    prod[i]=0;
    for (int j=0;j<LEN;j++){
      prod[i] += d2S_1[i][j]*dS[j];
    }
  }

}


void Utils::invertNxN(double** Mat, double** Inverse, int n) {

  for (int i=0;i<n;i++){
    for (int j=0;j<n;j++){
      Inverse[i][j]=0.0;
    }
  }
  
  double** a = new double*[1+n];
  for (int i=0;i<n;i++) a[i]=new double[1+n];
 
  for (int i=0;i<n;i++){
    for (int j=0;j<n;j++){
      a[i][j]=Mat[i][j];
    }
  }

  int* indx = new int[1+n];
  double d;

  Utils::ludcmp(a,n,indx,&d);
  
  double *col = new double[1+n];
  
  for (int j=0;j<n;j++){
    for (int i=0;i<n;i++) col[i]=0.0;
    col[j]=1.0;
    Utils::lubksb(a,n,indx,col);
    for(int i=0;i<n;i++) Inverse[i][j]=col[i];
  }

  for (int i=0;i<n;i++){
    delete [] a[i];
  }
  delete [] a;
  delete [] indx;
  delete [] col;

}
  


void Utils::ludcmp(double **a, int n, int *indx, double *d){

  int i,imax,j,k;
  double big,dum,sum,temp;
  double *vv;

  vv=new double[1+n];

  *d=1.0;
  for (i=0;i<n;i++){
    big=0.0;
    for (j=0;j<n;j++)
      if ((temp=fabs(a[i][j]))>big) big=temp;
    if (big ==0) cout << "Singular matrix!\n";
    vv[i]=1.0/big;
  }

  for (j=0;j<n;j++){
    for (i=0;i<j;i++){
      sum=a[i][j];
      for (k=0;k<i;k++) sum-=a[i][k]*a[k][j];
      a[i][j]=sum;
    }

    big=0.0;
    for (i=j;i<n;i++){
      sum=a[i][j];
      for (k=0;k<j;k++)
	sum -= a[i][k]*a[k][j];
      a[i][j]=sum;
      if ((dum=vv[i]*fabs(sum))>=big){
	big=dum;
	imax=i;
      }
    }
    if (j!=imax){
      for (k=0;k<n;k++){
	dum=a[imax][k];
	a[imax][k]=a[j][k];
	a[j][k]=dum;
      }
      *d = -(*d);
      vv[imax]=vv[j];
    }
    indx[j]=imax;
    if (a[j][j] == 0.0) a[j][j]=1e-20;
    if (j!=n) {
      dum=1.0/a[j][j];
      for (i=j+1;i<=n;i++) a[i][j] *= dum;
    }
  }
  delete [] vv;
}

void Utils::lubksb(double **a, int n, int *indx, double b[]){
  int i,ii=0,ip,j;
  double sum;

  for (i=0;i<n;i++){
    ip=indx[i];
    sum=b[ip];
    b[ip]=b[i];
    if (ii)
      for (j=ii;j<i-1;j++) sum -= a[i][j]*b[j];
    else if (sum) ii=i;
    b[i]=sum;
  }
  for (i=n;i>=0;i--){
    sum=b[i];
    for (j=i+1;j<=n;j++) sum -= a[i][j]*b[j];
    b[i]=sum/a[i][i];
  }
}

double Utils::det3x3(double A[1+3][1+3]){
  double a[1+2][1+2];
  double temp, m;
  
  temp = 0.0;
  for(int i=0;i<3;i++){
    m = ((double)(2*(i%2)-1));
    for(int k=0;k<3;k++){
      for(int j=1;j<3;j++){
	if(k < i){
	  a[j-1][k] = A[j][k];
	}
	if(k > i){
	  a[j-1][k-1] = A[j][k];
	}
      }
    }
    temp = temp + A[1][i]*m*Utils::det2x2(a);
  }
  return temp;
}


void Utils::adjoint3x3(double A[4][4], double Aadj[4][4]){
  double a[1+2][1+2];
  double m, temp;
  
  for(int i=0;i<3;i++){
    for(int l=0;l<3;l++){
      m = ((double)(2*((i+l+1)%2) - 1));
      for(int j=0;j<3;j++){
	for(int k=0;k<3;k++){
	  if(j < i){
	    if(k < l){
	      a[j][k] = A[j][k];
	    }
	    if(k > l){
	      a[j][k-1] = A[j][k];
	    }
	  }
	  if(j > i){
	    if(k < l){
	      a[j-1][k] = A[j][k];
	    }
	    if(k > l){
	      a[j-1][k-1] = A[j][k];
	    }
	  }
	}
      }
      Aadj[l][i] = m*Utils::det2x2(a);
    }
  }
}

double Utils::det2x2(double A[3][3]){
   
  double temp;
  temp = A[0][0]*A[1][1]-A[0][1]*A[1][0];
  return temp;
}


// vout = v -[(v.u)] *( u / |u.u|)
void Utils::gramschmidt(int LEN, double* v_out, double* u_in, double* v_in){
  double udotv = Utils::dotProd(v_in, u_in, LEN);
  double udotu = Utils::dotProd(u_in, u_in, LEN);
  for(int i=0;i<LEN;i++){
    v_out[i] = v_in[i]-udotv*u_in[i]/udotu;
  }
}


// find tanget copy to y1
void Utils::splineTangents(int LEN, double* x, double* y, double* y2, double* y1){
  double sig, dx;
  for(int i=0;i<LEN-1;i++){
    sig = (y[i+1]-y[i])/(x[i+1]-x[i]);
    dx = x[i+1]-x[i];
    y1[i] = sig - dx*y2[i]/3.0 - dx*y2[i+1]/6.0;
  }
  y1[LEN] = sig + dx*y2[LEN-1]/6.0 + dx*y2[LEN]/3.0;
}

void Utils::S_from_angs(double** angs, double* S, double* masses, int LEN, int natoms){

  //create mass-weighted array in atomic units, and then calculate S
  double** temparray = new double*[1+LEN];
  for (int i=0;i<LEN;i++){
    temparray[i]=new double[1+natoms*3];
  }

  for (int i=0;i<LEN;i++){
    for (int j=0;j<natoms*3;j++){
      temparray[i][j]=angs[i][j];
    }
  }

  Utils::angs_to_mwcs(temparray, LEN, natoms, masses);

  S[1]=0;
  double* dvec = new double[1+natoms*3];
  for (int i=1;i<LEN;i++){
    for (int j=0;j<natoms*3;j++){
      dvec[j]=temparray[i][j]-temparray[i-1][j];
    }
    
    S[i] = S[i-1] + Utils::vecMag(dvec, natoms*3);
  }

  for (int i=0;i<LEN;i++){
    delete [] temparray[i];
  }
  delete [] temparray;
  delete [] dvec;

}


void Utils::angs_to_mwcs(double** temparray, int nn, int natoms, double* amasses){
 
  for (int i=0;i<nn;i++){
    for (int j=0;j<natoms;j++){
      for (int k=0;k<3;k++){
	temparray[i][3*j+k]*=(ANGtoBOHR*sqrt((double)amasses[j]));
      }
    }
  }
}

void Utils::anggrads_to_mwcgrads(double** temparray, int nn, int natoms, double* amasses){

  for (int i=0;i<nn;i++){
    for (int j=0;j<natoms;j++){
      for (int k=0;k<3;k++){
        temparray[i][3*j+k]/=ANGtoBOHR*sqrt((double)amasses[j]);
      }
    }
  }
}



double Utils::dotProd(double* v, double* u, int LEN){
  double dp=0.0;
  for(int j=0;j<LEN;j++){
    dp = dp + v[j]*u[j];
  }
  return dp;
}


double Utils::vecMag(double* u, int LEN){
  double dp=Utils::dotProd(u,u,LEN);
  double vm=sqrt(dp);
  return vm;
}

