#include "turbomole.h"
#include <iostream> 
#include <limits> 

/* 
 * turbomole.cpp: a Turbomole interface for MGSM
 *
 * Notes: 
 * 1) The inpfileq has been adapted: A turboDIR path needs to be defined and RI and COSMO options need to be specified (check example inpfileq)!
 * 2) Turbomole-specific gscreate and gsm.qsh files need to be available! 
 * 3) for details check the EXAMPLE folders
 *
 */

using namespace std;

#define COPY_GRAD 0

void Turbomole::alloc(int natoms0)
{
  natoms = natoms0;
  anumbers = new int[natoms+1];
  anames = new string[natoms+1];

  return;
}

void Turbomole::init(string infilename, int natoms0, int* anumbers0, string* anames0, int run, int rune)
{
  gradcalls = 0;
  nscffail = 0;
  firstrun = 1;

  natoms = natoms0;
  anumbers = new int[natoms+1];
  anames = new string[natoms+1];

  for (int i=0;i<natoms0;i++)
    anumbers[i] = anumbers0[i];
   // cout <<"atom numbers:"  << anumbers0[i].c_str() <<endl;
  for (int i=0;i<natoms0;i++)
    anames[i] = anames0[i];
   // cout <<"atom names:"  << anames0[i].c_str() <<endl;


  runNum = run;
  runend = rune;
  string nstr = StringTools::int2str(run,4,"0");

  //cout << "  -Initializing Turbomole info ..." << endl;
  //cout << "  -opening inpfile to read Turbomole parameters" << endl;
  ifstream infile;
  infile.open(infilename.c_str()); //infile=inpfileq
  if (!infile){
    cout << "!!!!!Error opening inputfile!!!!" << endl;
    exit(-1);
  }

  //cout <<"  -reading file..." << endl;

#if TURBOMOLE
  printf("  Turbomole initialized \n");
#endif

  // pass infile to stringtools and get the line containing tag
  string tag="TURBOMOLE Scratch Info";
  bool found=StringTools::findstr(infile, tag);

  if (!found){
    cout << "!!!!Could not find tag for TURBOMOLE Info!!!!" << endl;
    exit(-1);
  }

  string line, templine, tagname;

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
  //runName=StringTools::trimRight(templine);
  //runName+=nstr;
  string runends = StringTools::int2str(runend,3,"0");
  //runName+="."+runends;
  //cout <<"   -run name set to: " << runName << endl;


  //runName0 = StringTools::int2str(runNum,4,"0")+"."+StringTools::int2str(runend,4,"0");

  getline(infile, line);
  vector<string> tok_line = StringTools::tokenize(line, " ,\t");
  templine=StringTools::newCleanString(tok_line[0]);
  tagname=StringTools::trimRight(templine);
  cout << "tagname: " <<tagname << endl;

  //bool RI, COSMO;
  if (tagname=="RI"){
     RI=atof(tok_line[1].c_str());
     cout <<" RI is set to: " << RI << endl;
  }
  else
     RI=0;

  getline(infile, line);
  tok_line = StringTools::tokenize(line, " ,\t");
  templine=StringTools::newCleanString(tok_line[0]);
  tagname=StringTools::trimRight(templine);
  cout << "tagname: " <<tagname << endl;

  if (tagname=="COSMO"){
     COSMO=atof(tok_line[1].c_str());
     cout <<" COSMO is set to: " << COSMO << endl;
  }
  else
     COSMO=0;

  getline(infile, line);
  tok_line = StringTools::tokenize(line, " ,\t");
  templine=StringTools::newCleanString(tok_line[0]);
  tagname=StringTools::trimRight(templine);
  cout << "tagname: " <<tagname << endl;

  if (tagname=="turboDIR"){
     turboDIR=tok_line[1].c_str();
     cout <<" turboDIR is set to: " << turboDIR << endl;
  }
  else
     turboDIR="";

// HERE

  scrdir=scrBaseDir+runName;
  cout << "scrdir: "<< scrdir <<endl;


  infile.close();
  //cout << "  -Finished initializing Turbomole info" << endl;
 

  char* turboPath;
  turboPath = getenv ("TMSCRATCH");
//    printf (" TMSCRATCH is: %s \n",turboPath);
#if TURBOMOLE
  if (turboPath==NULL)
  {
    printf(" couldn't find TMSCRATCH environmental variable \n");
    printf(" EXITING Early! \n");
    exit(-1);
  }
#else
  turboPath = "none";
#endif
  string turboPaths(turboPath);

  fileloc = turboPaths+"/";

  nstr=StringTools::int2str(run,4,"0");
  turboinfile=fileloc+"turboin"+nstr+runends;
  if (RI)
    turbooutfile=fileloc+"COORD_"+nstr+runends+"/ridft.out"; 
  else
    turbooutfile=fileloc+"COORD_"+nstr+runends+"/dscf.out";
  cout << "turbooutfile: " << turbooutfile <<endl;

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

double Turbomole::grads(double* coords, double* grad)
{
  //printf(" tmg"); fflush(stdout);
  //cout << "natoms: "<< natoms <<endl;

  int badgeom = check_array(3*natoms,coords);
  if (badgeom)
  {
    printf(" ERROR: Geometry contains NaN, exiting! \n");
    exit(-1);
  }

  //cout << "geometry is OK" << endl;

  if (ncpu<1) ncpu = 1;



  //cout << "started Turbomole::grads" << endl;
  for (int i=0;i<3*natoms;i++)
    grad[i] = 0.;

  int num,k,c;
  double V = -1.;

  string runends=StringTools::int2str(runend,3,"0");
  string nstr=StringTools::int2str(runNum,4,"0");
  string molname = fileloc+"molecule"+nstr+runends;

  cout << "molname: " << molname <<endl;
//  string molname = scrBaseDir+"molecule"+nstr;
//  string molname = "scratch/molecule"+nstr;
  ofstream geomfile(molname.c_str());

  cout << " geomfile " << molname << endl;
  cout << " natoms " << natoms << endl;

//  geomfile << setprecision(10); 

  // print the molecule coordinate section

  for(int j=0;j<natoms;j++)
  {
    cout << "COORDS:" << coords[3*j+0] <<"  "<< coords[3*j+1] <<"  "<< coords[3*j+2] <<endl;
    geomfile << setw(2) << anames[j];
    geomfile << setw(16)<< coords[3*j+0]; 
    geomfile << setw(16)<< coords[3*j+1];
    geomfile << setw(16)<< coords[3*j+2] << endl;
  }
  geomfile.close();
 
  // system command here to process XYZ
  cout << "about to call ./gscreate" << endl;
  nstr=StringTools::int2str(runNum,4,"0");
  string cmd = "./gscreate "+nstr+runends;
  system(cmd.c_str());
  if (!firstrun)
  {
    cmd = "mv "+turbooutfile+" "+turbooutfile+"_prev";
    system(cmd.c_str());
  }
  else
  {
    firstrun = 0;
    cmd = "cp "+turbooutfile+" "+turbooutfile+"_start";
    system(cmd.c_str());
  }
//  else firstrun = 0;

  string calc_command;
//  nstr=StringTools::int2str(ncpu,4,"0");
  string nstrc = StringTools::int2str(ncpu,1,"0");

  string cosmo_cmd;
  if (COSMO){
    string cosmo_filename = "cosmo_inp";
    ifstream cosmo_file;
    cosmo_file.open(cosmo_filename.c_str());
    if (!cosmo_file){
      cosmo_cmd="";
      }
    else
      cosmo_cmd="cosmoprep < cosmo_inp > cosmo.log;";
    }
  else
    cosmo_cmd="";

  cout << "RI: " << RI <<endl;

  if (ncpu==1)
   if (RI)
    calc_command="cd "+fileloc+"COORD_"+nstr+runends+"; "+cosmo_cmd+" mv gradient gradient_prev; "+turboDIR+"/ridft > ridft.out; "+turboDIR+"/rdgrad > rdgrad.out ";
   else
    calc_command="cd "+fileloc+"COORD_"+nstr+runends+"; "+cosmo_cmd+" mv gradient gradient_prev; "+turboDIR+"/dscf > dscf.out; "+turboDIR+"/grad > grad.out ";
  else
   if (RI)
    calc_command="cd "+fileloc+"COORD_"+nstr+runends+"; export PARNODES="+nstrc+"; export PARA_ARCH=SMP;"+cosmo_cmd+" mv gradient gradient_prev; "+turboDIR+"_smp/ridft_smp > ridft.out;  "+turboDIR+"_smp/rdgrad_smp > rdgrad.out ";
   else
    calc_command="cd "+fileloc+"COORD_"+nstr+runends+"; export PARNODES="+nstrc+"; export PARA_ARCH=SMP; "+cosmo_cmd+" mv gradient gradient_prev; "+turboDIR+"_smp/dscf_smp > dscf.out;  "+turboDIR+"_smp/grad_smp > grad.out ";

  int length2=StringTools::cleanstring(calc_command);
  
  cout << calc_command.c_str() << endl;
  system(calc_command.c_str());
  //fflush(stdout);

  string gradfile = "scratch/gradient"+nstr;
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
  gradcopy_command=gradcopy_command+"/gradient scratch/gradient"+nstr;
  cout << " gradcopy_command: " << gradcopy_command << endl;
  int length=StringTools::cleanstring(gradcopy_command);
  system(gradcopy_command.c_str());
  fflush(stdout);
#else
  gradfile=fileloc+"COORD_"+nstr+runends+"/gradient"; 
#endif

  //printf(" tmg1"); fflush(stdout);
  //sleep(1);
  FILE *goutfile;

  string turbooutfilename = turbooutfile; //ridft or dscf ...
  ifstream turbofile;
  turbofile.open(turbooutfilename.c_str());

  string test = "SCF failed to converge";

  string line;
  int getgrad = 1;

  cout << "now test whether turboout exists" <<endl;

  if (!turbofile)
  {
    printf(" failed to open turboout file \n");
    getgrad = 0;
  }
  while (getline(turbofile, line) && getgrad)
  {
    if (StringTools::contains(line, test))
    {
      cout << " SCF failed,";
     // cout << " skipping node for now " << endl;
      getgrad = 0;
      V = 999;
    }
  }


  cout << "turboout exists and now open gradient file" << endl;

  //printf(" tmg2"); fflush(stdout);
  if (getgrad)
  {
    int success = scangradient(gradfile, grad, natoms);

    goutfile = fopen(gradfile.c_str(),"r");
    if(goutfile == NULL)
    {
      //printf("\n Error opening Turbomole output file: gradient ");
      nscffail++;
      V = 999.;
    }
    else if (success>-1)
    {
  //    V = scanenergy(goutfile);
      cout << "now get energy" <<endl;
      V = get_energy(turbooutfilename);
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
  //printf(" done with Turbomole \n");  

  gradcalls++;

  turbofile.close();

  //printf(" tmge"); fflush(stdout);
 
  cout << "ENERGY [kcal/mol]:" << V*627.5 << endl;

  return V * 627.5;
}

double Turbomole::get_energy(string filename) {

  string oname = filename;
  ifstream output(oname.c_str(),ios::in);
  if (!output) { printf(" error opening Turbomole file: %s \n",oname.c_str()); return 10000.;} 
  string line;
  vector<string> tok_line;
  energy = 0;
  while(!output.eof()) 
  { 
    getline(output,line);
    //cout << " RR " << line << endl;
    if (line.find("total energy      =")!=string::npos)
    {
      cout << "  DFT out: " << line << endl;
      tok_line = StringTools::tokenize(line, " \t");
      energy=atof(tok_line[4].c_str());
      cout << "ENERGY [hartree]: " <<energy <<endl;
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


int Turbomole::scangradient(string file, double* grad, int natoms)
{

  cout << "starting the scan of the gradient" << endl;
  ifstream gradfile;
  int iter=0; 

  gradfile.open(file.c_str());
  if (!gradfile)
  {
    printf(" Error opening gradient file! %s \n",file.c_str());
    return -1;
  }

  string line;
  bool success = true;
  cout << "reading gradient from... " << file.c_str() << endl;

  success=(bool)getline(gradfile, line);
  success=(bool)getline(gradfile, line);
  success=(bool)getline(gradfile, line);

  //for (int i=0;i<natoms;i++)
  for (int i=0;i<natoms*2+1;i++)
  {
    if(gradfile.eof()) // and not natom x gradient eintraege; file=gradient
    {
      printf(" missing data in gradient(1) \n"); fflush(stdout);
      grad[3*i+0] = grad[3*i+1] = grad[3*i+2] = 1.;
      break;
    }
    success=(bool)getline(gradfile, line);
   // cout << "RR " << line << endl;
    int length=StringTools::cleanstring(line);
    vector<string> tok_line = StringTools::tokenize(line, " \t");
   // if (tok_line.size()<3)
    if (tok_line.size()<3 or tok_line.size()>3 )
    {
      continue;
//      printf(" missing data in gradient(2) \n"); fflush(stdout);
     // grad[3*iter+0] = grad[3*iter+1] = grad[3*iter+2] = 1.;
    }
    else
    {
      string temp1=tok_line[0].c_str();
      temp1.replace(16, 1, "E");
      string temp2=tok_line[1].c_str();
      temp2.replace(16, 1, "E");
      string temp3=tok_line[2].c_str();
      temp3.replace(16, 1, "E");
      grad[3*iter+0]=atof(temp1.c_str())*(ANGtoBOHR);
      grad[3*iter+1]=atof(temp2.c_str())*(ANGtoBOHR); //conversion from H/Bohr to H/Angstrom!
      grad[3*iter+2]=atof(temp3.c_str())*(ANGtoBOHR);
    //  cout << "GRADIENT_" << grad[3*i+0] << " "<< grad[3*i+1] << " " << grad[3*i+2] << endl;
      iter++;
    }
  }
  cout << "GRADIENT" <<endl;
  //cout.precision(6);
  for (int i=0;i<iter;i++)
  {
    cout << grad[3*i+0] << " "<< grad[3*i+1] << " " << grad[3*i+2] << endl;
  }
#if 0
  cout << " gradient: " << endl;
  for (int i=0;i<natoms;i++) 
    cout << grad[3*i+0] << " " << grad[3*i+1] << " " << grad[3*i+2] << endl;
#endif

  gradfile.close();

  return 0;
}


void Turbomole::write_xyz_grad(double* coords, double* grad, string filename) 
{

  cout << "in write_xyz_grad" <<endl;
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

  cout << "gradfile: " << gradfile_string << endl;

  gradfile << natoms << endl << energy << endl;
  for (int i=0;i<natoms;i++)
    gradfile << anames[i] << " " << grad[3*i+0] << " " << grad[3*i+1] << " " << grad[3*i+2] << endl;

  gradfile.close();

  return;
}
