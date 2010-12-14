/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#include <assert.h>
#include "simplify.h"
#include "geograph.h"

using namespace std;

#define dot(u,v)   ((u).y * (v).y + (u).x * (v).x)
#define norm2(v)   dot(v,v)        // norm2 = squared length of vector
#define norm(v)    sqrt(norm2(v))  // norm = length of vector
#define d2(u,v)    norm2(u-v)      // distance squared = norm2 of difference
#define d(u,v)     norm(u-v)       // distance = norm of difference


bool Simplify::show_progress = false;
int  Simplify::total_points = 0;
int  Simplify::last_total_points = 0;

static  Datum	localWGS84("WGS84");

//intersect_src - >= 1 - zachowac node - wartosc w tablicy tez trzeba zachowac!!!
//
void Simplify::GeneralizeElement(
								 Coords refCoords,
								 //unsigned char zoom_factor,
								 vector<Coords>* sV,
								 vector<Coords>* vector_src,
								 vector<int>* intersect_src,
								 float tolerance)
{
	//tolerancja - oczko 'grida'

	// dot product (3D) which allows vector operations in arguments

	// poly_simplify():
	//    Input:  tol = approximation tolerance
	//            V[] = polyline array of vertex points
	//            n   = the number of points in V[]
	//    Output: sV[] = simplified polyline vertices (max is n)
	//    Return: m   = the number of points in sV[]
	//int poly_simplify( float tol, Point* V, int n, Point* sV )

	//float tol = MAP_STEP(zoom_factor) - MAP_STEP(zoom_factor)/4;

	//zmiany tolerance tutaj:
	tolerance *= 0.3;

	vector<int> intersect;
	bool  b_intersect = false;

	int		i, pv;            // misc counters
	Coords	last_coord;
	float  tol2 = tolerance * tolerance;       // tolerance squared
	vector<Coords> vector_desc;
	//Point* vt = new Point[n];      // vertex buffer
	//int*   mk = new int[n] = {0};  // marker buffer

	// STAGE 1.  Vertex Reduction within tolerance of prior vertex cluster

	// start at the beginning - zmienic - poczatek liczyc wg AlignGrid!
	// vector_src  -- V
	// vector_desc -- vt

	//if( refCoords == Coords(0.0,0.0) )
	//	vector_desc.push_back((*vector_src)[0]);
	//else
		vector_desc.push_back(AlignToGrid((*vector_src)[0],refCoords));
	intersect.push_back( (*intersect_src)[0] >= 1 ? 1 : 0  );
	/*  if( show_progress == true ) {
	cout << "\rGen";
	}
	*/
	for (i = 1, pv=0; i < (int)vector_src->size(); i++) {
		if ( (d2((*vector_src)[i], (*vector_src)[pv]) < tol2) )	{
			if( (*intersect_src)[i] >= 1) 
				b_intersect = true;	
		} 
		b_intersect |= ((*intersect_src)[i] >= 1 );

		//cout<<AlignToGrid((*vector_src)[i],refCoords)<<",";
		/*
		if( AlignToGrid((*vector_src)[i],refCoords) == vector_desc.back() ) {
			if( b_intersect == true ) {
				if( intersect.back() == 0 ) {
					
					intersect_src->erase( intersect_src->begin() + pv );
					vector_src->erase( vector_src->begin() + pv );

					intersect.pop_back();
					vector_desc.pop_back();
				} 
				intersect.push_back( b_intersect ? 1 : 0 );
				vector_desc.push_back(AlignToGrid((*vector_src)[i],refCoords));
			} else {
				intersect_src->erase( intersect_src->begin() + i );
				vector_src->erase( vector_src->begin() + i );
			}
		} else {*/
			intersect.push_back( b_intersect ? 1 : 0 );
			vector_desc.push_back(AlignToGrid((*vector_src)[i],refCoords));
		//}

		b_intersect = false;
		pv = i;
	}
	if (pv < (int)vector_src->size()-1) {
		vector_desc.push_back(AlignToGrid((*vector_src)[vector_src->size()-1],refCoords));
		intersect.push_back(1);
	}

	// STAGE 2.  Douglas-Peucker polyline simplification
	intersect[0] = intersect[vector_desc.size()-1] = 1;       // mark the first and last vertices

	if( vector_desc.size() > 500 )
		show_progress = true;
	else
		show_progress = false;
	total_points = (int)vector_desc.size()-1;

	simplifyDP( tolerance, &vector_desc, 0, static_cast<int>(vector_desc.size()-1), &intersect );

	// copy marked vertices to the output simplified polyline
	last_coord = Coords(180.0,180.0);
	for (i = 0; i < (int)vector_desc.size(); i++)
	{
		//(*intersect_src)[i] = intersect[i];
		if (intersect[i] == 1 ) {
			sV->push_back(vector_desc[i]);
		/*		
			if( !(last_coord == vector_desc[i]) ) {
				sV->push_back(vector_desc[i]);
				last_coord = vector_desc[i];
			} else {
				if( (*intersect_src)[i] >= 100 )
					if( (*intersect_src)[i-1] < 100)
						(*intersect_src)[i-1] = (*intersect_src)[i];
					else
						assert(false);
				(*intersect_src)[i] = 0;
			}
		*/
		} else
			(*intersect_src)[i] = 0;
	}
	//cout << "\r              ";
}

// simplifyDP():
//  This is the Douglas-Peucker recursive simplification routine
//  It just marks vertices that are part of the simplified polyline
//  for approximating the polyline subchain v[j] to v[k].
//    Input:  tol = approximation tolerance
//            v[] = polyline array of vertex points
//            j,k = indices for the subchain v[j] to v[k]
//    Output: mk[] = array of markers matching vertex array v[]
void Simplify::simplifyDP( float tol, vector<Coords>* v, int j, int k, vector<int>* intersect_src )
{

	if (k <= j+1) // there is nothing to simplify
		return;

	// check for adequate approximation by segment S from v[j] to v[k]
	int         maxi = j;          // index of vertex farthest from S
	double      maxd2 = 0;         // distance squared of farthest vertex
	double      tol2 = tol * tol;  // tolerance squared
	Coords P0 = (*v)[j];
	Coords P1 = (*v)[k];
	Coords u = P1 - P0;   // segment direction vector
	double      cu = dot(u,u);     // segment length squared

	// test each vertex v[i] for max distance from S
	// compute using the Feb 2001 Algorithm's dist_Point_to_Segment()
	// Note: this works in any dimension (2D, 3D, ...)

	//vector  w;
	Coords   w;
	Coords   Pb;                // base of perpendicular from v[i] to S
	double  b, cw, dv2;        // dv2 = distance v[i] to S squared

	for (int i=j+1; i<k; i++) {
		// compute distance squared
		w = (*v)[i] - P0;
		cw = dot(w,u);
		if ( cw <= 0 )
			dv2 = d2((*v)[i], P0);
		else if ( cu <= cw )
			dv2 = d2((*v)[i], P1);
		else {
			b = cw / cu;
			Pb = P0 + u * b;
			dv2 = d2((*v)[i], Pb);
		}
		// test with current max distance squared
		if (dv2 <= maxd2)
			continue;
		// v[i] is a new max vertex
		maxi = correctFixed(v,intersect_src,i);
		maxd2 = (dv2);
	}
	if (maxd2 > tol2)        // error is worse than the tolerance
	{
		// split the polyline at the farthest vertex from S
		(*intersect_src)[maxi] = 1;      // mark v[maxi] for the simplified polyline
		// recursively simplify the two subpolylines at v[maxi]
		simplifyDP( tol, v, j, maxi, intersect_src );  // polyline v[j] to v[maxi]
		simplifyDP( tol, v, maxi, k, intersect_src );  // polyline v[maxi] to v[k]
	}
	// else the approximation is OK, so ignore intermediate vertices
	return;
}

// Nie wolno zatrzymac wspolrzednych o tych samych koordynatach, jezeli juz
// sasiednie sa zatrzymane i maja te same koordynaty!
int	Simplify::correctFixed(vector<Coords>* v,vector<int>* intersect_src,int i) {
	int j;
	
	for( j = i-1; j >= 0; --j ) {
		if( !((*v)[j] == (*v)[i]) )
			break;
		if( (*intersect_src)[j] > 0 )
			return j;
	}

	for( j = i+1; j < (*intersect_src).size(); ++j ) {
		if( !((*v)[j] == (*v)[i]) )
			break;
		if( (*intersect_src)[j] > 0 )
			return j;
	}
	return i;
}

#undef dot
#undef norm2
#undef norm
#undef d2
#undef d

void Simplify::ConvertTo3Byte(Coords &coord) {
	coord.x = int((coord.x * (double(1 << 30) / double(90.0))) / 256);
	coord.x = (coord.x * 256.0) / (double(1 << 30) / double(90.0));

	coord.y = int((coord.y * (double(1 << 30) / double(90.0))) / 256);
	coord.y = (coord.y * 256.0) / (double(1 << 30) / double(90.0));
}

Coords Simplify::AlignToGrid(Coords coord,Coords refCoords)
{
	Coords m_diff;
	//IntCoordinates m_diff;
	Coords m_coord = coord;
	static Coords t_refCoords = coord;
	static bool initialized = false;
	
	if( initialized == false ) {
		t_refCoords.x = int(t_refCoords.x *10.0)/10.0;
		t_refCoords.y = int(t_refCoords.y *10.0)/10.0;

		ConvertTo3Byte(t_refCoords);
		initialized = true;
	}
	
	m_diff.y = int((m_coord.y - t_refCoords.y)/MAP_STEP24+((m_coord.y - t_refCoords.y)<0?(-0.5):0.5));
	m_diff.x = int((m_coord.x - t_refCoords.x)/MAP_STEP24+((m_coord.x - t_refCoords.x)<0?(-0.5):0.5));

	FixError(m_diff,coord,t_refCoords);

	m_coord.y = t_refCoords.y + (double(m_diff.y)*MAP_STEP24);
	m_coord.x = t_refCoords.x + (double(m_diff.x)*MAP_STEP24);
	
	return m_coord;
}

void Simplify::FixError(Coords &intCoord,Coords trueCoord,Coords refCoords)
{
	//sprawdze jaki blad - i bledy sasiednich
	Coords m_checkPoint;
	//Datum datum("W84");
	double distance,direction;

	double      min_error = 999999999;
	int         dx,dy;
	int         best_dx,best_dy;
	best_dx = 0;
	best_dy = 0;
	for(dx = -1; dx < 2; dx++ )
	{
		for(dy = -1; dy < 2; dy++ )
		{
			//m_checkPoint = refCoords + (intCoord + Coords(IntCoordinates(dx,dy))) * MAP_STEP(zoom_factor);
			m_checkPoint.x = refCoords.x + (intCoord.x + (int)dx) * MAP_STEP24;
			m_checkPoint.y = refCoords.y + (intCoord.y + (int)dy) * MAP_STEP24;

			localWGS84.calcDistance(trueCoord,m_checkPoint,distance,direction);

			if( distance < min_error )
			{
				min_error = distance;
				best_dx = dx;best_dy = dy;
			}
		}
	}
	if( best_dx || best_dy )
	{
		intCoord.y += best_dy;
		intCoord.x += best_dx;
	}
}
