#include "qchem.h"

using namespace std;

#define COPY_GRAD 0

//NOTE: .qcin file still in gfstringq.exe directory
//either change qchem/parallel.csh or cd in


void QChem::alloc(int natoms0)
{
  natoms = natoms0;
  anumbers = new int[natoms+1];
  anames = new string[natoms+1];

  return;
}

void QChem::init(string infilename, int natoms0, int* anumbers0, string* anames0, int run, int rune)
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
#if QCHEM
  if (qcPath==NULL)
  {
    printf(" couldn't find QCSCRATCH environmental variable \n");
    printf(" EXITING Early! \n");
    exit(-1);
  }
#else
  qcPath = "none";
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

int QChem::read_hess(double* hess)
{
  printf("\n ERROR: entering untested function \n");
  exit(1);

  int success = 1;
  int N3 = 3*natoms;

  string qcoutfilename = qcoutfileh;
  ifstream qcfile;
  qcfile.open(qcoutfilename.c_str());

  int reading = 0;
  int cont = 1;
  string line;
  if (!qcfile)
  {
    printf(" failed to open qcout file \n");
    return 0;
  }
  while (getline(qcfile, line) && cont)
  {
    if (line.find("Hessian of the SCF Energy")!=string::npos)
    {
      reading = 1;    
      cont = 0;
      break;
    }
  }
  int n = 0;
  while (!qcfile.eof())
  {
    n++;
    getline(qcfile,line);
    for (int j=0;j<N3;j++)
    {
      getline(qcfile,line);
    // cout << " RR: " << line << endl;
      int length=StringTools::cleanstring(line);
      vector<string> tok_line = StringTools::tokenize(line, " \t");
      for (int k=1;k<tok_line.size();k++)
        hess[N3*j+6*(n-1)+k-1]=atof(tok_line[k].c_str());
    }
    if (n*6>N3) break;
  }
  qcfile.close();

#if 1
  printf("  printing Hessian \n");
  for (int i=0;i<N3;i++)
  {
    for (int j=0;j<N3;j++)
      printf(" %10.6f",hess[N3*i+j]);
    printf("\n");
  }
#endif
  return success;
}

double QChem::grads(double* coords, double* grad)
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
    grad[i] = 0.;

  int num,k,c;
  double V = -1.;

#if 0
  string rmqcin="rm -f ";
  rmqcin=rmqcin+qcinfile;
  system(rmqcin.c_str());
#endif

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


  string gradfile = "scratch/GRAD"+nstr;
#if COPY_GRAD
  string gradcopy_command;
  gradcopy_command="cp ";
#if 0
  if (ncpu>1)
    gradcopy_command=gradcopy_command+scrdir+".0";
  else
#endif
    gradcopy_command=gradcopy_command+scrdir;
  nstr=StringTools::int2str(runNum,4,"0");
  gradcopy_command=gradcopy_command+"/GRAD scratch/GRAD"+nstr;
  //cout << " gradcopy_command: " << gradcopy_command << endl;
  int length=StringTools::cleanstring(gradcopy_command);
  system(gradcopy_command.c_str());
  fflush(stdout);
#else
  gradfile=scrdir+"/GRAD";
#endif

  //printf(" qcg1"); fflush(stdout);
  //sleep(1);
  FILE *goutfile;

  string qcoutfilename = qcoutfile;
  ifstream qcfile;
  qcfile.open(qcoutfilename.c_str());

  string test = "SCF failed to converge";

  string line;
  int getgrad = 1;
  if (!qcfile)
  {
    printf(" failed to open qcout file \n");
    getgrad = 0;
  }
  while (getline(qcfile, line) && getgrad)
  {
    if (StringTools::contains(line, test))
    {
      cout << " SCF failed,";
     // cout << " skipping node for now " << endl;
      getgrad = 0;
      V = 999;
    }
  }


  //printf(" qcg2"); fflush(stdout);
  if (getgrad)
  {
    int success = scangradient(gradfile, grad, natoms);

    goutfile = fopen(gradfile.c_str(),"r");
    if(goutfile == NULL)
    {
      //printf("\n Error opening QChem output file: GRADxx ");
      nscffail++;
      V = 999.;
    }
    else if (success>-1)
    {
  //    V = scanenergy(goutfile);
      V = get_energy(qcoutfilename);
      fclose(goutfile);
    }
    else if (success==-1)
    {
      nscffail++;
      V = 999.;
    }
  } //if getgrad
  else
    nscffail++;

  if (nscffail>25)
  {
    printf("\n\n Too many SCF failures: %i, exiting \n",nscffail);
    exit(1);
  }
  //printf(" done with Q-Chem \n");  

  gradcalls++;

  qcfile.close();

  //printf(" qcge"); fflush(stdout);

  return V * 627.5;
}

double QChem::get_energy(string filename) {

  string oname = filename;
  ifstream output(oname.c_str(),ios::in);
  if (!output) { printf(" error opening Q-Chem file: %s \n",oname.c_str()); return 10000.; }
  string line;
  vector<string> tok_line;
  energy = 0;
  while(!output.eof()) 
  { 
    getline(output,line);
//    cout << " RR " << line << endl;
    if (line.find("Total energy in the final basis set")!=string::npos)
    {
      //cout << "  DFT out: " << line << endl;
      tok_line = StringTools::tokenize(line, " \t");
      energy=atof(tok_line[8].c_str());
      break;
    }
  }
 
//  printf(" DFT energy: %1.4f \n",energy); 

  if (abs(energy)<0.00001 || (energy != energy))
  {
    printf(" energy zero, DFT failed \n");
    return 10000.;
  }

  output.close();

  return energy;
}


int QChem::scangradient(string file, double* grad, int natoms)
{

  //cout << "starting the scan of the gradient" << endl;
  ifstream gradfile;
  gradfile.open(file.c_str());
  if (!gradfile)
  {
    printf(" Error opening gradient file! %s \n",file.c_str());
    return -1;
  }

  string line;
  bool success = true;
  //cout << "reading gradient... " << endl;

  success=getline(gradfile, line);
  success=getline(gradfile, line);
  success=getline(gradfile, line);

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
    vector<string> tok_line = StringTools::tokenize(line, " \t");
    if (tok_line.size()<3)
    {
      printf(" missing data in GRAD(2) \n"); fflush(stdout);
      grad[3*i+0] = grad[3*i+1] = grad[3*i+2] = 1.;
    }
    else
    {
      grad[3*i+0]=atof(tok_line[0].c_str())*(ANGtoBOHR);
      grad[3*i+1]=atof(tok_line[1].c_str())*(ANGtoBOHR);
      grad[3*i+2]=atof(tok_line[2].c_str())*(ANGtoBOHR);
    }
  }
#if 0
  cout << " gradient: " << endl;
  for (int i=0;i<natoms;i++) 
    cout << grad[3*i+0] << " " << grad[3*i+1] << " " << grad[3*i+2] << endl;
#endif

  gradfile.close();

  return 0;
}


void QChem::write_xyz_grad(double* coords, double* grad, string filename)
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
