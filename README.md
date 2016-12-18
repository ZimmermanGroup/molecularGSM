## Overview
The growing string method is a reaction path and transition state finding method developed in c++.

For more information, check out the wiki page:
https://github.com/ZimmermanGroup/molecularGSM/wiki

Sample tutorial files can be found under the tutorial folder:
https://github.com/ZimmermanGroup/molecularGSM/tree/master/tutorial

## Installation
This code can be built using CMake. To do so:

1. Load/install CMake
2. Load Intel compilers (if not loaded)
3. Clone this repository and checkout tribits branch

```bash
		$ git clone https://github.com/ZimmermanGroup/molecularGSM.git
        $ cd molecularGSM
		$ git checkout tribits
```

4. Clone TriBITS repository
```bash
	 $ git clone https://github.com/TriBITSPub/TriBITS.git
```

5. Create a BUILD directory at the same level as GSM
```bash
    $ mkdir BUILD
    $ cd BUILD
```

6. Configure using CMake
```bash
    $ cmake -D GSM_ENABLE_QCHEM=1 ../
```
    - other options:
        - GSM_ENABLE_QCHEM_SF=1
        - GSM_ENABLE_ORCA=1
        - GSM_ENABLE_GAUSSIAN=1
        - GSM_ENABLE_MOLPRO=1
        - GSM_ENABLE_ASE=1
    - If no option is specified, the code will use MOPAC as its energy calculator. Check mopac.cpp to make sure charge/multiplicity is correct, since that is hard-coded.

7. After successful configuration. To compile:
```bash
    $make -j8
```

8. An executable named "gsm.${CALCULATOR}.exe" will be created in BUILD/GSM directory, where ${CALCULATOR} is the name of the QM package.

To run gsm, copy the executable to the working directory (where the input files are) or reference it using the full path. 

##CTest

There are five test examples: alanine dipeptide isomerization, ammonia borane reactions, diels alder reaction, ethylene rotation, and methanol formaldehyde reaction. After building the executable you can use type $ ctest to run the tests. When each test is complete, the output will be compared with the standard output in each test directory. If the difference in coordinates of a each atom is more than 0.001, the test will fail.
