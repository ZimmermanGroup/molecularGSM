#include "eckart.h"

using namespace std;

void Eckart::Eckart_align_string(double** angs, int nstring, double* masses, int natoms){

  double** total_thetas = new double*[1+nstring];
  for (int i=0;i<nstring;i++){
    total_thetas[i] = new double[1+3];
  }

  cout << "\tAligning the current string of " << nstring << " nodes to the Eckart frame" << endl;
  for (int i=0;i<nstring-1;i++){
    Eckart::Eckart_align(angs[i], angs[i+1], 0.001, total_thetas[i], 200, masses, natoms);
  }
  
  for (int i=0;i<nstring;i++){
    delete [] total_thetas[i];
  }
  delete [] total_thetas;


}


void Eckart::Eckart_align_with_grads(double* anchor_struct, double* structure, double* grad, double** rot_mat, double* masses, int natoms){

  double* thetas = new double[1+3];
  double** inverse = new double*[1+3];
  for (int i=0;i<3;i++){
    inverse[i] = new double[1+3];
  }

  Eckart::Eckart_align(anchor_struct, structure, 0.001, thetas, 200, masses, natoms);
  Utils::get_rotation_matrix(rot_mat, thetas);
  Utils::invertNxN(rot_mat, inverse, 3);
  Utils::Rotate_structure(inverse, grad, natoms);

  delete [] thetas;
  for (int i=0;i<3;i++){
    delete [] inverse[i];
  }
  delete [] inverse;

}

void Eckart::Eckart_align_string_and_gradients(double** angs, double** ang_gradients, int nstring, double* masses, int natoms){
  cout << "Aligning the current string of " << nstring << " nodes and gradients to the Eckart frame" << endl;
   
  double** total_thetas = new double*[1+nstring];
  for (int i=0;i<nstring;i++){
    total_thetas[i] = new double[1+3];
  }

  double*** rotation_matrices = new double**[1+nstring];
  double*** inverses = new double**[1+nstring];
  for (int i=0;i<nstring;i++){
    rotation_matrices[i] = new double*[1+3];
    inverses[i] = new double*[1+3];
    for (int j=0;j<3;j++){
      rotation_matrices[i][j] = new double[1+3];
      inverses[i][j] = new double[1+3];
    }
  }

  for (int i=1;i<nstring;i++){
    Eckart::Eckart_align(angs[i-1], angs[i], 0.001, total_thetas[i], 200, masses, natoms);
    Utils::get_rotation_matrix(rotation_matrices[i], total_thetas[i]);
    Utils::invertNxN(rotation_matrices[i], inverses[i], 3);
    Utils::Rotate_structure(inverses[i], ang_gradients[i], natoms);
  }

  for (int i=0;i<nstring;i++){
    for (int j=0;j<3;j++){
      delete [] rotation_matrices[i][j];
      delete [] inverses[i][j];
    }
    delete [] rotation_matrices[i];
    delete [] inverses[i];
  }
  delete [] rotation_matrices;
  delete [] inverses;

  for (int i=0;i<nstring;i++){
    delete [] total_thetas[i];
  }
  delete [] total_thetas;

}

void Eckart::centroid_to_origin(double* structure, int natoms){

  double x = 0;
  double y = 0;
  double z = 0;

  for (int i=0;i<natoms;i++){
    x += structure[3*i+0];
    y += structure[3*i+1];
    z += structure[3*i+2];
  }

  x *= 1/natoms;
  y *= 1/natoms;
  z *= 1/natoms;

  for (int i=0;i<natoms;i++){
    structure[3*i+1] -= x;
    structure[3*i+2] -= y;
    structure[3*i+3] -= z;
  }

}

void Eckart::Eckart_align(double* xyzreact, double* xyzprod, double* masses, int natoms, double rfrac){

  double tol = 0.001;
  double* total_thetas = new double[1+3];
  int max_iter = 200;

  Eckart_align(xyzreact, xyzprod, tol, total_thetas, max_iter, masses, natoms, rfrac);

  delete [] total_thetas;

}


void Eckart::Eckart_align(double* xyzreact, double* xyzprod, double tol, double* total_thetas, int max_iter, double* masses, int natoms, double rfrac){

#if 0  
  cout << "natoms = " << natoms << endl;
  cout << "xyzreact is: " << endl;
  Utils::display_structure_nonames(xyzreact, natoms);
  fflush(stdout);
  cout << "xyzprod is: " << endl;
  Utils::display_structure_nonames(xyzprod, natoms);
  fflush(stdout);
  cout << "tol = " << tol << endl;
  cout << "max_ter = " << max_iter << endl;
  cout << "masses are: " << endl;
  for (int i=0;i<natoms;i++){
    cout << masses[i] << "  ";
    fflush(stdout);
  }
  cout << endl;
#endif

  double* grad = new double[1+3];
  double* hess_evals = new double[1+3];
  double** hess = new double*[1+3];
  double** hess_evecs = new double*[1+3];
  double** hess_inverse = new double*[1+3];
  double** rot_mat = new double*[1+3];
  for (int i=0;i<3;i++)
  {
    hess[i] = new double[1+3];
    hess_evecs[i] = new double[1+3];
    hess_inverse[i] = new double[1+3];
    rot_mat[i] = new double[1+3];
  }
  for (int i=0;i<3;i++)
  for (int j=0;j<4;j++)
  {
    hess[i][j] = 0.;
    hess_evecs[i][j] = 0.;
    hess_inverse[i][j] = 0.;
    rot_mat[i][j] = 0.;
  }

  for (int i=0;i<3;i++){
    total_thetas[i]=0;
  }

  double total_mass = 0.0;
  for (int i=0;i<natoms;i++){
    total_mass+=masses[i];
  }

  double* COMreact = new double[1+3];
  double* COMprod = new double[1+3];
  for (int i=0;i<3;i++){
    COMreact[i]=0.0;;
    COMprod[i]=0.0;
  }

  for (int i=0;i<natoms;i++){
    for (int j=0;j<3;j++){
      COMreact[j]+=xyzreact[3*i+j]*masses[i];
      COMprod[j]+=xyzprod[3*i+j]*masses[i];
    }
  }

  for (int i=0;i<3;i++){
    COMreact[i]/=total_mass;
    COMprod[i]/=total_mass;
  }

  for (int i=0;i<natoms;i++){
    for (int j=0;j<3;j++){
      xyzreact[3*i+j]-=COMreact[j];
      xyzprod[3*i+j]-=COMprod[j];
    }
  }

  double* mwcreact = new double[1+natoms*3];
  double* mwcprod = new double[1+natoms*3];

  Utils::ang_to_mwc(mwcreact, xyzreact, natoms, masses);
  Utils::ang_to_mwc(mwcprod, xyzprod, natoms, masses);

  double* thetas = new double[1+3];
  for (int i=0;i<4;i++) thetas[i] = 0.;

  double gradmag;

  if (rfrac < 1.0) max_iter = 1;
  for (int i=0;i<max_iter;i++){ 
    //    cout << "starting iter: " << i+1 << endl;
    fflush(stdout);    
    gradmag = d2grad(grad, mwcreact, mwcprod, natoms);
    d2hessian(hess, mwcreact, mwcprod, natoms);

    //    cout << "the gradient of the Eckart distance is: " << gradmag << endl;

    Utils::diagonalize3x3(hess, hess_evecs, hess_evals, 250);

    double mag_thetas = Utils::vecMag(thetas, 3);

    if ((gradmag<tol)&&(hess_evals[0]>=0)&&(hess_evals[1]>=0)&&(hess_evals[2]>=0)){
      break;
    }

  //  if (i==max_iter && gradmag > tol){
  //    cout <<"ECKART ALIGNMENT FAILURE!!!" << endl;
  //  }

    double temp =0;
    int vec_index =0;
    for (int j=0;j<3;j++){
      if (hess_evals[j]<temp && fabs(hess_evals[j])>0.01){
	temp = hess_evals[j];
	vec_index = j;
      }
    }

    if (vec_index != 0){
      Utils::Rot_around_vec(hess_evecs[vec_index], mwcprod, natoms);
    }

    Utils::diagonalize3x3(hess, hess_evecs, hess_evals, 250);

    for (int j=0;j<3;j++){
      if (hess_evals[j]>0.01 || hess_evals[j]<-0.01){
	hess_evals[j] = 1/hess_evals[j];
      }
    }

    for (int j=0;j<3;j++){
      for (int k=0;k<3;k++){
	hess_inverse[j][k]=0.0;
	for (int m=0;m<3;m++){
	  hess_inverse[j][k] +=hess_evecs[m][j]*hess_evals[m]*hess_evecs[m][k];
	}
      }
    }

    Utils::Mat_times_vec(hess_inverse, grad, thetas, 3);
    
    for (int j=0;j<3;j++){
      thetas[j]*=-rfrac; 
    }
    for (int j=0;j<3;j++){
      total_thetas[j]+=thetas[j];
    }

    Utils::get_rotation_matrix(rot_mat, thetas);
    Utils::Rotate_structure(rot_mat, mwcprod, natoms);

  }

	
  Utils::mwc_to_ang(xyzreact, mwcreact, natoms, masses);
  Utils::mwc_to_ang(xyzprod, mwcprod, natoms, masses);

//CPMZ returning to previous COM
  if (rfrac < 1.0)
  for (int i=0;i<natoms;i++){
    for (int j=0;j<3;j++){
      xyzreact[3*i+j]+=COMreact[j];
      xyzprod[3*i+j]+=COMprod[j];
    }   
  }

#if 0
  cout << " after align: " << endl;
  cout << " " << natoms << endl;
  cout << "xyzreact is: " << endl;
  Utils::display_structure_nonames(xyzreact, natoms);
  fflush(stdout);
  cout << " " << natoms << endl;
  cout << "xyzprod is: " << endl;
  Utils::display_structure_nonames(xyzprod, natoms);
  fflush(stdout);
#endif

  for (int i=0;i<3;i++)
  {
    delete [] hess[i];
    delete [] hess_evecs[i];
    delete [] hess_inverse[i];
    delete [] rot_mat[i];
  }
  delete [] hess;
  delete [] hess_evecs;
  delete [] hess_inverse;
  delete [] rot_mat;

  delete [] grad;
  delete [] hess_evals;
  delete [] COMreact;
  delete [] COMprod;
  delete [] mwcreact;
  delete [] mwcprod;
  delete [] thetas;
}



void Eckart::Eckart_align(double* xyzreact, double* xyzprod, double* masses, int natoms)
{

  double tol = 0.001;
  double* total_thetas = new double[1+3];
  int max_iter = 200;

  Eckart_align(xyzreact, xyzprod, tol, total_thetas, max_iter, masses, natoms);

  delete [] total_thetas;

}


void Eckart::Eckart_align(double* xyzreact, double* xyzprod, double tol, double* total_thetas, int max_iter, double* masses, int natoms){

#if 0  
  cout << "natoms = " << natoms << endl;
  cout << "xyzreact is: " << endl;
  Utils::display_structure_nonames(xyzreact, natoms);
  fflush(stdout);
  cout << "xyzprod is: " << endl;
  Utils::display_structure_nonames(xyzprod, natoms);
  fflush(stdout);
  cout << "tol = " << tol << endl;
  cout << "max_ter = " << max_iter << endl;
  cout << "masses are: " << endl;
  for (int i=0;i<natoms;i++){
    cout << masses[i] << "  ";
    fflush(stdout);
  }
  cout << endl;
#endif

  double* grad = new double[1+3];
  double* hess_evals = new double[1+3];
  double** hess = new double*[1+3];
  double** hess_evecs = new double*[1+3];
  double** hess_inverse = new double*[1+3];
  double** rot_mat = new double*[1+3];
  for (int i=0;i<3;i++){
    hess[i] = new double[1+3];
    hess_evecs[i] = new double[1+3];
    hess_inverse[i] = new double[1+3];
    rot_mat[i] = new double[1+3];
  }
  for (int i=0;i<3;i++)
  for (int j=0;j<4;j++)
  {
    hess[i][j] = 0.;
    hess_evecs[i][j] = 0.;
    hess_inverse[i][j] = 0.;
    rot_mat[i][j] = 0.;
  }

  for (int i=0;i<3;i++){
    total_thetas[i]=0;
  }

  double total_mass = 0.0;
  for (int i=0;i<natoms;i++){
    total_mass+=masses[i];
  }

  double* COMreact = new double[1+3];
  double* COMprod = new double[1+3];
  for (int i=0;i<3;i++){
    COMreact[i]=0.0;;
    COMprod[i]=0.0;
  }

  for (int i=0;i<natoms;i++){
    for (int j=0;j<3;j++){
      COMreact[j]+=xyzreact[3*i+j]*masses[i];
      COMprod[j]+=xyzprod[3*i+j]*masses[i];
    }
  }

  for (int i=0;i<3;i++){
    COMreact[i]/=total_mass;
    COMprod[i]/=total_mass;
  }

  for (int i=0;i<natoms;i++){
    for (int j=0;j<3;j++){
      xyzreact[3*i+j]-=COMreact[j];
      xyzprod[3*i+j]-=COMprod[j];
    }
  }

  double* mwcreact = new double[1+natoms*3];
  double* mwcprod = new double[1+natoms*3];

  Utils::ang_to_mwc(mwcreact, xyzreact, natoms, masses);
  Utils::ang_to_mwc(mwcprod, xyzprod, natoms, masses);

  double* thetas = new double[1+3];

  double gradmag;

  for (int i=0;i<max_iter;i++){
    //cout << "starting iter: " << i+1 << endl;
    //fflush(stdout);    
    gradmag = d2grad(grad, mwcreact, mwcprod, natoms);
    d2hessian(hess, mwcreact, mwcprod, natoms);

    //    cout << "the gradient of the Eckart distance is: " << gradmag << endl;

    Utils::diagonalize3x3(hess, hess_evecs, hess_evals, 250);
    //printf(" hess_evals: %7.6f %7.6f %7.6f \n",hess_evals[0],hess_evals[1],hess_evals[2]);

    double mag_thetas = Utils::vecMag(thetas, 3);

    if ((gradmag<tol)&&(hess_evals[0]>=0)&&(hess_evals[1]>=0)&&(hess_evals[2]>=0)){
      break;
    }

    if (i==max_iter && gradmag > tol){
      cout <<"ECKART ALIGNMENT FAILURE!!!" << endl;
    }

    double temp =0;
    int vec_index=0;
    for (int j=0;j<3;j++){
      if (hess_evals[j]<temp && fabs(hess_evals[j])>0.01){
	temp = hess_evals[j];
	vec_index = j;
      }
    }

    if (vec_index != 0){
      Utils::Rot_around_vec(hess_evecs[vec_index], mwcprod, natoms);
    }

    Utils::diagonalize3x3(hess, hess_evecs, hess_evals, 250);

    for (int j=0;j<3;j++){
      if (hess_evals[j]>0.01 || hess_evals[j]<-0.01){
	hess_evals[j] = 1/hess_evals[j];
      }
    }

    for (int j=0;j<3;j++){
      for (int k=0;k<3;k++){
	hess_inverse[j][k]=0.0;
	for (int m=0;m<3;m++){
	  hess_inverse[j][k] +=hess_evecs[m][j]*hess_evals[m]*hess_evecs[m][k];
	}
      }
    }

    Utils::Mat_times_vec(hess_inverse, grad, thetas, 3);
    
    for (int j=0;j<3;j++){
      thetas[j]*=-1;
    }
    for (int j=0;j<3;j++){
      total_thetas[j]+=thetas[j];
    }

    Utils::get_rotation_matrix(rot_mat, thetas);
    Utils::Rotate_structure(rot_mat, mwcprod, natoms);

  }

	
  Utils::mwc_to_ang(xyzreact, mwcreact, natoms, masses);
  Utils::mwc_to_ang(xyzprod, mwcprod, natoms, masses);

#if 0
  cout << " after align: " << endl;
  cout << " " << natoms << endl;
  cout << "xyzreact is: " << endl;
  Utils::display_structure_nonames(xyzreact, natoms);
  fflush(stdout);
  cout << " " << natoms << endl;
  cout << "xyzprod is: " << endl;
  Utils::display_structure_nonames(xyzprod, natoms);
  fflush(stdout);
#endif

  for (int i=0;i<3;i++){
    delete [] hess[i];
    delete [] hess_evecs[i];
    delete [] hess_inverse[i];
    delete [] rot_mat[i];
  }
  delete [] hess;
  delete [] hess_evecs;
  delete [] hess_inverse;
  delete [] rot_mat;

  delete [] grad;
  delete [] hess_evals;
  delete [] COMreact;
  delete [] COMprod;
  delete [] mwcreact;
  delete [] mwcprod;
  delete [] thetas;
}



double Eckart::d2grad(double* grad, double* initial, double* final, int natoms){
  //  cout << "starting d2grad" << endl;
  fflush(stdout);
  grad[0]=0;
  grad[1]=0;
  grad[2]=0;
  for (int i=0;i<natoms;i++){
    //    cout << "starting cycle: " << i << endl;
    fflush(stdout);
    grad[0]+=2*(initial[3*i+1]*final[3*i+2]-initial[3*i+2]*final[3*i+1]);
    grad[1]+=2*(initial[3*i+2]*final[3*i+0]-initial[3*i+0]*final[3*i+2]);
    grad[2]+=2*(initial[3*i+0]*final[3*i+1]-initial[3*i+1]*final[3*i+0]);
    //cout << "ending cycle: " << i << endl;
    fflush(stdout);
  }

  return Utils::vecMag(grad, 3);

}

void Eckart::d2hessian(double** hess, double* initial, double* final, int natoms){
  //cout << "starting d2hessian" << endl;
  fflush(stdout);
  
  /*
  cout << "initial is: " << endl;
  Utils::display_structure_nonames(initial, natoms);
  fflush(stdout);
  cout << "final is: "<< endl;
  Utils::display_structure_nonames(final, natoms);
  fflush(stdout);
  cout << "natoms = " << natoms << endl;
  fflush(stdout);
  cout << "hess = " << endl;
  Utils::display_matrix(hess, 3);
  fflush(stdout);
  */

  for (int i=0;i<3;i++){
    for (int j=0;j<3;j++){
      hess[i][j]=0.0;
    }
  }

  //  cout << "OK A" << endl;
  fflush(stdout);

  double dot=0;
  for (int i=0;i<natoms;i++){
    for (int j=0;j<3;j++){
      dot+= initial[3*i+j]*final[3*i+j];
    }
  }

  //  cout << "OK B" << endl;
  fflush(stdout);

  double** xd = new double*[1+3];
  for (int i=0;i<3;i++){
    xd[i] = new double[1+3];
    for (int j=0;j<3;j++){
      xd[i][j]=0.0;
    }
  }

  //  cout << "OK C" << endl;
  fflush(stdout);

  for (int i=0;i<natoms;i++){
    xd[0][0] += initial[3*i+0]*final[3*i+0];
    xd[1][1] += initial[3*i+1]*final[3*i+1];
    xd[2][2] += initial[3*i+2]*final[3*i+2];
    xd[1][0] += initial[3*i+1]*final[3*i+0];
    xd[2][0] += initial[3*i+2]*final[3*i+0];
    xd[2][1] += initial[3*i+2]*final[3*i+1];
  }


  //  cout << "OK D" << endl;
  fflush(stdout);

  xd[0][1] = xd[1][0];
  xd[0][2] = xd[2][0];
  xd[1][2] = xd[2][1];

  //  cout << "OK E" << endl;
  fflush(stdout);

  hess[0][0] = 2*(dot-xd[0][0]);
  hess[1][1] = 2*(dot-xd[1][1]);
  hess[2][2] = 2*(dot-xd[2][2]);
  hess[1][0] = -2*xd[1][0];
  hess[2][0] = -2*xd[2][0];
  hess[2][1] = -2*xd[2][1];
  hess[0][1] = hess[1][0];
  hess[0][2] = hess[2][0];
  hess[1][2] = hess[2][1];

  //  cout << "OK F" << endl;
  fflush(stdout);

  
  for (int i=0;i<3;i++){
    delete [] xd[i];
  }
  delete [] xd;
  

  // cout << "OK G" << endl;

}
