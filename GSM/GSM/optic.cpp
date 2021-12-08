#include "icoord.h"
#include "gstring.h"

#define MAX_DIST 4.0

int ICoord::distance_matrix_ic(ICoord ic1, ICoord ic2)
{
  int maxangles = ic1.nangles+ic2.nangles;
  int** angle_list = new int*[maxangles];
  for (int i=0;i<maxangles;i++) angle_list[i] = new int[3];

  int* angle_miss = new int[maxangles];
  for (int i=0;i<maxangles;i++) angle_miss[i] = -1;

  int nangle_miss = 0;
  nbonds = 0;
  nangles = 0;
  int nangles0 = 0;

  //printf(" ic1.nbonds: %i ic2.nbonds: %i \n",ic1.nbonds,ic2.nbonds);

  //go through first struct, add bonds to list and id missing bonds in second struct
  for (int i=0;i<ic1.natoms;i++)
  for (int j=0;j<i;j++)
  {
    if (ic1.distance(i,j)<MAX_DIST || ic2.distance(i,j)<MAX_DIST)
    {
      bonds[nbonds][0] = i;
      bonds[nbonds][1] = j;
      //printf(" adding bond: %i %i \n",bonds[nbonds][0],bonds[nbonds][1]);
      nbonds++;
    }
  }
  printf(" added %i bonds via distance matrix ",nbonds);

#if 1
//CPMZ generally decent interpolation
  for (int i=0;i<ic2.nangles;i++)
  {
    int found = 0;
    for (int j=0;j<ic1.nangles;j++)
    {
      if (ic2.angles[i][1] == ic1.angles[j][1])
      {
        if (ic2.angles[i][0] == ic1.angles[j][0] && ic2.angles[i][2] == ic1.angles[j][2])
          found = 1;
        if (ic2.angles[i][0] == ic1.angles[j][2] && ic2.angles[i][2] == ic1.angles[j][0])
          found = 1;
      }
      if (found) break;
    }
    if (!found)
    {
      angle_miss[nangle_miss] = i;
      nangle_miss++;
    }
  }

  for (int j=0;j<ic1.nangles;j++)
  {
    angle_list[nangles0][0] = ic1.angles[j][0];
    angle_list[nangles0][1] = ic1.angles[j][1];
    angle_list[nangles0][2] = ic1.angles[j][2];
    nangles0++;
  }
  for (int i=0;i<nangle_miss;i++)
  {
    angle_list[nangles0][0] = ic2.angles[angle_miss[i]][0];
    angle_list[nangles0][1] = ic2.angles[angle_miss[i]][1];
    angle_list[nangles0][2] = ic2.angles[angle_miss[i]][2];
    nangles0++;
  }

  printf(" saving angle union: %i \n",nangles0);
  for (int i=0;i<ic1.nangles;i++)
  {
    angles[i][0] = angle_list[i][0];
    angles[i][1] = angle_list[i][1];
    angles[i][2] = angle_list[i][2];
  //  printf(" angle[%i]: %i %i %i \n",i,angle_list[i][0],angle_list[i][1],angle_list[i][2]);
  }
  nangles = ic1.nangles;

  for (int i=ic1.nangles;i<nangles0;i++)
  {
    angles[i][0] = angle_list[i][0];
    angles[i][1] = angle_list[i][1];
    angles[i][2] = angle_list[i][2];
  //  printf(" angle[%i]: %i %i %i \n",i,angle_list[i][0],angle_list[i][1],angle_list[i][2]);
  }
  nangles = nangles0;
#else
  nangles = 0;
#endif
 
  ntor = 0;

  update_ic();

  for (int i=0;i<maxangles;i++) delete [] angle_list[i];
  delete [] angle_list;
  delete [] angle_miss;

  return 0;
}

int ICoord::union_ic(ICoord ic1, ICoord ic2)
{

  int maxbonds = ic1.nbonds+ic2.nbonds;
  int maxangles = ic1.nangles+ic2.nangles;
  int maxtor = ic1.ntor+ic2.ntor;
  int** bond_list = new int*[maxbonds];
  int** angle_list = new int*[maxangles];
  int** tor_list = new int*[maxtor];
  for (int i=0;i<maxbonds;i++) bond_list[i] = new int[2];
  for (int i=0;i<maxangles;i++) angle_list[i] = new int[3];
  for (int i=0;i<maxtor;i++) tor_list[i] = new int[4];

  int* bond_miss = new int[maxbonds];
  for (int i=0;i<maxbonds;i++) bond_miss[i] = -1;
  int* angle_miss = new int[maxangles];
  for (int i=0;i<maxangles;i++) angle_miss[i] = -1;
  int* tor_miss = new int[maxtor];
  for (int i=0;i<maxtor;i++) tor_miss[i] = -1;

  int nbond_miss = 0;
  int nangle_miss = 0;
  int ntor_miss = 0;
  nbonds = 0;
  nangles = 0;
  ntor = 0;
  int nbonds0 = 0;
  int nangles0 = 0;
  int ntor0 = 0;

  printf(" ic1.nbonds: %i ic2.nbonds: %i \n",ic1.nbonds,ic2.nbonds);

  //go through first struct, add bonds to list and id missing bonds in second struct
  for (int i=0;i<ic2.nbonds;i++)
  {
    int found = 0;
    for (int j=0;j<ic1.nbonds;j++)
    {
      if (ic2.bonds[i][0] == ic1.bonds[j][0] && ic2.bonds[i][1] == ic1.bonds[j][1])
        found = 1;
      else if (ic2.bonds[i][1] == ic1.bonds[j][0] && ic2.bonds[i][0] == ic1.bonds[j][1])
        found = 1;
      else if (ic2.bonds[i][0] == ic1.bonds[j][1] && ic2.bonds[i][1] == ic1.bonds[j][0])
        found = 1;
      if (found) break;
    }
    if (!found)
    {
      bond_miss[nbond_miss] = i;
      nbond_miss++;
    }
  }

  for (int j=0;j<ic1.nbonds;j++)
  {
    bond_list[nbonds0][0] = ic1.bonds[j][0];
    bond_list[nbonds0][1] = ic1.bonds[j][1];
    nbonds0++;
  }
  for (int i=0;i<nbond_miss;i++)
  {
    bond_list[nbonds0][0] = ic2.bonds[bond_miss[i]][0];
    bond_list[nbonds0][1] = ic2.bonds[bond_miss[i]][1];
    nbonds0++;
  }

  for (int i=0;i<ic2.nangles;i++)
  {
    int found = 0;
    for (int j=0;j<ic1.nangles;j++)
    {
      if (ic2.angles[i][1] == ic1.angles[j][1])
      {
        if (ic2.angles[i][0] == ic1.angles[j][0] && ic2.angles[i][2] == ic1.angles[j][2])
          found = 1;
        if (ic2.angles[i][0] == ic1.angles[j][2] && ic2.angles[i][2] == ic1.angles[j][0])
          found = 1;
      }
      if (found) break;
    }
    if (!found)
    {
      angle_miss[nangle_miss] = i;
      nangle_miss++;
    }
  }

  for (int j=0;j<ic1.nangles;j++)
  {
    angle_list[nangles0][0] = ic1.angles[j][0];
    angle_list[nangles0][1] = ic1.angles[j][1];
    angle_list[nangles0][2] = ic1.angles[j][2];
    nangles0++;
  }
  for (int i=0;i<nangle_miss;i++)
  {
    angle_list[nangles0][0] = ic2.angles[angle_miss[i]][0];
    angle_list[nangles0][1] = ic2.angles[angle_miss[i]][1];
    angle_list[nangles0][2] = ic2.angles[angle_miss[i]][2];
    nangles0++;
  }


  for (int i=0;i<ic2.ntor;i++)
  {
    int found = 0;
    for (int j=0;j<ic1.ntor;j++)
    {
      if (ic2.torsions[i][1] == ic1.torsions[j][1] && ic2.torsions[i][2] == ic1.torsions[j][2])
      {
        if (ic2.torsions[i][0] == ic1.torsions[j][0] && ic2.torsions[i][3] == ic1.torsions[j][3])
          found = 1;
      }
      if (ic2.torsions[i][2] == ic1.torsions[j][1] && ic2.torsions[i][2] == ic1.torsions[j][1])
      {
        if (ic2.torsions[i][0] == ic1.torsions[j][3] && ic2.torsions[i][3] == ic1.torsions[j][0])
          found = 1;
      }
      if (found) break;
    }
    if (!found)
    {
      tor_miss[ntor_miss] = i;
      ntor_miss++;
    }
  }

  for (int j=0;j<ic1.ntor;j++)
  {
    tor_list[ntor0][0] = ic1.torsions[j][0];
    tor_list[ntor0][1] = ic1.torsions[j][1];
    tor_list[ntor0][2] = ic1.torsions[j][2];
    tor_list[ntor0][3] = ic1.torsions[j][3];
    ntor0++;
  }
  for (int i=0;i<ntor_miss;i++)
  {
    tor_list[ntor0][0] = ic2.torsions[tor_miss[i]][0];
    tor_list[ntor0][1] = ic2.torsions[tor_miss[i]][1];
    tor_list[ntor0][2] = ic2.torsions[tor_miss[i]][2];
    tor_list[ntor0][3] = ic2.torsions[tor_miss[i]][3];
    ntor0++;
  }


  printf(" saving bond union: %i ",nbonds0);
  for (int i=0;i<ic1.nbonds;i++)
  {
    bonds[i][0] = bond_list[i][0];
    bonds[i][1] = bond_list[i][1];
  //  printf(" bond[%i]: %i %i \n",i,bond_list[i][0],bond_list[i][1]);
  }
  nbonds = ic1.nbonds;
  //ic_create_nobonds();

  for (int i=ic1.nbonds;i<nbonds0;i++)
  {
    bonds[i][0] = bond_list[i][0];
    bonds[i][1] = bond_list[i][1];
  //  printf(" 2bond[%i]: %i %i \n",i,bond_list[i][0],bond_list[i][1]);
  }
  nbonds = nbonds0;
  //ic_create_nobonds();

#if 1 //doing union for angles/tor

  printf(" saving angle union: %i ",nangles0);
  for (int i=0;i<ic1.nangles;i++)
  {
    angles[i][0] = angle_list[i][0];
    angles[i][1] = angle_list[i][1];
    angles[i][2] = angle_list[i][2];
   // printf(" angle[%i]: %i %i %i \n",i,angle_list[i][0],angle_list[i][1],angle_list[i][2]);
  }
  nangles = ic1.nangles;

  for (int i=ic1.nangles;i<nangles0;i++)
  {
    angles[i][0] = angle_list[i][0];
    angles[i][1] = angle_list[i][1];
    angles[i][2] = angle_list[i][2];
   // printf(" angle[%i]: %i %i %i \n",i,angle_list[i][0],angle_list[i][1],angle_list[i][2]);
  }
  nangles = nangles0;

#if 1
  printf(" saving torsion union: %i ",ntor0);
  for (int i=0;i<ic1.ntor;i++)
  {
    torsions[i][0] = tor_list[i][0];
    torsions[i][1] = tor_list[i][1];
    torsions[i][2] = tor_list[i][2];
    torsions[i][3] = tor_list[i][3];
   // printf(" torsion[%i]: %i %i %i %i \n",i,tor_list[i][0],tor_list[i][1],tor_list[i][2],tor_list[i][3]);
  }
  ntor = ic1.ntor;
#endif

#if 1
  for (int i=ic1.ntor;i<ntor0;i++)
  {
    torsions[i][0] = tor_list[i][0];
    torsions[i][1] = tor_list[i][1];
    torsions[i][2] = tor_list[i][2];
    torsions[i][3] = tor_list[i][3];
   // printf(" torsion[%i]: %i %i %i %i \n",i,tor_list[i][0],tor_list[i][1],tor_list[i][2],tor_list[i][3]);
  }
  ntor = ntor0;
#endif

#if 0
  for (int i=0;i<ntor;i++)
  {
    if (bond_exists(torsions[i][0],torsions[i][3]))
    {
      printf(" WARNING: bond matches tor: %i %i %i %i,",torsions[i][0],torsions[i][1],torsions[i][2],torsions[i][3]);
      printf(" removing tor \n");
      ntor--;
      for (int j=i;j<ntor;j++)
      {
        torsions[j][0] = torsions[j+1][0];
        torsions[j][1] = torsions[j+1][1];
        torsions[j][2] = torsions[j+1][2];
        torsions[j][3] = torsions[j+1][3];
      }
    }
  } //loop i over ntor0
#endif

#endif // doing union for angles/tor

#if 0
  printf(" WARNING: disabling tor \n");
  ntor =0;
#endif

  printf(" total number of coordinates: %3i \n",nbonds+nangles+ntor);
  printf("\n");

  update_ic();




#if 1
  for (int i=0;i<maxbonds;i++) delete [] bond_list[i];
  for (int i=0;i<maxangles;i++) delete [] angle_list[i];
  for (int i=0;i<maxtor;i++) delete [] tor_list[i];
#endif
  delete [] bond_list;
  delete [] angle_list;
  delete [] tor_list;

  return 0;

}

int ICoord::copy_ic(ICoord ic1)
{
  for (int i=0;i<ic1.nbonds;i++)
  {
    bonds[i][0] = ic1.bonds[i][0];
    bonds[i][1] = ic1.bonds[i][1];
  }
  for (int i=0;i<ic1.nangles;i++)
  {
    angles[i][0] = ic1.angles[i][0];
    angles[i][1] = ic1.angles[i][1];
    angles[i][2] = ic1.angles[i][2];
  }
  for (int i=0;i<ic1.ntor;i++)
  {
    torsions[i][0] = ic1.torsions[i][0];
    torsions[i][1] = ic1.torsions[i][1];
    torsions[i][2] = ic1.torsions[i][2];
    torsions[i][3] = ic1.torsions[i][3];
  }
  nbonds = ic1.nbonds;
  nangles = ic1.nangles;
  ntor = ic1.ntor;

  return 0;
}
