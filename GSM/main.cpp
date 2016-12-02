/*!
 * \mainpage Growing String Method
 *
 * The Growing String Method (GSM) is a reaction path (RP) and transition state (TS) finding tool.
 * GSM is utilized in two main fashions, double-ended (DE) and single-ended (SE).
 * DE requires a reactant and product pair, wheras SE only requires a reactant and a driving
 * coorindate. The driving coordinate are internal coordinates (angles, bonds, and torsions) 
 * corresponding to the assumed ideal reaction coordinate. 
 *
 * GSM is written in C++11, and requires the Intel C++ Composer XE 2013 and higher
 * as well as the MKL library. Furthermore, GSM requires an electronic structure package that is 
 * properly sourced, and available at the command line. The following electronic structure packages
 * are implemented with GSM:
 * 	- QChem
 * 	- ORCA
 * 	- GAUSSIAN
 * 	- Mopac
 * 	- Molpro
 * 
 * For more information, check out the wiki page:
 * https://github.com/ZimmermanGroup/molecularGSM/wiki
 *
 * We recommend running CTest to check that the executable was linked properly to the electronic
 * structure package and MKL. The TEST folders also provide a "template" for performing calculations.
 *
 *
 * \version 1.0
 * \date 2016-12-1
 * \copyright MIT Licence
 */
#include <iostream>
#include <fstream>
#include <stdio.h>

#include "gstring.h"


using namespace std;

int main(int argc, char* argv[]){
  string inpfile;
  string xyzfile;
  string nprocs;
  switch (argc){
  case 1:
    inpfile="inpfileq";
    xyzfile="initial.xyz";
    nprocs="1";
    break;
  case 2:
    inpfile="inpfileq";
    xyzfile=argv[1];
    nprocs="1";
    break;
  case 3:
    inpfile="inpfileq";
    xyzfile=argv[1];
    nprocs=argv[2];
    break;
  default:
    cout << "Invalid command line options." << endl;
    return -1;
  }

  int nnprocs = atoi(nprocs.c_str());
  printf(" Number of QC processors: %i \n",nnprocs);
  int name = atoi(xyzfile.c_str());
  GString gstr;
  gstr.init(inpfile, name, nnprocs);
  gstr.String_Method_Optimization();


  return 0;
}


/*! \page page1 Double-Ended 
 *   \tableofcontents
 *       \section sec Description
 *     		The DE GSM grows and optimizes a string of molecular geometries called nodes between a reactant and product pair. 
 *     		This is done in two phases called the growth and optimization phase. 
 *     		Once the string is sufficiently optimized and is behaving, the node with the highest energy is optimized to the exact
 *     		transition state using climbing image and an exact eigenvector optimization. 
 *     		The main driver for DE and SE, growth and optimization is the GString#String_Method_Optimization
 *          \subsection subsection1 Growth Phase
 *          		The growth phase begins in GString#starting_string which calls GString#get_tangents that forms 
 *          		an internal coordinate tangent vector connecting the reactant and product.
 *          		This tangent vector is a linear interpolation vector connecting the pair.
 *          		In the growth phase the tangent vectors are referenced to left and right respectively. 
 *              Then starting_string grows two nodes, one on the reactant side and one on the product side 
 *              along the tangent vectors. 
 *              
 *           \subsection subsection2 Optimization Phase
 *                More text.
 *             */
/*! \page page2 Single-Ended
 *   Even more info.
 *        For more info see page \ref page1.
 *   */
	

