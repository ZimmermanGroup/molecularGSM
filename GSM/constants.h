#ifndef CONSTANTS_H
#define CONSTANTS_H

// unir conversions

const double ECHARGE=1.60217646e-19; // unit = C
const double EMASS=9.10938188e-31; // mass of electron kg
const double AMU=1.66053886e-27; // mass of proton kg

const double SPEEDc=299792458; // m/s light speed
const double HPLANCK=6.626068e-34; // h, Joule*sec

const double HARTREEtoKCAL=627.5095;
const double HARTREEtoEV=27.2116;
const double BOHRtoANG=0.52917720859;
const double ANGtoBOHR =1.0000000/BOHRtoANG;

const double NAVOGAD=6.0221415e+23; // mol^(-1)
const double RGAS=8.314472 ; // J/mol/K
const double KBOLTZ=RGAS/NAVOGAD; //boltzmann J/K

const double radToDegree =180.000/3.14159265;
const double degreetorad =3.14159/180.000;
const double ZERO = 0.00000000;
const double ONE  = 1.00000000;
const double PI   = 3.14159265;

const double CALtoJOULE=4.18400;

// omega^2 comes in units of Hartree/bohr/bohr/mass_og_hydrogen
// need to divide by 2*pi*c to get cm_inverse
//const define OMEGAtoCMINV= 1/200/PI/SPEEDc*sqrt(627.5*4184/(0.52918*0.52819)/1e-20*1.0e-3);
// AMU*Nav=1e-3
const double OMEGAtoCMINV= 5137.02;

#endif
