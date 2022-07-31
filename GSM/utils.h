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
#include <mkl_types.h>
#include <cstdlib>
#include <ctime>
#include <sys/time.h>

#include "constants.h"

using namespace std;

void trans(double* A, double* B, MKL_INT m, MKL_INT n);
MKL_INT Diagonalize(double* A, double* eigen, MKL_INT size);
MKL_INT SVD(double* A, double* Ut, double* eigen, MKL_INT size1, MKL_INT size2);
MKL_INT Invert(double* A, MKL_INT m);

MKL_INT mat_root(double* A, MKL_INT size);
MKL_INT mat_root_inv(double* A, MKL_INT size);

MKL_INT mat_times_mat(double* C, double* A, double* B, MKL_INT size); //square matrices
MKL_INT mat_times_mat_bt(double* C, double* A, double* B, MKL_INT size); //square matrices
MKL_INT mat_times_mat_at_bt(double* C, double* A, double* B, MKL_INT size); //square matrices
MKL_INT mat_times_mat(double* C, double* A, double* B, MKL_INT N, MKL_INT M, MKL_INT K); //rectangular
MKL_INT mat_times_mat_bt(double* C, double* A, double* B, MKL_INT N, MKL_INT M, MKL_INT K); //rectangular

MKL_INT sign(double x);
void cross(double* x, double* r1, double* r2);
MKL_INT close_val(double x1, double x2, double diff);
MKL_INT check_array(MKL_INT size, double* A);

namespace Utils
{

  void normalize_string_step_direc(double** a, MKL_INT nstring, MKL_INT LEN);

  double step_direc_overlap(double** a, double** b, MKL_INT nstring, MKL_INT LEN);

  void display_matrix(double** a, MKL_INT LEN);

  void eigen_decomp(double** mat, double** evecs, double* evals, MKL_INT n);

  void matrix_times_matrix(double** A, double** B, double** C, MKL_INT LEN);

  void display_structure_nonames(double* a, MKL_INT natoms);
  void display_structure(double* a, MKL_INT natoms, string* anames);

  void jacobi(float **a, MKL_INT n, float d[], float **v, MKL_INT *nrot);
  void eigsrt(float d[], float **v, MKL_INT n);

  void S_straight_line_in_angs(double** string_angs, double* S, MKL_INT nstring, MKL_INT natoms);

  void subtract_arrays(double* a, double* b, double* diff, MKL_INT LEN);

  void vector_outer_prod(double* vec1, double* vec2, MKL_INT LEN, double** output);
  void copy_2D_array(double** poMKL_INTer1, double** poMKL_INTer2, MKL_INT LEN1, MKL_INT LEN2);

  void copy_structure(double* structure1, double* structure2, MKL_INT natoms);
  void alloc_dpoMKL_INTer_1D(double* poMKL_INTer_name, MKL_INT LEN);
  void alloc_dpoMKL_INTer_2D(double** poMKL_INTer_name, MKL_INT LEN1, MKL_INT LEN2);
  void alloc_dpoMKL_INTer_3D(double*** poMKL_INTer_name, MKL_INT LEN1, MKL_INT LEN2, MKL_INT LEN3);

  void get_rotation_matrix(double** rotMat, double* thetas);
  void Rotate_structure(double** RotMat, double* structure, MKL_INT natoms);
  void Rotate_hessian(double** Rot_mat, double** hessian, MKL_INT natoms);

  void Rot_around_vec(double* vec, double* structure, MKL_INT natoms);

  void mwc_to_ang(double** angs, double** mwc, MKL_INT nstring, MKL_INT natoms, double* amasses);
  void mwc_to_ang(double* angs, double* mwc, MKL_INT natoms, double* amasses);
  void ang_to_mwc(double** mwc, double** ang, MKL_INT nstring, MKL_INT natoms, double* amasses);
  void ang_to_mwc(double* mwc, double* ang, MKL_INT natoms, double* amasses);

  void mwcgrad_to_anggrad(double** ang_grad, double** mwc_grad, MKL_INT nstring, MKL_INT natoms, double* amasses);
  void mwcgrad_to_anggrad(double* ang_grad, double* mwc_grad, MKL_INT natoms, double* amasses);
  void anggrad_to_mwcgrad(double** mwc_grad, double** ang_grad, MKL_INT nstring, MKL_INT natoms, double* amasses);
  void anggrad_to_mwcgrad(double* mwc_grad, double* ang_grad, MKL_INT natoms, double* amasses);
  void diagonalize3x3(double** hmwc, double** smwc,
		      double* w2, MKL_INT ndiag);

  void projectfrommatrix3x3(double* vector, double** hmwc);
  double randomf(double a, double b);

  void Mat_times_vec(double** d2S_1, double* dS, double* prod, MKL_INT LEN);
  void normalize(double* u, MKL_INT LEN);
  void invertNxN(double** Mat, double** Inverse, MKL_INT n);
  void ludcmp(double **a, MKL_INT n, MKL_INT *indx, double *d);
  void lubksb(double **a, MKL_INT n, MKL_INT *indx, double b[]);


  double det3x3(double A[1+3][1+3]);
  void adjoMKL_INT3x3(double A[4][4], double Aadj[4][4]);
  double det2x2(double A[3][3]);
  void gramschmidt(MKL_INT LEN, double* v_out, double* u_in, double* v_in);
  void splineTangents(MKL_INT LEN, double* x, double* y, double* y2, double* y1);
  void getSpline(MKL_INT LEN, double* x, double* y, double* y2);
  double evalSpline(MKL_INT LEN, double x, double* xa, double* ya, double* y2a);
  void S_from_angs(double** angs, double* S, double* masses, MKL_INT LEN, MKL_INT natoms);
  void Rmat_from_lincart(double** r, double* xyz, MKL_INT natoms);
  void normalize_S(double* normalized_s, double* S, MKL_INT LEN);
  void angs_to_mwcs(double** temparray, MKL_INT nn, MKL_INT natoms, double* amasses);
  void anggrads_to_mwcgrads(double** temparray, MKL_INT nn, MKL_INT natoms, double* amasses);
  double dotProd(double* v, double* u, MKL_INT LEN);
  double vecMag(double* u, MKL_INT LEN);

  void generate_Project_RT_tan(double** Proj, double* structure, double* tangent);
  void generate_Project_RT(double** Proj, double* structure);


};

#endif




