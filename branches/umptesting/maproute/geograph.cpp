/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/


#ifdef WIN32
#define  _USE_MATH_DEFINES
#endif

#define MAX_ITER 100000

#include <math.h>
#include "geograph.h"
#include "datums.h"

using namespace std;

Datum::Datum(double dX,double dY,double dZ,long double majorAxis,long double flattering)
{
  this->datumCode = "CUST";
  datumName = "CUST : Custom Datum";
  this->dX = dX;
  this->dY = dY;
  this->dZ = dZ;

  double wgs84f = 298.257223563;
  double wgs84major = 6378137.0;
  dF = 10000.0/wgs84f - 10000.0/flattering;
  f = 1.0/flattering;
  flattening = flattering;

  dA = wgs84major - majorAxis;
  semiMajorAxis = majorAxis;
  semiMinorAxis = semiMajorAxis * (1-f);
}

Datum::Datum(const char* _datumC)
{
  string datumC = _datumC;
  int t_dat = 0;
  int t_elp = 0;
  while( string(Datum_list[t_dat].code) != "END" && string(Datum_list[t_dat].code) != datumC )
    t_dat++;
  if( string(Datum_list[t_dat].code) == "END" )
  {
    Datum("W84");
    return;
  }
  while( (string(Elipsoid_list[t_elp].name)) != "END" && (string(Elipsoid_list[t_elp].code)) != (string(Datum_list[t_dat].elipsoid)) )
    t_elp++;
  this->datumCode = Datum_list[t_dat].code;
  datumName = string(Datum_list[t_dat].code) + " :" + string(Datum_list[t_dat].name) + " " + string(Datum_list[t_dat].country);
  dX = Datum_list[t_dat].dX;
  dY = Datum_list[t_dat].dY;
  dZ = Datum_list[t_dat].dZ;

  double wgs84f = 298.257223563;
  double wgs84major = 6378137.0;
  dF = 10000.0/wgs84f - 10000.0/Elipsoid_list[t_elp].flattering;
  f = 1.0/Elipsoid_list[t_elp].flattering;
  flattening = Elipsoid_list[t_elp].flattering;

  dA = wgs84major - Elipsoid_list[t_elp].majorAxis;
  semiMajorAxis = Elipsoid_list[t_elp].majorAxis;
  semiMinorAxis = semiMajorAxis * (1-f);
}

void Datum::DatumList(vector<string> *list)
{
  int t_dat = 0;

  list->clear();
  while( (string(Datum_list[t_dat].code)) != "END" )
  {
    list->push_back(string(Datum_list[t_dat].code) + " :" + string(Datum_list[t_dat].name) + " " + string(Datum_list[t_dat].country));
    t_dat++;
  }
}


void Datum::calcDistance(Coords a,Coords b,double &distance,double &direction)
{
  double a_lat_r,a_lon_r,b_lat_r,b_lon_r,tana;

  lambda = 0;

  if( a == b )
  {
    distance = 0;
    direction = 0;
    return;
  }

  //this->datum = datum;

  a_lat_r = a.y * M_PI /180.0;
  a_lon_r = a.x * M_PI /180.0;
  b_lat_r = b.y * M_PI /180.0;
  b_lon_r = b.x * M_PI /180.0;

  tanU1 = (1.0-f)*tan(a_lat_r);
  U1 = atan(tanU1);
  sinU1 = sin( U1 );
  cosU1 = cos( U1 );
  tanU2 = (1.0-f)*tan(b_lat_r);
  U2 = atan( tanU2 );
  sinU2 = sin( U2 );
  cosU2 = cos( U2 );

  lambda = b_lon_r-a_lon_r;
  diffLong = b_lon_r-a_lon_r;

  Ddistance = 0;
  distance = lambdaDiff();

  //tana=cosU2 * sin( lambda ) / (cosU1 * sinU2 - sinU1 * cosU2 * cos( lambda ));
  tana = atan2( cosU2* sin(lambda), (cosU1 * sinU2 - sinU1 * cosU2 * cos( lambda )));
  //direction = atan( tana ); //radiany
  //direction = (direction / M_PI ) * 180.0 + 360.0;
  if( tana < 0 )
	  direction = tana * 180.0 / M_PI + 360.0;
  else
	  direction = tana * 180.0 / M_PI;
}

double Datum::lambdaDiff()
{
	int iter = 0;
	do
	{
		distance = Ddistance;
		sin2s =( cosU2 * sin( lambda ) * cosU2 * sin( lambda ))+( cosU1 * sinU2 - sinU1 * cosU2 * cos( lambda ))*( cosU1 * sinU2 - sinU1 * cosU2 * cos( lambda ));
		coss = ( sinU1 * sinU2 )+( cosU1 * cosU2 * cos( lambda ));
		tans = sqrt( sin2s )/coss;
		s = atan( tans );
		if( sin2s == 0 )
			sina = 0;
		else
			sina = cosU1 * cosU2 * sin( lambda )/sqrt( sin2s );
		if( (cos(asin( sina ))*cos(asin( sina ))) == 0 )
			cos2sm = 0;
		else
			cos2sm = coss - 2.0 * sinU1 * sinU2 /( cos( asin( sina )) * cos( asin( sina )));
		U2 = cos(asin( sina ))*cos(asin( sina ))*(semiMajorAxis * semiMajorAxis - semiMinorAxis*semiMinorAxis)/(semiMinorAxis*semiMinorAxis);
		iter++;

		if( iter > MAX_ITER )
			break;

		A = 1.0 + U2/16384.0*(4096.0+ U2 *(-768.0 + U2 *(320.0-175.0* U2 )));
		B = U2 /1024.0*(256.0+ U2 *(-128.0+ U2 *(74.0-47.0* U2 )));
		Ds = B * sqrt( sin2s )*( cos2sm + B /4.0 * ( coss *(-1.0 + 2.0 * cos2sm * cos2sm )- B / 6.0 * cos2sm *(-3.0 + 4.0 * sin2s )*(-3.0 + 4.0 * cos2sm * cos2sm )));
		C = f/16.0 * cos(asin( sina )) * cos(asin( sina ))*(4.0+f*(4.0-3.0 * cos(asin( sina )) * cos(asin( sina ))));
		lambda = diffLong + (1.0 - C) * f * sina *(acos( coss )+ C * sin(acos( coss ))*( cos2sm + C * coss *(-1.0 + 2.0 * cos2sm * cos2sm )));

		Ddistance = semiMajorAxis * A *( atan( tans )- Ds );
	}
	while( fabs(Ddistance - distance) > fabs(Ddistance) * 0.05 );
	return Ddistance;
}

void Datum::calcDestination(Coords src,double distance,double direction,Coords &dest)
{
  double a_lat_r,a_lon_r;

  // = datum.flattening * (1.0 - datum.f);
  this->distance = 0;
  lambda = 0;

  this->distance = distance;
  a_lat_r = src.y * M_PI/180.0;
  a_lon_r = src.x * M_PI/180.0;
  azimut = direction * M_PI/180.0;

  tanU1 = (1-f) * tan(a_lat_r);
  tans1 = tanU1 / cos(azimut);

  sinU1 = sin(atan(tanU1));
  cosU1 = cos(atan(tanU1));
  sina = cosU1 * sin(azimut);

  u2 = cos(asin(sina)) * cos(asin(sina)) * ((semiMajorAxis * semiMajorAxis)-(semiMinorAxis * semiMinorAxis))/(semiMinorAxis * semiMinorAxis);
  A = 1.0 + u2 / 16384.0 * (4096.0 + u2 *(-768.0 + u2 * (320.0 - 175.0 * u2)));
  B = u2 / 1024.0 * (256.0 + u2 * (-128.0 + u2 * (74.0 - 47.0 * u2)));

  sigma = distance / (semiMinorAxis * A);
  sigmaDiff();

  topTerm = sin(atan(tanU1)) * cos(sigma) + cos(atan(tanU1)) * sin(sigma) * cos(azimut);
  bottomTerm = (1.0 - f) * sqrt(sina * sina + (sin(atan(tanU1)) *
    sin(sigma) - cos(atan(tanU1)) * cos(sigma) * cos(azimut)) * (sin(atan(tanU1)) *
    sin(sigma) - cos(atan(tanU1)) * cos(sigma) * cos(azimut)));

  tanfi = topTerm/bottomTerm;
  fi = atan(tanfi);
  dest.y = fi * 180.0 / M_PI;

  tanlambda = sin(sigma) * sin(azimut)/(cosU1 * cos(sigma) - sin(atan(tanU1)) * sin(sigma)*cos(azimut));
  lambda = atan2(sin(sigma) * sin(azimut),(cosU1 * cos(sigma) - sin(atan(tanU1)) * sin(sigma) * cos(azimut)));
  C = f/16.0 * cos(asin(sina)) * cos(asin(sina)) * (4.0 + f * (4.0 - 3.0 * cos(asin(sina)) * cos(asin(sina))));
  L = lambda - (1.0 - C) * f * sina * (sigma + C * sin(sigma) * (cos(sigma2m) + C * cos(sigma) * (-1.0 + 2.0 * cos(sigma2m) * cos(sigma2m))));
  lambda2 = a_lon_r + L;
  dest.x = lambda2 * 180.0 /M_PI;
}

void Datum::sigmaDiff()
{
  double Doldsigma;
  Dsigma = 0;
  do
  {
    Doldsigma = Dsigma;

    sigma2m = 2.0 * atan(tans1) + sigma;
    Bsina = B * sin(sigma);
    cos2sigmam = cos(sigma2m);
    cossigmaetc = cos(sigma) * (-1.0 + 2.0 * cos2sigmam * cos2sigmam );
    bracket1 = -3.0 + 4.0 * sin(sigma) * sin(sigma);
    bracket2 = -3.0 + 4.0 * cos2sigmam * cos2sigmam;

    Dsigma = Bsina * (cos2sigmam + B/4.0 * (cossigmaetc - B/6.0 * cos2sigmam * bracket1 * bracket2));
    sigma = distance / (semiMinorAxis * A) + Dsigma;
  } while (fabs(Doldsigma - Dsigma) > 0.0000001);
}


