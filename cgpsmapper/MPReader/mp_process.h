#ifndef __MP_PROCESS
#define __MP_PROCESS

#include <vector>
#include "point.h"
#include ".\..\sqlite-source\sqlite3.h"
#include "wx\wx.h"
#include "wx\html\htmlwin.h"
#include "LuaPlus\LuaPlus.h"

struct MPLine_car_restriction {
	std::string restriction;

	void setRestrictions(std::string restr);

	void setDenied_emergency();
	void setDenied_delivery();
	void setDenied_car();
	void setDenied_bus();
	void setDenied_taxi();
	void setDenied_pedestrian();
	void setDenied_bicycle();
	void setDenied_truck();

	bool getDenied_emergency();
	bool getDenied_delivery();
	bool getDenied_car();
	bool getDenied_bus();
	bool getDenied_taxi();
	bool getDenied_pedestrian();
	bool getDenied_bicycle();
	bool getDenied_truck();
};

struct MPLine_restriction : public MPLine_car_restriction {
	int type;
	std::string	points;
	std::string	roads;
	std::string	params;

	MPLine_restriction() {type=0;};
};

struct MPLine_route : public MPLine_car_restriction {
	int speed;
	int road_class;
	int	one_way;
	int toll;

	MPLine_route() {speed=0;road_class=0;one_way=0;toll=0;};
};

struct MPLine_segment {
	int vertex;
	__int64 node_id;
	int bound;
	std::string leftCity;
	std::string leftRegion;
	std::string leftCountry;
	std::string rightCity;
	std::string rightRegion;
	std::string rightCountry;
	std::string leftZip; 
	std::string rightZip;
	int leftStart;
	int leftEnd;
	int leftType;
	int rightStart;
	int rightEnd;
	int rightType;
	
	int restriction_id;

	MPLine_segment() {
	vertex = 0;
	node_id = -1;
	bound = 0;
	leftStart = 0;
	leftEnd = 0;
	leftType = 0;
	rightStart = 0;
	rightEnd = 0;
	rightType = 0;
	restriction_id = -1;
	}
};

class MPLine {
protected:
	sqlite3		*MPbase;
	char		*s;
	const char	*tail;
	size_t		coord_size;
	double*		x;
	double*		y;

	bool	getVector(int poly_id,int layer,std::vector< poly_t > *polygons);
	bool	setVector(int poly_id,int layer,std::vector< poly_t > *polygons);

public:
	__int64			next_node_id;

	MPLine();
	~MPLine();
	void	setDB(sqlite3* _MPbase);
	bool	getLine(const int _id,const int layer,std::vector< poly_t > *polygons);
	bool	setLine(const int _id,const int layer,std::vector< poly_t > *polygons);
	bool	getLine(const int _id,point_int& start,point_int& end,int &vertex0);

	int		duplicateLine(int line_id);
	void	deleteLine(int line_id);

	bool	getSegment(int line_id,int vertex,MPLine_segment& segment,bool create_new = false);
	bool	setSegment(int line_id,int vertex,MPLine_segment& segment,bool insert = false);
	bool	getSegments(int line_id,std::vector<MPLine_segment>* segments);
	bool	setSegments(int line_id,std::vector<MPLine_segment>* segments);

	bool	splitLine(int line_id,int vertex,int &new_line_id);
	bool	selfIntersect(int line_id,bool split = true); //zwraca true jesli byl podzial, lub sam dzieli (jesli true)
	bool	selfIntersect(int line_id1, int line_id2);
	int		directionStart(const int line_id); // pomiêdzy pierwszymi punktami
	int		directionGlobal(const int line_id); //pierwszy i ostatni

	void	revert(int line_id);
	int		connectionPoint(int line_id_1,int line_id_2); //0 - brak, 10 - koniec 1, pocz¹tek 2, 11 - koniec z koncem etc..

	bool	mergeLine(int line_id_1,int line_id_2,int connection_type = -1); //todo

	//routing
	bool	generateRoutingAtPoint(int line_id,int vertex);
};

class MPPoly : public MPLine {
public:
	bool	getPoly(int poly_id,int layer,std::vector< poly_t > *polygons);
};

class MPMergeParams {
public:
	bool	merge;
	long	max_angle;
	bool	fix_minimum_distance;
	bool	generate_routing;
	bool	detect_bound;
	bool	new_id;
	bool	new_speed;
	bool	new_route_class;
};

class MPProcess {
protected:
	sqlite3		*MPbase;

	//lua part
	LuaPlus::LuaStateOwner state;
	wxHtmlWindow*	m_htmlSQL;

	bool		updateRestrictions();
	wxString	updateRestriction(int line_id,wxString restriction);
public:
	void	setDB(sqlite3* _MPbase) {MPbase = _MPbase;};
	bool	checkRouting(sqlite3* _MPbase);
	bool	splitRoadsAtSegments(sqlite3* _MPbase,wxApp* mainApp,wxGauge* m_progress);

	//Merge part
	void	mergeRoads(wxApp* _mainApp,wxGauge* m_progress,MPMergeParams* merge_params);

	//Check self-intersect
	void	checkIntersect(wxApp* _mainApp,wxGauge* m_progress);

	//Lua part
	int		doString(const char* script);
	void	registerLua(wxHtmlWindow* m_htmlSQL);
	void	setPrintLuaText(const char* text);
	void	setPrintLuaInt(int value);

	int		execQuery(const char* s);
	void	execSelect(const char* s);

};


#endif
