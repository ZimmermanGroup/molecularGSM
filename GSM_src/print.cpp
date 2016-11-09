#include "icoord.h"

void ICoord::print_xyz(){

 printf(" %i \n",natoms);
 printf("\n");
 for (int i=0;i<natoms;i++) 
 {
     cout << "  " << anames[i];
     printf(" %f %f %f \n",coords[3*i+0],coords[3*i+1],coords[3*i+2]);
 }
// printf("\n");

}

void ICoord::print_ic()
{
   printf("\n printing internals \n");
   printf(" number of bonds: %i\n",nbonds);
   for (int i=0;i<nbonds;i++)
      printf(" bond %2i: %2i to %2i: %5.2f \n",i+1,bonds[i][0]+1,bonds[i][1]+1,bondd[i]);
   printf("\n");

#if 0  
   for (int i=0;i<natoms;i++)
      printf(" atom %i is %i coordinate \n",i+1,coordn[i]);
   printf("\n");
#endif

   printf(" number of angles: %i\n",nangles);
   for (int i=0;i<nangles;i++)
      printf(" angle %3i: %2i %2i %2i: %6.1f \n",i+1,angles[i][0]+1,angles[i][1]+1,angles[i][2]+1,anglev[i]);
   printf("\n");

#if 1
   printf(" number of torsions: %i\n",ntor);
   for (int i=0;i<ntor;i++)
      printf(" torsion %3i: %2i %2i %2i %2i: %6.1f \n",i+1,torsions[i][0]+1,torsions[i][1]+1,torsions[i][2]+1,torsions[i][3]+1,torv[i]);
   printf("\n");
#endif

#if 1
   printf(" number of improper torsions: %i\n",nimptor);
   for (int i=0;i<nimptor;i++)
      printf(" imptor %2i: %2i %2i %2i %2i: %1.1f \n",i+1,imptor[i][0]+1,imptor[i][1]+1,imptor[i][2]+1,imptor[i][3]+1,imptorv[i]);
   printf("\n");
#endif

   printf(" number of nonbonds: %i\n",n_nonbond);
#if 0
   for (int i=0;i<n_nonbond;i++)
      printf(" nonbond %i: %i to %i: %1.4f \n",i+1,nonbond[i][0],nonbond[i][1],nonbondd[i]);
   printf("\n");
#endif
   printf("\n");
}

void ICoord::print_bonds()
{
   printf("\n printing internals \n");
   printf(" number of bonds: %i\n",nbonds);
   for (int i=0;i<nbonds;i++)
      printf(" bond %i: %i to %i: %1.4f \n",i+1,bonds[i][0]+1,bonds[i][1]+1,bondd[i]);
   printf("\n");
#if 0  
   for (int i=0;i<natoms;i++)
      printf(" atom %i is %i coordinate \n",i+1,coordn[i]);
   printf("\n");
#endif
//   printf("\n");
}


void print_xyz_gen(int natoms, string* anames, double* coords){
   
 printf(" %i \n",natoms);
 printf("\n");
 for (int i=0;i<natoms;i++)
 {
     cout << "  " << anames[i];
     printf(" %f %f %f \n",coords[3*i+0],coords[3*i+1],coords[3*i+2]);
 }
// printf("\n");
   
}

void print_triple_xyz(int natoms, string* anames, int* anumbers, double* coords0, double* coords1, double* coords2, double* e){
   
 printf(" %i \n",natoms);
 printf("\n");
 for (int i=0;i<natoms;i++)
 {
     cout << "  " << anames[i];
     printf(" %f %f %f \n",coords0[3*i+0],coords0[3*i+1],coords0[3*i+2]);
 }
 printf(" %i \n",natoms);
 printf("\n");
 for (int i=0;i<natoms;i++)
 {
     cout << "  " << anames[i];
     printf(" %f %f %f \n",coords1[3*i+0],coords1[3*i+1],coords1[3*i+2]);
 }
 printf(" %i \n",natoms);
 printf("\n");
 for (int i=0;i<natoms;i++)
 {
     cout << "  " << anames[i];
     printf(" %f %f %f \n",coords2[3*i+0],coords2[3*i+1],coords2[3*i+2]);
 }
// printf("\n");
   
}

void ICoord::print_xyz_save(string xyzfile_string){

  ofstream xyzfile;
//  string xyzfile_string = "xyzfile.txt";
  xyzfile.open(xyzfile_string.c_str());
  xyzfile.setf(ios::fixed);
  xyzfile.setf(ios::left);
  xyzfile << setprecision(6);

   xyzfile << " " << natoms << endl;
//   xyzfile << " " << seenergy << endl;
   xyzfile << " " << " " << endl;
   for (int i=0;i<natoms;i++) 
   {
     xyzfile << "  " << anames[i];
     xyzfile << " " << coords[3*i+0] << " " << coords[3*i+1] << " " << coords[3*i+2];
     xyzfile << endl;
   }

  return;
}

void ICoord::print_xyz_save(string xyzfile_string, double energy){

  ofstream xyzfile;
//  string xyzfile_string = "xyzfile.txt";
  xyzfile.open(xyzfile_string.c_str());
  xyzfile.setf(ios::fixed);
  xyzfile.setf(ios::left);
  xyzfile << setprecision(6);

   xyzfile << " " << natoms << endl;
   xyzfile << " " << energy << endl;
   for (int i=0;i<natoms;i++) 
   {
     xyzfile << "  " << anames[i];
     xyzfile << " " << coords[3*i+0] << " " << coords[3*i+1] << " " << coords[3*i+2];
     xyzfile << endl;
   }

  return;
}


void print_double_xyz_save(string xyzfile_string, int natoms, string* anames, int* anumbers, double* coords0, double* coords1, double* e){

  ofstream xyzfile;
  xyzfile.open(xyzfile_string.c_str());
  xyzfile.setf(ios::fixed);
  xyzfile.setf(ios::left);
  xyzfile << setprecision(6);

   xyzfile << " " << natoms << endl;
   xyzfile << " " << e[0] << endl;
   for (int i=0;i<natoms;i++) 
   {
     xyzfile << "  " << anames[i];
     xyzfile << " " << coords0[3*i+0] << " " << coords0[3*i+1] << " " << coords0[3*i+2];
     xyzfile << endl;
   }
   xyzfile << " " << natoms << endl;
   xyzfile << " " << e[1] << endl;
   for (int i=0;i<natoms;i++) 
   {
     xyzfile << "  " << anames[i];
     xyzfile << " " << coords1[3*i+0] << " " << coords1[3*i+1] << " " << coords1[3*i+2];
     xyzfile << endl;
   }
   xyzfile << endl;

  return;
}

void print_triple_xyz_save(string xyzfile_string, int natoms, string* anames, int* anumbers, double* coords0, double* coords1, double* coords2, double* e){

  ofstream xyzfile;
  xyzfile.open(xyzfile_string.c_str());
  xyzfile.setf(ios::fixed);
  xyzfile.setf(ios::left);
  xyzfile << setprecision(6);

   xyzfile << " " << natoms << endl;
   xyzfile << " " << e[0] << endl;
   for (int i=0;i<natoms;i++) 
   {
     xyzfile << "  " << anames[i];
     xyzfile << " " << coords0[3*i+0] << " " << coords0[3*i+1] << " " << coords0[3*i+2];
     xyzfile << endl;
   }
   xyzfile << " " << natoms << endl;
   xyzfile << " " << e[1] << endl;
   for (int i=0;i<natoms;i++) 
   {
     xyzfile << "  " << anames[i];
     xyzfile << " " << coords1[3*i+0] << " " << coords1[3*i+1] << " " << coords1[3*i+2];
     xyzfile << endl;
   }
   xyzfile << " " << natoms << endl;
   xyzfile << " " << e[2] << endl;
   for (int i=0;i<natoms;i++) 
   {
     xyzfile << "  " << anames[i];
     xyzfile << " " << coords2[3*i+0] << " " << coords2[3*i+1] << " " << coords2[3*i+2];
     xyzfile << endl;
   }

  return;
}

