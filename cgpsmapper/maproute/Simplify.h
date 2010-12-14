/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#ifndef SimplifyH
#define SimplifyH

#include <vector>
#include "line.h"

class Simplify
{
public:
	//generalizacja obiektow - dotyczy RGN40 i RGN80
	//dane wejsciowe - vektor wsp. lub RGN40/RGN80
	//rgn_element_dest - obiekt docelowy - juz utworzony
	//rgn_element_src  - zrodlo
	//vector<Coordinates>* vector_src - jw
	//intersections[]  - tablica int wskazujaca ktore wektory MUSZA zostac zachowane - z powodu skrzyzowan
	//void TRE2_element::GeneralizeElement(RGN_element* rgn_element_dest,RGN_element* rgn_element_src,int intersections[]);
	static void GeneralizeElement(Coords refCoords,std::vector<Coords>* vector_desc,std::vector<Coords>* vector_src,std::vector<int>* intersect_src,float tolerance);
	static void simplifyDP(float tol, std::vector<Coords>* v, int j, int k, std::vector<int>* intersect_src);
	static int	correctFixed(std::vector<Coords>* v,std::vector<int>* intersect_src,int i);

	static void ConvertTo3Byte(Coords &coord);
	static Coords AlignToGrid(Coords coord,Coords refCoords);
protected:
	static void FixError(Coords &intCoord,Coords trueCoord,Coords refCoords);
	static bool show_progress;
	static int  total_points;
	static int  last_total_points;
};

#endif
