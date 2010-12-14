/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#ifndef LINE_H
#define LINE_H

#include <string>
#include <vector>
#include "./shapelib/shapefil.h"

//const double MAP_STEP24 = 2.1457672119140625e-005;
const double MAP_STEP24 = 1.0e-005;

class Coords {
public:
	double x;
	double y;
	bool	bound;

	Coords() {x=0;y=0;bound=false;};
	Coords(double x,double y) {this->x = x; this->y = y;bound=false;};

	bool operator==(Coords aCoords) { 
		int ax1 = int(x*1000000);
		int ay1 = int(y*1000000);
		int ax2 = int(aCoords.x*1000000);
		int ay2 = int(aCoords.y*1000000);
		if( ax1 == ax2 && ay1 == ay2 ) return true;
		return false;};
	Coords operator-(Coords aCoords) {
		Coords t_ret;
        t_ret.x = this->x - aCoords.x;
        t_ret.y = this->y - aCoords.y;
		return t_ret;
	};
	Coords operator+(Coords aCoords) {
		Coords t_ret;
        t_ret.x = this->x + aCoords.x;
        t_ret.y = this->y + aCoords.y;
		return t_ret;
	};
	Coords operator*(double factor) {
		Coords t_ret;
        t_ret.x = this->x *factor;
        t_ret.y = this->y *factor;
		return t_ret;
	};
};

std::ostream& operator<<(std::ostream& o,const Coords& coord);

/* dane o segmencie - numeracja etc */
class Segment {
public:
	int nodeStart; //poczatek danego segmentu od 0
	__int64 nodeId;
	int	bound;
	__int64	originalRgnID;

	//uzywane tylko przy revert
	__int64	nodeEnd;
	__int64	nodeIdEnd;

	//uzywane przez navteq - walidowanie skrzyzowan
	bool	valid_intersection;
	int		zLevel;

	std::string	leftCity;
	std::string	leftRegion;
	std::string	leftCountry;
	std::string	rightCity;
	std::string	rightRegion;
	std::string	rightCountry;

	std::string	leftZip;
	std::string	rightZip;

	int		leftStart;	//numeracja budynkow
	int		leftType;
	int		leftEnd;
	int		rightStart;
	int		rightType;
	int		rightEnd;

	Segment();
	Segment(int nodeStart);

	Segment(int nodeStart,std::string leftCity,std::string leftRegion,std::string leftCountry,
		std::string rightCity, std::string rightRegion, std::string rightCountry,std::string leftZip, std::string rightZip,
		int leftStart, int leftType, int leftEnd, int rightStart, int rightType, int rightEnd,__int64 originalRgnID = -1,int zLevel = 0);
	void Revert();

	bool operator() (const Segment &seg1, const Segment &seg2) {
		return seg1.nodeStart < seg2.nodeStart;
	};
};

class Line {
public:
	std::vector<Coords> points;
	std::vector<Segment> segments;

	//vector<int> intersecting_roads;

	__int64	rgnId;
	__int64	order;
	//standard attributes

	int		level;
	int		endLevel;
	int		type;
	int		direction;
	std::string	label;
	std::string	label3;
	std::string	description;
	std::string	city;
	std::string	region;
	std::string	country;
    std::string	zip;
	std::string restrictionMPC;
	int		oneWay;
	int		toll;
	int		speed;
	int		roadClass;

	int		StreetVE;
	int		StreetVD;
	int		StreetVC;
	int		StreetVB;
	int		StreetVT;
	int		StreetVP;
	int		StreetVI;
	int		StreetVR;
	int		RestrAttributes;

	int		zLevel;

	//tylko dla formatu MPC
	__int64	nonReferenceNode;
	__int64	referenceNode;


	double	minX,maxX,minY,maxY;
	double	length;
	bool	merged;

	/*
	czytanie danych wprost z SHP,
	dodawanie danych obiektow SHP od przodu lub do tylu*/
	Line(const Line &line,int split_point);
	Line();

	void RecalcMinMax();
	void InitIntersection();
	bool Merge( Line* aLine );
	void Revert();

	Segment* GetSegment(int node,bool &changeCoords,Coords &a1,Coords &a2,Coords &b1,Coords &b2,bool bound = false,bool no_distance_check = false);
	//Segment* GetSegment(int node,int &reducedRgn,Coords &reducedCoord,Coords &newCoord,int &nodeId1,int &nodeId2,bool bound = false);

	static	Coords	AlignToGrid(Coords coord);
	static	void	FixError(Coords &intCoord,Coords trueCoord);
	static	bool	GetDirectionDifference(Coords ap1,Coords ap2,Coords bp1,Coords bp2);//true jezeli 'podobny' kierunek
	static	int		GetDirectionDifferenceAngle(Coords ap1,Coords ap2,Coords bp1,Coords bp2);//true jezeli 'podobny' kierunek
	void	Smooth();
};


#endif
