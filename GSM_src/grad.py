#!/usr/bin/python

from ase import Atoms
from ase.visualize import view
from ase.calculators.emt import EMT
from ase.constraints import FixAtoms
from ase.io import write
from ase.io import read
import sys

nargs = len(sys.argv)
argv = sys.argv[1]
print 'argv: ',argv
fname = 'structure'+argv
slab = read(fname)
slab.set_calculator(EMT())
energy = slab.get_potential_energy() 
grads = slab.get_forces()
#print grads
f = open('GRAD'+argv, 'w')
f.write(str(energy))
f.write('\n')
f.write(str(grads))
f.write('\n')
f.close()

