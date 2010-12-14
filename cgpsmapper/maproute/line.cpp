/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#include <assert.h>
#include "line.h"
#include "Config.h"
#include "geograph.h"
#include "Simplify.h"
#include "merge.h" // nodIdMapping , nextNodeId

using namespace std;

ostream& operator<<(ostream& o,const Coords& coord){ 
	o<<"("<<coord.x<<","<<coord.y<<")"; return o;
};

Segment::Segment() {
	valid_intersection = false;
	bound = 0;
	nodeId = -1;
	nodeStart = 0;
	leftCity = "";
	leftRegion = "";
	leftCountry = "";
	rightCity = "";
	rightRegion = "";
	rightCountry = "";
	leftZip = "";
	rightZip = "";
	leftStart = 0;
	leftType = 0;
	leftEnd = 0;
	rightStart = 0;
	rightType = 0;
	rightEnd = 0;
	originalRgnID = -1;
	zLevel = 0;
};

Segment::Segment(int nodeStart) {
	valid_intersection = false;
	bound = 0;
	nodeId = -1;
	this->nodeStart = nodeStart;
	leftCity = "";
	leftRegion = "";
	leftCountry = "";
	rightCity = "";
	rightRegion = "";
	rightCountry = "";
	leftZip = "";
	rightZip = "";
	leftStart = 0;
	leftType = 0;
	leftEnd = 0;
	rightStart = 0;
	rightType = 0;
	rightEnd = 0;
	originalRgnID = -1;
	zLevel = 0;
};

Segment::Segment(int nodeStart,string leftCity,string leftRegion,string leftCountry,
		string rightCity, string rightRegion, string rightCountry,string leftZip, string rightZip,
		int leftStart, int leftType, int leftEnd, int rightStart, int rightType, int rightEnd,__int64 originalRgnID,int zLevel) 
{
	valid_intersection = false;
	nodeId = -1;
	bound = 0;
	this->zLevel = zLevel;
	this->originalRgnID = originalRgnID;
	this->nodeStart = nodeStart;
	this->leftCity = leftCity;
	this->leftRegion = leftRegion;
	this->leftCountry = leftCountry;
	this->rightCity = rightCity;
	this->rightRegion = rightRegion;
	this->rightCountry = rightCountry;
	this->leftZip = leftZip;
	this->rightZip = rightZip;
	this->leftStart = leftStart;
	this->leftType = leftType;
	this->leftEnd = leftEnd;
	this->rightStart = rightStart;
	this->rightType = rightType;
	this->rightEnd = rightEnd;
};

void Segment::Revert() {
	int a = leftStart;
	leftStart = leftEnd;
	leftEnd = a;

	a = rightStart;
	rightStart = rightEnd;
	rightEnd = a;
/*
	a = nodeStart;
	nodeStart = nodeEnd;
	nodeEnd = a;
*/
	__int64 b = nodeId;
	nodeId = nodeIdEnd;
	nodeIdEnd = b;
}


/********************/
Line::Line() {
//	Coords new_coord;
	merged = false;
	rgnId = -1;
	order = -1;

	level = -1;
	endLevel = -1;
	rgnId = -1;
	direction = 0;
	oneWay = 0;
	toll = -1;
	speed = -1;
	roadClass = -1;
	RecalcMinMax();

	//Obsluga segmentow jest jeszcze wadliwa
	//this->segments.push_back(segment);
}

Line::Line(const Line &line,int split_point) {
	vector<Segment>::iterator segment_i;
	merged = true;
	rgnId = -1;
	order = -1;

	direction = line.direction;
	level = line.level;
	endLevel = line.endLevel;
	oneWay = line.oneWay;
	toll = line.toll;
	roadClass = line.roadClass;
	type = line.type;
	label = line.label;
	label3 = line.label3;
	description = line.description;
	city = line.city;
	region = line.region;
	country = line.country;
	zip = line.zip;
	speed = line.speed;

	int t_point = 0;
	while( t_point <= split_point ) {
		points.push_back(line.points[t_point]);
		t_point++;
	}

	segments = line.segments;
	if( segments.size() ) {
		segment_i = segments.begin();
		while( segment_i < segments.end() ) {
			if( (*segment_i).nodeStart > split_point ) {
				segments.erase(segment_i,segments.end());
				break;
			}
			segment_i++;
		}
	}
	RecalcMinMax();
}

void Line::RecalcMinMax() {
	double distance,direction;

	minX = 90;
	minY = 180;
	maxX = -90;
	maxY = -180;
	this->length = 0;

	int t_point = 0;
	while( t_point < (int)points.size() ) {
		minX = min(points[t_point].x,minX);
		minY = min(points[t_point].y,minY);
		maxX = max(points[t_point].x,maxX);
		maxY = max(points[t_point].y,maxY);
	
		if( t_point+1 < (int)points.size() ) {
			ConfigReader::WGS84.calcDistance( points[t_point], points[t_point+1], distance, direction);
			this->length += distance;
		}
		t_point++;
	}

	maxX += 0.0001;
	maxY += 0.0001;
	minX -= 0.0001;
	minY -= 0.0001;
}

int Line::GetDirectionDifferenceAngle(Coords ap1,Coords ap2,Coords bp1,Coords bp2) {
	double distance,direction1,direction2;

	ConfigReader::WGS84.calcDistance( ap1, ap2, distance, direction1);
	ConfigReader::WGS84.calcDistance( bp1, bp2, distance, direction2);

	if( abs((int)direction1-(int)direction2) <=	ConfigReader::ANGLE_DIFF ) return abs((int)direction1-(int)direction2);
	if( direction1 < direction2 ) {
		direction1+=360;
		if( abs((int)direction1-(int)direction2) <=	ConfigReader::ANGLE_DIFF ) return abs((int)direction1-(int)direction2);
	} else {
		direction2+=360;
		if( abs((int)direction1-(int)direction2) <=	ConfigReader::ANGLE_DIFF ) return abs((int)direction1-(int)direction2);
	}
	return 360;
}

bool Line::GetDirectionDifference(Coords ap1,Coords ap2,Coords bp1,Coords bp2) {
	double distance,direction1,direction2;

	ConfigReader::WGS84.calcDistance( ap1, ap2, distance, direction1);
	ConfigReader::WGS84.calcDistance( bp1, bp2, distance, direction2);

	if( abs((int)direction1-(int)direction2) <=	ConfigReader::ANGLE_DIFF ) return true;
	if( direction1 < direction2 ) {
		direction1+=360;
		if( abs((int)direction1-(int)direction2) <=	ConfigReader::ANGLE_DIFF ) return true;
	} else {
		direction2+=360;
		if( abs((int)direction1-(int)direction2) <=	ConfigReader::ANGLE_DIFF ) return true;
	}
	return false;
}


Coords Line::AlignToGrid(Coords coord) {
	Coords m_diff;
	Coords m_coord = coord;

	m_diff.x = int((coord.x - 10.0)/(MAP_STEP24)+((coord.x - 10.0)<0?(-0.5):0.5));
	m_diff.y = int((coord.y - 10.0)/(MAP_STEP24)+((coord.y - 10.0)<0?(-0.5):0.5));

	FixError(m_diff,coord);

	m_coord.x = 10.0 + (double(m_diff.x)*MAP_STEP24);
	m_coord.y = 10.0 + (double(m_diff.y)*MAP_STEP24);
	
	return m_coord;
}

void Line::FixError(Coords &intCoord,Coords trueCoord) {
	//2.1457672119140625e-005
	//sprawdze jaki blad - i bledy sasiednich
	Coords m_checkPoint;//TinyCoordinates m_checkPoint;
	Coords refCoords(10.0,10.0);
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
			//m_checkPoint = refCoords + (TinyCoordinates(intCoord) + TinyCoordinates(IntCoordinates(dx,dy))) * MAP_STEP24;
			m_checkPoint = refCoords;
			m_checkPoint.x = intCoord.x + dx * MAP_STEP24;
			m_checkPoint.y = intCoord.y + dy * MAP_STEP24;

			ConfigReader::WGS84.calcDistance( trueCoord, m_checkPoint, distance, direction);
			//datum.calcDistance(trueCoord,m_checkPoint,distance,direction);

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

void Line::Smooth() {
	vector<int> intersect_src;
	vector<Coords> vector_desc;
	vector<Segment>::iterator segment_i;
	int t_point;

	for( t_point = 0; t_point < (int)points.size(); t_point ++ ) {
		intersect_src.push_back(-1);
	}

	for( segment_i = segments.begin(); segment_i < segments.end(); segment_i++ ) {
		if( (*segment_i).nodeStart >= t_point ) {
			cout<<"Line : "<<(int)this->rgnId<<endl;
			cout<<"Segment :"<<(int)(*segment_i).nodeStart<<endl;
			cout<<"Points  :"<<(int)t_point<<endl;

			return;
		}
		intersect_src[(*segment_i).nodeStart] = (*segment_i).nodeStart+100;

	}

	Simplify::GeneralizeElement(Coords(10.0,10.0),&vector_desc,&points,&intersect_src,static_cast<float>(MAP_STEP24));

	t_point = 0;
	segment_i = segments.begin();
	for(vector<int>::iterator intersect_i = intersect_src.begin(); intersect_i < intersect_src.end(); intersect_i++ ) {
		if( (*intersect_i) >= 100 ) {
			(*segment_i).nodeStart = t_point;

			assert((*segment_i).nodeStart < vector_desc.size());

			segment_i++;
		}
		if( (*intersect_i) != 0 ) 
			t_point++;

	}
/*
	if( segment_i !=segments.end() ) {
		cout<<"minX: "<<this->minX<<" maxX: "<<this->maxX<<" minY: "<<this->minY<<" maxY: "<<this->maxY<<endl;
	}
*/
	assert(segment_i == segments.end() );
	points = vector_desc;
}

//do napisania - GetSegment ktory bedzie zwiekszal dystans!!
Segment* Line::GetSegment(int node,bool &changeCoords,Coords &a1,Coords &a2,Coords &b1,Coords &b2,bool bound,bool no_distance_check) {
	double distance, direction;
	double newdistance;
	Segment* ret_segment = NULL;
	vector<Coords>::iterator coord_i;
	vector<Segment>::iterator segment_i;

	vector<Segment>::iterator segment_b;
	//czy istnieje segment dla danego wezla? jezeli nie - trzeba stworzyc

	changeCoords = false;
	__int64 a = this->rgnId;

	for( segment_i = segments.begin(); segment_i < segments.end(); segment_i++ ) {
		if( (*segment_i).nodeStart > node ) {
			Segment newSegment(node);
			if( segment_i < segments.end() ) {
				segment_i = segments.insert(segment_i,newSegment);
				ret_segment = &(*segment_i);
			} else {
				segment_i = segments.begin();
				segments.push_back(newSegment);
				ret_segment = &(segments.back());
			}
			break;
		}
		if( (*segment_i).nodeStart == node ) {
			ret_segment = &(*segment_i);
			break;
		}
	}

	if( ret_segment == NULL ) {
		Segment newSegment(node);
		segments.push_back(newSegment);
		ret_segment = &(segments.back());
		segment_i = segments.end()-1;
	}

	segment_b = segment_i+1;
	if( segment_b < segments.end() ) {
		if( /*(*segment_b).nodeStart - 1 == node &&*/ (*segment_b).nodeId > -1 && ret_segment->nodeId > -1) {
			ConfigReader::WGS84.calcDistance( points[ret_segment->nodeStart], points[(*segment_b).nodeStart], newdistance, direction);
			if( newdistance < ConfigReader::CRITICAL_DISTANCE && no_distance_check == false) {
				changeCoords = true;
				distance = (ConfigReader::CRITICAL_DISTANCE - newdistance);
				distance /= 2.0;
				if( distance < ConfigReader::DIFF_DISTANCE )
					distance = ConfigReader::DIFF_DISTANCE;
				if( bound ) {
					distance *= 2.0;
					//distance += 10.0;
				}
				ConfigReader::WGS84.calcDestination(points[(*segment_b).nodeStart],distance,direction,b2);
				ConfigReader::WGS84.calcDistance( points[(*segment_b).nodeStart], points[ret_segment->nodeStart], distance, direction);
				if( bound == false ) {
					distance = (ConfigReader::CRITICAL_DISTANCE - newdistance)/2.0;
					if( distance < 0.5 )
						distance = 0.5;
					ConfigReader::WGS84.calcDestination(points[ret_segment->nodeStart],distance,direction,a2);
				} else
					a2 = points[ret_segment->nodeStart];
				a1 = points[ret_segment->nodeStart];
				b1 = points[(*segment_b).nodeStart];

				if( (*segment_b).bound && (*segment_i).bound ) {
					cout<<"Critical error - road has 2 bound points which are too close to each other -"<<endl
						<<"Bound points cannot be moved - so I am 'unbounding' one of the node"<<endl
						<<"Fix these points manually and remember to change coordinates in the other map too"<<endl;
					cout<<"RgnId: "<<this->rgnId<<endl;
					(*segment_b).bound = false;
					//changeCoords = false;
					//exit(10);
				}
				if( (*segment_b).bound )
					b2 = b1;
				if( (*segment_i).bound )
					a2 = a1;
				return ret_segment;
			}

		}
	}

	if( segment_i > segments.begin() ) {
		segment_b = segment_i-1;
		if( /*(*segment_b).nodeStart + 1 == node &&*/ (*segment_b).nodeId > -1 && ret_segment->nodeId > -1) {			
			ConfigReader::WGS84.calcDistance( points[ret_segment->nodeStart], points[(*segment_b).nodeStart], newdistance, direction);
			if( newdistance < ConfigReader::CRITICAL_DISTANCE && no_distance_check == false) {
				changeCoords = true;
				distance = (ConfigReader::CRITICAL_DISTANCE - newdistance);
				if( distance < ConfigReader::DIFF_DISTANCE )
					distance = ConfigReader::DIFF_DISTANCE;
				if( bound ) {
					distance *= 2.0;
					//distance += 10.0;
				}
				ConfigReader::WGS84.calcDestination(points[(*segment_b).nodeStart],distance,direction,b2);
				ConfigReader::WGS84.calcDistance( points[(*segment_b).nodeStart], points[ret_segment->nodeStart], distance, direction);
				if( bound == false ) {
					distance = (ConfigReader::CRITICAL_DISTANCE - newdistance)/2.0;
					if( distance < 0.5 )
						distance = 0.5;
					ConfigReader::WGS84.calcDestination(points[ret_segment->nodeStart],distance,direction,a2);
				} else
					a2 = points[node];
				a1 = points[ret_segment->nodeStart];
				b1 = points[(*segment_b).nodeStart];
				if( (*segment_b).bound && (*segment_i).bound ) {
					cout<<"Critical error - road has 2 bound points which are too close to each other -"<<endl
						<<"Bound points cannot be moved - so I am 'unbounding' one of the node"<<endl
						<<"Fix these points manually and remember to change coordinates in the other map too"<<endl;
					cout<<"RgnId: "<<this->rgnId<<endl;
					changeCoords = false;
					(*segment_b).bound = false;
					//exit(10);
				}
				if( (*segment_b).bound )
					b2 = b1;
				if( (*segment_i).bound )
					a2 = a1;
				return ret_segment;
			}
		}
	}
	return ret_segment;
}

void Line::Revert() {
	vector<Coords>::iterator coords_i = points.end()-1;
	vector<Coords> new_coords;
	bool changeCoords;
	Coords a1,a2,b1,b2;

	oneWay *= -1;

	while( coords_i >= points.begin() ) {
		new_coords.push_back((*coords_i));
		coords_i--;
	}

	points = new_coords;


	if( segments.size() ) {
		int t_point = (int)points.size()-1;
		vector<Segment>::iterator segment_i,segment_i2;
		vector<Segment> new_segments;
		//przed revert
/*
		cout<<"Points: "<<t_point<<endl;
		for( segment_i = segments.begin(); segment_i < segments.end(); segment_i++ ) {
			cout<<"N:"<<(*segment_i).nodeStart<<", ID:"<<(*segment_i).nodeId<<", L:"<<(*segment_i).leftStart<<"->"<<(*segment_i).leftEnd<<
				", R:"<<(*segment_i).rightStart<<"->"<<(*segment_i).rightEnd<<endl;
		}*/
		//1 - pierwszy i ostatni wezel musi miec zdefiniowane segmenty - jezeli brak,
		//    to trzeba dodac
		GetSegment(0,changeCoords,a1,a2,b1,b2,false,true);
		GetSegment(t_point,changeCoords,a1,a2,b1,b2,false,true);

		//2 - zamienienie nodeStart
		//  nodeStart = nodes - nodeStart
		for( segment_i = segments.begin(); segment_i < segments.end(); segment_i++ ) {
			Segment new_segment = (*segment_i);
			new_segment.nodeStart = t_point - (*segment_i).nodeStart;
			new_segments.push_back( new_segment );
		}

		//3 - przesuwam informacje o numeracji do poprzedniego wezla
		segment_i2 = segments.begin();
		segment_i = new_segments.begin();
		segment_i++;
		for( ; segment_i < new_segments.end(); segment_i++ ) {

			(*segment_i).rightCity = (*segment_i2).leftCity;
			(*segment_i).rightCountry = (*segment_i2).leftCountry;
			(*segment_i).rightRegion = (*segment_i2).leftRegion;
			(*segment_i).rightZip = (*segment_i2).leftZip;
			(*segment_i).rightStart = (*segment_i2).leftEnd;
			(*segment_i).rightEnd = (*segment_i2).leftStart;
			(*segment_i).rightType = (*segment_i2).leftType;

			(*segment_i).leftCity = (*segment_i2).rightCity;
			(*segment_i).leftCountry = (*segment_i2).rightCountry;
			(*segment_i).leftRegion = (*segment_i2).rightRegion;
			(*segment_i).leftZip = (*segment_i2).rightZip;
			(*segment_i).leftStart = (*segment_i2).rightEnd;
			(*segment_i).leftEnd = (*segment_i2).rightStart;
			(*segment_i).leftType = (*segment_i2).rightType;

			//(*segment_i).nodeStart = t_point - (*segment_i2).nodeStart;
			segment_i2++;
		}
		segment_i = new_segments.begin();
		(*segment_i).leftStart = 0;
		(*segment_i).leftEnd = 0;
		(*segment_i).leftType = 0;

		(*segment_i).rightStart = 0;
		(*segment_i).rightEnd = 0;
		(*segment_i).rightType = 0;

		//4 - odwracam kolejnosc wezlow
		segments.clear();
		for( segment_i = new_segments.end() - 1; segment_i >= new_segments.begin(); segment_i--) {
			Segment new_segment = (*segment_i);
			segments.push_back( new_segment );
		}
		//segments = new_segments;
/*
		for( segment_i = segments.begin(); segment_i < segments.end(); segment_i++ ) {
			cout<<"N:"<<(*segment_i).nodeStart<<", ID:"<<(*segment_i).nodeId<<", L:"<<(*segment_i).leftStart<<"->"<<(*segment_i).leftEnd<<
				", R:"<<(*segment_i).rightStart<<"->"<<(*segment_i).rightEnd<<endl;
		}
*/
	}
}

bool Line::Merge( Line* aLine ) {
	Segment*	segment_this;
	Segment*	segment_aLine;

	vector<Segment>::iterator segment_i;
	vector<Coords>::iterator c_1,c_2;
	int				t_point = 0;
	bool			changeCoords;
	Coords			a1,a2,b1,b2;

	if( this->segments.size() == 0 )
		this->GetSegment(0,changeCoords,a1,a2,b1,b2,false,true);

	segment_this = this->GetSegment(0,changeCoords,a1,a2,b1,b2);

	if( aLine->segments.size() == 0 )
		aLine->GetSegment(0,changeCoords,a1,a2,b1,b2,false,true);

	if( points[0] == aLine->points[aLine->points.size()-1] ) {
		//nie moze byc ZADNEGO INNEGO WSPOLNEGO PUNKTU!!
		for( c_1 = points.begin() ; c_1 != points.end(); c_1++ ) {
			for( c_2 = aLine->points.begin() ; c_2 != aLine->points.end(); c_2++ ) {
				if( (*c_1) == (*c_2) && !( c_1 == points.begin() && c_2 == aLine->points.end()-1) )
					return false;
			}
		}
		//
			for( segment_i = aLine->segments.begin(); segment_i != aLine->segments.end(); segment_i++ ) {
			(*segment_i).nodeStart += 0;
			if( (*segment_i).originalRgnID < 0 )
				(*segment_i).originalRgnID = aLine->rgnId;
		}

		t_point = (int)aLine->points.size()-1;
		if( aLine->segments.size() ) {
			//segment_i = aLine->segments.end()-1;
			if( aLine->segments.back().nodeStart == t_point ) {
				segment_aLine = aLine->GetSegment(t_point,changeCoords,a1,a2,b1,b2);

				if( segment_aLine->nodeId > -1 && segment_this->nodeId < 0 )
					segment_this->nodeId = segment_aLine->nodeId;
				else if( segment_aLine->nodeId < 0 && segment_this->nodeId > -1 )
					segment_aLine->nodeId = segment_this->nodeId; // bez znaczenia - zaraz usuwam ten segment
				else if( segment_aLine->nodeId > -1 && segment_this->nodeId > -1 && segment_aLine->nodeId != segment_this->nodeId) {
					LineMerge::nodIdMapping[segment_this->nodeId] = LineMerge::nextNodeId;
					LineMerge::nodIdMapping[segment_aLine->nodeId] = LineMerge::nextNodeId;
					segment_this->nodeId = LineMerge::nextNodeId;
					segment_aLine->nodeId = LineMerge::nextNodeId;
					LineMerge::nextNodeId++;
				}
				aLine->segments.erase(aLine->segments.end()-1);
			}
		}

		/*
				nodIdMapping[segment_1->nodeId] = nextNodeId;
				nodIdMapping[segment_2->nodeId] = nextNodeId;

				segment_1->nodeId = nextNodeId;
				segment_2->nodeId = nextNodeId;
				nextNodeId++;
*/

		for( segment_i = segments.begin(); segment_i != segments.end(); segment_i++ ) {
			(*segment_i).nodeStart += (int)aLine->points.size()-1;

			if( (*segment_i).originalRgnID < 0 )
				(*segment_i).originalRgnID = this->rgnId;
		}

		aLine->segments.insert(aLine->segments.end(),segments.begin(),segments.end());
		segments = aLine->segments;

		t_point = 1;
		while( t_point < (int)points.size() ) {
			aLine->points.push_back(points[t_point]);
			t_point++;
		}
		points = aLine->points;
	} else {
		//
		for( c_1 = points.begin() ; c_1 != points.end(); c_1++ ) {
			for( c_2 = aLine->points.begin() ; c_2 != aLine->points.end(); c_2++ ) {
				if( (*c_1) == (*c_2) && !( c_1 == points.end()-1 && c_2 == aLine->points.begin()) )
					return false;
			}
		}

		for( segment_i = segments.begin(); segment_i != segments.end(); segment_i++ ) {
			(*segment_i).nodeStart += 0;

			if( (*segment_i).originalRgnID < 0 )
				(*segment_i).originalRgnID = this->rgnId;
		}

		t_point = (int)points.size()-1;
		if( segments.size() ) {
			if( segments.back().nodeStart == t_point ) {
				segment_this = this->GetSegment(t_point,changeCoords,a1,a2,b1,b2);
				segment_aLine = aLine->GetSegment(0,changeCoords,a1,a2,b1,b2);

				if( segment_aLine->nodeId > -1 && segment_this->nodeId < 0 )
					segment_this->nodeId = segment_aLine->nodeId;
				else if( segment_aLine->nodeId < 0 && segment_this->nodeId > -1 )
					segment_aLine->nodeId = segment_this->nodeId; // bez znaczenia - zaraz usuwam ten segment
				else if( segment_aLine->nodeId > -1 && segment_this->nodeId > -1 && segment_aLine->nodeId != segment_this->nodeId) {
					LineMerge::nodIdMapping[segment_this->nodeId] = LineMerge::nextNodeId;
					LineMerge::nodIdMapping[segment_aLine->nodeId] = LineMerge::nextNodeId;
					segment_aLine->nodeId = LineMerge::nextNodeId;
					segment_this->nodeId = LineMerge::nextNodeId;
					LineMerge::nextNodeId++;
				}

				segments.erase(segments.end()-1);
			}
		}

		for( segment_i = aLine->segments.begin(); segment_i != aLine->segments.end(); segment_i++ ) {
			(*segment_i).nodeStart += (int)points.size()-1;

			if( (*segment_i).originalRgnID < 0 )
				(*segment_i).originalRgnID = aLine->rgnId;
		}

		segments.insert(segments.end(),aLine->segments.begin(),aLine->segments.end());

		t_point = 1;
		while( t_point < (int)aLine->points.size() ) {
			this->points.push_back(aLine->points[t_point]);
			t_point++;
		}
	}

	for( segment_i = segments.begin(); segment_i != segments.end(); segment_i++ ) {
		(*segment_i).nodeStart = (*segment_i).nodeStart;
	}
	this->maxX = max(maxX,aLine->maxX);
	this->minX = min(minX,aLine->minX);
	this->maxY = max(maxY,aLine->maxY);
	this->minY = min(minY,aLine->minY);
	this->length += aLine->length;
	return true;
}