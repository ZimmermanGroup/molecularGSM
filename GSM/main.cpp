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
 * Common build errors are 
 * 1. INTEL is not loaded or MKLROOT is not set as an environment variable.
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
 * \section Overview
 *Delocalized internal coordinates are fundamental to the GSM because they
 *enable direct and efficient constrained optimization. 
 *Furthermore, delocalized internal coordinates are a more natural 
 *coordinate system with less coupling in the Hessian
 *because they are linear combinations of bonds, angles and torsions.
 *Cartesian coordinates, on the other hand, are highly coupled coordinate 
 *system.
 *On this page we will describe the generation of delocalized internal
 *coordinatess, and how they are used in the GSM. 
 *	
 * 		\subsection secg Generation 
 * 		GString#structure_init reads the xyzfile for the atom names and
 * 		Cartesian coordinates.
 *		ICoord#alloc allocates memory and ICoord#reset sets the coordinates and atom information
 *		in the ICoord class.
 *		To construct delocalized internal coordinates, we require a complete set of 
 *		internal coordinates. Internal coordinates are bonds, angles, and 
 *		torsions and as such can be measured using atom-connectivity.
 *		The function responsible for forming the internal coorindates is 
 *		ICoord#ic_create.
 *		In DE GSM, ICoord#union_ic forms the union of reactant and product
 *		internal cooridnates as the full set of internal coordinates.
 *		The linear transformation describing the transformation from Cartesian
 *		coordinates to internal coordinates is the Wilson B matrix. 
 *		ICoord#bmat_alloc allocates memory for the B matrix, delocalized internal
 *		coordinates U, and other important variables, and ICoord#bmatp_create 
 *		creates the B matrix. 
 *		The function ICoord#bmatp_to_U diagonalizes G=B<SUP>T</SUP>B to provide two sets of
 *		eigenvectors; a set of m=3N-6 eigenvectors with eigenvalues &lambda;>0 
 *		called U and a set of n-m eigenvectors with eigenvalues &lambda;=0 (to 
 *		numerical precision) called R.
 *		The set U exactly spans the 3N-6 molecular
 *		space and are therefore a non-redundant coordinate system. 
 *		The coordinates U are saved as the transpose ICoord#Ut. 
 * 		The component of each coordinate (ICoord#q) in U is formed in
 * 		ICoord#bmat_create. Additionally, the B matrix—which was constructed
 *		from the original primitive internals—is transformed to the active
 *		coordinate set according to B=(U<SUP>T</SUP>)B<SUP>p</SUP>.
 *		This active B matrx is used by ICoord#grad_to_q to transform a gradient
 *		calculated in Cartesian coordinates, to a gradient described in 
 *		delocalized internal coordinates. The back transformation from 
 *		 delocalized internal coordinates to Cartesian coordinates
 *		also uses the B matrix, and is performed in ICoord#ic_to_xyz and
 *		ICoord#ic_to_xyz_opt.
 * 		
 * 		\subsection secI Constrained Optimization and Linear Interpolation
 *    One of the main advantages of delocalized internal coordinates is their
 *    ease of use for constrained optimization and linear interpolation.
 *		Linear interpolation and constrained optimization begins by forming an 
 *		internal coordinate constraint vector. In DE GSM, the
 *		constraint vector is the tangent vector between reactant and product, and
 *		is calculated by GString#tangent_1. 
 *		In SE GSM the constraint vector is the driving coordinate scaled appropriately, 
 *		and is calculated by GString#tangent_1b.
 *		The internal coordinate constraint vector is projected into the coordinate
 *		space defined by the primitive internal coordinates. A set V containting 
 *		U and the constraint vector is formed. This set is redundant because it contains
 *		the 3N-6 non-redundant set U and the constraint vector. Therefore, it can be 
 *		Schmidt orthogonalized to make the vectors U and the constraint vector orthogonal
 *		and the space non-redundant. 
 *		This procedure is performed in ICoord#opt_constraint.
 *		The new non-redundant molecular space obtained from ICoord#opt_constraint contains
 *		3N-7 adjustable degrees of freedom
 *		and one degree of freedom corresponding to the constraint vector. The constraint
 *		vector can either be kept constant to perform a constrained optimization, or incremented
 *		to linearly interpret along the constraint vector. Both are done in the GSM. 
 *		Constrained optimization is performed in ICoord#opt_c, and linear interpolation
 *		is performed when adding nodes (GString#starting_string and GString#addNode), when 
 *		reparametrizing the string (GString#ic_reparam_g and GString#ic_reparam), and when
 *		performing climbing image (ICoord#walk_up).
 *
 *
 * 		\subsection ref1 References
 * 		
 * 		Baker, Jon and Kessi, Alain and Delley, Bernard, "The generation and use of delocalized internal coordinates in geometry optimization" The Journal of Chemical Physics, 105, 192-212 (1996), DOI:http://dx.doi.org/10.1063/1.471864\n
 * 		http://scitation.aip.org/content/aip/journal/jcp/105/1/10.1063/1.471864
 *
 * \page page2 Double-Ended GSM
 *   \tableofcontents
 *\section sec Description
 *		DE GSM grows and optimizes a string of molecular geometries called nodes between a
 *		reactant and product pair and finds the exact transition state (TS) structure between them.
 *		This is done in two phases called the growth and optimization 
 *		phase. The growth phase grows the string of nodes from the outside-in, and performs 
 *		initial optimization. The optimization phase finishes optimizing the string
 *		and when the string is sufficiently optimized and behaving,
 *		the node with  the highest energy is optimized to the exact transition state using climbing
 *		image and eigenvector following optimization. The driver for both DE and SE methods and growth and 
 *		optimization phases is the GString#String_Method_Optimization. 
 *		
 *\subsection subsection1 Growth Phase
 *		The growth phase begins in GString#starting_string which grows two nodes along the tangent vector connecting
 *		the reactant product pair, one on the reactant side and the other on the product side.
 *		The new nodes can be optimized subject to the tangent vector
 *		constraint, and a new tangent vector formed between them. Two more nodes can be 
 *		added and the process repeated until the number of nodes requested are formed.
 *		A description of how delocalized internal coordinates, are formed and used to add nodes
 *		and perform constrained optimization is given in \ref page1. 
 *		
 *		The driver for adding nodes (GString#addNode) and node optimization (GString#opt_steps) 
 *		during the growth phase is GString#growth_iters.
 *		GString#opt_steps sends each active node to be optimized in ICoord#opt_c. 
 *		In DE the added nodes are active, whereas in SE only 
 *		frontier nodes are active (see \ref page3). The ICoord#opt_c function optimizes the node
 *		subject to the constraint, formed in ICoord#opt_constraint. Each node is optimized
 *		at least one time and at most
 *		three times. If the node converges to within GString#CONV_TOL it will optimize less than 
 *		three times. The maximum number of iterations are hard-coded 
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
 *	Climbing image will be activated (GString#climb will equal 1) once the totalgrad < 0.3 and one peak is found
 *	(GString#find_peaks). When GString#opt_steps is called 
 *	the function GString#get_tangents_1e will form a three-way tangent on the TS node. This forms a
 *	more accurate RP direction for climbing image and approximating the Hessian.
 *	The function which performs the climbing image is ICoord#walk_up, and is called within
 *	ICoord#update_ic_eigen_h.
 *
 *  The local TS search (GString#find = 1) will commence once the gradient of
 *  the TS node is small, the totalgrad is small, and the gradient in the 
 *  direction of the RP at the TS node is smalll.
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
 *	\subsection ref2 References:
 *	P. M. Zimmerman, “Reliable Transition State Searches Integrated with the Growing String Method,” Journal of Chemical Theory and Computation, 9, 3043-3050 (2013)\n
 *	http://pubs.acs.org.proxy.lib.umich.edu/doi/full/10.1021/ct400319w\n
 *
 * \page page3 Single-Ended GSM
 * \tableofcontents
 * \section gen Overview
 * 	SE operates using the same drivers as DE (GString#growth_iters and GString#opt_iters),
 * 	but has some differences that are particular to SE.
 * 	Some of the differences have already been discussed in \ref page1 and \ref page2.
 * 	On this page we will further highlight the differences.
 * 	
 * 	\subsection sec3 General Description and Differences
 * 	Unlike DE, which requires a reactant and a product, SE only requires the reactant and 
 *  driving coordinates which are internal coordinates e.g. add/break bond. Therefore,
 *  SE can be used to search for pruducts that haven't been found yet. 
 * 	But because SE doesn't have a product structure, it can't form the internal coordinate 
 * 	tangent in the same way as DE. Instead, SE uses the GString#tangent_1b function to form the
 * 	constrain vector using the driving coordinate .
 * 	As the reaction proceeds,the tangent vector is scalled appropriately using atom-connectivity 
 * 	measurements. 
 * 	Another differences between SE and DE is that SE only optimizes the frontier node during the growth phase
 * 	and doesn't undergo string reparametrization until the string is done growing. Nodes are set as active
 * 	using GString#set_fsm_active and the variable GString#nnR keeps track of the reactant node. 
 *
 * 	Besides those differences, SSM must keep careful track of how the string is being grown because the 
 *  number of nodes it uses is not constant like DE and it needs to detect when it has passed a TS to finish growing.
 *	In GString#growth_iters, the function GString#past_ts checks if the string has grown
 *	over the TS. GString#opt_iters checks if the TS node is the second to last node and will 
 *	add another node to the string. The function GString#addCNode, will add a node if more space 
 *	between the TS and product is needed, i.e. if the distance from the TS to product requires more than
 *	two nodes for the overall string spacing to remain even. 
 *	When the product node is found, it undergoes optimization using ICoord#opt_b. 
 *	After the string is fully grown it behaves identically to DE because the string is fully grown and the 
 *	product is optimized. 
 *
 * 	\subsection ref3 References:
 * 	 P. M. Zimmerman "Single-ended transition state finding with the growing string method" J. Comput. Chem. 2015, 36, 601–611. DOI: 10.1002/jcc.23833\n
 * 	 http://onlinelibrary.wiley.com/doi/10.1002/jcc.23833/abstract
 **/
	

