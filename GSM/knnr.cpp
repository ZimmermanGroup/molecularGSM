#include <iostream>
#include <fstream>
#include <stdio.h>
using namespace std;

#include "qchem.h"
#include "knnr.h"
#include "icoord.h"

#define MAX_GEOMS 2500
#define FULL_RESET 1

void print_xyz_gen(int natoms, string* anames, double* coords);

//NOTE: distances matrix not recomputed when adding points


int KNNR::add_extra_points()
{
  if (npts<1)
  {
    printf(" not enough points to do add_extra_points! \n");
    return 0;
  }
  string nstr = StringTools::int2str(runnum,4,"0");

  string* filesxyz1 = new string[MAX_GEOMS];
  int npts1 = get_files("scratch/qcsave"+nstr,"xyz",filesxyz1);

  string* filesgrad1 = new string[MAX_GEOMS];
  int nptsg = get_files("scratch/qcsave"+nstr,"grad",filesgrad1);
  if (nptsg!=npts1)
  {
    printf(" grad and xyz mismatch: %i vs %i \n",nptsg,npts1);
    exit(1);
  }

  string* fileshess1 = new string[MAX_GEOMS];
  int nptsh = get_files("scratch/qcsave"+nstr,"icp",fileshess1);
  if (nptsh!=npts1)
  {
    printf(" hess and xyz mismatch: %i vs %i \n",nptsh,npts1);
    exit(1);
  }

  string* newfilesxyz = new string[MAX_GEOMS];
  string* newfilesgrad = new string[MAX_GEOMS];
  string* newfileshess = new string[MAX_GEOMS];
 //adds new files to files lists
  int nptsn = compare_add_files(npts1,filesxyz1,filesgrad1,fileshess1,newfilesxyz,newfilesgrad,newfileshess);


  printf(" files (current/previous): %i/%i \n",npts1,npts);
  //printf(" nptsn: %i \n",nptsn); fflush(stdout);

  npts1 = npts + nptsn;

  if (nptsn>0)
  {
    reassign_mem(npts1);
    int npts0 = npts;
    npts = npts1;

#if FULL_RESET
     read_xyzs(energies,xyz,filesxyz);
     read_xyzs(NULL,grads,filesgrad);
     read_hess(nic,fileshess);
#else
    for (int i=npts0;i<npts1;i++)
    {
      string filename = newfilesxyz[i-npts0];
      energies[i] = read_one_xyz(i,filename,xyz[i]);
      filename = newfilesgrad[i-npts0];
      read_one_xyz(i,filename,grads[i]);
      filename = newfileshess[i-npts0];
      read_one_hess(i,nic,filename);
    } //reading new files
#endif

#if 0
    ICoord ic1, ic2;
    setup_ic(ic1,ic2);

    printf(" recomputing all distances (FIX ME) \n");
    get_distances(npts1,xyz,distances,ic1,ic2);

    release_ic(ic1,ic2);
#endif

  } //if adding new points

  //printf(" found %i new points, %i total \n",nptsn,npts);

  delete [] newfilesxyz;
  delete [] newfilesgrad;
  delete [] newfileshess;
  delete [] filesxyz1;
  delete [] filesgrad1;
  delete [] fileshess1;

  return nptsn;
}

int KNNR::compare_add_files(int npts1, string* filesxyz1, string* filesgrad1, string* fileshess1, string* newfilesxyz, string* newfilesgrad, string* newfileshess)
{
  int nf = 0;

  for (int i=0;i<npts1;i++)
  {
    int found = 0;
    for (int j=0;j<npts;j++)
    if (filesxyz[j]==filesxyz1[i])
    {
      found = 1;
      break;
    }
    if (!found)
    {
      filesxyz[npts+nf]  = newfilesxyz[nf] = filesxyz1[i];
      filesgrad[npts+nf] = newfilesgrad[nf] = filesgrad1[i];
      fileshess[npts+nf] = newfileshess[nf] = fileshess1[i];
      //printf(" added(%i): %s %s %s \n",nf,newfilesxyz[nf].c_str(),newfilesgrad[nf].c_str(),newfileshess[nf].c_str()); fflush(stdout);
      nf++;
    } 
  } //loop i over all files

  return nf;
}

void KNNR::reassign_mem(int npts1)
{
  int N3 = natoms*3;

  //printf(" in reassign, npts: %i npts1: %i \n",npts,npts1); fflush(stdout);

#if FULL_RESET
    delete [] energies;
    delete [] distances;
    delete [] distancesu;
    for (int i=0;i<npts;i++)
      delete [] xyz[i];
    delete [] xyz;
    for (int i=0;i<npts;i++)
      delete [] grads[i];
    delete [] grads;
    for (int i=0;i<npts;i++)
      delete [] hess[i];
    delete [] hess;
    delete [] useH;

    energies = new double[npts1];
    distances = new double[npts1*npts1];
    distancesu = new double[npts1*npts1];
    xyz = new double*[npts1];
    for (int i=0;i<npts1;i++)
      xyz[i] = new double[N3];
    grads = new double*[npts1];
    for (int i=0;i<npts1;i++)
      grads[i] = new double[N3];
    hess = new double*[npts1];
    for (int i=0;i<npts1;i++)
      hess[i] = new double[nic*nic];
    useH = new int[npts1];
#else
    double* energies1 = new double[npts1];
    for (int i=0;i<npts;i++)
      energies1[i] = energies[i];
    delete [] energies;
    energies = energies1;

    double* distances1 = new double[npts1*npts1];
    for (int i=0;i<npts;i++)
    for (int j=0;j<npts;j++)
      distances1[i*npts1+j] = distances[i*npts+j];
    delete [] distances;
    distances = distances1;
    delete [] distancesu;
    distancesu = new double[npts1*npts1];

    double** xyz1 = new double*[npts1];
    for (int i=0;i<npts;i++)
      xyz1[i] = xyz[i];
    delete [] xyz;
    xyz = xyz1;
    for (int i=npts;i<npts1;i++)
      xyz[i] = new double[N3];

    double** grads1 = new double*[npts1];
    for (int i=0;i<npts;i++)
      grads1[i] = grads[i];
    delete [] grads;
    grads = grads1;
    for (int i=npts;i<npts1;i++)
      grads[i] = new double[N3];

    double** hess1 = new double*[npts1];
    for (int i=0;i<npts;i++)
      hess1[i] = hess[i];
    delete [] hess;
    hess = hess1;
    for (int i=npts;i<npts1;i++)
      hess[i] = new double[nic*nic];
    useH = new int[npts1];
#endif

  //printf(" done with reassign \n"); fflush(stdout);

  return;
}

void KNNR::setup_ic(ICoord& ic1, ICoord& ic2)
{
  string nstr = StringTools::int2str(runnum,4,"0");
  string icfile = "scratch/qcsave"+nstr+".ics";

  //ic1.init(file0);
  ic1.alloc(natoms);
  ic1.reset(natoms,anames,anumbers,xyz[0]);

//CPMZ change this!
  nic = ic1.read_ics(icfile);
  //ic1.print_ic();

  ic2.alloc(natoms);
  ic2.reset(natoms,ic1.anames,ic1.anumbers,xyz[0]);
  ic2.copy_ic(ic1);
  //ic2.print_ic();
 
  ic1.bmat_alloc();
  ic2.bmat_alloc();

  ic1.grad1.xyz_grad = 1;
  ic2.grad1.xyz_grad = 1;

  if (nic<1)
  {
    printf(" ERROR: %i nic's \n",nic);
    exit(1);
  }

  return;
}

void KNNR::release_ic(ICoord& ic1, ICoord& ic2)
{
  ic1.bmat_free();
  ic2.bmat_free();
  ic1.freemem();
  ic2.freemem();

  return;
}

int KNNR::begin(int runnum0, int natoms0)
{
  runnum = runnum0;
  natoms = natoms0;
  int N3 = natoms*3;

  string nstr = StringTools::int2str(runnum,4,"0");
  if (printl)
    printf(" run: %i --> %s \n",runnum,nstr.c_str());

  filesxyz = new string[MAX_GEOMS];
  npts = get_files("scratch/qcsave"+nstr,"xyz",filesxyz);
  if (npts<1)
  {
    printf("  kNNR found no geometries \n");
    return 0;
  }
  filesgrad = new string[MAX_GEOMS];
  int nptsg = get_files("scratch/qcsave"+nstr,"grad",filesgrad);
  if (nptsg!=npts)
  {
    printf("  grad and xyz mismatch: %i vs %i \n",nptsg,npts);
    exit(1);
  }
  fileshess = new string[MAX_GEOMS];
  int nptsh = get_files("scratch/qcsave"+nstr,"icp",fileshess);
  if (nptsh!=npts)
  {
    printf("  hess and xyz mismatch: %i vs %i \n",nptsh,npts);
    exit(1);
  }

  file0 = filesxyz[0];

  anames = new string[N3];
  xyz = new double*[npts+1];
  for (int i=0;i<npts;i++)
    xyz[i] = new double[N3];
  for (int i=0;i<npts;i++)
  for (int j=0;j<N3;j++)
    xyz[i][j] = 0.;
  energies = new double[npts];

 //read in geometries
  xyz_read(anames,xyz[0],filesxyz[0]);
  read_xyzs(energies,xyz,filesxyz);
 
  anumbers = new int[natoms];
  for (int i=0;i<natoms;i++)
    anumbers[i]=PTable::atom_number(anames[i]);

  if (printl)
  {
    printf(" energies:");
    for (int i=0;i<npts;i++)
      printf(" %4.3f",energies[i]);
    printf("\n");
  }


  nbonds = 0; nangles = 0; ntor = 0;
  int** bonds = NULL; int** angles = NULL; int** torsions = NULL;
  ICoord ic1, ic2;
  setup_ic(ic1,ic2);

  distances = new double[npts*npts];
  distancesu = new double[npts*npts]; //for later
  get_distances(npts,xyz,ic1,ic2);

#if 0
  printf(" distances: \n");
  for (int i=0;i<npts;i++)
  {
    for (int j=0;j<npts;j++)
      printf(" %4.3f",distances[i*npts+j]);
    printf("\n");
  }
#endif


  grads = new double*[npts]; 
  for (int i=0;i<npts;i++)
    grads[i] = new double[N3];
  for (int i=0;i<npts;i++)
  for (int j=0;j<N3;j++)
    grads[i][j] = 0.;

 //read in gradients
  read_xyzs(NULL,grads,filesgrad);

 
  hess = new double*[npts];
  for (int i=0;i<npts;i++)
    hess[i] = new double[nic*nic];
  for (int i=0;i<npts;i++)
  for (int j=0;j<nic*nic;j++)
    hess[i][j] = 0.;
  useH = new int[npts];
  for (int i=0;i<npts;i++)
    useH[i] = 1;

 //read in hessians
  int nhess = read_hess(nic,fileshess);
  printf("\n");

#if 1
 //using all Hessian information
  if (printl)
    printf(" using Hessian for all points! \n");
  for (int i=0;i<npts;i++)
    useH[i] = 1;
#endif

 //CPMZ here
  id = new int[MAX_GEOMS];
  for (int i=0;i<npts;i++)
    id[i] = i;


  //CPMZ
  //release_ic();

  return npts;
}

void KNNR::freemem()
{
  printf(" freeing all memory allocated in KNNR \n");

  delete [] anames;
  for (int i=0;i<npts;i++)
    delete [] xyz[i];
  delete [] xyz;
  delete [] energies;
  delete [] distances;
  delete [] distancesu;
  for (int i=0;i<npts;i++) 
    delete [] grads[i];
  delete [] grads;
  for (int i=0;i<npts;i++)
    delete [] hess[i];
  delete [] hess;
  delete [] useH;

  delete [] id;

}

double KNNR::test_points(int k0)
{
  ICoord ic1, ic2;
  setup_ic(ic1,ic2);

  double errort = 0.;
  double error1 = 0.;
  int found = 0;
  for (int i=0;i<npts;i++)
  {
    error1 = fabs(predict_point(i,k0,ic1,ic2));
    errort += error1;
    if (error1>0.0000001)
      found++;
  }
  printf(" total error: %8.6f average: %8.6f found: %i \n",errort,errort/found,found);

  return errort;
}




double KNNR::predict_point(int pt, int k, ICoord& ic1, ICoord& ic2)
{
  if (npts<k) return 99.;

  printf("\n now testing on data point: %i \n",pt);

  int* knn = new int[k];
  double* knnd = new double[k];
  for (int i=0;i<k;i++) knn[i] = -1;
  for (int i=0;i<k;i++) knnd[i] = 10000.;

///CPMZ here
#if 1
  get_distances_u(npts,pt,ic1,ic2);
  int nfound = find_knn(pt,k,knn,knnd,2); //type 1 is pIC distances, 2 is U
#else
  int nfound = find_knn(pt,k,knn,knnd,1);
#endif

  //printf(" found %i nn \n",nfound);


  double totdist = 0.;
  for (int i=0;i<k;i++)
    totdist += knnd[i];
  for (int i=0;i<k;i++)
  if (knnd[i] < 0.00001)
    totdist = 0.;

  if (nfound<k) 
  {
    printf(" didn't find enough neighbors \n");
    exit(1);
  }
  double dscale = 10*nic*0.01/1.0;
  if (totdist > dscale*k)
  {
    printf(" too far away: %4.3f / %4.3f \n",totdist,dscale*k);
    delete [] knn;
    delete [] knnd;
    return 0.;
  }

  double* knnw = new double[k];

#if 1
  //exponential weighting
  double alpha = 0.05;
  for (int i=0;i<k;i++)
    knnw[i] = exp(-knnd[i]/alpha);
#else
 //inverse weighting
  for (int i=0;i<k;i++)
  if (knnd[i] > 0.00001)
    knnw[i] = 1./knnd[i];
  else
    knnw[i] = 1000000.;
#endif

  double norm = 0.;
  for (int i=0;i<k;i++)
    norm += knnw[i];
  for (int i=0;i<k;i++)
    knnw[i] = knnw[i] / norm;

  if (norm < 0.000001)
  {
    printf(" low weights \n");
    return 99.;
  }

#if 0
  printf(" weights:");
  for (int i=0;i<k;i++)
    printf(" %4.3f",knnw[i]);
  printf("\n");
#endif

  double E0 = 0.;
  for (int i=0;i<k;i++)
    E0 += knnw[i] * energies[knn[i]];
  printf(" E: %8.6f E0(knn): %8.6f \n",energies[pt],E0);

#if 1
  printf(" energies of knn:");
  for (int i=0;i<k;i++)
    printf(" %8.6f",energies[knn[i]]);
  printf("\n"); fflush(stdout);
#endif


 //current coordinates
  int N3 = natoms*3;
  ic1.reset(xyz[pt]);
  ic1.bmatp_create();
  ic1.bmatp_to_U();
  ic1.bmat_create();
  for (int j=0;j<N3;j++)
    ic1.grad[j] = grads[pt][j];
  ic1.grad_to_q();
  ic2.reset(xyz[pt]);
  ic2.bmatp_create();
  ic2.bmatp_to_U();
  ic2.bmat_create();

  int nic = ic1.nbonds + ic1.nangles + ic1.ntor;
  int nicd = ic1.nicd0;
  //printf(" nic: %i nicd: %i \n",nic,nicd);

  double* q0 = new double[nicd];
  for (int i=0;i<nicd;i++)
    q0[i] = ic1.q[i];

 //actual gradient
  double* gradq0 = new double[nicd];
  for (int j=0;j<nicd;j++)
    gradq0[j] = ic1.gradq[j];

 //interpolated gradient
  double* Hg = new double[nicd];
  double* dq = new double[nicd];
  double* gradqf = new double[nicd];
  for (int j=0;j<nicd;j++) gradqf[j] = 0.;
  double** gradq = new double*[k];
  for (int i=0;i<k;i++)
    gradq[i] = new double[nicd];
  for (int i=0;i<k;i++)
  for (int j=0;j<nicd;j++)
    gradq[i][j] = 0.;
  double** gradq1 = new double*[k];
  for (int i=0;i<k;i++)
    gradq1[i] = new double[nicd];
  for (int i=0;i<k;i++)
  for (int j=0;j<nicd;j++)
    gradq1[i][j] = 0.;
  double* gradqd = new double[nicd];
  for (int j=0;j<nicd;j++) gradqd[j] = 0.;

  double* Ut = ic1.Ut;
  double* dq1 = new double[nic];

  double E1 = 0.; //gradient energy correction
  double E2 = 0.; //Hessian energy correction
  for (int i=0;i<k;i++)
  {
    int nni = knn[i];
    double kw = knnw[i];
    //printf(" nni: %i kw: %4.3f \n",nni,kw);

    ic1.reset(xyz[nni]);
    for (int j=0;j<N3;j++)
      ic1.grad[j] = grads[nni][j];
    ic1.bmatp_create();
    ic1.bmat_create();
    ic1.grad_to_q();

#if 1
    get_dqpic(dq1,ic2,ic1);
    for (int j=0;j<nicd;j++) dq[j] = 0.;
    for (int j=0;j<nicd;j++)
    for (int l=0;l<nic;l++)
      dq[j] += Ut[j*nic+l] * dq1[l];
#else
    for (int j=0;j<nicd;j++)
      dq[j] = q0[j] - ic1.q[j];
#endif

   //energy correction due to gradient
    double E1p = 0.;
    for (int j=0;j<nicd;j++)
      E1p += ic1.gradq[j] * dq[j]; //gT dq
    E1 += kw * E1p;


   //interpolated gradient
    for (int j=0;j<nicd;j++)
      gradq[i][j] = ic1.gradq[j];
 

    //printf(" useH[nni]: %i nni: %i\n",useH[nni],nni); fflush(stdout);
    if (useH[nni]<3)
    {
     //1. use stored hessian --> U
      for (int j=0;j<nic*nic;j++)
        ic1.Hintp[j] = hess[nni][j];
      ic1.Hintp_to_Hint();

#if 0
      double* Hnow = new double[nicd*nicd];
      double* Heig = new double[nicd];
      for (int j=0;j<nicd*nicd;j++)
        Hnow[j] = ic1.Hint[j];
      Diagonalize(Hnow,Heig,nicd);

      printf(" Heig:");
      for (int j=0;j<nicd;j++)
        printf(" %4.3f",Heig[j]);
      printf("\n");
      delete [] Hnow;
      delete [] Heig;
#endif

     //2. update gradient
      for (int j=0;j<nicd;j++) gradqd[j] = 0.;
      for (int j=0;j<nicd;j++)
      for (int l=0;l<nicd;l++)
        gradqd[j] += ic1.Hint[j*nicd+l] * dq[l];

      for (int j=0;j<nicd;j++)
        gradq1[i][j] = gradqd[j];

     //3. update energy
      double E2p = 0.;
      for (int j=0;j<nicd;j++) Hg[j] = 0.;
      for (int j=0;j<nicd;j++)
      for (int l=0;l<nicd;l++)
        Hg[j] += ic1.Hint[j*nicd+l] * dq[l];
      for (int j=0;j<nicd;j++)
        E2p += dq[j] * Hg[j];
      E2 += 0.5 * kw * E2p;

    }//if useH

  } //loop i over kNN


  //add 1st order term to 2nd order gradient
  for (int i=0;i<k;i++)
  for (int j=0;j<nicd;j++)
    gradq1[i][j] += gradq[i][j];

  for (int i=0;i<k;i++)
  for (int j=0;j<nicd;j++)
    gradqf[j] += knnw[i] * gradq1[i][j];

#if 1
 //measuring differences between gradient estimates
  double* gerr = new double[k];
  double* govl = new double[k];
  double gnorm = 0.;
  for (int i=0;i<k;i++) gerr[i] = 0.;
  for (int i=0;i<k;i++) govl[i] = 0.;
  for (int i=0;i<k;i++)
  for (int j=0;j<nicd;j++)
  {
    govl[i] += gradq0[j] * gradq1[i][j];
    double ge = gradq0[j] - gradq1[i][j];
    gerr[i] += ge*ge;
    gnorm += gradq0[j] * gradq0[j];
  }
  //printf(" gnorm: %5.4f norm: %5.4f \n",gnorm,sqrt(gnorm));
  gnorm = sqrt(gnorm);
  for (int i=0;i<k;i++)
  {
    gerr[i] = sqrt(gerr[i]/N3);
    govl[i] = govl[i] / gnorm;
  }
  double gerra = 0.;
  for (int i=0;i<k;i++)
    gerra += gerr[i];
  gerra = gerra / k / sqrt(N3);

  printf(" gerr:");
  for (int i=0;i<k;i++)
    printf(" %6.5f",gerr[i]);
  printf("\n");
  printf(" govl:");
  for (int i=0;i<k;i++)
    printf(" %4.3f",govl[i]);
  printf("\n");
  delete [] govl;
  delete [] gerr;
#if 1
  for (int i=0;i<k;i++)
  {
    printf(" gradq1(nni: %i):",knn[i]);
    for (int j=0;j<nicd;j++) 
      printf(" %5.4f",gradq1[i][j]);
    printf("\n");
  }
#endif
#endif


  double error0 = energies[pt] - E0;
  double error1 = energies[pt] - (E0+E1);
  double error2 = energies[pt] - (E0+E1+E2);
  printf(" error(0): %8.6f error(1): %8.6f error(2): %8.6f",error0,error1,error2);
  printf(" totald: %4.3f \n",totdist);
  printf(" e(2)-e(0): %6.4f e(1)-e(0): %6.4f \n",fabs(error2)-fabs(error0),fabs(error1)-fabs(error0));

  double graderr0 = 0.;
  double graderr1 = 0.;
  double gradrms = 0.;
  for (int i=0;i<nicd;i++)
  {
    gradrms += gradq0[i] * gradq0[i];
    double ge1 = gradqf[i] - gradq0[i];
    graderr1 += ge1 * ge1;
  }
  gradrms = sqrt(gradrms/N3);
  graderr1 = sqrt(graderr1/N3);

  printf(" dist/graderrs(1): %4.3f %6.5f \n",totdist,graderr1);
  printf(" dist/grel(1): %4.3f %5.4f \n",totdist,graderr1/gradrms);
  //printf(" dist/grel(1)/gerr(avg): %4.3f %5.4f %5.4f \n",totdist,graderr1/gradrms,gerra);

#if 1
  printf(" gradqf vs gradq0: \n");
  for (int j=0;j<nicd;j++)
    printf(" %5.4f",gradqf[j]);
  printf("\n");
  for (int j=0;j<nicd;j++)
    printf(" %5.4f",gradq0[j]);
  printf("\n");
#endif

  delete [] q0;
  for (int i=0;i<k;i++)
    delete [] gradq[i];
  delete [] gradq;
  for (int i=0;i<k;i++)
    delete [] gradq1[i];
  delete [] gradq1;
  delete [] gradqd;
  delete [] gradq0;
  delete [] dq1;

#if 0
  printf(" knn:");
  for (int i=0;i<k;i++)
    printf(" %i",knn[i]);
  printf("\n");
#endif
#if 1
  printf(" knnd:");
  for (int i=0;i<k;i++)
    printf(" %4.3f",knnd[i]);
//    printf(" %4.3f/%i",knnd[i],knn[i]);
  printf("\n");
#endif
#if 1
  printf(" knnw:");
  for (int i=0;i<k;i++)
    printf(" %4.3f",knnw[i]);
  printf("\n");
#endif
#if 0
  printf(" dist(%i):",pt);
  for (int i=0;i<npts;i++)
    printf(" %4.3f",distances[pt*npts+i]);
  printf("\n");
#endif

  delete [] knn;
  delete [] knnd;
  delete [] knnw;

  return graderr1/gradrms;
//  return error1;
}


double KNNR::grad_knnr(double* coords, double &Ep, double* g1, double* Ut, int k)
{
  //printf("\n in grad_knnr \n"); fflush(stdout);

  if (npts<k) return 99.;

  double errorest = 1000.;
 
  int* knn = new int[k+1];
  double* knnd = new double[k+1];
  for (int i=0;i<k;i++) knn[i] = -1;
  for (int i=0;i<k;i++) knnd[i] = 10000.;

  ICoord ic1, ic2;
  setup_ic(ic1,ic2);

 //current coordinates/Ut
  int nic = ic1.nbonds + ic1.nangles + ic1.ntor;
  ic1.reset(coords);
  ic1.bmatp_create();
  ic1.bmatp_to_U();
  for (int i=0;i<nic*nic;i++)
    ic1.Ut[i] = Ut[i]; //from original coordinate system
  ic1.bmat_create();
  ic2.reset(coords);
  ic2.bmatp_create();
  ic2.bmatp_to_U();
  for (int i=0;i<nic*nic;i++)
    ic1.Ut[i] = Ut[i]; //from original coordinate system
  ic2.bmat_create();

 //uses U for distances
  int nfound = find_knn_xyz(coords,k,knn,knnd,ic1,ic2);
  //printf(" found %i nn \n",nfound); fflush(stdout);

 //reset to current coordinates
  ic1.reset(coords);
  ic1.bmat_create();


  int r_one = 0;
  double totdist = 0.;
  for (int i=0;i<k;i++)
    totdist += knnd[i];
  for (int i=0;i<k;i++)
  if (knnd[i] < 0.001)
  {
    totdist = 0.;
    r_one = i+1;
  }

  if (nfound<k) 
  {
    printf(" didn't find enough neighbors (%i/%i) \n",nfound,k);
    release_ic(ic1,ic2);
    delete [] knn;
    delete [] knnd;
    return 99.0;
  }
  if (totdist > 1.0*k)
  {
    //printf(" too far away: %4.3f \n",totdist);
    release_ic(ic1,ic2);
    delete [] knn;
    delete [] knnd;
    return 99.0;
  }

  double* knnw = new double[k];

#if 1
  //exponential weighting
  double alpha = 0.15;
  for (int i=0;i<k;i++)
    knnw[i] = exp(-knnd[i]/alpha);
#else
 //inverse weighting
  for (int i=0;i<k;i++)
  if (knnd[i] > 0.00001)
    knnw[i] = 1./knnd[i];
  else
    knnw[i] = 1000000.;
#endif

  double norm = 0.;
  for (int i=0;i<k;i++)
    norm += knnw[i];
  for (int i=0;i<k;i++)
    knnw[i] = knnw[i] / norm;

  if (norm < 0.000001)
  {
    printf(" low weights \n");
    return 99.;
  }

  if (r_one)
  {
    for (int i=0;i<k;i++)
      knnw[i] = 0.;
    knnw[r_one-1] = 1.;
  }


#if 0
  printf(" weights:");
  for (int i=0;i<k;i++)
    printf(" %4.3f",knnw[i]);
  printf("\n");
#endif

  double E0 = 0.;
  for (int i=0;i<k;i++)
    E0 += knnw[i] * energies[knn[i]];
  //printf(" E0(knn): %8.6f \n",E0);

#if 0
  printf(" energies of knn:");
  for (int i=0;i<k;i++)
    printf(" %8.6f",energies[knn[i]]);
  printf("\n"); fflush(stdout);
#endif

  int N3 = natoms*3;
  int nicd = ic1.nicd0;
  //printf(" nic: %i nicd: %i \n",nic,nicd);

  double* q0 = new double[nicd];
  for (int i=0;i<nicd;i++)
    q0[i] = ic1.q[i];

#if 0
  printf(" q0:");
  for (int i=0;i<nicd;i++)
    printf(" %4.3f",q0[i]);
  printf("\n");
#endif
#if 0
  printf(" Ut:");
  for (int i=0;i<nic;i++)
  {
    for (int j=0;j<nic;j++)
      printf(" %4.3f",Ut[i*nic+j]);
    printf("\n");
  }
  printf("\n");
#endif

 //interpolated gradient
  double* Hg = new double[nicd];
  double* dq = new double[nicd];
  double* gradq = new double[nicd];
  for (int j=0;j<nicd;j++) gradq[j] = 0.;
  double* gradq1 = new double[nicd];
  for (int j=0;j<nicd;j++) gradq1[j] = 0.;
  double* gradqd = new double[nicd];
  for (int j=0;j<nicd;j++) gradqd[j] = 0.;
  double* dq1 = new double[nic];

  double E1 = 0.; //gradient energy correction
  double E2 = 0.; //Hessian energy correction
  for (int i=0;i<k;i++)
  if (knnw[i]>0.000001)
  {
    int nni = knn[i];
    double kw = knnw[i];
    //printf(" nni: %i kw: %4.3f \n",nni,kw);

    ic1.reset(xyz[nni]);
    for (int j=0;j<N3;j++)
      ic1.grad[j] = grads[nni][j];
    ic1.bmatp_create();
    ic1.bmat_create();
    ic1.grad_to_q();

#if 1
    get_dqpic(dq1,ic2,ic1);
    for (int j=0;j<nicd;j++) dq[j] = 0.;
    for (int j=0;j<nicd;j++)
    for (int l=0;l<nic;l++)
      dq[j] += Ut[j*nic+l] * dq1[l];
#else
    for (int j=0;j<nicd;j++)
      dq[j] = q0[j] - ic1.q[j];
#endif

   //energy correction due to gradient
    double E1p = 0.;
    for (int j=0;j<nicd;j++)
      E1p += ic1.gradq[j] * dq[j]; //gT dq
    E1 += kw * E1p;


   //interpolated gradient
    for (int j=0;j<nicd;j++)
      gradq[j] += kw * ic1.gradq[j];
 

    //printf(" useH[nni]: %i nni: %i\n",useH[nni],nni); fflush(stdout);
    if (useH[nni]<3 && !r_one)
    {
     //1. use stored hessian --> U
      for (int j=0;j<nic*nic;j++)
        ic1.Hintp[j] = hess[nni][j];
      ic1.Hintp_to_Hint();

     //2. update gradient
      for (int j=0;j<nicd;j++) gradqd[j] = 0.;
      for (int j=0;j<nicd;j++)
      for (int l=0;l<nicd;l++)
        gradqd[j] += ic1.Hint[j*nicd+l] * dq[l];
 
      for (int j=0;j<nicd;j++)
        gradq1[j] += kw * gradqd[j];

     //3. update energy
      double E2p = 0.;
      for (int j=0;j<nicd;j++) Hg[j] = 0.;
      for (int j=0;j<nicd;j++)
      for (int l=0;l<nicd;l++)
        Hg[j] += ic1.Hint[j*nicd+l] * dq[l];
      for (int j=0;j<nicd;j++)
        E2p += dq[j] * Hg[j];
      E2 += 0.5 * kw * E2p;

    }//if useH

#if 0
  printf(" grad:");
  for (int j=0;j<N3;j++)
    printf(" %4.3f",grads[nni][j]);
  printf("\n");
  printf(" gradq:");
  for (int j=0;j<nicd;j++)
    printf(" %4.3f",gradq[j]);
  printf("\n");
#endif


  } //loop i over kNN
  //printf("\n");

  //add 1st order term to 2nd order gradient
  for (int j=0;j<nicd;j++)
    gradq1[j] += gradq[j];

 //copy full gradient
  for (int j=0;j<nicd;j++)
    g1[j] = gradq1[j];

#if 0
  printf(" gradq vs. gradq1: \n");
  for (int j=0;j<nicd;j++)
    printf(" %5.4f",gradq[j]);
  printf("\n");
#endif
#if 0
  printf(" gradq(knnr):");
  for (int j=0;j<nicd;j++)
    printf(" %12.10f",gradq1[j]);
  printf("\n");
#endif

  delete [] q0;
  delete [] gradq;
  delete [] gradq1;
  delete [] gradqd;
  delete [] dq1;

#if 0
  printf(" knn:");
  for (int i=0;i<k;i++)
    printf(" %i",knn[i]);
  printf("\n");
#endif
#if 1
  printf(" knnd:");
  for (int i=0;i<k;i++)
    printf(" %4.3f",knnd[i]);
//    printf(" %4.3f/%i",knnd[i],knn[i]);
//  printf("\n");
#endif
#if 0
  printf(" knnw:");
  for (int i=0;i<k;i++)
    printf(" %4.3f",knnw[i]);
  printf("\n");
#endif


  delete [] knn;
  delete [] knnd;
  delete [] knnw;

  release_ic(ic1,ic2);


  Ep = E0 + E1 + E2;
  //printf(" energy: %8.6f \n",Ep);

  errorest = totdist;
  return errorest;
}


int KNNR::get_files(string fileprefix, string filesuffix, string* files)
{
  int npts = 0;

  string nstr = StringTools::int2str(runnum,4,"0");
#if 0
  printf(" loop to get files \n");
#else
  string cmd = "rm filelist"+nstr;
  //printf(" cmd1: %s \n",cmd.c_str()); fflush(stdout);
  system(cmd.c_str());
  cmd = "ls "+fileprefix+"*"+filesuffix+" > filelist"+nstr;
  //printf(" cmd2: %s \n",cmd.c_str()); fflush(stdout);
  system(cmd.c_str());

  string flist = "filelist"+nstr;
  ifstream infile;
  infile.open(flist.c_str());
  if (!infile){
    printf(" Error: couldn't open %s \n",flist.c_str());
    exit(-1);
  } 

  int n = 0;  
  string line;
  bool success=true;
  while (!infile.eof())
  {
    success=getline(infile, line);
    //cout << "RR: " << line << endl;
    int length=StringTools::cleanstring(line);
    vector<string> tok_line = StringTools::tokenize(line, " \t");
    if (tok_line.size()>0)
      files[n++]=tok_line[0];
  }
  
  infile.close();
#endif

  npts = n;

  return npts;
}



int KNNR::read_ics(int& nbonds, int** bonds, int& nangles, int** angles, int& ntor, int** torsions, string filename)
{
  ifstream infile;
  infile.open(filename.c_str());
  if (!infile){
    printf(" Error: couldn't open icfile: %s \n",filename.c_str());
    exit(-1);
  } 

  string line;
  bool success=true;
  int type = 1;
  success=getline(infile, line);
  while (!infile.eof())
  {
    success=getline(infile, line);
    //cout << "RR0: " << line << endl;
    int length=StringTools::cleanstring(line);
    vector<string> tok_line = StringTools::tokenize(line, " \t");
    if (tok_line.size()>0)
    {
      if (type==1)
      {
        nbonds = atoi(tok_line[1].c_str());
        bonds = new int*[nbonds];
        for (int j=0;j<nbonds;j++)
          bonds[j] = new int[2];

        for (int i=0;i<nbonds;i++)
        {
          success=getline(infile, line);
          //cout << "RR: " << line << endl;
          length=StringTools::cleanstring(line);
          tok_line = StringTools::tokenize(line, " \t");
          bonds[i][0] = atoi(tok_line[0].c_str());
          bonds[i][1] = atoi(tok_line[1].c_str());
        }
        //printf(" found %i bonds \n",nbonds);
        type++;
      } //read bonds
      else if (type==2)
      {
        //printf(" reading angles \n"); fflush(stdout);
        nangles = atoi(tok_line[1].c_str());
        angles = new int*[nangles];
        for (int j=0;j<nangles;j++)
          angles[j] = new int[3];

        for (int i=0;i<nangles;i++)
        {
          success=getline(infile, line);
          //cout << "RR: " << line << endl;
          length=StringTools::cleanstring(line);
          tok_line = StringTools::tokenize(line, " \t");
          angles[i][0] = atoi(tok_line[0].c_str());
          angles[i][1] = atoi(tok_line[1].c_str());
          angles[i][2] = atoi(tok_line[2].c_str());
        }
        //printf(" found %i nangles \n",nangles);
        type++;
      } //read angles
      else if (type==3)
      {
        //printf(" reading torsions \n"); fflush(stdout);
        ntor = atoi(tok_line[1].c_str());
        torsions = new int*[ntor];
        for (int j=0;j<ntor;j++)
          torsions[j] = new int[4];

        for (int i=0;i<ntor;i++)
        {
          success=getline(infile, line);
          //cout << "RR: " << line << endl;
          length=StringTools::cleanstring(line);
          tok_line = StringTools::tokenize(line, " \t");
          torsions[i][0] = atoi(tok_line[0].c_str());
          torsions[i][1] = atoi(tok_line[1].c_str());
          torsions[i][2] = atoi(tok_line[2].c_str());
          torsions[i][3] = atoi(tok_line[3].c_str());
        }
        //printf(" found %i torsions \n",ntor);
        type++;
      } //read torsion
      if (type>3) break;
    }
  } //while !eof

#if 0
  printf(" bonds: \n");
  for (int i=0;i<nbonds;i++)
    printf(" %i %i \n",bonds[i][0],bonds[i][1]);
  printf(" angles: \n");
  for (int i=0;i<nangles;i++)
    printf(" %i %i %i \n",angles[i][0],angles[i][1],angles[i][2]);
  printf(" torsions: \n");
  for (int i=0;i<ntor;i++)
    printf(" %i %i %i %i \n",torsions[i][0],torsions[i][1],torsions[i][2],torsions[i][3]);
#endif

  
  infile.close();

  return nbonds + nangles + ntor;
}

void KNNR::xyz_read(string* anames, double* coords, string xyzfile)
{ 
   
  ifstream infile;
  infile.open(xyzfile.c_str());
  if (!infile){
    printf(" Error: couldn't open XYZ file \n");
    exit(-1);
  } 
  
  string line;
  bool success=true;
  success=getline(infile, line);
  if (success){
    int length=StringTools::cleanstring(line);
    //natoms=atoi(line.c_str());
  }
  //printf(" natoms: %i \n",natoms);
  
  success=getline(infile, line);
  
  for (int i=0;i<natoms;i++)
  {
    success=getline(infile, line);
    int length=StringTools::cleanstring(line);
    vector<string> tok_line = StringTools::tokenize(line, " \t");
    anames[i]=tok_line[0];
    coords[3*i+0]=atof(tok_line[1].c_str());
    coords[3*i+1]=atof(tok_line[2].c_str());
    coords[3*i+2]=atof(tok_line[3].c_str());
  }
  
  infile.close();

#if 0
  printf(" XYZ: \n");
  for (int i=0;i<natoms;i++)
    printf(" %s %8.6f %8.6f %8.6f \n",anames[i].c_str(),coords[3*i+0],coords[3*i+1],coords[3*i+2]);
#endif

  printf(" read XYZ"); fflush(stdout);
 
  return;
}   


int KNNR::read_xyzs(double* energies, double** coords, string* filenames)
{
  int N3 = natoms*3;

  int n = 0;
  int nfiles = npts;

  for (int i=0;i<nfiles;i++)
  {
    string filename = filenames[i];
    if (energies!=NULL)
      energies[i] = read_one_xyz(i,filename,coords[i]);
    else
      read_one_xyz(i,filename,coords[i]);
    n++;
  } //loop i over files

#if 0
  for (int i=0;i<nfiles;i++)
  {
    printf("\n geom %i from %s \n",i,filenames[i].c_str());
    for (int j=0;j<natoms;j++)
      printf(" %4.3f %4.3f %4.3f \n",coords[i][3*j+0],coords[i][3*j+1],coords[i][3*j+2]);
  }
#endif

  printf(" read geoms/grads (%i)",n);

  return n;
}


double KNNR::read_one_xyz(int n, string filename, double* coords)
{
  double energy = 0.;
  //printf(" working on: %s \n",filename.c_str()); fflush(stdout);

    ifstream infile;
    infile.open(filename.c_str());
    if (!infile){
      printf(" error opening: %s \n",filename.c_str());
      exit(-1);
    }

    string line;
    bool success=true;    

    int done = 0;
    while(!infile.eof() && !done)
    {
      success = getline(infile,line);
      success = getline(infile,line);

      //cout << " RR: " << line << endl; fflush(stdout);
      int length=StringTools::cleanstring(line);
      vector<string> tok_line = StringTools::tokenize(line, " \t");
      energy = atof(tok_line[0].c_str());

      if (!infile.eof())
      {
        for (int j=0;j<natoms;j++)
        {
          getline(infile,line);
          //cout << " RR: " << line << endl; fflush(stdout);
          int length=StringTools::cleanstring(line);
          vector<string> tok_line = StringTools::tokenize(line, " \t");
//        cout << " i: " << i << " string: " << line << endl;
          //if (tok_line.size()!=4) { printf(" size problem: %i \n",tok_line.size()); fflush(stdout); }
          coords[3*j+0]=atof(tok_line[1].c_str());
          coords[3*j+1]=atof(tok_line[2].c_str());
          coords[3*j+2]=atof(tok_line[3].c_str());
        }
        n++; 
        done = 1;
        if (n>MAX_GEOMS-1)
        {
          printf(" WARNING: EXCEEDING MAXGEOMS: %i \n",n);
          exit(-1);
        }
      } // if !eof
    } // while !eof

    infile.close();

  //printf(" done reading \n"); fflush(stdout);

  return energy;
}


int KNNR::read_hess(int nic, string* filenames)
{
  int nic2 = nic*nic;

  int n = 0;
  int nfiles = npts;

  for (int i=0;i<nfiles;i++)
  {
    string filename = filenames[i];
    read_one_hess(i,nic,filename);
  } //loop i over xyz files

#if 0
  printf(" useH:");
  for (int i=0;i<nfiles;i++)
    printf(" %i",useH[i]);
  printf("\n");
#endif
#if 0
  for (int i=0;i<nfiles;i++)
  {
    printf("\n geom %i from %s \n",i,filenames[i].c_str());
    for (int j=0;j<nic;j++)
    {
      for (int k=0;k<nic;k++)
        printf(" %4.3f",hess[i][j*nic+k]);
      printf("\n");
    }
  }
#endif

  printf(" read hessians");

  return n;
}

void KNNR::read_one_hess(int n, int nic, string filename)
{
  if (n>npts) return;

    //printf(" in read_one_hess for %s \n",filename.c_str()); fflush(stdout);
    ifstream infile;
    infile.open(filename.c_str());
    if (!infile){
      printf(" error opening: %s \n",filename.c_str());
      exit(-1);
    }

    string line;
    bool success=true;    

    int done = 0;
    while(!infile.eof() && !done)
    {
      success = getline(infile,line);
      //cout << "RR0: " << line << endl;
      success = getline(infile,line);
      //cout << "RR0: " << line << endl;
      int length=StringTools::cleanstring(line);
      vector<string> tok_line = StringTools::tokenize(line, " \t");
      if (tok_line.size()>2)
        useH[n] = atoi(tok_line[3].c_str());
      if (useH[n]<1)
        useH[n] = 0;

      if (!infile.eof())
      {
        for (int j=0;j<nic;j++)
        {
          getline(infile,line);
          //cout << " RR: " << line << endl; fflush(stdout);
          length=StringTools::cleanstring(line);
          tok_line = StringTools::tokenize(line, " \t");
          //cout << " i: " << i << " string: " << line << endl;
          for (int k=0;k<nic;k++)
            hess[n][j*nic+k]=atof(tok_line[k].c_str());
        }
        done = 1;
      } // if !eof
    } // while !eof

    infile.close();

  return;
}

void KNNR::get_distances(int npts1, double** xyz, ICoord& ic1, ICoord& ic2)
{
  for (int n=0;n<npts1;n++)
  for (int m=0;m<n;m++)
  {
    distances[n*npts+m] = get_distance(xyz[n],xyz[m],ic1,ic2);
  }
  for (int n=0;n<npts1;n++)
  for (int m=n+1;m<npts1;m++)
    distances[n*npts+m] = distances[m*npts+n];

  return;
}

double KNNR::get_distance(double* xyz1, double* xyz2, ICoord& ic1, ICoord& ic2)
{
  double d = 0.;

  int nbonds = ic1.nbonds;
  int nangles = ic1.nangles;
  int ntor = ic1.ntor;
  int nic = nbonds + nangles + ntor;

  double* dq = new double[nic];

  ic1.reset(xyz1);
  ic2.reset(xyz2);

  ic1.update_ic();
  ic2.update_ic();

  for (int i=0;i<nbonds;i++)
    dq[i] = ic1.bondd[i] - ic2.bondd[i];
  
  for (int i=0;i<nangles;i++)
    dq[nbonds+i] = (ic1.anglev[i] - ic2.anglev[i])*3.14159/180.;
 
  for (int i=0;i<ntor;i++)
  {
    double tordiff = ic1.torv[i] - ic2.torv[i];
    double torfix = 0.;
    if (tordiff>180.) torfix = 360.;
    else if (tordiff<-180.) torfix = -360.;
    dq[nbonds+nangles+i] = (tordiff + torfix)*3.14159/180.;
  }

  for (int i=0;i<nic;i++)
    d += dq[i]*dq[i];
  d = sqrt(d);

#if 0
  printf(" dq:");
  for (int i=0;i<nic;i++)
    printf(" %4.3f",dq[i]);
  printf("\n");
#endif

  delete [] dq;

  return d;
} 

void KNNR::get_dqpic(double* dq1, ICoord& ic1, ICoord& ic2)
{
  int nbonds = ic1.nbonds;
  int nangles = ic1.nangles;
  int ntor = ic1.ntor;

  ic1.update_ic();
  ic2.update_ic();

  for (int i=0;i<nbonds;i++)
    dq1[i] = ic1.bondd[i] - ic2.bondd[i];

  for (int i=0;i<nangles;i++)
    dq1[nbonds+i] = (ic1.anglev[i] - ic2.anglev[i])*3.14159/180.;
 
  for (int i=0;i<ntor;i++)
  {
    double tordiff = ic1.torv[i] - ic2.torv[i];
    double torfix = 0.;
    if (tordiff>180.) torfix = 360.;
    else if (tordiff<-180.) torfix = -360.;
    dq1[nbonds+nangles+i] = (tordiff + torfix)*3.14159/180.;
  }

  return;
}

void KNNR::get_distances_u(int npts1, int pt, ICoord& ic1, ICoord& ic2)
{
  ic1.reset(xyz[pt]);
  ic1.bmatp_create();
  ic1.bmatp_to_U();
  ic1.bmat_create();
  ic2.reset(xyz[pt]);
  ic2.bmatp_create();
  ic2.bmatp_to_U();
  ic1.bmat_create();

//only calc needed points
  int n = pt;
  for (int m=0;m<npts1;m++)
  if (m!=n)
  {
    distancesu[n*npts+m] = get_distance_u(xyz[n],xyz[m],ic1,ic2);
  }
  else
    distancesu[n*npts+m] = 0.;

  return;
}


double KNNR::get_distance_u(double* xyz1, double* xyz2, ICoord& ic1, ICoord& ic2)
{
  int nicd = ic1.nicd0;
  double d = 0.;

  int nbonds = ic1.nbonds;
  int nangles = ic1.nangles;
  int ntor = ic1.ntor;
  int nic = nbonds + nangles + ntor;

  double* dq1 = new double[nic];
  double* dq = new double[nicd];
  for (int i=0;i<nicd;i++) dq[i] = 0.;

  ic1.reset(xyz1);
  ic2.reset(xyz2);

  get_dqpic(dq1,ic1,ic2);

  double* Ut = ic1.Ut;
  for (int i=0;i<nicd;i++)
  for (int j=0;j<nic;j++)
    dq[i] += Ut[i*nic+j] * dq1[j];

  for (int i=0;i<nicd;i++)
    d += dq[i]*dq[i];
  d = sqrt(d);

#if 0
  printf(" dq:");
  for (int i=0;i<nicd;i++)
    printf(" %4.3f",dq[i]);
  printf(" (%4.3f)\n",d);
#endif

  delete [] dq1;
  delete [] dq;

  return d;
} 

int KNNR::find_knn(int pt, int k, int* knn, double* knnd, int type)
{
  int found = 0;
  //int* close_n = new int[k];
  //double* close_d = new double[k];
  int* close_n = knn;
  double* close_d = knnd;
  for (int i=0;i<k;i++) close_n[i] = -1;
  for (int i=0;i<k;i++) close_d[i] = 10000.;
  double* distances1 = distances;
  if (type==2)
  {
    printf("\n using new distances in coords U \n\n");
    distances1 = distancesu; 
  }

  //printf(" in knn for %i (k=%i) \n",pt,k);

  for (int i=0;i<npts;i++)
  if (i!=pt)
  {
    for (int j=0;j<k;j++)
    if (distances1[pt*npts+i]<close_d[j])
    {
      //printf(" %4.3f is closer than %4.3f \n",distances[pt*npts+i],close_d[j]);
      //printf(" close_d: %4.3f %4.3f %4.3f \n",close_d[0],close_d[1],close_d[2]);
      for (int l=k-1;l>j;l--)
      {
        close_d[l] = close_d[l-1];
        close_n[l] = close_n[l-1];
      }
      close_d[j] = distances1[pt*npts+i];
      close_n[j] = i;

      //printf(" close_d: %4.3f %4.3f %4.3f \n",close_d[0],close_d[1],close_d[2]);

      break;
    } //loop j over current close set
  } //loop i over npts

  for (int i=0;i<k;i++)
  if (close_n[i]>-1)
    found++;

  return found;
}


int KNNR::find_knn_xyz(double* coords, int k, int* knn, double* knnd, ICoord& ic1, ICoord& ic2)
{
  int found = 0;
  //int* close_n = new int[k];
  //double* close_d = new double[k];
  int* close_n = knn;
  double* close_d = knnd;
  for (int i=0;i<k;i++) close_n[i] = -1;
  for (int i=0;i<k;i++) close_d[i] = 10000.;

  //printf(" find_knn_xyz \n");

  double* dnow = new double[npts];
#if 1
  for (int i=0;i<npts;i++)
    dnow[i] = get_distance_u(xyz[i],coords,ic1,ic2);
#else
  for (int i=0;i<npts;i++)
    dnow[i] = get_distance(xyz[i],coords,ic1,ic2);
#endif

#if 0
  if (npts>10)
  {
    print_xyz_gen(natoms,anames,xyz[8]);
    print_xyz_gen(natoms,anames,xyz[9]);
    print_xyz_gen(natoms,anames,coords);
  }
#endif

  for (int i=0;i<npts;i++)
  {
    for (int j=0;j<k;j++)
    if (dnow[i]<close_d[j])
    {
      //printf(" %4.3f is closer than %4.3f \n",distances[pt*npts+i],close_d[j]);
      //printf(" close_d: %4.3f %4.3f %4.3f \n",close_d[0],close_d[1],close_d[2]);
      for (int l=k-1;l>j;l--)
      {
        close_d[l] = close_d[l-1];
        close_n[l] = close_n[l-1];
      }
      close_d[j] = dnow[i];
      close_n[j] = i;

      //printf(" close_d: %4.3f %4.3f %4.3f \n",close_d[0],close_d[1],close_d[2]);

      break;
    } //loop j over current close set
  } //loop i over npts

  for (int i=0;i<k;i++)
  if (close_n[i]>-1)
    found++;

  delete [] dnow;

  return found;
}




void KNNR::add_point(double energy1, double* xyz1, double* grad1, double* hess1)
{
  int N3 = natoms*3;

  int npts1 = npts + 1;
  reassign_mem(npts1);
  energies[npts] = energy1;

  for (int i=0;i<N3;i++)
    xyz[npts][i] = xyz1[i];
  for (int i=0;i<N3;i++)
    grads[npts][i] = grad1[i];
  for (int i=0;i<nic*nic;i++)
    hess[npts][i] = hess1[i];

  ICoord ic1, ic2;
  setup_ic(ic1,ic2);

#if 0
  printf(" recomputing all distances (FIX ME) \n");
  get_distances(npts1,xyz,distances,ic1,ic2);
#endif

  release_ic(ic1,ic2);

  npts = npts1;

  return;
}
