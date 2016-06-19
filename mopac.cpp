#include "mopac.h"
using namespace std;
#include "constants.h"

//#define MOPAC_LEVEL "PM6 NOSYM 1SCF GRADIENTS UHF TRIPLET CHARGE=1"
#define MOPAC_LEVEL "PM6 NOSYM 1SCF GRADIENTS UHF AUX"

void Mopac::write_ic_input(ofstream& inpfile, int anum, ICoord ic){

  //printf("\n\n in write_ic_input() for atom %i \n",anum);

//  printf(" nbonds: %i \n",ic.nbonds);
 
  for (int i=0;i<nfrz0;i++)
  {
    if (anum==frzlist[i] || anum < 4)
    {
      //printf(" this atom was moved or is in the first 3, doing xyz \n");
      inpfile << xyz[3*anum+0] << " 0 " << xyz[3*anum+1] << " 0 " << xyz[3*anum+2] << " 0 " << endl;
      return;
    }
  }

//  printf(" atom attached to moved atom, freezing bond \n"); 

  int b1,c1,d1;
  for (int i=0;i<ic.nbonds;i++)
  {
    b1 = -1;
    if (ic.bonds[i][0] == anum || ic.bonds[i][1] == anum)
    for (int j=0;j<nfrz0;j++)
    {
      //printf(" found %i, checking for bond partner %i \n",anum,frzlist[j]);
      if (ic.bonds[i][0]==frzlist[j]
        || ic.bonds[i][1]==frzlist[j])
      {
        b1 = frzlist[j];
        break;
      }
    }

    if (b1>-1)
    {
      //printf(" found attachment: %i %i \n",b1,anum);
      break;
    }
  }

//could change this to "xyz list" if statement
  c1 = -1; d1 = -1;
  for (int i=0;i<natoms;i++)
  {
    if (c1==-1)
    for (int j=0;j<i;j++)
    {
      if (!frzlistb[i] && !frzlistb[j])
      {
        c1 = i; d1 = j;
        //printf(" found angle/tor possibility: %i %i \n",c1,d1);
        break;
      }
    }
  }

  if (b1==-1 || c1==-1 || d1==-1)
  {
    printf(" failed to find IC for atom %i, writing xyz \n",anum);
    inpfile << xyz[3*anum+0] << " 0 " << xyz[3*anum+1] << " 0 " << xyz[3*anum+2] << " 0 " << endl;
  }
  else
  {
    double rv,anglev,torv;
    rv = ic.distance(anum,b1);
    anglev = ic.angle_val(anum,b1,c1);
    torv = ic.torsion_val(anum,b1,c1,d1);
    //printf(" r: %1.3f angle: %1.3f tor: %1.3f \n",rv,anglev,torv);
    inpfile << " " << rv << " 0 " << anglev << " 1 " << torv << " 1 " << b1+1 << " " << c1+1 << " " << d1+1 << endl;
  }


  return;
}

double Mopac::opt() {

  string filename = "scratch/testmopac.mop";

  energy = opt(filename);
 
  return energy;
}

void Mopac::opt_header(ofstream& inpfile) {

  inpfile << MOPAC_LEVEL << endl;
  inpfile << "   MOPAC run " << endl;
  inpfile << "   ready to go? " << endl;

  return;
}

void Mopac::grad_header(ofstream& inpfile) {

//printf(" WARNING: mopac set to anion \n");

  inpfile << MOPAC_LEVEL << endl;
  inpfile << "   MOPAC run " << endl;
  inpfile << "   ready to go? " << endl;

  return;
}

double Mopac::opt(string filename, ICoord icoords) {

#if 0
  printf(" WARNING: bypassing ICs! \n");
  return opt(filename);
#endif

   //printf(" in mopac/opt() w/IC freeze \n");

  energy0 = energy = 0;

#if SKIPMOPAC
  printf(" skipping mopac opt! \n");
#endif

  ofstream inpfile;
  string inpfile_string = filename;
  ofstream xyzfile;
  string xyzfile_string = "scratch/testmopac.xyz";
#if !SKIPMOPAC
  inpfile.open(inpfile_string.c_str());
  inpfile.setf(ios::fixed);
  inpfile.setf(ios::left);
  inpfile << setprecision(6);

  xyzfile.open(xyzfile_string.c_str());
  xyzfile.setf(ios::fixed);
  xyzfile.setf(ios::left);
  xyzfile << setprecision(6);
  xyzfile << " " << natoms << endl << endl;

  opt_header(inpfile);

  for (int i=0;i<natoms;i++)
  {
    if (!frzlistb[i])
      inpfile << " " << anames[i] << " " << xyz[3*i+0] << " 1 " << xyz[3*i+1] << " 1 " << xyz[3*i+2] << " 1 " << endl;
    else
    {
      inpfile << " " << anames[i] << " ";
      write_ic_input(inpfile,i,icoords);
    }
    xyzfile << " " << anames[i] << " " << xyz[3*i+0] << " " << xyz[3*i+1] << " " << xyz[3*i+2] << endl;
  }

  string cmd = "/tmp/MOPAC2012.exe2 "+filename;
  system(cmd.c_str());

  energy = read_output(filename);
 
  printf(" initial energy: %1.4f final energy: %1.4f \n",energy0,energy); 

  // need to retrieve final geometry, write to xyz
  xyz_read(inpfile_string);
  xyz_save(inpfile_string+".xyz");
#endif

  xyzfile.close();
  inpfile.close();

  if (abs(energy)<0.00001)
  {
    printf(" energy zero, mopac failed \n");
    return 10000;
  }

  return energy;
}


// standard opt
double Mopac::opt(string filename) {

   //printf(" in mopac/opt() \n");

  energy0 = energy = 0;

#if SKIPMOPAC
  printf(" skipping mopac opt! \n");
#endif

  ofstream inpfile;
  string inpfile_string = filename;
  ofstream xyzfile;
  string xyzfile_string = "scratch/testmopac.xyz";
#if !SKIPMOPAC
  inpfile.open(inpfile_string.c_str());
  inpfile.setf(ios::fixed);
  inpfile.setf(ios::left);
  inpfile << setprecision(6);

  xyzfile.open(xyzfile_string.c_str());
  xyzfile.setf(ios::fixed);
  xyzfile.setf(ios::left);
  xyzfile << setprecision(6);
  xyzfile << " " << natoms << endl << endl;

  opt_header(inpfile);

  for (int i=0;i<natoms;i++)
  {
    if (!frzlistb[i])
      inpfile << " " << anames[i] << " " << xyz[3*i+0] << " 1 " << xyz[3*i+1] << " 1 " << xyz[3*i+2] << " 1 " << endl;
    else
      inpfile << " " << anames[i] << " " << xyz[3*i+0] << " 0 " << xyz[3*i+1] << " 0 " << xyz[3*i+2] << " 0 " << endl;
    xyzfile << " " << anames[i] << " " << xyz[3*i+0] << " " << xyz[3*i+1] << " " << xyz[3*i+2] << endl;
  }
//  xyzfile << " " << natoms << endl << endl;

  string cmd = "/tmp/MOPAC2012.exe2 "+filename;
  system(cmd.c_str());

  energy = read_output(filename);
 
  printf(" initial energy: %1.4f final energy: %1.4f \n",energy0,energy); 

  // need to retrieve final geometry, write to xyz
  xyz_read(inpfile_string);
  xyz_save(inpfile_string+".xyz");
#endif

  xyzfile.close();
  inpfile.close();

  if (abs(energy)<0.00001)
  {
    printf(" energy zero, mopac failed \n");
    return 10000;
  }

  return energy;
}


// gradient
double Mopac::grads(string filename)
{

  //printf(" in mopac/grad for file %s \n",filename.c_str());

  energy0 = energy = 0;

#if SKIPMOPAC
//  printf(" skipping mopac grad! \n");
#endif

  ofstream inpfile;
  string inpfile_string = filename;
//  ofstream xyzfile;
//  string xyzfile_string = "scratch/testmopac.xyz";
#if !SKIPMOPAC
  inpfile.open(inpfile_string.c_str());
  inpfile.setf(ios::fixed);
  inpfile.setf(ios::left);
  inpfile << setprecision(6);


//  xyzfile.open(xyzfile_string.c_str());
//  xyzfile.setf(ios::fixed);
//  xyzfile.setf(ios::left);
//  xyzfile << setprecision(6);
//  xyzfile << " " << natoms << endl << endl;

  grad_header(inpfile);

  for (int i=0;i<natoms;i++)
  {
    inpfile << " " << anames[i] << " " << xyz[3*i+0] << " 1 " << xyz[3*i+1] << " 1 " << xyz[3*i+2] << " 1 " << endl;
//    xyzfile << " " << anames[i] << " " << xyz[3*i+0] << " " << xyz[3*i+1] << " " << xyz[3*i+2] << endl;
  }

  string cmd = "/tmp/MOPAC2012.exe2 "+filename;
  system(cmd.c_str());

  //sleep(1);
  energy = read_grad(filename);
#endif
 
  //printf(" energy: %1.4f \n",energy); 

//  xyzfile.close();
  inpfile.close();

  gradcalls++;
#if WRITE_FILES
  //printf(" Mopac ID: %i \n",rnum);
  string nstr = StringTools::int2str(gradcalls,4,"0");
  write_xyz_grad("scratch/qcsave"+id+"."+nstr);
#endif

  return energy;
}

double Mopac::read_output(string filename) {

  energy = -1;

  string oname = filename+".out";
  ifstream output(oname.c_str(),ios::in);
  string line;
  vector<string> tok_line;
  while(!output.eof()) 
  { 
    getline(output,line);
//    cout << " RR " << line << endl;
    if (line.find("CYCLE:     1")!=string::npos)
    {
      cout << "Initial: " << line << endl;
      tok_line = StringTools::tokenize(line, " \t");
//      energy0=atof(tok_line[10].c_str());
    }
    if (line.find("FINAL")!=string::npos)
    {
      //cout << "Final: " << line << endl;
      tok_line = StringTools::tokenize(line, " \t");
      energy=atof(tok_line[5].c_str());
    }
  }

  output.close();

  return energy;
}

double Mopac::read_grad(string filename) {

  energy = -1;

  //printf(" Mopac read_grad: %s \n",filename.c_str());

  string oname = filename+".out";
  ifstream output(oname.c_str(),ios::in);
  string line;
  vector<string> tok_line;
  while(!output.eof()) 
  { 
    getline(output,line);
   // cout << " RR " << line << endl;
    if (line.find("DERIVATIVES")!=string::npos)
    {
      //cout << "Found Grad Section " << line << endl;
      getline(output,line);
      getline(output,line);
      for (int i=0;i<3*natoms;i++)
      {
        getline(output,line);
        tok_line = StringTools::tokenize(line, " \t");
        grad[i]=atof(tok_line[6].c_str())/627.5;
        //cout << " " << grad[i] << endl;
      }
      break;
    }
    if (line.find("FINAL HEAT")!=string::npos)
    {
      //cout << "Final: " << line << endl;
      tok_line = StringTools::tokenize(line, " \t");
      energy=atof(tok_line[5].c_str());
    }
  }

  //for (int i=0;i<natoms;i++)
 //   printf(" %i  %1.4f %1.4f %1.4f \n",i,grad[3*i+0],grad[3*i+1],grad[3*i+2]);

  output.close();

  return energy;
}

void Mopac::alloc(int natoms_i) {
 
//  printf(" in mopac/alloc() \n");

  natoms = natoms_i;
  anumbers = new int[natoms];
  anames = new string[natoms];

  xyz0 = new double[3*natoms];
  xyz = new double[3*natoms];
  grad = new double[3*natoms];
  for (int i=0;i<3*natoms;i++)
    grad[i] = 0.;

  nfrz = 0;
  frzlist = new int[natoms_i];
  frzlistb = new int[natoms_i];

  gradcalls = 0;
  rnum = rand()%1000+1;
  id = StringTools::int2str(rnum,4,"0");
  //printf(" Mopac ID: %i \n",rnum);

  return;
}

void Mopac::init(int natoms_i, int* anumbers_i, string* anames_i, double* xyz_i) {
 
//  printf(" in mopac/init() \n");

  natoms = natoms_i;
  anumbers = new int[natoms];
  anames = new string[natoms];

  xyz0 = new double[3*natoms];
  xyz = new double[3*natoms];
  grad = new double[3*natoms];
  for (int i=0;i<3*natoms;i++)
    grad[i] = 0.;

  for (int i=0;i<natoms;i++)
    anumbers[i] = anumbers_i[i];
  for (int i=0;i<natoms;i++)
    anames[i] = anames_i[i];
  for (int i=0;i<3*natoms;i++)
    xyz0[i] = xyz[i] = xyz_i[i];  

  nfrz = 0;
  frzlist = new int[natoms_i];
  frzlistb = new int[natoms_i];

  gradcalls = 0;
  rnum = rand()%1000+1;
  id = StringTools::int2str(rnum,4,"0");
  //printf(" Mopac ID: %i \n",rnum);

  return;
}

void Mopac::freemem() {

  delete [] xyz0;
  delete [] xyz;
  delete [] grad;
  delete [] anumbers;
  delete [] anames;

  delete [] frzlist;
  delete [] frzlistb;

  return;
}

void Mopac::freeze(int* frzlist_new, int nfrz_new, int nfrz0_new) {

  nfrz0 = nfrz0_new;
  nfrz = nfrz_new;
  for (int i=0;i<natoms;i++)
    frzlistb[i] = 0;
  for (int i=0;i<nfrz;i++)
    frzlistb[frzlist_new[i]] = 1;
  for (int i=0;i<nfrz;i++)
    frzlist[i] = frzlist_new[i];

  printf(" freeze list: ");
  for (int i=0;i<nfrz;i++)
    printf("%i ",frzlist[i]);
  printf("\n");
  
  return;
}

void Mopac::reset(double* xyz_i)
{
  for (int i=0;i<3*natoms;i++)
    xyz0[i] = xyz[i] = xyz_i[i];  

  return;
}

void Mopac::reset(int natoms_i, int* anumbers_i, string* anames_i, double* xyz_i) {
 
//  printf(" in mopac/reset() \n");

  if (natoms!=natoms_i)
  {
    printf(" mopac reset failed due to different # of atoms \n");
    return;
  }

  for (int i=0;i<natoms;i++)
    anumbers[i] = anumbers_i[i];
  for (int i=0;i<natoms;i++)
    anames[i] = anames_i[i];
  for (int i=0;i<3*natoms;i++)
    xyz0[i] = xyz[i] = xyz_i[i];  

  nfrz = 0;

  return;
}

void Mopac::xyz_save(string filename){

  ofstream xyzfile;
  string xyzfile_string = "xyzfile.txt";
  xyzfile.open(filename.c_str());
  xyzfile.setf(ios::fixed);
  xyzfile.setf(ios::left);
  xyzfile << setprecision(6);

   xyzfile << " " << natoms << endl;
   xyzfile << " " << endl;
   for (int i=0;i<natoms;i++)
   {
     xyzfile << "  " << anames[i];
     xyzfile << " " << xyz[3*i+0] << " " << xyz[3*i+1] << " " << xyz[3*i+2];
     xyzfile << endl;
   }

   xyzfile.close();
}

void Mopac::xyz_read(string filename){ 
   
//  printf(" in xyz_read \n");
  string oname = filename+".out";
  ifstream output(oname.c_str(),ios::in);
  string line;
  vector<string> tok_line;
  int count = 0;
  int i = 0;
  while(!output.eof()) 
  { 
    getline(output,line);
//    cout << " RR " << line << endl;
    if (count == 2)
    {
      if (!StringTools::cleanstring(line)) break;
      vector<string> tok_line = StringTools::tokenize(line, " \t");
      xyz[3*i+0]=atof(tok_line[2].c_str());
      xyz[3*i+1]=atof(tok_line[3].c_str());
      xyz[3*i+2]=atof(tok_line[4].c_str());
//      cout << tok_line[0] << " " << tok_line[1] << " " << tok_line[2] << " " << endl; 
      i++;
    }

    if (line.find("CARTESIAN COORDINATES")!=string::npos && count == 0)
    {
//      cout << "Initial: " << line << endl;
      count++;
    }
    else if (line.find("CARTESIAN COORDINATES")!=string::npos && count == 1)
    {
//      cout << "Final: " << line << endl;
      count++;
      getline(output,line);
      getline(output,line);
      getline(output,line);
    }
  }

  output.close();
 
}   

void Mopac::write_xyz_grad(string filename)
{
  ofstream xyzfile;
  string xyzfile_string = filename+".xyz";
  xyzfile.open(xyzfile_string.c_str());
  xyzfile.setf(ios::fixed);
  xyzfile.setf(ios::left);
  xyzfile << setprecision(15);

  xyzfile << natoms << endl << energy << endl;
  for (int i=0;i<natoms;i++)
    xyzfile << anames[i] << " " << xyz[3*i+0] << " " << xyz[3*i+1] << " " << xyz[3*i+2] << endl;

  xyzfile.close();

  ofstream gradfile;
  string gradfile_string = filename+".grad";
  gradfile.open(gradfile_string.c_str());
  gradfile.setf(ios::fixed);
  gradfile.setf(ios::left);
  gradfile << setprecision(6);

  gradfile << natoms << endl << energy << endl;
  for (int i=0;i<natoms;i++)
    gradfile << anames[i] << " " << grad[3*i+0] << " " << grad[3*i+1] << " " << grad[3*i+2] << endl;

  gradfile.close();

  return;
}
