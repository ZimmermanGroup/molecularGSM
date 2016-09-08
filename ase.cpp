#include "ase.h"

using namespace std;


void ASE::alloc(int natoms0)
{
  natoms = natoms0;
  anumbers = new int[natoms+1];
  anames = new string[natoms+1];

  return;
}

void ASE::init(string infilename, int natoms0, int* anumbers0, string* anames0, int run, int rune)
{
  gradcalls = 0;
  nscffail = 0;
  firstrun = 1;

  natoms = natoms0;
  anumbers = new int[natoms+1];
  anames = new string[natoms+1];

  for (int i=0;i<natoms0;i++)
    anumbers[i] = anumbers0[i];
  for (int i=0;i<natoms0;i++)
    anames[i] = anames0[i];

  runNum = run;
  runend = rune;

#if ASE
  printf("  ASE initialized \n");
#endif

  string nstr = StringTools::int2str(runNum,4,"0");
  string runends = StringTools::int2str(runend,2,"0");
  aseoutfile="aseout"+nstr+runends;
//  cout << " aseoutfile: " << aseoutfile << endl;

#if 0
  printf(" anames: ");
  for (int i=0;i<natoms;i++)
    printf(" %s",anames[i].c_str());
  printf("\n");
  printf(" anumbers: ");
  for (int i=0;i<natoms;i++)
    printf(" %i",anumbers[i]);
  printf("\n");
#endif

  return;
}


double ASE::grads(double* coords, double* grad)
{
  //printf(" qcg"); fflush(stdout);

  int badgeom = check_array(3*natoms,coords);
  if (badgeom)
  {
    printf(" ERROR: Geometry contains NaN, exiting! \n");
    exit(-1);
  }

  if (ncpu<1) ncpu = 1;

  for (int i=0;i<3*natoms;i++)
    grad[i] = 0.;

  int num,k,c;
  double V = -1.;

  string nstr=StringTools::int2str(runNum,4,"0");
  string runends=StringTools::int2str(runend,2,"0");
  string endstr = nstr+"."+runends;

  string molname = "scratch/structure"+endstr;
  ofstream geomfile(molname.c_str());
//  cout << " geomfile " << molname << endl;

  // print the molecule coordinate section
  geomfile << natoms << endl << endl;
  for(int j=0;j<natoms;j++)
  {
    geomfile << setw(2) << anames[j];
    geomfile << setw(16)<< coords[3*j+0];
    geomfile << setw(16)<< coords[3*j+1];
    geomfile << setw(16)<< coords[3*j+2] << endl;
  }
  geomfile.close();
 
  string ncpustr=StringTools::int2str(ncpu,1,"0");
//  string cmd = "./goase "+endstr;
  string chgstr = StringTools::int2str(CHARGE,1,"0");
  string cmd = "./grad.py "+endstr+" "+ncpustr+" "+chgstr;
  system(cmd.c_str());


  string gradfile = "scratch/GRAD"+endstr;

  V = get_energy_grad(gradfile, grad, natoms);

  if (nscffail>25)
  {
    printf("\n\n Too many SCF failures: %i, exiting \n",nscffail);
    exit(1);
  }
  //printf(" done with ASE grad call \n");  

  gradcalls++;

  return V * 627.5;
//  return V;
}


double ASE::get_energy_grad(string file, double* grad, int natoms)
{
  ifstream gradfile;
  gradfile.open(file.c_str());
  if (!gradfile)
  {
    printf(" Error opening gradient file! %s \n",file.c_str());
    return -1;
  }

  string line;
  bool success = true;

  success=getline(gradfile, line);
  double V = -1 * atof(line.c_str()) / 27.2114;
  //printf(" found E: %7.5f \n",V);

  for (int i=0;i<natoms;i++)
  {
    if(gradfile.eof())
    {
      printf(" missing data in GRAD(1) \n"); fflush(stdout);
      grad[3*i+0] = grad[3*i+1] = grad[3*i+2] = 1.;
      break;
    }
    success=getline(gradfile, line);
    //cout << "RR " << line << endl;
    int length=StringTools::cleanstring(line);
    vector<string> tok_line = StringTools::tokenize(line, " \t[]");
    //printf(" tok_line: %s %s %s \n",tok_line[0].c_str(),tok_line[1].c_str(),tok_line[2].c_str());
    if (tok_line.size()<3)
    {
      printf(" missing data in GRAD(2) \n"); fflush(stdout);
      grad[3*i+0] = grad[3*i+1] = grad[3*i+2] = 1.;
    }
    else
    {
      grad[3*i+0] = atof(tok_line[0].c_str())/27.2114;
      grad[3*i+1] = atof(tok_line[1].c_str())/27.2114;
      grad[3*i+2] = atof(tok_line[2].c_str())/27.2114;
    }
  } //loop i over natoms

#if 0
  cout << " gradient: " << endl;
  for (int i=0;i<natoms;i++) 
    cout << grad[3*i+0] << " " << grad[3*i+1] << " " << grad[3*i+2] << endl;
#endif

//exit(1);

  gradfile.close();

  return V; 
}


void ASE::write_xyz_grad(double* coords, double* grad, string filename)
{
  ofstream xyzfile;
  string xyzfile_string = filename+".xyz";
  xyzfile.open(xyzfile_string.c_str());
  xyzfile.setf(ios::fixed);
  xyzfile.setf(ios::left);
  xyzfile << setprecision(6);

  xyzfile << natoms << endl << energy << endl;
  for (int i=0;i<natoms;i++)
    xyzfile << anames[i] << " " << coords[3*i+0] << " " << coords[3*i+1] << " " << coords[3*i+2] << endl;

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
