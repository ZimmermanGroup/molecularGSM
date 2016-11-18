#ifndef KNNR_H
#define KNNR_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdio>
#include <vector>
#include <cstring>
#include <math.h>

#include "stringtools.h"
#include "pTable.h"
#include "constants.h"
#include "utils.h"
//pointers only
class ICoord;

class KNNR {
  
  private:
  
   string file0;
   int runnum;
   string* filesxyz;
   string* filesgrad;
   string* fileshess;

   int natoms;
   double** xyz;
   int* anumbers;
   string* anames;

   int* id;
   double* distances;
   double* distancesu;
   double* energies;
   double** grads;
   double** hess;
   int* useH;

   int nic;
   int nbonds; int nangles; int ntor;


   int read_ics(int& nbonds, int** bonds, int& nangles, int** angles, int& ntor, int** torsions, string filename);
   int read_xyzs(double* energies, double** xyz, string* filenames);
   double read_one_xyz(int n, string filename, double* coords);

   void xyz_read(string* anames, double* coords, string xyzfile);

   int read_hess(int nic, string* filenames);
   void read_one_hess(int n, int nic, string filename);
   int get_files(string fileprefix, string filesuffix, string* files);

   void get_distances(int npts, double** xyz, ICoord& ic1, ICoord& ic2);
   double get_distance(double* xyz1, double* xyz2, ICoord& ic1, ICoord& ic2);
   void get_distances_u(int npts, int pt, ICoord& ic1, ICoord& ic2);
   double get_distance_u(double* xyz1, double* xyz2, ICoord& ic1, ICoord& ic2);
   void get_dqpic(double* dq1, ICoord& ic1, ICoord& ic2);

   void setup_ic(ICoord& ic1, ICoord& ic2);
   void release_ic(ICoord& ic1, ICoord& ic2);

  //prototyping functions
   int find_knn(int pt, int k, int* knn, double* knnd, int type);
   double predict_point(int pt, int k, ICoord& ic1, ICoord& ic2);

  //for unknown structures
   int find_knn_xyz(double* coords, int k, int* knn, double* knnd, ICoord& ic1, ICoord& ic2);

   int compare_add_files(int npts1, string* filesxyz1, string* filesgrad1, string* fileshess1, string* newfilesxyz, string* newfilesgrad, string* newfileshess);
   void reassign_mem(int npts1);

  public:

   int npts;
   int printl;

   int begin(int runnum, int natoms0);
   double test_points(int k0);
   double grad_knnr(double* coords, double &E1, double* grads, double* Ut, int k);
   int add_extra_points();
   void add_point(double energy, double* xyz, double* grad, double* hess);
   void freemem();


};

#endif
