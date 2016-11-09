#ifndef LST_H
#define LST_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "constants.h"
#include "utils.h"

using namespace std;

namespace LST
{
  void getTangents_withLST_from_nodes_only(double** angs, double** dangstromsds, int nstring, int num_interp, int natoms, double* masses, int max_iters, double lst_grad_tol);
  void LST_perpgrads_from_mwcoords(double** mw_coords, double** mw_grads, double** mw_perp_grads, int num_interp, int natoms, double* masses, int nstring, int max_iters, double lst_grad_tol);
  void get_single_tangent_from_fstring(double** interp_string, double* dangstromds, int node_picked, int num_interp, double* masses, int natoms);
  void getTangents_from_fstring(double** interp_string, double** dangstromsds, int* nodes_picked, int nstring, int num_interp, double* masses, int natoms);
  void LST_stringbuild(double** interp_string, double** ang_coords, int nstring, int* nodes_picked, int num_interp, double* masses, int natoms, int max_iters, double lst_grad_tol);
  void LST_pickout(double** fstring, double* s_new, int nnOld, int nnNew, int* nodes_picked, int num_interp, int natoms, double* masses, double** angs_pos);
  void simple_LST_pickout(double** fstring, int natoms, int num_interp, double s_return, double* return_struct, int* picked);
  void LSTinterpolate(double* xyz1, double* xyz2, double* xyzf, double f, int max_iter, int natoms, double grad_tol);
  void LSTinterpolate(double* xyz1, double* xyz2, double* xyzf, double f, int max_iter, int natoms, double grad_tol, double* initial_guess);  
};

#endif


