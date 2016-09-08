#include "orca.h"

using namespace std;


void ORCA::alloc(int natoms0)
{
  natoms = natoms0;
  anumbers = new int[natoms+1];
  anames = new string[natoms+1];

  return;
}

void ORCA::init(string infilename, int natoms0, int* anumbers0, string* anames0, int run, int rune)
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

#if ORCA
  printf("  ORCA initialized \n");
#endif

  string nstr = StringTools::int2str(runNum,4,"0");
  string runends = StringTools::int2str(runend,2,"0");
  outfile="scratch/orcain"+nstr+"."+runends+".out";
//  cout << " outfile: " << outfile << endl;

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


double ORCA::grads(double* coords, double* grad)
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
//  geomfile << natoms << endl << endl;
  for(int j=0;j<natoms;j++)
  {
    geomfile << setw(2) << anames[j];
    geomfile << setw(16)<< coords[3*j+0];
    geomfile << setw(16)<< coords[3*j+1];
    geomfile << setw(16)<< coords[3*j+2] << endl;
  }
  geomfile.close();
 
  string ncpustr=StringTools::int2str(ncpu,1,"0");
  string cmd = "./ograd "+endstr+" "+ncpustr;
  system(cmd.c_str());


  string gradfile = outfile;

  V = get_energy_grad(gradfile, grad, natoms);

  if (nscffail>25)
  {
    printf("\n\n Too many SCF failures: %i, exiting \n",nscffail);
    exit(1);
  }
  //printf(" done with ORCA grad call \n");  

  gradcalls++;

  return V * 627.5;
//  return V;
}


double ORCA::get_energy_grad(string file, double* grad, int natoms)
{
  //printf("  in ORCA::get_energy_grad for file: %s \n",file.c_str());

  ifstream gradfile;
  gradfile.open(file.c_str());
  if (!gradfile)
  {
    printf(" Error opening output file! %s \n",file.c_str());
    return -1;
  }

  string line;
  bool success = true;

  double V = 0.;

  //Total Energy (3)
  //CARTESIAN GRADIENT (skip 2 lines)
  vector<string> tok_line;
  int done = 0;
  while (!gradfile.eof() && done<2)
  {
    success=getline(gradfile, line);
    if (line.find("Total Energy")!=string::npos)
    {
      //cout << " RRe: " << line << endl; fflush(stdout);
      tok_line = StringTools::tokenize(line, " \t");
      V = atof(tok_line[3].c_str());
      done++;
    }
    if (line.find("CARTESIAN GRADIENT")!=string::npos)
    {
      success=getline(gradfile, line);
      success=getline(gradfile, line);
      //cout << " RR0g: " << line << endl;
      for (int j=0;j<natoms;j++)
      {
        success=getline(gradfile, line);
        //cout << " RRg: " << line << endl;
        tok_line = StringTools::tokenize(line, " \t");
        for (int k=0;k<3;k++)
          grad[3*j+k] = atof(tok_line[3+k].c_str())*ANGtoBOHR;
      }
      done++;
    }
  }
  if (done<1)
  {
    printf("\n\n Couldn't find the energy from ORCA \n");
    exit(1);
  }

#if 0
  cout << " gradient: " << endl;
  for (int i=0;i<natoms;i++) 
    cout << grad[3*i+0] << " " << grad[3*i+1] << " " << grad[3*i+2] << endl;
#endif

  gradfile.close();

  return V; 
}


void ORCA::write_xyz_grad(double* coords, double* grad, string filename)
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
