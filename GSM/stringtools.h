/***************************************************************************
                          stringtools.h  -  description
                             -------------------
    begin                : Mon Dec 10 2001
    copyright            : (C) 2001 by André Simon
    email                : andre.simon1@gmx.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STRINGTOOLS_H
#define STRINGTOOLS_H

#include <string>
#include <cctype>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdio.h>
#include <cmath>
#include <cstdlib>


#define WS_CHARS " \n\r\t"

#ifdef _WIN32
  #define PATH_SEPARATOR_CHAR '\\'
  #define PATH_SEPARATOR_STR "\\"
#else
  #define PATH_SEPARATOR_CHAR '/'
  #define PATH_SEPARATOR_STR "/"
#endif

using namespace std;

/**Methoden zur Stringbearbeitung
  *@author Andre Simon
  */

namespace StringTools
  {

     string genfilename(const string& pref, const string &suff, 
			int nfields, int nxyz);


  /** \param s String
      \returns lowercase string  */
  string lowerCase(const string &s);
  double atod(const string &s);

  /** \param String
      \returns Integer value */
  int str2int(string &s);

  /** gibt integer als  String zurueck */
  string int2str(int integer, int size=0, const string spaceStr= " ");

  string double2str(double val, int precision);

  /** gibt True zurueck, falls c ein Buchstabe ist */
  bool isAlpha(unsigned char c);

  /** wandelt String s in Integer um */
  int str2int(unsigned char *s);

  /* entfernt whitespace von stringende*/
  string trimRight(const string &);

  /** gibt naechsten Character der Zeile zurck, der kein Whitespace ist*/
  unsigned char getNextNonWs(const string &line, int index=0);

  unsigned int getNextNonWsPos(const string &line, int index=0);

  string validateDirPath(const string & path);

  int cleanstring(string& line);
  string newCleanString(string line);

  vector<string> tokenize(string str, string delims);
  bool findstr(ifstream &fstr, string tag);
  bool findstr(ifstream &fstr, string tag, string & outstring);
  bool contains(string s1, string s2);
  bool iscomment(string s);

};

#endif


 
 
