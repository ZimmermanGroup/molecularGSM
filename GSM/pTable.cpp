#include "pTable.h"

char atom_symbol[103][3]={"00",
			 "H","He","Li","Be","B","C","N","O","F","Ne",
			 "Na","Mg","Al","Si","P","S","Cl","Ar",
			 "K","Ca","Sc","Ti","V","Cr","Mn","Fe","Co",
			 "Ni","Cu","Zn","Ga","Ge","As","Se","Br","Kr",
			 "Rb","Sr","Y","Zr","Nb","Mo","Tc","Ru","Rh",
			 "Pd","Ag","Cd","In","Sn","Sb","Te","I","Xe",
			 "Cs","Ba","La","Ce","Pr","Nd","Pm","Sm","Eu",
			 "Gd","Tb","Dy","Ho","Er","Tm","Yb","Lu","Hf",
			 "Ta","W","Re","Os","Ir","Pt","Au","Hg","Tl",
			 "Pb","Bi","Po","At","Rn","Fr","Ra","Ac","Th",
			 "Pa","U","Np","Pu","Am","Cm","Bk","Cf","Es",
			 "Fm","Md","No"};

char atom_names[103][20]={
"atom0", "Hydrogen ", "Helium ", "Lithium ", "Beryllium ", "Boron ",
"Carbon ", "Nitrogen ", "Oxygen ", "Fluorine ", "Neon ", "Sodium ",
"Magnesium ", "Aluminium ", "Silicon ", "Phosphorus ", "Sulfur ",
"Chlorine ", "Argon ", "Potassium ", "Calcium ", "Scandium ",
"Titanium ", "Vanadium ", "Chromium ", "Manganese ", "Iron ", "Cobalt", 
"Nickel ", "Copper ", "Zinc ", "Gallium ", "Germanium ", "Arsenic", 
"Selenium ", "Bromine ", "Krypton ", "Rubidium ", "Strontium ",
"Yttrium ", "Zirconium ", "Niobium ", "Molybdenum ", "Technetium ",
"Ruthenium ", "Rhodium ", "Palladium ", "Silver ", "Cadmium ", "Indium", 
"Tin ", "Antimony ", "Tellurium ", "Iodine ", "Xenon ", "Caesium ",
"Barium ", "Lanthanum ", "Cerium ", "Praseodymiu ", "Neodymium ",
"Promethium ", "Samarium ", "Europium ", "Gadolinium ", "Terbium ",
"Dysprosium ", "Holmium ", "Erbium ", "Thulium ", "Ytterbium ",
"Lutetium ", "Hafnium", "Tantalum ", "Tungsten ", "Rhenium ", "Osmium", 
"Iridium ", "Platinum ", "Gold ", "Mercury ", "Thallium ", "Lead ",
"Bismuth ", "Polonium ", "Astatine ", "Radon ", "Francium ", "Radium", 
"Actinium", "Thorium ", "Protactiniu ", "Uranium ", "Neptunium ",
"Plutonium ", "Americium ", "Curium ", "Berkelium ", "Californium ",
"Einsteinium ", "Fermium ", "Mendelevium ", "Nobelium "};




double atom_masses[103]={-1.00,
   1.0079 , 4.0026 , 6.941 , 9.0121 , 10.811 , 12.010 , 14.006 ,
   15.999 , 18.998 , 20.179 , 22.989 , 24.305 , 26.981 , 28.085 ,
   30.973 , 32.065 , 35.453 , 39.948 , 39.098 , 40.078 , 44.955 ,
   47.867 , 50.941 , 51.996 , 54.00 , 55.845 , 58.933 , 58.693 ,
   63.546 , 65.409 , 69.723 , 72.64 , 74.921 , 78.96 , 79.904 , 83.798
   , 85.467 , 87.62 , 88.905 , 91.224 , 92.906 , 95.94 , 98.00 ,
   101.07 , 102.90 , 106.42 , 107.86 , 112.41 , 114.81 , 118.71 ,
   121.76 , 127.60 , 126.90 , 131.29 , 132.90 , 137.32 , 138.90 ,
   140.11 , 140.90 , 144.24 , 145.00 , 150.36 , 151.96 , 157.25 ,
   158.92 , 162.50 , 164.93 , 167.25 , 168.93 , 173.04 , 174.96 ,
   178.49 , 180.94 , 183.84 , 186.20 , 190.23 , 192.21 , 195.07 ,
   196.96 , 200.59 , 204.38 , 207.2 , 208.98 , 209 , 210 , 222 , 223 ,
   226 , 227 , 232 , 231 , 238 , 237 , 244 , 243 , 247 , 247 , 251 ,
			 252 , 257 , 258 , 259 };


//Don't use this list of atomic masses, its leftover from when I was trying to
//verify my code against Baron's original version
/*
double atom_masses[103]={-1.00,
			 1 , 4.0026 , 6.941 , 9.0121 , 10.811 , 12 , 14.006 ,
			 15.999 , 18.998 , 20.179 , 22.989 , 24.305 , 26.981 , 28.085 ,
			 30.973 , 32.065 , 35.453 , 39.948 , 39.098 , 40.078 , 44.955 ,
			 47.867 , 50.941 , 51.996 , 54.00 , 55.845 , 58.933 , 58.693 ,
			 63.546 , 65.409 , 69.723 , 72.64 , 74.921 , 78.96 , 79.904 , 83.798
			 , 85.467 , 87.62 , 88.905 , 91.224 , 92.906 , 95.94 , 98.00 ,
			 101.07 , 102.90 , 106.42 , 107.86 , 112.41 , 114.81 , 118.71 ,
			 121.76 , 127.60 , 126.90 , 131.29 , 132.90 , 137.32 , 138.90 ,
			 140.11 , 140.90 , 144.24 , 145.00 , 150.36 , 151.96 , 157.25 ,
			 158.92 , 162.50 , 164.93 , 167.25 , 168.93 , 173.04 , 174.96 ,
			 178.49 , 180.94 , 183.84 , 186.20 , 190.23 , 192.21 , 195.07 ,
			 196.96 , 200.59 , 204.38 , 207.2 , 208.98 , 209 , 210 , 222 , 223 ,
			 226 , 227 , 232 , 231 , 238 , 237 , 244 , 243 , 247 , 247 , 251 ,
                         252 , 257 , 258 , 259 };

*/

double PTable::atom_mass(int anumber){
   if ( (anumber>0) && (anumber<103) ) {
      double mass=atom_masses[anumber];
      return mass;}
   else
   {      
      cout << endl <<"Now in file: :" <<__FILE__
	   << " at line: "<< __LINE__<< endl;
      cout << "Failed to find mass of atom: " << anumber << endl;
      exit(-1);
   }
}



int PTable::atom_number(string &aname){
   // The way it written it is a large loop, So be careful , 
   // dont use this a lot
   bool found= false;
   for (int a=1; a<103; a++){ 
      // given the atom name find atom number
      if (aname==atom_symbol[a]){ found=true; return a;} 
   }
   if (!found) {
      cout << endl <<"Now in file: :" <<__FILE__
	   << " at line: "<< __LINE__<< endl;
      cout << "Failed to find atom named : " << aname << endl;
      exit(-1);
   }
}


string PTable::atom_name(int aNumber){
   if ((aNumber<1)||(aNumber>102)) {
      cout << endl <<"Now in file: :" <<__FILE__
	   << " at line: "<< __LINE__<< endl;
      cout << "Failed to find atom with atomic number : " << aNumber << endl;
      exit(-1);
   }

   string name;
   name=atom_symbol[aNumber];
   return name;

}
