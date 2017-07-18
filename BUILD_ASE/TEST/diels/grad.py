#!/usr/bin/python

from ase import Atoms
from ase.visualize import view
from ase.calculators.emt import EMT
from ase.calculators.nwchem import NWChem
from ase.constraints import FixAtoms
from ase.io import write
from ase.io import read
import sys
import os

nargs = len(sys.argv)
argv1 = sys.argv[1]
argv2 = sys.argv[2]
#print 'argv1: ',argv1
fname = 'scratch/structure'+argv1
slab = read(fname)

#slab.set_calculator(EMT())
slab.set_calculator(NWChem())
slab.calc.label = 'scratch/nwchem'+argv1
inputfile = 'nwchem'+argv1+'.nw'
outputfile = 'nwchem'+argv1+'.out'
##slab.calc.command = 'cd scratch; mpirun -np '+argv2+' nwchem '+inputfile+' > '+outputfile
slab.calc.command = 'cd scratch; mpirun --mca btl ^openib -np '+argv2+' nwchem '+inputfile+' > '+outputfile
print 'NWChem command: ',slab.calc.command
os.system('rm -f '+slab.calc.label+'.scrdir')
os.system('ln -s $PBSTMPDIR '+slab.calc.label+'.scrdir')

slab.calc.parameters.xc = 'B3LYP'
#slab.calc.parameters.basis = 'LANL2DZ ECP'
#slab.calc.parameters.ecp = 'LANL2DZ ECP'
slab.calc.reset()

energy = - slab.get_potential_energy()
grads = - slab.get_forces()
#print grads
f = open('scratch/GRAD'+argv1, 'w')
f.write(str(energy))
f.write('\n')
f.write(str(grads))
f.write('\n')
f.close()
