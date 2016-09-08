/***************************************************************************
Contians small array and other math functions from baron's code
***************************************************************************/

#ifndef ECKART_H
#define ECKART_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "constants.h"
#include "utils.h"

using namespace std;

namespace Eckart
{

  void centroid_to_origin(double* structure, int natoms);
  double d2grad(double* grad, double* initial, double* final, int natoms);
  void d2hessian(double** hess, double* initial, double* final, int natoms);
  void Eckart_align(double* xyzreact, double* xyzprod, double tol, double* total_thetas, int max_iter, double* masses, int natoms);
  void Eckart_align(double* xyzreact, double* xyzprod, double* masses, int natoms);
  void Eckart_align(double* xyzreact, double* xyzprod, double tol, double* total_thetas, int max_iter, double* masses, int natoms, double rfrac);
  void Eckart_align(double* xyzreact, double* xyzprod, double* masses, int natoms, double rfrac);

  void Eckart_align_string(double** angs, int nstring, double* masses, int natoms);

  //  void Eckart_align_string_and_gradients(double** angs, double** ang_gradients, double** ang_gradients_perp, double** dangsds, int nstring, double* masses, int natoms);

  void Eckart_align_with_grads(double* anchor_struct, double* structure, double* grad, double** rot_mat, double* masses, int natoms);

  void Eckart_align_string_and_gradients(double** angs, double** ang_gradients, int nstring, double* masses, int natoms);


};

#endif


 
 
