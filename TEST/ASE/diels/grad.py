#!/usr/bin/python

import shutil
from ase import Atoms           # pip install ase if it fails
from ase.visualize import view
from ase.calculators.emt import EMT
from ase.calculators.qchem import QChem
from ase.constraints import FixAtoms
from ase.io import write
from ase.io import read
import sys
import os

nargs = len(sys.argv)
argv1 = sys.argv[1]
argv2 = sys.argv[2]
#print 'argv1: ',argv1
os.system("cp scratch/structure"+argv1+" scratch/structure"+argv1+".xyz")
fname = 'scratch/structure'+argv1+'.xyz'
atoms = read(fname)
folder = 'scratch/qchem'+argv1

#atoms.set_calculator(EMT())
calc = QChem(
    jobtype='FORCE',
    method='B3LYP',
    basis='6-31G*',
    nt=8,
)

atoms.calc = calc

atoms.calc.label = 'scratch/qchem'+argv1
inputfile = 'qchem'+argv1+'.inp'
outputfile = 'qchem'+argv1+'.out'
##atoms.calc.command = 'cd scratch; mpirun -np '+argv2+' nwchem '+inputfile+' > '+outputfile
# atoms.calc.command = 'cd scratch; mpirun --mca btl ^openib -np '+argv2+' nwchem '+inputfile+' > '+outputfile
atoms.calc.command = f'cd scratch; qchem -nt 8 '+inputfile+' > '+outputfile
print('QChem command: ',atoms.calc.command)
# os.system('ln -s $PBSTMPDIR '+atoms.calc.label+'.scrdir')

# atoms.calc.parameters.xc = 'B3LYP'
#atoms.calc.parameters.basis = 'LANL2DZ ECP'
#atoms.calc.parameters.ecp = 'LANL2DZ ECP'
atoms.calc.reset()

cwd = os.getcwd()

if not os.path.exists(folder):
    os.mkdir(folder)
os.chdir(folder)

energy = - atoms.get_potential_energy()
grads = - atoms.get_forces()
#print grads
f = open('scratch/GRAD'+argv1, 'w')
f.write(str(energy))
f.write('\n')
f.write(str(grads))
f.write('\n')
f.close()

shutil.copy2('scratch/GRAD'+argv1, cwd+'/scratch')
os.chdir(cwd)

os.system('rm -rf '+folder)
os.system('rm -f scratch/structure'+argv1)
os.system('rm -f '+fname)
