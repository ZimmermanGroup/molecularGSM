#!/usr/bin/python

import os,sys
import argparse
import subprocess
import time
from datetime import datetime

def submit_job(basis, charge, functional, COSMO, epsilon, disp, NAME, add_bond, break_bond, angle, torsion, nodes, mem, structure):

  print structure

  if not os.path.isfile(structure):
    print ("No coord.xyz! Please provide one!")
    sys.exit()

  if add_bond==[] and break_bond==[] and angle==[] and torsion==[]:
    print ("Nothing to be done, since add_bond, break_bond, angle and torsion are empty!!!")
    sys.exit()

  if add_bond==[] and break_bond==[]:
    TS_TYPE="0"
  else:
    TS_TYPE="1"

  RI = 1
  REACT=1
  wd=os.getcwd()
  
  if os.path.isdir(wd+"/scratch"):
    print ("existing scratch folder is deleted")
    os.system("rm -r "+wd+"/scratch")
  
  if os.path.isfile(wd+"/inpfileq"):
    print ("existing input file is deleted")
    os.system("rm "+wd+"/inpfileq")
  
  cmd1 = "mkdir scratch"
  os.system(cmd1)
  
  inp_file="inpfileq"
  inp = open(inp_file, 'w')

  print "working directors:", wd

  print("write input file")
  inp.write("# FSM/GSM/SSM inpfileq\n")
  inp.write("\n")
  inp.write("------------- TURBOMOLE Scratch Info ------------------------\n")
  inp.write("$TMSCRATCH/    # path for scratch dir. end with \"/\n")
  inp.write("GSM_go1q       # name of run \n")
  inp.write("RI                      "+str(RI)+"           # if RI should be used\n")
  inp.write("COSMO                   "+str(COSMO)+"\n")
  inp.write("turboDIR       /programs/turbomole/current/bin/em64t-unknown-linux-gnu\n")
  inp.write("---------------------------------------------------------\n")
  inp.write("\n")
  inp.write("------------ String Info --------------------------------\n")
  inp.write("SM_TYPE                 GSM    # SSM, FSM or GSM\n")
  inp.write("RESTART                 0      # read restart.xyz\n")
  inp.write("MAX_OPT_ITERS           100    # maximum iterations\n")
  inp.write("STEP_OPT_ITERS          30     # for FSM/SSM\n")
  inp.write("CONV_TOL                0.0005 # perp grad \n")
  inp.write("ADD_NODE_TOL            0.1    # for GSM\n")
  inp.write("SCALING                 1.0    # for opt steps\n")
  inp.write("SSM_DQMAX               0.8    # add step size\n")
  inp.write("GROWTH_DIRECTION        0      # normal/react/prod: 0/1/2\n")
  inp.write("INT_THRESH              2.0    # intermediate detection\n")
  inp.write("MIN_SPACING             5.0    # node spacing SSM\n")
  inp.write("BOND_FRAGMENTS          1      # make IC's for fragments\n") 
  inp.write("INITIAL_OPT             150    # opt steps first node\n")
  inp.write("FINAL_OPT               150    # opt steps last SSM node\n")
  inp.write("PRODUCT_LIMIT           100.0  # kcal/mol\n")
  inp.write("TS_FINAL_TYPE           "+str(TS_TYPE)+"       # any/delta bond: 0/1\n")
  inp.write("NNODES                  11     # including endpoints\n")
  inp.write("---------------------------------------------------------")
  
  inp.close()
  
  outfile1="scratch/ISOMERS0000"
  out1 = open(outfile1, 'w')
  

  out1.write("NEW\n")
  for rule in add_bond:
    out1.write("  ADD %s %s\n"%(rule[0],rule[1]))
  for rule in break_bond:
    out1.write("  BREAK %s %s\n"%(rule[0],rule[1]))
  for rule in angle:
    out1.write("  ANGLE %s %s %s %s\n"%(rule[0],rule[1],rule[2],rule[3]))
  for rule in torsion:
    out1.write("  TORSION %s %s %s %s\n"%(rule[0],rule[1],rule[2],rule[3]))
  out1.close()

  cmd2 = "cp "+wd+"/"+structure+" "+wd+"/scratch/initial0000.xyz"
  os.system(cmd2)
  
  out1.close()
  
  if os.path.isfile(wd+"/tm_define"):
    print ("existing tm_define file is deleted")
    os.system("rm "+wd+"/tm_define")
  
  
  outfile2="tm_define"
  out2 = open(outfile2, 'w')
  out2.write("\n\n")
  out2.write("a coord\n")
  out2.write("*\n")
  out2.write("no\n")
  out2.write("b\n")
  out2.write("all "+str(basis)+"\n")
  out2.write("*\n")
  out2.write("eht\n")
  out2.write("\n")
  if charge!=0:
    out2.write(str(charge)+"\n")
  out2.write("\n\n\n")
  if disp!="":
    out2.write("dsp\n")
    out2.write(str(disp)+"\n")
    out2.write("*\n")
  out2.write("dft\n")
  out2.write("on\n")
  out2.write("func\n")
  out2.write(str(functional)+"\n")
  out2.write("*\n")
  if RI:
    out2.write("ri\n")
    out2.write("on\n")
    out2.write("*\n")
  out2.write("scf\n")
  out2.write("iter\n")
  out2.write("1200\n")
  out2.write("\n")
  out2.write("*\n")
  out2.close()
  
  
  if COSMO:
    if os.path.isfile(wd+"/cosmo_inp"):
      print ("existing cosmo_inp file is deleted")
      os.system("rm cosmo_inp")
    outfile3="cosmo_inp"
    out3 = open(outfile3,"w")
    out3.write(str(epsilon))
    out3.write("\n\n\n\n\n\n\n\n\n\n\n\n")
    out3.write("r all b\n\n")
    out3.write("*\n\n\n")
    out3.close()
  
  # submit job
  cmd=wd+"/gsm.qsh "+str(nodes)+" "+str(mem)+" "+str(REACT)+" "+str(NAME)
  print cmd
  os.system(cmd)

  return 0

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Submit a molecular GSM calculation.') 
#For example: \./run_mgsm_argparse.py -del \"1 2\" -del \"3 4\" -add \"2 3\" -c 1 -b def2-SVP -cosmo=off')
    parser.add_argument('-b', '--basis', type=str, help='Specify basis set (default=def2-SVP)!', default="def2-SVP")
    parser.add_argument('-c', '--charge', type=str, help='Specify charge (default=0)!', default=0)
    parser.add_argument('-f', '--functional', type=str, help='Specify density functional(default=b-p)!', default="b-p")
    parser.add_argument('-cos', '--cosmo', type=str, help='Specify if COSMO on (1) or off(0)(default=1)!', default=1)
    parser.add_argument('-eps', '--epsilon', type=str, help='Specify dielectric permittivity (default=Infinity)', default="Infinity")
    parser.add_argument('-disp', '--disp', type=str, help='Specify dispersion (default=off) [options: off, on --> 3, bj --> d3-bj]!', default="off")
    parser.add_argument('-n', '--name', type=str, help='Specify name(default=TEST)!', default="TEST")
    parser.add_argument('-a', '--add', nargs=2, action='append', help='Specify add_bonds as string, e.g. 1 2', default=[])
    parser.add_argument('-d', '--delete', nargs=2, action='append',help='Specify break_bonds as string, e.g. 1 2', default=[])
    parser.add_argument('-ang', '--angle', nargs=4, action='append', help='Specify angle changes as string, e.g. 1 2 3 180', default=[])
    parser.add_argument('-tor', '--torsion', nargs=4, action='append',help='Specify torsion changes as string, e.g. 1 2 3 4 180', default=[])
    parser.add_argument('-nodes', '--nodes', type=str, help='Specify number of nodes used for ridft/dscf and rdgrad/grad calcs. (default=1)', default="1")
    parser.add_argument('-mem', '--memory', type=str, help='Specify memory used in ridft/dscf and rdgrad/grad calcs. (default=1 GB)', default="1")
    parser.add_argument('-struct', '--structure', type=str, help='Specify name of structure.xyz (default=coord.xyz)', default="coord.xyz")

    args = parser.parse_args()
 
    submit_job(basis=args.basis, charge=args.charge, functional=args.functional, COSMO=args.cosmo, epsilon=args.epsilon, disp=args.disp, NAME=args.name, add_bond=args.add, break_bond=args.delete, angle=args.angle, torsion=args.torsion, nodes=args.nodes, mem=args.memory, structure=args.structure)
