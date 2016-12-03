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
 * @PROJECT_NUMBER
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


/*!
 * \page page1 Delocalized Internal Coordinates 
 *     	Among growing string methods ours is particularly 	efficient and robust because it takes advantage of delocalized internal coordinates. 
 *          coordinates that define the  R/P are delocalized internal coordinates which are 
 *          non-redundant. The Gstring#opt_constraint function Schmidt orthogonalizes the 
 *          delocalized internal coordinates against ictan in order to form a new set of 
 *          delocalized internal coordinates that contain ictan as its last vector. 
 *          Therefore, by incrementing the component of the last vector in the new set the
 *           molecular geometry is distorted in the direction of the ictan vector. 
 *     		
 *
 * \page page2 Double-Ended 
 *   \tableofcontents
 *       \section sec Description
 *     		DE GSM grows and optimizes a string of molecular geometries called nodes between a
 *     		reactant and product pair. This is done in two phases called the growth and optimization 
 *     		phase. Once the string is sufficiently optimized and is behaving, the node with  
 *     		the highest energy is optimized to the exact transition state using climbing image and
 *     		eigenvector optimization. The driver for DE and SE methods, growth and optimization is 
 *     		GString#String_Method_Optimization. 
 *        \subsection subsection1 Growth Phase
 *          During the growth phase Gstring#starting_string grows two nodes, one on 
 *          the reactant side and one on the product side along the tangent vector connecting
 *          the reactant product pair. 
 *          The new nodes can be optimized subject to the tangent vector (constraint), and a new 
 *          tangent vector formed between them. Two more nodes can then be added and the process 
 *          repeated until the number of nodes requested are formed.
 *          A description of how nodes are added and optimized is give in \ref page1
 *
 *          The driver for node optimization and adding nodes during the growth phase is 
 *          GString#growth_iters. 
 *          Overall, it handles adding new nodes 
 *          (GString#addNode), sending nodes to be optimized,
 *          (GString#opt_steps), forming tangents vectors, and reparametrizing node spacing 
 *          (GString#ic_reparam_g). 
 *
 *
 *          GString#opt_steps optimizes each added node at least one time and at most three times.
 *
 *          
 *        \subsection subsection2 Optimization Phase
 *                More text.
 *             */
/*! \page page3 Single-Ended
 *   Even more info.
 *        For more info see page \ref page2.
 *   */
	

