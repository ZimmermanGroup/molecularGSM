#include "LST.h"

using namespace std;

void LST::getTangents_withLST_from_nodes_only(double** angs, double** dangstromsds, int nstring, int num_interp, int natoms, double* masses, int max_iters, double lst_grad_tol){

  double** interp_string = new double*[1+num_interp];
  for (int i=1;i<=num_interp;i++){
    interp_string[i] = new double[1+natoms*3];
  }

  double* SS = new double[1+num_interp];
  Utils::S_from_angs(angs, SS, masses, nstring, natoms);

  int* nodes_picked = new int[1+nstring];

  LST_stringbuild(interp_string, angs, nstring, nodes_picked, num_interp, masses, natoms, max_iters, lst_grad_tol);

  double temp1, temp2;

  double* y = new double[1+num_interp];
  double* y1 = new double[1+num_interp];
  double* y2 = new double[1+num_interp];

  Utils::S_from_angs(interp_string, SS, masses, num_interp, natoms);

  for(int k=1;k<=natoms*3;k++){
    for(int j=1;j<=num_interp;j++){
      y[j] = interp_string[j][k];
    }
    Utils::getSpline(num_interp, SS, y, y2);
    Utils::splineTangents(num_interp, SS, y, y2, y1);
    for(int j=1;j<=nstring;j++){
      dangstromsds[j][k] = y1[nodes_picked[j]];
    }
  }

  for(int i=1;i<=nstring;i++){
    temp1 = Utils::vecMag(dangstromsds[i],natoms*3);
    for(int j=1;j<=natoms*3;j++){
      dangstromsds[i][j] = dangstromsds[i][j]/temp1;
    }
  }

  cout << "The tangents calculated are: " << endl;
  for (int i=1;i<=nstring;i++){
    cout << "image " << i << endl;
    Utils::display_structure_nonames(dangstromsds[i], natoms);
  }


  for (int i=1;i<=num_interp;i++){
    delete [] interp_string[i];
  }
  delete [] interp_string;

  delete [] nodes_picked;
  delete [] y;
  delete [] y1;
  delete [] y2;
  delete [] SS;

}

void LST::get_single_tangent_from_fstring(double** interp_string, double* dangstromds, int node_picked, int num_interp, double* masses, int natoms){

  double* y = new double[1+num_interp];
  double* y1 = new double[1+num_interp];
  double* y2 = new double[1+num_interp];
  double* SS = new double[1+num_interp];  

  Utils::S_from_angs(interp_string, SS, masses, num_interp, natoms);

  double temp = 0;
  for(int k=1;k<=natoms*3;k++){
    for(int j=1;j<=num_interp;j++){
      y[j] = interp_string[j][k];
    }
    Utils::getSpline(num_interp, SS, y, y2);
    Utils::splineTangents(num_interp, SS, y, y2, y1);
    dangstromds[k] = y1[node_picked];
  }
  
  double temp1 = Utils::vecMag(dangstromds ,natoms*3);
  for(int j=1;j<=natoms*3;j++){
    dangstromds[j] *= 1/temp1;
  }
    
  delete [] y;
  delete [] y1;
  delete [] y2;
  delete [] SS;

}

void LST::getTangents_from_fstring(double** interp_string, double** dangstromsds, int* nodes_picked, int nstring, int num_interp, double* masses, int natoms){

  double temp1, temp2;

  double* y = new double[1+num_interp];
  double* y1 = new double[1+num_interp];
  double* y2 = new double[1+num_interp];
  double* SS = new double[1+num_interp];

  Utils::S_from_angs(interp_string, SS, masses, num_interp, natoms);

  for(int k=1;k<=natoms*3;k++){
    for(int j=1;j<=num_interp;j++){
      y[j] = interp_string[j][k];
    }
    Utils::getSpline(num_interp, SS, y, y2);
    Utils::splineTangents(num_interp, SS, y, y2, y1);
    for(int j=1;j<=nstring;j++){
      dangstromsds[j][k] = y1[nodes_picked[j]];
    }
  }

  for(int i=1;i<=nstring;i++){
    temp1 = Utils::vecMag(dangstromsds[i],natoms*3);
    for(int j=1;j<=natoms*3;j++){
      dangstromsds[i][j] = dangstromsds[i][j]/temp1;
    }
  }

  delete [] y;
  delete [] y1;
  delete [] y2;
  delete [] SS;

}

void LST::LST_stringbuild(double** interp_string, double** ang_coords, int nstring, int* nodes_picked, int num_interp, double* masses, int natoms, int max_iters, double lst_grad_tol){
  
  //initialize the fstring
  for (int i=1;i<=num_interp;i++){
    for (int j=1;j<=natoms*3;j++){
      interp_string[i][j]=0.0;
    }
  }
  
  //The f array will hold the f values for each piece of the fstring
  double* f= new double[1+num_interp];
  f[1]=0;

  double* SS = new double[1+nstring];

  Utils::S_from_angs(ang_coords, SS, masses, nstring, natoms);

  //Normalize SS into norm_s
  double* norm_s = new double[1+nstring];
  
  for (int i=1;i<=nstring;i++){
    norm_s[i] = SS[i]/SS[nstring];
  }

  //Utils::normalize_S(norm_s, SS, nstring);

  //Figure out what values of the interpolation factor f we should use for each piece of the fstring
  //First - determine the number of interpolations to do between each pair of nodes based on node spacing
  double* f_distribute = new double[1+nstring-1];

  for (int i=1;i<=nstring-1;i++){
    f_distribute[i]=(num_interp-nstring)*(norm_s[i+1]-norm_s[i]);
    if (f_distribute[i]>=ceil(f_distribute[i]-0.5000000000000)){
      f_distribute[i]=ceil(f_distribute[i]);
    }
    else {
      f_distribute[i]=floor(f_distribute[i]);
    }
  }

  fflush(stdout);


  //OK - this really requires some comments.  Due to the above scheme to distribute the number of nodes
  //in a given interval, there is a chance that we can end up with something greater than num_interp 
  //as the sum of all elements in f_distribute.  If we do, the code below will find the interval with the
  //most interpolations and take out one.  This will iterate until the sum of f_distribute equals num_interp.

  //find some of f_distribute pieces.
  int sum=nstring;
  for (int i=1;i<=nstring-1;i++){
    sum+=(int)f_distribute[i];
  }

  int interval_with_max=1;
  if (sum>num_interp){
    for (int i=1;i<=sum-num_interp;i++){
      for (int j=1;j<=nstring-1;j++){
	if (f_distribute[j] > f_distribute[interval_with_max]){
	  interval_with_max=j;
	}
      }
      f_distribute[interval_with_max]=f_distribute[interval_with_max]-1;
    }
  }


  int interval_with_min=1;
  if (sum<num_interp){
    for (int i=1;i<=num_interp-sum;i++){
      for (int j=1;j<=nstring-1;j++){
        if (f_distribute[j] < f_distribute[interval_with_max]){
          interval_with_min=j;
        }
      }
      f_distribute[interval_with_min]=f_distribute[interval_with_min]+1;
    }
  }

  sum=nstring;
  for (int i=1;i<=nstring-1;i++){
    sum+=(int)f_distribute[i];
  }

  
  //first, let's place the node f values (most zeroes, and one at the end) in the fstring.
  int place=1;
  for (int i=1;i<=nstring-1;i++){
    f[place]=0;
    nodes_picked[i]=place;
    place+=(int)f_distribute[i]+1;
  }
  f[num_interp]=1;
  nodes_picked[nstring]=num_interp;

  fflush(stdout);

  //next, go through each loop and add in the other f-values
  int accum=1;
  int j=1;
  for (int i=1;i<=nstring-1;i++){
    for (int j=1;j<=f_distribute[i];j++){
      f[accum+j]=j/(f_distribute[i]+1);
    }
    accum+=(int)f_distribute[i]+1;
  }

  //If the f array value equals 0 or 1 at a given point, this corresponds to 
  //a node in the fstring and the coordinates should be put in place.
  int node=1;
  for (int i=1;i<=num_interp;i++){
    if (f[i]==1||f[i]==0){
      for (int m=1;m<=natoms*3;m++){
	interp_string[i][m]=ang_coords[node][m];
      }
      node++;
    }
  }

  //  cout << endl;

  //Keeping track of the current interval (to determine which nodes to interpolate between),
  //interpolate each member of the f-string that is not an input node.
  int interval=1;
  cout << "starting interpolations ";
  for (int i=1;i<=num_interp;i++){
    if (f[i]==0 || f[i]==1){
      if (i!=1){
	interval++;
      }
    }    
    else{
      cout << i << " ";
      LSTinterpolate(ang_coords[interval], ang_coords[interval+1], interp_string[i], f[i], max_iters, natoms, lst_grad_tol);
    }
  }
  cout << endl;

  delete[] f;
  delete[] norm_s;
  delete[] f_distribute;
  delete [] SS;

}


void LST::simple_LST_pickout(double** fstring, int natoms, int num_interp, double S_return, double* return_struct, int* picked){

  double* fstr_S = new double[1+num_interp];
  fstr_S[1] = 0;
  double* diff = new double[1+natoms*3];
  for (int i=2;i<=num_interp;i++){
    for (int j=1;j<=natoms*3;j++){
      diff[j] = fstring[i][j] - fstring[i-1][j];
    }
    fstr_S[i] = fstr_S[i-1] + Utils::vecMag(diff, natoms*3);
  }

  for (int i=2;i<=num_interp;i++){
    if (fstr_S[i-1] < S_return && fstr_S[i] > S_return){
      if (fabs(fstr_S[i-1]-S_return) < fabs(fstr_S[i]-S_return)){
	*picked = i-1;
      }
      else{
	*picked = i;
      }
      break;
    }
  }

  for (int i=1;i<=natoms*3;i++){
    return_struct[i] = fstring[*picked][i];
  }

  delete [] fstr_S;
  delete [] diff;


}

//pickes new nodes based on nnNew, S_new and the fstring, and then places them angspos
void LST::LST_pickout(double** fstring, double* s_new, int nnOld, int nnNew, int* nodes_picked, int num_interp, int natoms, double* masses, double** angs_pos) {
   
  double** angspos_old = new double*[1+nnOld];
  for (int i=1;i<=nnOld;i++){
    angspos_old[i] = new double[1+natoms*3];
  }

  for (int i=1;i<=nnOld;i++){
    for (int j=1;j<=natoms*3;j++){
      angspos_old[i][j] = angs_pos[i][j];
    }
  }

  double* fstr_S = new double[1+num_interp];

  Utils::S_from_angs(fstring, fstr_S, masses, num_interp, natoms);

  //normalize fstr_S
  for (int i=1;i<=num_interp;i++){
    fstr_S[i]*=(1/fstr_S[num_interp]);
  }

  //Pick out new nodes from the fstring by comparing actual and desired ss_new values.
  for (int i=1;i<=nnNew;i++){
    //    cout <<"Now searching where to place new node: " << i << endl << endl;
    for (int j=1;j<=num_interp-1;j++){      
      if (s_new[i]>=fstr_S[j] && s_new[i]<=fstr_S[j+1]) {
	//cout <<"success! at interpolation number: ";
	if ((s_new[i]-fstr_S[j])/(fstr_S[j+1]-fstr_S[j])<0.50000){
	  // cout << j << endl;;
	  nodes_picked[i]=j;
	  for (int m=1;m<=natoms*3;m++){
	    angs_pos[i][m]=fstring[j][m];
	  }
	}
	else {
	  // cout << j+1 << endl;;
	  nodes_picked[i]=j+1;
	  for (int m=1;m<=natoms*3;m++){
	    angs_pos[i][m]=fstring[j+1][m];
	  }
	}
      }
    }
    if (i==nnNew){
      for (int j=1;j<=natoms*3;j++){
	angs_pos[i][j]=fstring[num_interp][j];
      }
    }
  }
    
  for (int i=1;i<=nnOld;i++){
    delete [] angspos_old[i];
  }
  delete [] angspos_old;
  delete [] fstr_S;

}



void LST::LSTinterpolate(double* xyz1, double* xyz2, double* xyzf, double f, int max_iter, int natoms, double grad_tol) {
 
  double** r_1 = new double*[1+natoms];
  double** r_2 = new double*[1+natoms];
  double** r_c = new double*[1+natoms];
  double** r_i = new double*[1+natoms];
  for (int i=1;i<=natoms;i++){
    r_1[i] = new double[1+natoms];
    r_2[i] = new double[1+natoms];
    r_c[i] = new double[1+natoms];
    r_i[i] = new double[1+natoms];
  }

  double* dS = new double[1+natoms*3];
  double* step = new double[1+natoms*3];

  double** d2S = new double*[1+natoms*3];
  double** d2S_inv = new double*[1+natoms*3];
  for (int i=1;i<=natoms*3;i++){
    d2S[i] = new double[1+natoms*3];
    d2S_inv[i] = new double[1+natoms*3];
  }

  double* xyz_stor = new double[1+natoms*3];

  //Interpolate cartesian distances to get initial cartesian guess
  for (int i=1;i<=natoms*3;i++) {
    xyzf[i]=xyz1[i] + f*(xyz2[i]-xyz1[i]);
    xyz_stor[i] = xyzf[i];
  }

  //construct r matrices of reference structures from cartesian information
  Utils::Rmat_from_lincart(r_1, xyz1, natoms);
  Utils::Rmat_from_lincart(r_2, xyz2, natoms);
  Utils::Rmat_from_lincart(r_c, xyzf, natoms);

  //Interpolate r matrices to get r_i
  for (int i=1;i<=natoms;i++){
    for (int j=1;j<=natoms;j++){
      r_i[i][j]=r_1[i][j] + f*(r_2[i][j]-r_1[i][j]);
    }
  }
  
  //  cout << "LST minimization attempted........." << endl;

  //Loop through the gradient based optimization scheme
  for (int iter=1;iter<=max_iter;iter++) {
  
    //Initialize dS and d2S
    for (int i=1;i<=natoms*3;i++){
      dS[i]=0;
      for (int j=1;j<=natoms*3;j++){
	d2S[i][j]=0;
      }
    }
    
    //evaluate 1st derivative of S wrt cartesians
    for (int i=1;i<=natoms;i++){
      for (int j=1;j<=3;j++){
	for (int k=1;k<=natoms;k++){
          if (i!=k){
	    dS[3*(i-1)+j]+=2*((r_c[k][i]-r_i[k][i])/(pow(r_i[k][i],4)))*(1/r_c[k][i])*(xyzf[3*(i-1)+j]-xyzf[3*(k-1)+j]);
          }
        }
	dS[3*(i-1)+j]+=2*(10e-3)*(xyzf[3*(i-1)+j]-xyz_stor[3*(i-1)+j]);
      }
    }

    //evaluate 2nd derivative of S wrt cartesians
    //first, let's do the diagonal elements of the matrix
    for (int i=1;i<=natoms;i++){
      for (int j=1;j<=3;j++){
        for (int k=1;k<=natoms;k++){
          if (i!=k){
            d2S[3*(i-1)+j][3*(i-1)+j]+=2*((1/pow(r_i[i][k], 4)) + (1/pow(r_i[i][k], 3))*(1/r_c[i][k]) + (1/pow(r_i[i][k]*r_c[i][k], 3))*pow(xyzf[3*(i-1)+j]-xyzf[3*(k-1)+j], 2));
          }
        }
      }
    }

    //Next, evaluate pieces that are wrt the same cartesian component on different atomic centers
    for (int i=1;i<=natoms;i++){
      for (int j=1;j<=natoms;j++){
        for (int k=1;k<=3;k++){
          if (i!=j){
            d2S[3*(i-1)+k][3*(j-1)+k]=-2*((1/pow(r_i[i][j], 4))-(1/pow(r_i[i][j], 3))*(1/r_c[i][j])+(1/pow(r_i[i][j]*r_c[i][j], 3))*(xyzf[3*(i-1)+k]-xyzf[3*(j-1)+k]));
          }
        }
      }
    }

    //Now, evaluate pieces that are wrt different cartesian components, but same atomic center
    for (int i=1;i<=natoms;i++){
      for (int j=1;j<=3;j++){
        for (int k=1;k<=3;k++){
          for (int m=1;m<=natoms;m++){
            if (i!=m && k!=j){
              d2S[3*(i-1)+j][3*(i-1)+k]+=2*(1/pow(r_i[i][m]*r_c[i][m],3))*(xyzf[3*(i-1)+j]-xyzf[3*(m-1)+j])*(xyzf[3*(i-1)+k]-xyzf[3*(m-1)+k]);
            }
          }
        }
      }
    }

    //Now, evaluate pieces that are wrt to different cartesian components on different atomic centers
    for (int i=1;i<=natoms;i++){
      for (int j=1;j<=natoms;j++){
        for (int k=1;k<=3;k++){
          for (int m=1;m<=3;m++){
            if (i!=j && k!=m){
              d2S[3*(i-1)+k][3*(j-1)+m]=-2*(1/pow(r_i[i][j]*r_c[i][j], 3))*(xyzf[3*(i-1)+k]-xyzf[3*(j-1)+k])*(xyzf[3*(i-1)+m]-xyzf[3*(j-1)+m]);
            }
          }
        }
      }
    }

    for (int i=1;i<=natoms*3;i++){
      for (int j=1;j<=natoms*3;j++){
	d2S[i][j]+=2*(10e-3);
      }
    }

    Utils::invertNxN(d2S, d2S_inv, natoms*3);

    Utils::Mat_times_vec(d2S_inv, dS, step, natoms*3);

    for (int i=1;i<=natoms*3;i++){
      step[i]*=-1;
    }

    //Check for convergence based on magnitude of gradient
    double dSmag=Utils::vecMag(dS, natoms*3);
    double SCALING = Utils::vecMag(step, natoms*3);
    //if the mag. of the 1st der is less than threshold, break out of loop
    //    cout << "dSmag = " << dSmag << " tol = " << grad_tol << endl;

    if ((dSmag < grad_tol)){                                                      // || (Utils::vecMag(step, natoms*3)<0.00000000001)){
      //      cout << "********** LST minimization successful! ***********" << endl;
      //cout << "It took " << iter << " steps" << endl;
      //      cout << "dSmag is: " << dSmag << " SCALING is: " << SCALING;
      //cout << "and occured at step number: " << iter << endl;
      //      cout << "the structure is: " << endl;
      //      Utils::display_structure_nonames(xyzf, natoms);
      break;
    }
    else if(iter==max_iter){
      cout << "LST minimization failed! dSmag = " << dSmag << " and the tolerance is: " << grad_tol << endl;
    }
    
    //update working cartesians
    for (int i=1;i<=natoms*3;i++){
      xyzf[i]+=step[i];
    }
    
    Utils::Rmat_from_lincart(r_c, xyzf, natoms);
  }
  
  delete [] dS;
  delete [] step;
  delete [] xyz_stor;

  for (int i=1;i<=natoms*3;i++){
    delete [] d2S[i];
    delete [] d2S_inv[i];
  }
  delete [] d2S;
  delete [] d2S_inv;

  for (int i=1;i<=natoms;i++){
    delete [] r_1[i];
    delete [] r_2[i];
    delete [] r_i[i];
    delete [] r_c[i];
  }
  delete [] r_1;
  delete [] r_2;
  delete [] r_i;
  delete [] r_c;
  
}

void LST::LSTinterpolate(double* xyz1, double* xyz2, double* xyzf, double f, int max_iter, int natoms, double grad_tol, double* initial_guess) {

  double** r_1 = new double*[1+natoms];
  double** r_2 = new double*[1+natoms];
  double** r_c = new double*[1+natoms];
  double** r_i = new double*[1+natoms];
  for (int i=1;i<=natoms;i++){
    r_1[i] = new double[1+natoms];
    r_2[i] = new double[1+natoms];
    r_c[i] = new double[1+natoms];
    r_i[i] = new double[1+natoms];
  }

  double* dS = new double[1+natoms*3];
  double* step = new double[1+natoms*3];

  double** d2S = new double*[1+natoms*3];
  double** d2S_inv = new double*[1+natoms*3];
  for (int i=1;i<=natoms*3;i++){
    d2S[i] = new double[1+natoms*3];
    d2S_inv[i] = new double[1+natoms*3];
  }

  double* xyz_stor = new double[1+natoms*3];

  //Interpolate cartesian distances to get initial cartesian guess
  for (int i=1;i<=natoms*3;i++) {
    //    xyzf[i]=xyz1[i] + f*(xyz2[i]-xyz1[i]);
    xyzf[i] = initial_guess[i];

    xyz_stor[i] = xyzf[i];
  }

  //construct r matrices of reference structures from cartesian information
  Utils::Rmat_from_lincart(r_1, xyz1, natoms);
  Utils::Rmat_from_lincart(r_2, xyz2, natoms);
  Utils::Rmat_from_lincart(r_c, xyzf, natoms);

  //Interpolate r matrices to get r_i
  for (int i=1;i<=natoms;i++){
    for (int j=1;j<=natoms;j++){
      r_i[i][j]=r_1[i][j] + f*(r_2[i][j]-r_1[i][j]);
    }
  }

  //  cout << "LST minimization attempted........." << endl;

  //Loop through the gradient based optimization scheme
  for (int iter=1;iter<=max_iter;iter++) {

    //Initialize dS and d2S
    for (int i=1;i<=natoms*3;i++){
      dS[i]=0;
      for (int j=1;j<=natoms*3;j++){
        d2S[i][j]=0;
      }
    }

    //evaluate 1st derivative of S wrt cartesians
    for (int i=1;i<=natoms;i++){
      for (int j=1;j<=3;j++){
        for (int k=1;k<=natoms;k++){
          if (i!=k){
            dS[3*(i-1)+j]+=2*((r_c[k][i]-r_i[k][i])/(pow(r_i[k][i],4)))*(1/r_c[k][i])*(xyzf[3*(i-1)+j]-xyzf[3*(k-1)+j]);
          }
        }
        dS[3*(i-1)+j]+=2*(10e-3)*(xyzf[3*(i-1)+j]-xyz_stor[3*(i-1)+j]);
      }
    }

    //evaluate 2nd derivative of S wrt cartesians
    //first, let's do the diagonal elements of the matrix
    for (int i=1;i<=natoms;i++){
      for (int j=1;j<=3;j++){
        for (int k=1;k<=natoms;k++){
          if (i!=k){
            d2S[3*(i-1)+j][3*(i-1)+j]+=2*((1/pow(r_i[i][k], 4)) + (1/pow(r_i[i][k], 3))*(1/r_c[i][k]) + (1/pow(r_i[i][k]*r_c[i][k], 3))*pow(xyzf[3*(i-1)+j]-xyzf[3*(k-1)+j], 2));
          }
        }
      }
    }

    //Next, evaluate pieces that are wrt the same cartesian component on different atomic centers
    for (int i=1;i<=natoms;i++){
      for (int j=1;j<=natoms;j++){
        for (int k=1;k<=3;k++){
          if (i!=j){
            d2S[3*(i-1)+k][3*(j-1)+k]=-2*((1/pow(r_i[i][j], 4))-(1/pow(r_i[i][j], 3))*(1/r_c[i][j])+(1/pow(r_i[i][j]*r_c[i][j], 3))*(xyzf[3*(i-1)+k]-xyzf[3*(j-1)+k]));
          }
        }
      }
    }

    //Now, evaluate pieces that are wrt different cartesian components, but same atomic center
    for (int i=1;i<=natoms;i++){
      for (int j=1;j<=3;j++){
        for (int k=1;k<=3;k++){
          for (int m=1;m<=natoms;m++){
            if (i!=m && k!=j){
              d2S[3*(i-1)+j][3*(i-1)+k]+=2*(1/pow(r_i[i][m]*r_c[i][m],3))*(xyzf[3*(i-1)+j]-xyzf[3*(m-1)+j])*(xyzf[3*(i-1)+k]-xyzf[3*(m-1)+k]);
            }
          }
        }
      }
    }

    //Now, evaluate pieces that are wrt to different cartesian components on different atomic centers
    for (int i=1;i<=natoms;i++){
      for (int j=1;j<=natoms;j++){
        for (int k=1;k<=3;k++){
          for (int m=1;m<=3;m++){
            if (i!=j && k!=m){
              d2S[3*(i-1)+k][3*(j-1)+m]=-2*(1/pow(r_i[i][j]*r_c[i][j], 3))*(xyzf[3*(i-1)+k]-xyzf[3*(j-1)+k])*(xyzf[3*(i-1)+m]-xyzf[3*(j-1)+m]);
            }
          }
        }
      }
    }

    for (int i=1;i<=natoms*3;i++){
      for (int j=1;j<=natoms*3;j++){
        d2S[i][j]+=2*(10e-3);
      }
    }

    Utils::invertNxN(d2S, d2S_inv, natoms*3);

    Utils::Mat_times_vec(d2S_inv, dS, step, natoms*3);

    for (int i=1;i<=natoms*3;i++){
      step[i]*=-1;
    }

    //Check for convergence based on magnitude of gradient
    double dSmag=Utils::vecMag(dS, natoms*3);
    double SCALING = Utils::vecMag(step, natoms*3);
    //if the mag. of the 1st der is less than threshold, break out of loop
    //    cout << "\tdSmag = " << dSmag << " tol = " << grad_tol << endl;
    if ((dSmag < grad_tol)){                                                      // || (Utils::vecMag(step, natoms*3)<0.00000000001)){
      //      cout << "********** LST minimization successful! ***********" << endl;
      //cout << "It took " << iter << " steps" << endl;
      //      cout << "dSmag is: " << dSmag << " SCALING is: " << SCALING;
      //cout << "and occured at step number: " << iter << endl;
      break;
    }
    else if(iter==max_iter){
      cout << "LST minimization failed!" << endl;
    }

    //update working cartesians
    for (int i=1;i<=natoms*3;i++){
      xyzf[i]+=step[i];
    }

    Utils::Rmat_from_lincart(r_c, xyzf, natoms);
  }

  delete [] dS;
  delete [] step;
  delete [] xyz_stor;

  for (int i=1;i<=natoms*3;i++){
    delete [] d2S[i];
    delete [] d2S_inv[i];
  }
  delete [] d2S;
  delete [] d2S_inv;

  for (int i=1;i<=natoms;i++){
    delete [] r_1[i];
    delete [] r_2[i];
    delete [] r_i[i];
    delete [] r_c[i];
  }
  delete [] r_1;
  delete [] r_2;
  delete [] r_i;
  delete [] r_c;

}
