##Questions?
Contact Cody Aldaz, email: craldaz@umich.edu

## Overview
The growing string method is a reaction path and transition state finding method developed in c++.

For more information, check out the wiki page:
https://github.com/ZimmermanGroup/molecularGSM/wiki

Sample tutorial files can be found under the tutorial folder:
https://github.com/ZimmermanGroup/molecularGSM/tree/master/tutorial

## Installation
To compile:

1. Set FC and LINKERFLAGS in Makefile
 --I have used Intel 12 and 13 compilers 
2. type: make
3. copy gfstringq.exe to run directory
4. copy nwchem.py to your ase/calculators/ directory


To run gfstringq.exe:

1. Setup grad.py by setting scratch directory to local scratch
 --default is $PBSTMPDIR
2. Set XC and basis in grad.py
 --default B3LYP/6-31G**
3. Check inpfileq for string method settings
4. To run:
 a. NWChem must be available at command line (e.g. source setnw)
 b. export OMP_NUM_THREADS=1
 c. to execute: ./gfstringq.exe jobnumber numberofcores > scratch/paragsmXXXX
 d. initialXXXX.xyz and ISOMERSXXXX must be present in scratch
5. example/qmakeg can create a queuing script in scratch/ called go_gsm_dft.qsh
 --add "#PBS -t jobnumber1,jobnumber2,..." at the top of go_gsm_dft.qsh


Analysis:

1. stringfile.xyz#### contains the reaction path and TS
 --variants on this file w/"g" at the end are growth phase strings
 --"fr" at the end is a partial Hessian analysis, showing the first 3 vibrational modes
 --comment lines are energies relative to first structure in kcal/mol
2. paragsm#### contains the optimization output
3. ./status shows the current state of the various runs


Examples in example/ directory:

0007: This is a Diels-Alder reaction. initial0007.xyz contains input for SE-GSM (SSM) or DE-GSM (GSM)

0076: H2 addition to SiH2. initial0076.xyz contains SSM and GSM input.
