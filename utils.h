/***************************************************************************
Contians small array and other math functions from baron's code
***************************************************************************/

#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cstdlib>
#include <ctime>
#include <sys/time.h>

#include "constants.h"

using namespace std;

void trans(double* A, double* B, int m, int n);
int Diagonalize(double* A, double* eigen, int size);
int SVD(double* A, double* Ut, double* eigen, int size1, int size2);
int Invert(double* A, int m);

int mat_root(double* A, int size);
int mat_root_inv(double* A, int size);

int mat_times_mat(double* C, double* A, double* B, int size); //square matrices
int mat_times_mat_bt(double* C, double* A, double* B, int size); //square matrices
int mat_times_mat_at_bt(double* C, double* A, double* B, int size); //square matrices
int mat_times_mat(double* C, double* A, double* B, int N, int M, int K); //rectangular
int mat_times_mat_bt(double* C, double* A, double* B, int N, int M, int K); //rectangular

int sign(double x);
void cross(double* x, double* r1, double* r2);
int close_val(double x1, double x2, double diff);
int check_array(int size, double* A);

namespace Utils
{

  void normalize_string_step_direc(double** a, int nstring, int LEN);

  double step_direc_overlap(double** a, double** b, int nstring, int LEN);

  void display_matrix(double** a, int LEN);

  void eigen_decomp(double** mat, double** evecs, double* evals, int n);

  void matrix_times_matrix(double** A, double** B, double** C, int LEN);

  void display_structure_nonames(double* a, int natoms);
  void display_structure(double* a, int natoms, string* anames);

  void jacobi(float **a, int n, float d[], float **v, int *nrot);
  void eigsrt(float d[], float **v, int n);

  void S_straight_line_in_angs(double** string_angs, double* S, int nstring, int natoms);

  void subtract_arrays(double* a, double* b, double* diff, int LEN);

  void vector_outer_prod(double* vec1, double* vec2, int LEN, double** output);
  void copy_2D_array(double** pointer1, double** pointer2, int LEN1, int LEN2);

  void copy_structure(double* structure1, double* structure2, int natoms);
  void alloc_dpointer_1D(double* pointer_name, int LEN);
  void alloc_dpointer_2D(double** pointer_name, int LEN1, int LEN2);
  void alloc_dpointer_3D(double*** pointer_name, int LEN1, int LEN2, int LEN3);

  void get_rotation_matrix(double** rotMat, double* thetas);
  void Rotate_structure(double** RotMat, double* structure, int natoms);
  void Rotate_hessian(double** Rot_mat, double** hessian, int natoms);

  void Rot_around_vec(double* vec, double* structure, int natoms);

  void mwc_to_ang(double** angs, double** mwc, int nstring, int natoms, double* amasses);
  void mwc_to_ang(double* angs, double* mwc, int natoms, double* amasses);
  void ang_to_mwc(double** mwc, double** ang, int nstring, int natoms, double* amasses);
  void ang_to_mwc(double* mwc, double* ang, int natoms, double* amasses);

  void mwcgrad_to_anggrad(double** ang_grad, double** mwc_grad, int nstring, int natoms, double* amasses);
  void mwcgrad_to_anggrad(double* ang_grad, double* mwc_grad, int natoms, double* amasses);
  void anggrad_to_mwcgrad(double** mwc_grad, double** ang_grad, int nstring, int natoms, double* amasses);
  void anggrad_to_mwcgrad(double* mwc_grad, double* ang_grad, int natoms, double* amasses);
  void diagonalize3x3(double** hmwc, double** smwc,
		      double* w2, int ndiag);

  void projectfrommatrix3x3(double* vector, double** hmwc);
  double randomf(double a, double b);

  void Mat_times_vec(double** d2S_1, double* dS, double* prod, int LEN);
  void normalize(double* u, int LEN);
  void invertNxN(double** Mat, double** Inverse, int n);
  void ludcmp(double **a, int n, int *indx, double *d);
  void lubksb(double **a, int n, int *indx, double b[]);


  double det3x3(double A[1+3][1+3]);
  void adjoint3x3(double A[4][4], double Aadj[4][4]);
  double det2x2(double A[3][3]);
  void gramschmidt(int LEN, double* v_out, double* u_in, double* v_in);
  void splineTangents(int LEN, double* x, double* y, double* y2, double* y1);
  void getSpline(int LEN, double* x, double* y, double* y2);
  double evalSpline(int LEN, double x, double* xa, double* ya, double* y2a);
  void S_from_angs(double** angs, double* S, double* masses, int LEN, int natoms);
  void Rmat_from_lincart(double** r, double* xyz, int natoms);
  void normalize_S(double* normalized_s, double* S, int LEN);
  void angs_to_mwcs(double** temparray, int nn, int natoms, double* amasses);
  void anggrads_to_mwcgrads(double** temparray, int nn, int natoms, double* amasses);
  double dotProd(double* v, double* u, int LEN);
  double vecMag(double* u, int LEN);

  void generate_Project_RT_tan(double** Proj, double* structure, double* tangent);
  void generate_Project_RT(double** Proj, double* structure);


};

#endif




