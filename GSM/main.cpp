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
 * structure package and MKL. The TEST folders also provide a "template" for performing calculations
 * i.e. it has the requisite input files necessary for the calculation. 
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
 * \page page2 Double-Ended GSM
 *   \tableofcontents
 *\section sec Description
 *		DE GSM grows and optimizes a string of molecular geometries called nodes between a
 *		reactant and product pair and finds the exact transition state (TS) structure between them.
 *		This is done in two phases called the growth and optimization 
 *		phase. The growth phase grows the string of nodes from the outside-in, and performs 
 *		initial optimization. The optimization phase performs extensive optimization on the nodes
 *		and when the string is sufficiently optimized and behaving,
 *		the node with  the highest energy is optimized to the exact transition state using climbing
 *		image and eigenvector following optimization. The driver for both DE and SE methods and growth and 
 *		optimization phases is the GString#String_Method_Optimization. 
 *		
 *\subsection subsection1 Growth Phase
 *		The growth phase begins in GString#starting_string which grows two nodes, one on 
 *		the reactant side and one on the product side along the tangent vector connecting
 *		the reactant product pair. The new nodes can be optimized subject to the tangent vector
 *		constraint, and a new tangent vector formed between them. Two more nodes can be 
 *		added and the process repeated until the number of nodes requested are formed.
 *		A description of how delocalized internal coordinates, are formed and used to add nodes
 *		and perform constrained optimization is given in \ref page1. 
 *		
 *		The driver for adding nodes (GString#addNode) and node optimization (GString#opt_steps) 
 *		during the growth phase is GString#growth_iters.
 *		GString#opt_steps sends each active node to be optimized in ICoord#opt_c. 
 *		In DE the added nodes are active, wheras in SE only 
 *		frontier nodes are active (see \ref page3). The ICoord#opt_c function optimizes the node
 *		subject to the constraint, formed in ICoord#opt_constraint. Each node is optimized
 *		at least one time and at most
 *		three times. If the node converges to within GString#CONV_TOL it will optimize less than three times. The maximum 
 *		number of iterations are hard-coded 
 *		for DE but can be changed via the inpfileq for SE. ICoord#opt_c uses ICoord#update_ic_eigen
 *		to take Newton-Raphson Steps in the 3N-7 degrees of freedom corresponding to the nuclear 
 *		space orthogonal to the constraint vector. GString#ic_reparam_g repositions the nodes evenly
 *		along the string by adjusting the position of the nodes along the constraint vector.
 *
 *\subsection subsection2 Optimization Phase
 *	GString#opt_iters is the driver for optimizing the string of nodes subject to the constraint 
 *	vectors and for finding the exact TS structure.  
 *	Finding the exact TS structure is done in two steps that locates the TS structure efficiently
 *	and eliminates the possibility the TS search finds the wrong TS structure.
 *	The first step is the climbing image and the second step is a modified eigenvector following 
 *	optimization. 
 *
 *	Climbing image allows the node corresponding the TS structure to move upward in energy towards
 *	the maximum using the component of the gradient that is in the direction of the reaction path. 
 *	The maximum along the RP is a better structure for a local TS search.
 *	The local TS search is a modified eigenvector following approach. While
 *	conventional eigenvector following will ascend along the lowest
 *	eigenvalue vector of the Hessian, this criterion can be modified
 *	for increased stability since the approximate reaction path
 *	direction is known in GSM. To ensure that the correct mode is
 *	followed, the eigenvector of the Hessian with the highest
 *	overlap with the string tangent at the TS is targeted.
 *	Once the exact TS search commences, string optimization
 *	continues to proceed as usual. The TS node itself is not
 *	reparameterized, but equal spacing of nodes on either side of
 *	the string is still enforced (GString#ic_reparam). Because the maximized eigenvector of the TS node must overlap 
 *	with the reaction path tangent, the TS node remains effectively constrained within the
 *	reaction path. 
 *
 *	Climbing image will be activated once the totalgrad < 0.3 and one peak is found
 *	(GString#find_peaks), and GString#climb will equal 1. When GString#opt_steps is called 
 *	the function GString#get_tangents_1e will form a three-way tangent on the TS node. This forms a
 *	more accurate RP direction. 
 *	The function which performs the climbing image is ICoord#walk_up, and is called within
 *	ICoord#update_ic_eigen_h.
 *
 *  The local TS search will commence once the gradient of the TS node is small, and GString#find 
 *  will equal 1. 
 *	GString#get_eigenv_finite forms the eigenvectors corresponding to the imaginary frequencies.
 *	The eigenvector corresponding to the greatest overlap with the RP is used in the local TS 
 *	search as described above.
 *  The function ICoord#opt_eigen_ts is responsible for performing the local TS search.
 *	Convergence of the reaction path is
 *	considered complete when the TS node has a small root
 *	mean squared (RMS) gradient of GString#CONV_TOL (usually 0.0005 hartree/Å). The same
 *	RMS gradient threshold is used to temporarily cease node
 *	optimization during each string iteration.
 *
 **/
/*! \page page3 Single-Ended GSM
 *   Even more info.
 *        For more info see page \ref page2.
 *   */
	

