#include "molpro.h"
//#include "utils.h"

using namespace std;

//set to 1 to not run molpro
#define SAFE_MODE 0
#define ONLY_RHF 0
#define BASIS631GS 0
#define READ_SCF 1
#define PSPACE 0
//pspace now is 10. //was 100.
#define DYNWEIGHT 0
#define MEMORY 400
#define DIRECT 1


//run molpro for gradient of n, derivative coupling between n,m
int Molpro::run(int n, int m)
{
  //printf(" beginning Molpro run! \n"); fflush(stdout);
  int runGrad = 1;
  if (n<=0)
  {
    runGrad = 0;
    n *= -1;
  }

  if (n>nstates)
  {
    printf(" ERROR: n>nstates! (%i>%i) \n",n,nstates);
    exit(-1);
  }
  if (m>nstates)
  { 
    printf(" ERROR: n>nstates! \n");
    exit(-1);
  }
  //if (m==0) printf(" not computing derivative coupling \n");
#if ONLY_RHF
  printf(" WARNING: using RHF! \n");
#endif

  string filename = infile;

  //here construct Molpro input
  ofstream inpfile;
  string inpfile_string = filename;
  inpfile.open(inpfile_string.c_str());
  inpfile.setf(ios::fixed);
  inpfile.setf(ios::left);
  inpfile << setprecision(6);

  inpfile << "memory," << MEMORY << ",m" << endl;
  inpfile << "file,2," << scratchname << endl;
  inpfile << "symmetry,nosym" << endl;
  inpfile << "orient,noorient" << endl;
  inpfile << "geometry={" << endl;
  for (int i=0;i<natoms;i++)
    inpfile << " " << anames[i] << " " << xyz[3*i+0] << " " << xyz[3*i+1] << " " << xyz[3*i+2] << " " << endl;
  inpfile << "}" << endl;

  inpfile << endl << "basis=" << basis << endl << endl;

  inpfile << "start,2100.2 !read orbitals" << endl;

#if DIRECT
  inpfile << " direct " << endl;
#endif
#if !READ_SCF || ONLY_RHF
  //recalculate HF at each iteration
  inpfile << "hf" << endl;
#endif

#if !ONLY_RHF
  //first two settings from AIMS molpro example
  //inpfile << "gthresh,twoint=1.0d-13" << endl;
  //inpfile << "gthresh,energy=1.0d-7,gradient=1.0d-2" << endl;
  //inpfile << "data,copy, 2100.2, 3000.2" << endl;
  if (runGrad)
  {
    inpfile << "{casscf" << endl;
    inpfile << "closed," << nclosed << " !core orbs" << endl;
    inpfile << "occ," << nocc << "    !active orbs" << endl;
    inpfile << "wf," << nelec << ",1,0 !nelectrons,symm,singlet" << endl;
    inpfile << "state," << nstates << "   !nstates" << endl;
#if DYNWEIGHT
    inpfile << "dynw,-8.0   !dynamic weight" << endl;
#endif
    //if (nstates>=3)
    //  inpfile << "weight,0.2,0.4,0.4   !state averaging" << endl;
    //inpfile << "maxiter,40 " << endl;
    inpfile << "CPMCSCF,GRAD," << n << ".1" << endl;
    inpfile << "ciguess,2501.2" << endl;
    inpfile << "save,ci=2501.2" << endl;
    inpfile << "orbital,2100.2 !write orbitals" << endl;
    //inpfile << "{ iterations" << endl;
    //inpfile << " do,diagci,1,to,20 }" << endl; 

#if PSPACE
    inpfile << "pspace,100.0" << endl;
#endif
    //inpfile << "dm,2105.2" << endl;
    //inpfile << "diab,3000.2,save=3000.2" << endl;
    inpfile << "}" << endl;
    inpfile << endl;
#endif
    inpfile << "{forces; varsav}" << endl;
#if 0
    inpfile << "show,gradx" << endl;
    inpfile << "show,grady" << endl;
    inpfile << "show,gradz" << endl;
#endif
    inpfile << endl;
  }

#if !ONLY_RHF
  if (m>0)
  {
    inpfile << "{casscf" << endl;
    inpfile << "closed," << nclosed << " !core orbs" << endl;
    inpfile << "occ," << nocc << "    !active orbs" << endl;
    inpfile << "wf," << nelec << ",1,0 !nelectrons,symm,singlet" << endl;
    inpfile << "state," << nstates << "   !nstates" << endl;
#if DYNWEIGHT
    inpfile << "dynw,-8.0   !dynamic weight " << endl;
#endif
    //if (nstates>=3)
    //  inpfile << "weight,0.2,0.4,0.4   !state averaging" << endl;
    inpfile << "CPMCSCF,NACM," << n << ".1," << m << ".1 " << endl;
    //inpfile << "ciguess,2501.2" << endl;
    //inpfile << "save,ci=2501.2" << endl;
    inpfile << "orbital,2100.2 !write orbitals" << endl;
    //inpfile << "{ iterations" << endl;
    //inpfile << " do,diagci,1,to,20 }" << endl; 

#if PSPACE
    inpfile << "pspace,100.0" << endl;
#endif
    //inpfile << "dm,2105.2" << endl;
    //inpfile << "diab,3000.2,save=2100.2" << endl;
    inpfile << "}" << endl;
    inpfile << endl;
    inpfile << "{forces; varsav}" << endl;
#if 0
    inpfile << "show,gradx" << endl;
    inpfile << "show,grady" << endl;
    inpfile << "show,gradz" << endl;
#endif
    inpfile << endl;
  }
#endif

  inpfile << endl;


  //restart orbitals handled by file,2,...
  //inpfile << "start,2100.2" << endl;
#if 0
//varsav puts forces into gradx,grady,gradz
{forces; varsav}
show,gradx
show,grady
show,gradz
 
table, GRADX, GRADY, GRADZ   
  save,gopro1.log
#endif

  inpfile.close();

#if !SAFE_MODE
  //printf(" executing molpro \n"); fflush(stdout);
//  string cmd = "/export/zimmerman/paulzim/Molpro_serial/bin/molpro "+filename;
  string cmd = "/export/applications/Molpro/2012.1.9/molprop_2012_1_Linux_x86_64_i8/bin/molpro";
//  string cmd = "/export/applications/MolproCopy/2012.1.9/molprop_2012_1_Linux_x86_64_i8/bin/molpro";
  string nstr = StringTools::int2str(NPROCS,1,"0");
  cmd = cmd + " -W scratch";
  cmd = cmd + " -n " + nstr + " " + filename;
  system(cmd.c_str());
#endif

  int error = read_E();
  //printf(" error after read_E: %i \n",error);
  nrun++;

  return 0;
}



int Molpro::seed()
{
  //printf(" beginning Molpro run! \n"); fflush(stdout);

#if RHF_ONLY
  printf(" skipping prelim run, no MCSCF \n");
  return 0;
#endif

  printf("   Running preliminary RHF/MCSCF calculation \n");

  string filename = infile;

  //here construct Molpro input
  ofstream inpfile;
  string inpfile_string = filename;
  inpfile.open(inpfile_string.c_str());
  inpfile.setf(ios::fixed);
  inpfile.setf(ios::left);
  inpfile << setprecision(6);

  inpfile << "memory," << MEMORY << ",m" << endl;
  inpfile << "file,2," << scratchname << endl;
  inpfile << "symmetry,nosym" << endl;
  inpfile << "orient,noorient" << endl;
  inpfile << "geometry={" << endl;
  for (int i=0;i<natoms;i++)
    inpfile << " " << anames[i] << " " << xyz[3*i+0] << " " << xyz[3*i+1] << " " << xyz[3*i+2] << " " << endl;
  inpfile << "}" << endl;

  inpfile << endl << "basis=" << basis << endl << endl;

#if DIRECT
  inpfile << " direct " << endl;
#endif
  inpfile << "hf" << endl;
  //inpfile << "orbital,2100.2 !write orbitals" << endl; //on by default

 //write orbital settings etc from file MOLPRO_INIT
  for (int i=0;i<nhf_lines;i++)
    inpfile << hf_lines[i] << endl;
#if !ONLY_RHF
  inpfile << "{casscf" << endl;
  inpfile << "closed," << nclosed << " !core orbs" << endl;
  inpfile << "occ," << nocc << "    !active orbs" << endl;
  inpfile << "wf," << nelec << ",1,0 !nelectrons,symm,singlet" << endl;
  inpfile << "state," << nstates << "   !nstates" << endl;
  inpfile << "save,ci=2501.2" << endl;
  inpfile << "orbital,2100.2 !write orbitals" << endl;
  //inpfile << "{ iterations" << endl;
  //inpfile << " do,diagci,1,to,20 }" << endl; 
#if PSPACE
  inpfile << "pspace,100.0" << endl;
#endif
  //inpfile << "dm,2105.2" << endl;
  //inpfile << "diab,2100.2,save=3000.2" << endl;
  inpfile << "}" << endl;

#if 0 
  //for diabatic orbitals
  inpfile << "{casscf" << endl;
  inpfile << "closed," << nclosed << " !core orbs" << endl;
  inpfile << "occ," << nocc << "    !active orbs" << endl;
  inpfile << "wf," << nelec << ",1,0 !nelectrons,symm,singlet" << endl;
  inpfile << "state," << nstates << "   !nstates" << endl;
  inpfile << "diab,2100.2,save=3000.2" << endl;
  inpfile << "}" << endl;
  //inpfile << "data,copy, 2100.2, 3000.2" << endl;
#endif
#endif

  inpfile.close();

#if !SAFE_MODE
  //printf(" executing molpro \n"); fflush(stdout);
//  string cmd = "/export/zimmerman/paulzim/Molpro_serial/bin/molpro "+filename;
  string cmd = "/export/applications/Molpro/2012.1.9/molprop_2012_1_Linux_x86_64_i8/bin/molpro";
//  string cmd = "/export/applications/MolproCopy/2012.1.9/molprop_2012_1_Linux_x86_64_i8/bin/molpro";
  string nstr = StringTools::int2str(NPROCS,1,"0");
  cmd = cmd + " -W scratch";
  cmd = cmd + " -n " + nstr + " " + filename;
  system(cmd.c_str());
#endif

#if !RHF_ONLY
  int error = read_E();  
#endif

  nrun++;

  return 0;
}


double Molpro::getE(int n)
{
#if ONLY_RHF
  if (n>1)
  {
    printf(" WARNING: RHF but getE(%i) \n",n);
    return E[0];
  }
#endif
  if (n<1) 
  {
    printf(" n must be greater than 0 \n");
    return -1.;
  }
  return E[n-1];
}

int Molpro::read_E()
{
  //printf(" in Molpro::read_E \n");

  ifstream outfilei;
  outfilei.open(outfile.c_str());
  if (!outfilei){
    printf(" Error: couldn't open output file \n");
    return 1;
  }   

  string line;
  bool success=true;
  int found = 0;
  while(!outfilei.eof())
  {
    success=getline(outfilei, line);
    if (line.find("MCSCF STATE 1.1 Energy")!=string::npos)
    {
      //cout << " found: " << line << endl;
      int length=StringTools::cleanstring(line);
      vector<string> tok_line = StringTools::tokenize(line, " \t");
      E[0]=atof(tok_line[4].c_str());
      found++;
    }
    else if (line.find("MCSCF STATE 2.1 Energy")!=string::npos)
    {
      //cout << " found: " << line << endl;
      int length=StringTools::cleanstring(line);
      vector<string> tok_line = StringTools::tokenize(line, " \t");
      E[1]=atof(tok_line[4].c_str());
      found++;
      if (nstates==2) break;
    }
    else if (line.find("MCSCF STATE 3.1 Energy")!=string::npos)
    {
      //cout << " found: " << line << endl;
      int length=StringTools::cleanstring(line);
      vector<string> tok_line = StringTools::tokenize(line, " \t");
      E[2]=atof(tok_line[4].c_str());
      found++;
      if (nstates==3) break;
    }
    else if (line.find("MCSCF STATE 4.1 Energy")!=string::npos)
    {
      //cout << " found: " << line << endl;
      int length=StringTools::cleanstring(line);
      vector<string> tok_line = StringTools::tokenize(line, " \t");
      E[3]=atof(tok_line[4].c_str());
      found++;
      if (nstates==4) break;
    }
    else if (line.find("MCSCF STATE 5.1 Energy")!=string::npos)
    {
      //cout << " found: " << line << endl;
      int length=StringTools::cleanstring(line);
      vector<string> tok_line = StringTools::tokenize(line, " \t");
      E[4]=atof(tok_line[4].c_str());
      found++;
      if (nstates==4) break;
    }
#if ONLY_RHF
    if (line.find("RHF STATE 1.1 Energy")!=string::npos)
    {
      //cout << " found: " << line << endl;
      int length=StringTools::cleanstring(line);
      vector<string> tok_line = StringTools::tokenize(line, " \t");
      E[0]=atof(tok_line[4].c_str());
      found++;
    }
#endif

  }  //end while loop

  outfilei.close();

  int error = 1;
  if (found==nstates) error = 0;

#if 0
  for (int i=0;i<nstates;i++)
    printf(" E[%i]: %8.6f \n",i,E[i]);
#endif

  return error;
}

void Molpro::init_hf(int nhf_lines1, string* hf_lines1)
{
  nhf_lines = nhf_lines1;
  if (nhf_lines<1) return;

  hf_lines = new string[nhf_lines];
  for (int i=0;i<nhf_lines;i++)
    hf_lines[i] = hf_lines1[i];

  return;
}

void Molpro::init(int nstates0, int nclosed0, int nocc0, int nelec0, int natoms0, string* anames0, double* xyz0, int NPROCS0, string basis0)
{
  NPROCS = NPROCS0;

  infile = "scratch/gopro.com";
  outfile = "scratch/gopro.out";

  nclosed = nclosed0;
  nocc = nocc0;
  nelec = nelec0;

  basis = basis0;
  nhf_lines = 0;

#if 0
//correct values for pyrazine
  nclosed = 19;
  nocc = 22;
  nelec = 42;
#endif

  natoms = natoms0;
  nstates = nstates0;

  E = new double[nstates];
  for (int i=0;i<nstates;i++)
    E[i] = 0.;

  xyz = new double[3*natoms];
  anames = new string[3*natoms];
  dvec = new double[3*natoms];
  grad = new double[3*natoms];

  for (int i=0;i<natoms;i++)
  {
    anames[i] = anames0[i];
    xyz[3*i+0] = xyz0[3*i+0];
    xyz[3*i+1] = xyz0[3*i+1];
    xyz[3*i+2] = xyz0[3*i+2];
  }

  nrun = 0;

#if DYNWEIGHT
  printf(" using dynamic weighting \n");
#endif
//  printf(" Done with init for %i states \n",nstates);
#if 0
  for (int i=0;i<natoms;i++)
    printf(" %s %8.6f %8.6f %8.6f \n",anames[i].c_str(),xyz[3*i+0],xyz[3*i+1],xyz[3*i+2]);
#endif

  return;
}

void Molpro::freemem()
{
  delete [] E;
  delete [] xyz;
  delete [] anames;
  delete [] grad;
  delete [] dvec;

  return;
}


int Molpro::getGrad(double* grads)
{
  //printf(" in Molpro::getGrad \n");

  ifstream outfilei;
  outfilei.open(outfile.c_str());
  if (!outfilei){
    printf(" Error: couldn't open output file \n");
    return 1;
  }   

  string line;
  bool success=true;
  success=getline(outfilei, line);
  int cont = 0;
  while(!outfilei.eof())
  {
    success=getline(outfilei, line);
    if (line.find("SA-MC GRADIENT FOR STATE")!=string::npos)
    {
      //cout << " found: " << line << endl;
      getline(outfilei,line);
      getline(outfilei,line);
      getline(outfilei,line);
      cont = 1;
      break;
    }
#if ONLY_RHF
    if (line.find("SCF GRADIENT FOR STATE")!=string::npos)
    {
      //cout << " found: " << line << endl;
      getline(outfilei,line);
      getline(outfilei,line);
      getline(outfilei,line);
      cont = 1;
      break;
    }
#endif
  } 
    
  if (cont)
  for (int i=0;i<natoms;i++)
  {
    success=getline(outfilei, line);
    int length=StringTools::cleanstring(line);
    vector<string> tok_line = StringTools::tokenize(line, " \t");
    //cout << " RR: " << line << endl;
    grad[3*i+0]=atof(tok_line[1].c_str());
    grad[3*i+1]=atof(tok_line[2].c_str());
    grad[3*i+2]=atof(tok_line[3].c_str());
  }

  outfilei.close();
  
#if 0
  printf(" XYZ grad (1/Bohr): \n");
  for (int i=0;i<natoms;i++)
    printf(" %s %8.6f %8.6f %8.6f \n",anames[i].c_str(),grad[3*i+0],grad[3*i+1],grad[3*i+2]);
#endif

  for (int i=0;i<3*natoms;i++)
    grads[i] = grad[i];///BOHRtoANG;

  //printf(" done reading grad \n"); fflush(stdout);

  int error = 1;
  if (success && cont) error = 0;

  return error;
}



int Molpro::getDVec(double* dvecs)
{
  //printf(" in Molpro::getDVec \n");

  ifstream outfilei;
  outfilei.open(outfile.c_str());
  if (!outfilei){
    printf(" Error: couldn't open output file \n");
    return 1;
  }   

  for (int i=0;i<3*natoms;i++)
    dvec[i] = 0.;

  string line;
  bool success=true;
  success=getline(outfilei, line);
  int cont = 0;
  while(!outfilei.eof())
  {
    success=getline(outfilei, line);
    if (line.find("SA-MC NACME FOR STATES")!=string::npos)
    {
      //cout << " found: " << line << endl;
      getline(outfilei,line);
      getline(outfilei,line);
      getline(outfilei,line);
      cont = 1;
      break;
    }
  } 
    
  if (cont)
  for (int i=0;i<natoms;i++)
  {
    success=getline(outfilei, line);
    int length=StringTools::cleanstring(line);
    vector<string> tok_line = StringTools::tokenize(line, " \t");
    //cout << " RR: " << line << endl;
    dvec[3*i+0]=atof(tok_line[1].c_str());
    dvec[3*i+1]=atof(tok_line[2].c_str());
    dvec[3*i+2]=atof(tok_line[3].c_str());
  }

  outfilei.close();
  
#if 0
  printf(" XYZ dvec (1/Bohr): \n");
  for (int i=0;i<natoms;i++)
    printf(" %s %8.6f %8.6f %8.6f \n",anames[i].c_str(),dvec[3*i+0],dvec[3*i+1],dvec[3*i+2]);
#endif

  for (int i=0;i<3*natoms;i++)
    dvecs[i] = dvec[i];///BOHRtoANG;

  //printf(" done reading dvec \n"); fflush(stdout);

  int error = 1;
  if (success && cont) error = 0;

  return error;
}

void Molpro::reset(double* xyz1)
{
  for (int i=0;i<3*natoms;i++)
    xyz[i] = xyz1[i];
  return;
}

void Molpro::runname(string name)
{
  scratchname = name;
}

void Molpro::clean_scratch()
{
#if !SAFE_MODE
  system("rm scratch/gopro.*");
#endif
  return;
}

