#include "qchemsf.h"

using namespace std;

#define COPY_GRAD 0

//NOTE: .qcin file still in gfstringq.exe directory
//either change qchem/parallel.csh or cd in


void QChemSF::alloc(int natoms0)
{
  natoms = natoms0;
  anumbers = new int[natoms+1];
  anames = new string[natoms+1];

  return;
}

void QChemSF::init(string infilename, int natoms0, int* anumbers0, string* anames0, int run, int rune)
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

  E = new double[10];
  grad1 = new double[3*natoms];
  grad2 = new double[3*natoms];
  grad3 = new double[3*natoms];
  grad4 = new double[3*natoms];
  for (int i=0;i<10;i++) E[i] = 0.;
  for (int i=0;i<3*natoms;i++) 
    grad1[i] = grad2[i] = grad3[i] = grad4[i] = 0.;

  runNum = run;
  runend = rune;
  string nstr = StringTools::int2str(run,4,"0");

  //cout << "  -Initializing QCHEM info ..." << endl;
  //cout << "  -opening inpfile to read qchem parameters" << endl;
  ifstream infile;
  infile.open(infilename.c_str());
  if (!infile){
    cout << "!!!!!Error opening inputfile!!!!" << endl;
    exit(-1);
  }

  //cout <<"  -reading file..." << endl;

#if QCHEM
  printf("  Q-Chem initialized \n");
#endif

  // pass infile to stringtools and get the line containing tag
  string tag="QCHEM Scratch Info";
  bool found=StringTools::findstr(infile, tag);

  if (!found){
    cout << "!!!!Could not find tag for QCHEM Info!!!!" << endl;
    exit(-1);
  }

  string line, templine;

  // parse the input section here

  getline(infile, line);
  templine=StringTools::newCleanString(line);
  scrBaseDir=StringTools::trimRight(templine);
  if (scrBaseDir.find("$")!=string::npos)
  {
    scrBaseDir.resize(scrBaseDir.size()-1);
    scrBaseDir = scrBaseDir.substr(1);
    //printf(" found environmental variable: %s \n",scrBaseDir.c_str()); fflush(stdout);
    char* scrPath;
    scrPath = getenv (scrBaseDir.c_str());
    if (scrPath!=NULL)
    {
      string scrPaths(scrPath);
      scrBaseDir = scrPaths+"/";
    }
    else
    {
      printf("  environmental variable %s not set \n",scrBaseDir.c_str());
      exit(-1);
    }
  }
  //cout <<"  -scratch base directory set to: " << scrBaseDir << endl;

  getline(infile, line);
  templine=StringTools::newCleanString(line);
  runName=StringTools::trimRight(templine);
  runName+=nstr;
  string runends = StringTools::int2str(runend,3,"0");
  runName+="."+runends;
  cout <<"   -run name set to: " << runName << endl;


  runName0 = StringTools::int2str(runNum,4,"0")+"."+StringTools::int2str(runend,4,"0");


  scrdir=scrBaseDir+runName;

  infile.close();
  //cout << "  -Finished initializing QCHEM info" << endl;
 

  char* qcPath;
  qcPath = getenv ("QCSCRATCH");
//  if (qcPath!=NULL)
//    printf (" QCSCRATCH is: %s \n",qcPath);
#if QCHEMSF
  if (qcPath==NULL)
  {
    printf(" couldn't find QCSCRATCH environmental variable \n");
    printf(" EXITING Early! \n");
    exit(-1);
  }
#else
  printf(" shouldn't be here \n");
  exit(1);
//  qcPath = "none";
#endif
  string qcPaths(qcPath);

  fileloc = qcPaths+"/";
//  fileloc = "scratch/";
  nstr=StringTools::int2str(run,4,"0");
  qcinfile=fileloc+"qcin"+nstr+runends;
  qcoutfile=fileloc+"qcout"+nstr+runends;
  qcoutfileh="scratch/hess"+nstr+".xyz";
//  cout << " qcinfile: " << qcinfile << endl;
//  cout << " qcoutfile: " << qcoutfile << endl;

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

double QChemSF::calc_grads(double* coords)
{
  //printf(" qcg"); fflush(stdout);

  int badgeom = check_array(3*natoms,coords);
  if (badgeom)
  {
    printf(" ERROR: Geometry contains NaN, exiting! \n");
    exit(-1);
  }

  if (ncpu<1) ncpu = 1;

  //cout << "started QChem::grads" << endl;
  for (int i=0;i<3*natoms;i++)
    grad1[i] = grad2[i] = grad3[i] = grad4[i] = 0.;

  int num,k,c;
  double V = -1.;

  string runends=StringTools::int2str(runend,3,"0");
  string nstr=StringTools::int2str(runNum,4,"0");
  string molname = fileloc+"molecule"+nstr+runends;
//  string molname = scrBaseDir+"molecule"+nstr;
//  string molname = "scratch/molecule"+nstr;
  ofstream geomfile(molname.c_str());
//  cout << " geomfile " << molname << endl;

  // print the molecule coordinate section
  for(int j=0;j<natoms;j++)
  {
    geomfile << setw(2) << anames[j];
    geomfile << setw(16)<< coords[3*j+0];
    geomfile << setw(16)<< coords[3*j+1];
    geomfile << setw(16)<< coords[3*j+2] << endl;
  }
  geomfile.close();
 
  // system command here to process XYZ
  //cout << "about to call ./gscreate" << endl;
  nstr=StringTools::int2str(runNum,4,"0");
  string cmd = "./gscreate "+nstr+runends;
  system(cmd.c_str());
  if (!firstrun)
  {
    cmd = "mv "+qcoutfile+" "+qcoutfile+"_prev";
    system(cmd.c_str());
  }
  else firstrun = 0;

  string calc_command;
  nstr=StringTools::int2str(ncpu,4,"0");
  string nstrc = StringTools::int2str(ncpu,1,"0");
  if (ncpu==1)
    calc_command="cd "+fileloc+"; qchem -save ";
  else
#if THREADS_ON
    calc_command="cd "+fileloc+"; qchem -nt "+nstrc+" -save ";
#else
    calc_command="cd "+fileloc+"; qchem -np "+nstrc+" -save ";
#endif
  calc_command=calc_command+qcinfile;
  calc_command=calc_command+" "+qcoutfile;
  calc_command=calc_command+" "+runName+" ";
  int length2=StringTools::cleanstring(calc_command);
  
  //cout << calc_command.c_str() << endl;
  system(calc_command.c_str());
  //fflush(stdout);


  FILE *goutfile;

  string qcoutfilename = qcoutfile;
  ifstream qcfile;
  qcfile.open(qcoutfilename.c_str());

  string test = "SCF failed to converge";
  string test2 = "Target singlet state not found";
  string tdenergy = "Total energy for state";

  string line;
  int getgrad = 1;
  if (!qcfile)
  {
    printf(" failed to open qcout file \n");
    getgrad = 0;
  }
  while (getline(qcfile, line) && getgrad)
  {
    if (line.find(test)!=string::npos)
    {
      printf(" SCF failure \n");
     // cout << " skipping node for now " << endl;
      getgrad = 0;
      V = 999;
      break;
    }
    if (line.find(test2)!=string::npos)
    {
      printf("  need to increase CIS_N_ROOTS \n");
      exit(1);
     // cout << " skipping node for now " << endl;
      getgrad = 0;
      V = 999;
      break;
    }
  }
  qcfile.close();

  gradcalls++;

  //printf(" qcge"); fflush(stdout);

  double Eground = get_energy();
  get_grads();
  V = getE(0);

  return V * 627.5;
}

double QChemSF::get_energy() 
{

  string oname = qcoutfile;
  ifstream output(oname.c_str(),ios::in);
  if (!output) { printf(" error opening Q-Chem file: %s \n",oname.c_str()); return 10000.; }
  string line;
  vector<string> tok_line;
  energy = 0;
  int nf = 0;
  while(!output.eof()) 
  { 
    getline(output,line);
//    cout << " RR " << line << endl;
    if (line.find("Total energy in the final basis set")!=string::npos)
    {
      //cout << "  DFT out: " << line << endl;
      tok_line = StringTools::tokenize(line, " \t");
      energy=atof(tok_line[8].c_str());
    }
    if (line.find("State Energy is")!=string::npos)
    {
      //cout << "  DFT out: " << line << endl;
      tok_line = StringTools::tokenize(line, " \t");
      E[nf] = atof(tok_line[5].c_str());
      nf++;
    }
  }
 
  if (abs(energy)<0.00001 || (energy != energy))
  {
    printf(" energy zero, DFT failed \n");
    return 10000.;
  }

  output.close();

  return energy;
}

double QChemSF::getE(int ws)
{
  return E[ws]*627.5;
}
 
void QChemSF::getGrad(int ws, double* grads)
{
  double*    gradn = grad1;
  if (ws==1) gradn = grad2;
  if (ws==2) gradn = grad3;
  if (ws==3) gradn = grad4;
  for (int i=0;i<3*natoms;i++)
    grads[i] = gradn[i];

  return;
}

void QChemSF::get_grads()
{
  ifstream gradfile;
  gradfile.open(qcoutfile.c_str());
  if (!gradfile)
  {
    printf(" Error opening gradient file! %s \n",qcoutfile.c_str());
    return;
  }

  string line;
  bool success = true;

  int wg = 0;
  while (!gradfile.eof())
  {
    success=getline(gradfile, line);
    if (line.find("GSM-formatted gradient")!=string::npos)
    {
      double* gradn = grad1;
      if (wg==1) gradn=grad2;
      if (wg==2) gradn=grad3;
      if (wg==3) gradn=grad4;
      wg++;

      for (int i=0;i<natoms;i++)
      {
        success=getline(gradfile, line);
        //cout << "RR " << line << endl;
        int length=StringTools::cleanstring(line);
        vector<string> tok_line = StringTools::tokenize(line, " \t");
        gradn[3*i+0] = atof(tok_line[0].c_str())*(ANGtoBOHR);
        gradn[3*i+1] = atof(tok_line[1].c_str())*(ANGtoBOHR);
        gradn[3*i+2] = atof(tok_line[2].c_str())*(ANGtoBOHR);
      }
    }
  }

#if 0
  cout << " gradients: " << endl;
  for (int i=0;i<natoms;i++) 
    cout << grad1[3*i+0] << " " << grad1[3*i+1] << " " << grad1[3*i+2] << endl;
  printf("\n");
  for (int i=0;i<natoms;i++) 
    cout << grad2[3*i+0] << " " << grad2[3*i+1] << " " << grad2[3*i+2] << endl;
  printf("\n");
  for (int i=0;i<natoms;i++) 
    cout << grad3[3*i+0] << " " << grad3[3*i+1] << " " << grad3[3*i+2] << endl;
  printf("\n");
  for (int i=0;i<natoms;i++) 
    cout << grad4[3*i+0] << " " << grad4[3*i+1] << " " << grad4[3*i+2] << endl;
  printf("\n");
#endif

  gradfile.close();

  return;
}


void QChemSF::write_xyz_grad(double* coords, double* grad, string filename)
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
