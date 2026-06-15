## Questions?
Contact Paras Boruah (paraspb@umich.edu)

## Overview
The growing string method is a reaction path and transition state finding method developed in c++.

For more information, check out the wiki page:
https://github.com/ZimmermanGroup/molecularGSM/wiki

Sample tutorial files can be found under the tutorial folder:
https://github.com/ZimmermanGroup/molecularGSM/tree/master/tutorial

Running GSM with the XTB package (from the Grimme lab):
https://github.com/grimme-lab/xtb_docs/blob/master/source/gsm.rst

## Installation

### Using Pixi (Recommended)

The easiest way to install GSM is with [Pixi](https://pixi.sh/), which handles all dependencies automatically.

1. [Install Pixi](https://pixi.sh/latest/#installation) if you haven't already
2. Install GSM globally:
```bash
pixi global install \
    --git https://github.com/ZimmermanGroup/molecularGSM.git
```

This builds GSM with MOPAC support (default) and makes `gsm` available in your PATH.

#### Building with Different Backends

To build with a different energy calculator backend, set the appropriate environment variable:

| Backend | Command |
|---------|---------|
| Q-Chem | `GSM_ENABLE_QCHEM=1 pixi global install --git ...` |
| Q-Chem SF | `GSM_ENABLE_QCHEM_SF=1 pixi global install --git ...` |
| ORCA | `GSM_ENABLE_ORCA=1 pixi global install --git ...` |
| Gaussian | `GSM_ENABLE_GAUSSIAN=1 pixi global install --git ...` |
| Molpro | `GSM_ENABLE_MOLPRO=1 pixi global install --git ...` |
| ASE | `GSM_ENABLE_ASE=1 pixi global install --git ...` |
| Turbomole | `GSM_ENABLE_TURBOMOLE=1 pixi global install --git ...` |

For example, to install with Q-Chem:
```bash
GSM_ENABLE_QCHEM=1 pixi global install \
    --git https://github.com/ZimmermanGroup/molecularGSM.git
```

#### Development Setup

For development or to install from a local clone:

```bash
git clone https://github.com/ZimmermanGroup/molecularGSM.git
cd molecularGSM
pixi install
```

This makes the `gsm` executable available in the pixi environment:
```bash
pixi run gsm <input_number>
pixi run test  # run included test
```

You can also install globally from the local path:
```bash
pixi global install --path .
```

### Using CMake (Manual Build)

Alternatively, this code can be built using CMake directly. To do so:

1. Load/install CMake
2. Load MKL (On Athena use `intel/oneapi/mkl/2021.1.1` and GCC, e.g. `gcc/12.1.0`)
3. Clone this repository, use master branch

```bash
    $ git clone https://github.com/ZimmermanGroup/molecularGSM.git
    $ cd molecularGSM
```

4. Create a BUILD directory at the same level as GSM
```bash
    $ mkdir BUILD
    $ cd BUILD
```

5. Configure using CMake
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

6. After successful configuration. To compile:
```bash
    $make -j8
```

7. An executable named `gsm` will be created in `BUILD/GSM` directory.

To run gsm, copy the executable to the working directory (where the input files are) or reference it using the full path. 

## CTest

There are five test examples: alanine dipeptide isomerization, ammonia borane reactions, diels alder reaction, ethylene rotation, and methanol formaldehyde reaction. After building the executable you can use type $ ctest to run the tests. When each test is complete, the output will be compared with the standard output in each test directory. If the difference in coordinates of a each atom is more than 0.001, the test will fail.

