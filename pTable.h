/***************************************************************************
                          pTable.h  -  description
                             -------------------
    copyright            : (C) 2004 by Shaji Chempath
    University of California Berkeley
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PTABLE_H
#define PTABLE_H

#include <string>
#include <iostream>
#include <cctype>
#include <vector>
#include <cstdlib>


using namespace std;

namespace PTable
  {
     int  atom_number(string &aName);

     string  atom_name(int aNumber);

     double  atom_mass(int aNumber);
  }
#endif

