/***************************************************************************
                          stringtools.cpp  -  description
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


#include "stringtools.h"

using namespace std;

// Make a lowercase copy of s:
// (C) Bruce Eckel, Thinking in C++ Vol 2

// If pref="movie", suff="xyz" nxyz=235, and nfields=6 then
// returns "movie000235.xyz"
// nfields in the total number of digits in teh string
string StringTools::genfilename(const string& pref, const string &suff, 
				int nfields, int nxyz)
{
   string nstr="";
   string spacer="0";
   double limit=0.9999;
   for (int i=1; i<=nfields; i++) { limit=limit*10; nstr=nstr+"0";}
   if (nxyz>limit) {
      printf("\n Now in file %s at line %d \n ",__FILE__,__LINE__);
      cout << "ERROR in genfilename" << endl;
	 return nstr;
   }
   nstr=StringTools::int2str(nxyz,nfields,spacer);
   string fname=(pref+nstr)+("."+suff);
   return (fname);
}

string StringTools::lowerCase(const string& s)
{
  char* buf = new char[s.length()];
  s.copy(buf, s.length());
  for(unsigned int i = 0; i < s.length(); i++)
    buf[i] = tolower(buf[i]);
  string r(buf, s.length());
  delete buf;
  return r;
}
// converts  atof correctly
double StringTools::atod(const string& s)
{
  char* buf = new char[s.length()];
  s.copy(buf, s.length());
  for(unsigned int i = 0; i < s.length(); i++){
     if (s[i]=='D') buf[i]='E';
     if (s[i]=='d') buf[i]='e';
}
  string r(buf, s.length());
  delete buf;
  double d=atof(r.c_str());
  return d;
}

/** wandelt String s in Integer um */
// Needs removal of all blanks etc.
int StringTools::str2int(string &s)
{
  int intVal=0;
  bool neg = false;
  for (unsigned int i=0; i< s.length(); i++)
    {
      if (s[i]=='-')
        neg= ! neg;
      else
        intVal= (intVal*10)+(s[i]-48);
    }
  return (neg)? -intVal: intVal;
}

/** wandelt char* s in Integer um */
int StringTools::str2int(unsigned char *s)
{
  int intVal=0;
  bool neg = false;
  unsigned int i=0;
  while ( *(s+i) != '\0')
    {
      if (*(s+i)=='-')
        neg= ! neg;
      else
        intVal= (intVal*10)+(*(s+i)-48);
      i++;
    }
  return (neg)? -intVal: intVal;
}

/** gibt integer >=0 als  String zurueck,
  size=Mindestlaenge des ergebnisstrings,
  Rest wird mit Leerzeichen (spaceStr) aufgefuellt
 */
string StringTools::int2str(int integer, int size, const string spaceStr)
{
  int d;
  string s, z;
  do
    {
      d= integer % 10;
      z=d+'0';
      s.insert(0, z);

      integer/=10;
    }
  while (integer);

  for (int i=s.length(); i< size; i++)
    s.insert( 0 , spaceStr);

  return s;
}

string StringTools::double2str(double val, int precision)
{
  char *buffer;
  int decimal, sign;
  string tempstr;

  if (abs(val)<1.000e-12) {
     tempstr="0.000000e00";
  return(tempstr);
  }

  if (precision>12) {
     printf("\n Now in file %s at line %d \n ",__FILE__,__LINE__);
     printf("\n Now in file %s at line %d \n ",__FILE__,__LINE__);
  }
  char temp[25];
  int expn=lrint(log10(abs(val)));
  int decadd=0;
  double fac=1;
  if (expn<-5) {
     decadd=-5-expn;
     for (int i=1; i<=decadd; i++) {fac=fac*10;}
     val=val*fac;
  } else if( expn>5) {
     decadd=expn-5;
     for (int i=1; i<=decadd; i++) {fac=fac*10;}
     val=val/fac;
  }
  for (int i=0; i<25; i++){temp[i]=' ';} 
  buffer = fcvt(val, precision, &decimal, &sign);
     // OLDSTYLE
     //  sprintf (temp,"%c%c.%s x 10^%d ",sign?'-':'+',
     //                                buffer[0],buffer+1,decimal-1);
  if (expn<-5) decimal=decimal-decadd;
  if (expn>5) decimal=decimal+decadd;

  sprintf (temp," %c%c.%se%d ",sign?'-':'+',buffer[0],buffer+1,decimal-1);
  tempstr=temp;
  return(tempstr);

}



/** gibt true zurueck, falls c eine Ziffer ist*/
// bool StringTools::isDigit(unsigned char c)
// {
//   return ((c>='0') && (c<='9'));
// }
 /** gibt True zurueck, falls c ein Buchstabe/Underscore ist */
 bool StringTools::isAlpha(unsigned char c)
 {
   return (isalpha(c) || c == '_');
 }
// /** gibt TRUE zurueck, wenn c Space oder Tab ist */
// bool StringTools::isWhitespace (unsigned char c)
// {
//   return ((c=='\t') || (c==' '));
// }


string StringTools::trimRight(const string &value)
 {
  string::size_type where = value.find_last_not_of(" \t");

  if (where == string::npos)
   // string has nothing but space
   return string();

  if (where == (value.length() - 1))
   // string has no trailing space, don't copy its contents
   return value;

  return value.substr(0, where + 1);
 }

/** gibt naechsten Character der Zeile zurueck, der kein Whitespace ist */
unsigned char StringTools::getNextNonWs(const string &line, int index)
{
  unsigned char c;
  do
    {
      c=line[index++];
    }
  while (isspace(c));
  return c;
}

unsigned int StringTools::getNextNonWsPos(const string &line, int index){
  return line.find_first_not_of(WS_CHARS, index);
}

string StringTools::validateDirPath(const string & path){
   return (path[path.length()-1] !=PATH_SEPARATOR_CHAR)?
              path+PATH_SEPARATOR_CHAR : path;
}


// return a new clean string 
string StringTools::newCleanString(string line){
   // remove initial whitespace blank tab comma etc find where actaul 
   // string starts
   // then find end "look for end of line or #", if end of line > 150 anyway 
   // stop
   // erase 0 to start erase end to end
   string::size_type sbegin, send;
   const string delims(" \t,.;");
   const char *commentchar("#");
   int istart, iend, length;
  
   // find size of string
   length=line.length();
   if (length==0) return NULL;

   sbegin=line.find_first_not_of(delims);
   istart=sbegin;

   send=line.find_first_of(commentchar);
   if (send>length) {iend=length;} else {iend=send;} // no commentchar

   // clear from end of string
   line.erase((iend));
   line.erase(0,istart);

   string r=line;
   return r;
}


int StringTools::cleanstring(string& line){
   string::size_type sbegin, send;
   const string delims(" \t,.;");
   const char *commentchar("#");
   int istart, iend, length;
  
   // find size of string
   length=line.length();
   if (length==0) return length;

   sbegin=line.find_first_not_of(delims);
   istart=sbegin;

   send=line.find_first_of(commentchar);
   if (send>length) {iend=length;} else {iend=send;} // no commentchar

   line.erase((iend));
   line.erase(0,istart);

   return length;
}


 
// Function: tokenize, taken from web
//
// A simple and robust string tokenizer, extracts a vector of 
// tokens from a string (str) delimited by delims
//
vector<string> StringTools::tokenize(string str, string delims)
{
    string::size_type start_index, end_index;
    vector<string> ret;

    // Skip leading delimiters, to get to the first token
    start_index = str.find_first_not_of(delims);

    // While found a beginning of a new token
    //
    while (start_index != string::npos)
    {
	// Find the end of this token
	end_index = str.find_first_of(delims, start_index);
	
	// If this is the end of the string
	if (end_index == string::npos)
	    end_index = str.length();

	ret.push_back(str.substr(start_index, end_index - start_index));

	// Find beginning of the next token
	start_index = str.find_first_not_of(delims, end_index);
    }

    return ret;
}

// checks whether s1 contains s2 as a substring
bool StringTools::contains(string s1, string s2){
   int s2pos=s1.find(s2);
   bool contains_string=true;
   if (s2pos==string::npos) contains_string=false;
   return contains_string;
}

// If an input line starts with # or % then it is a comment
bool StringTools::iscomment(string s){
   const char *blank(" ");
   // go to beginning of line
   int index=s.find_first_not_of(blank);
   if ( (s[index]=='#')||(s[index]=='%') ) {return true;} else {return false;}
}

// finds the string containing tag in the file
// useful for locating the beginning of a particular section
// of input in a file
bool StringTools::findstr(ifstream &fstr, string tag){
   bool found=false;
   int linecount=0; // for debug purpose only
   string line;

   while(getline(fstr, line)){
      linecount++;
      found=contains(line, tag);
      if (found) break;
   }

   return found;
}

// finds the string containing tag in the file
// useful for locating the beginning of a particular section
// of input in a file AND RTEURS THE FOUND STRING
bool StringTools::findstr(ifstream &fstr, string tag, string & outline){
   bool found=false;
   string line;

   while(getline(fstr, line)){
      //      linecount++;
      found=contains(line, tag);
      if (found) {outline=line;break;}
   }
   return found;
}

