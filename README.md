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
3. Clone this repository

```bash
    $ git clone git@github.com:ZimmermanGroup/molecularGSM.git
```

4. Create a BUILD directory at the same level as GSM
```bash
    $ cd molecularGSM
    $ mkdir BUILD
    $ cd BUILD
```
5. Configure using CMake
    $ cmake -D GSM_ENABLE_QCHEM=1 ../
    - other options:
        - GSM_ENABLE_QCHEM_SF=1
        - GSM_ENABLE_ORCA=1
        - GSM_ENABLE_GAUSSIAN=1
        - GSM_ENABLE_MOLPRO=1
        - GSM_ENABLE_ASE=1
    - If no option is specified, the code will use MOPAC as its energy calculator. Check mopac.cpp to make sure charge/multiplicity is correct, since that is hardcoded.
6. After successful configuration. To compile:
    
    $make -j8

7. An executable named "gfstringq.exe" will be created in BUILD/GSM directory.

To run gfstringq.exe, copy the executable to the working directory (where the input files are) or create a symbolic link to it in the working directory.
