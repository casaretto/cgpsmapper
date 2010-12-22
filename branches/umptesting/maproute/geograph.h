/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#ifndef GeographH
#define GeographH

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <math.h>
#include "line.h"

class Datum
{
public:
	double flattening;
	double f;
	double dF;
	double semiMajorAxis;
	double semiMinorAxis;

	double dX,dY,dZ,dA; //dF == f

	std::string  datumCode;
	std::string  datumName;

	//  Datum();
	Datum(const char* datumCode);
	Datum(double dX,double dY,double dZ,long double majorAxis,long double flattering);

	static void DatumList(std::vector<std::string> *list);

	//distance
	//  void calcDistanceNew(Coordinates a,Coordinates b,double &distance,double &direction);
	void calcDistance(Coords a,Coords b,double &distance,double &direction);
	void calcDistance(Coords a,Coords b,std::string &distance,double &direction);
	void calcDestination(Coords a,double distance,double direction,Coords &b);
private:
	//distance calculations...
	double tanU1,U1,sinU1,cosU1;
	double tanU2,U2,sinU2,cosU2;
	double azimut;

	double diffLong,lambda;
	double sin2s,coss,tans,s,sina,cos2sm,A,B,Ds,C;
	double distance,Ddistance;

	double tans1,sigma2m,tana2,Bsina,cos2sigmam,cossigmaetc,bracket1,bracket2;
	double u2,Dsigma,sigma,topTerm,bottomTerm,tanfi,fi,tanlambda,lambda2,L,a2;

	double lambdaDiff();
	void sigmaDiff();
};

#endif
