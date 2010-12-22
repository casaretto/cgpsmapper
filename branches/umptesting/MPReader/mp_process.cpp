#include <string>
#include <vector>
#include "error.h"
#include "mp_process.h"
#include "sql_helpers.h"
#include "wx/tokenzr.h"

using namespace std;

void MPLine_car_restriction::setRestrictions(std::string restr) {
	restriction = restr;
}

void MPLine_car_restriction::setDenied_emergency() {
	if( !getDenied_emergency() )
		restriction.append(" emergency");
}
void MPLine_car_restriction::setDenied_delivery() {
	if( !getDenied_delivery() )
		restriction.append(" delivery");
}
void MPLine_car_restriction::setDenied_car() {
	if( !getDenied_car() )
		restriction.append(" car");
}
void MPLine_car_restriction::setDenied_bus() {
	if( !getDenied_bus() )
		restriction.append(" bus");
}
void MPLine_car_restriction::setDenied_taxi() {
	if( !getDenied_taxi() )
		restriction.append(" taxi");
}
void MPLine_car_restriction::setDenied_pedestrian() {
	if( !getDenied_pedestrian() )
		restriction.append(" pedestrian");
}
void MPLine_car_restriction::setDenied_bicycle() {
	if( !getDenied_bicycle() )
		restriction.append(" bicycle");
}
void MPLine_car_restriction::setDenied_truck() {
	if( !getDenied_truck() )
		restriction.append(" truck");
}

bool MPLine_car_restriction::getDenied_emergency() {
	if( !restriction.size() ) return false;
	if( restriction.find("emergency") < restriction.size() ) return true;
	return false;
}
bool MPLine_car_restriction::getDenied_delivery(){
	if( !restriction.size() ) return false;
	if( restriction.find("delivery")< restriction.size() ) return true;
	return false;
}
bool MPLine_car_restriction::getDenied_car(){
	if( !restriction.size() ) return false;
	if( restriction.find("car") < restriction.size() ) return true;
	return false;
}
bool MPLine_car_restriction::getDenied_bus(){
	if( !restriction.size() ) return false;
	if( restriction.find("bus") < restriction.size() ) return true;
	return false;
}
bool MPLine_car_restriction::getDenied_taxi(){
	if( !restriction.size() ) return false;
	if( restriction.find("taxi") < restriction.size() ) return true;
	return false;
}
bool MPLine_car_restriction::getDenied_pedestrian(){
	if( !restriction.size() ) return false;
	if( restriction.find("pedestrian") < restriction.size() ) return true;
	return false;
}
bool MPLine_car_restriction::getDenied_bicycle(){
	if( !restriction.size() ) return false;
	if( restriction.find("bicycle") < restriction.size() ) return true;
	return false;
}
bool MPLine_car_restriction::getDenied_truck(){
	if( !restriction.size() ) return false;
	if( restriction.find("truck") < restriction.size() ) return true;
	return false;
}

/*******************************************/

bool MPPoly::getPoly(int poly_id,int layer,std::vector< poly_t > *polygons) {
	s = sqlite3_mprintf("SELECT coordinates, parts_size , parts FROM MP_POLY where id = %i;",poly_id);
	bool t_ret = getVector(poly_id,layer,polygons);
	sqlite3_free(s);

	return t_ret;
}

/*******************************************/
MPLine::MPLine() {
	coord_size = 1000;

	x = new double[coord_size];
	y = new double[coord_size];
}

MPLine::~MPLine() {
	delete []x;
	delete []y;
}

void MPLine::setDB(sqlite3* _MPbase) {
	MPbase = _MPbase;
}

bool MPLine::getVector(const int poly_id,const int layer,std::vector< poly_t > *polygons) {
	sqlite3_stmt*	ppStmt;
	int				parts_size;
	int				a;
	size_t			tp_array;
	bool			t_ret = false;

	polygons->clear();

	int rc = sqlite3_prepare_v2(MPbase,s,-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {

			parts_size = sqlite3_column_int(ppStmt,1);
			int*	parts_array = new int[parts_size];
			int		new_max_coord_size = 0;

			memcpy(parts_array, sqlite3_column_blob(ppStmt, 2 ), parts_size * sizeof(int) );
			for( a = 0; a < (sqlite3_column_int(ppStmt,1)/2); a++ ) {
				if( parts_array[a*2] == layer ) // eksportujê tylko layer 0
					if( new_max_coord_size < parts_array[a*2+1] )
						new_max_coord_size = parts_array[a*2+1];
			}
			if( coord_size < new_max_coord_size ) {
				delete []x;
				delete []y;

				coord_size = new_max_coord_size;
				x = new double[new_max_coord_size];
				y = new double[new_max_coord_size];
			}

			new_max_coord_size = 0;
			for( a = 0; a < (parts_size/2); a++ ) {

				tp_array = parts_array[a*2+1]*sizeof( double );
				if( parts_array[a*2] != layer ) {
					new_max_coord_size += (tp_array * 2);
					continue;
				}
				t_ret = true;
				memcpy(x, &sqlite3_column_text(ppStmt, 0 )[new_max_coord_size], tp_array );
				memcpy(y, &sqlite3_column_text(ppStmt, 0 )[new_max_coord_size+tp_array], tp_array );

				new_max_coord_size += (tp_array * 2);

				poly_t vp;

				for( int c = 0; c < parts_array[a*2+1]; c++ ) {
					vp._points.push_back(point_t(x[c],y[c]));
				}
				polygons->push_back(vp);
			}
			
			delete[] parts_array;
			
			rc = sqlite3_step( ppStmt );
		}
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

	return t_ret;
}

bool MPLine::setVector(const int poly_id,const int layer,std::vector< poly_t > *lines) {
	int		rc;
	int		parts_size = 0;
	bool	t_ret = false;
	sqlite3_stmt*	ppStmt;


	for( std::vector< poly_t >::iterator t_l = lines->begin(); t_l != lines->end(); t_l++ ) {
		parts_size +=2;
	}

	int*	parts_array = new int[parts_size];
	int		t_total_size = 0;
	int		t_coord_ptr = 0;
	int		part = 0;

	for( std::vector< poly_t >::iterator i_p = lines->begin(); i_p != lines->end(); i_p ++ ) {
		parts_array[part*2] = layer;
		parts_array[part*2+1] = (*i_p)._points.size();
		part++;
		t_total_size += (*i_p)._points.size();
	}

	double* coord_array = new double[t_total_size*2];
	//copy coordinates

	std::vector< poly_t >::iterator t_single_line;
	vector_points::iterator t_i;
	for( t_single_line = lines->begin(); t_single_line != lines->end(); t_single_line ++ ) {
		for( t_i = (*t_single_line)._points.begin(); t_i < (*t_single_line)._points.end(); t_i++ ) {
			coord_array[t_coord_ptr] = t_i->x;
			t_coord_ptr++;
		}
		for( t_i = (*t_single_line)._points.begin(); t_i < (*t_single_line)._points.end(); t_i++ ) {
			coord_array[t_coord_ptr] = t_i->y;
			t_coord_ptr++;
		}
	}

	rc = sqlite3_prepare_v2(MPbase, s, -1, &ppStmt, &tail); 
	rc = sqlite3_bind_blob(ppStmt, 1, parts_array , parts_size * sizeof(int), SQLITE_STATIC);
	rc = sqlite3_bind_blob(ppStmt, 2, coord_array , t_total_size*2*sizeof( double ), SQLITE_STATIC);
	rc = sqlite3_step(ppStmt);
	if( rc == SQLITE_DONE )
		t_ret = true;
	sqlite3_finalize( ppStmt );

	delete []parts_array;
	delete []coord_array;

	return t_ret;
}

bool MPLine::setLine(const int line_id,const int layer,std::vector< poly_t > *lines) {
	rect_t	approx;
	int		parts_size = 0;
	int		vertex0 = 0;
	int		s_x,s_y,e_x,e_y;

	
	for( std::vector< poly_t >::iterator t_l = lines->begin(); t_l != lines->end(); t_l++ ) {
		approx.Extend( (*t_l) );
		parts_size +=2;
	}

	if( layer == 0 && lines->size() ) {
		vertex0 = lines->at(0)._points.size();

		s_x = (lines->at(0)._points.at(0).x* 10000000.0);
		s_y = (lines->at(0)._points.at(0).y* 10000000.0);

		e_x = ((*((lines->at(lines->size()-1)._points.end()-1))).x* 10000000.0);
		e_y = ((*((lines->at( lines->size()-1 )._points.end()-1))).y* 10000000.0);

		s = sqlite3_mprintf("UPDATE mp_line SET s_x = %i, s_y = %i, e_x = %i, e_y = %i,parts_size = %i, parts = ?, coordinates = ?, vertex0 = %i, min_X = %f,min_Y = %f,max_X = %f,max_Y = %f WHERE id = %i;",
		s_x, s_y, e_x, e_y,
		parts_size,vertex0,approx.x0,approx.y0,approx.x1,approx.y1,line_id);
	} else {
		s = sqlite3_mprintf("UPDATE mp_line SET parts_size = %i, parts = ?, coordinates = ?, vertex0 = %i, min_X = %f,min_Y = %f,max_X = %f,max_Y = %f WHERE id = %i;",
		parts_size,vertex0,approx.x0,approx.y0,approx.x1,approx.y1,line_id);
	}
	bool t_ret = setVector(line_id,layer,lines);
	sqlite3_free(s);

	return t_ret;
}

bool MPLine::getLine(const int _id,point_int& start,point_int& end,int &vertex0) {
	sqlite3_stmt*	ppStmt;

	s = sqlite3_mprintf("select s_x,s_y,e_x,e_y,vertex0 FROM MP_LINE where id = %i and to_delete <>1;",_id);
	int rc = sqlite3_prepare_v2(MPbase,s,-1,&ppStmt,NULL);
	sqlite3_free(s);
	rc = sqlite3_step( ppStmt );
	if ( rc == SQLITE_ROW ) {
		start.x = sqlite3_column_int(ppStmt,0);
		start.y = sqlite3_column_int(ppStmt,1);

		end.x = sqlite3_column_int(ppStmt,2);
		end.y = sqlite3_column_int(ppStmt,3);

		vertex0 = sqlite3_column_int(ppStmt,4);
	} else {
		sqlite3_finalize( ppStmt );
		return false;
	}
	sqlite3_finalize( ppStmt );
			
	return true;
}


bool MPLine::getLine(const int poly_id,const int layer,std::vector< poly_t > *polygons) {
	s = sqlite3_mprintf("SELECT coordinates, parts_size , parts FROM MP_LINE where id = %i and to_delete <> 1;",poly_id);
	bool t_ret = getVector(poly_id,layer,polygons);
	sqlite3_free(s);

	return t_ret;
}

int	MPLine::directionStart(const int line_id) {
	std::vector< poly_t > lines;

	getLine(line_id,0,&lines);

	if( lines.size() ) {
		if( lines[0]._points.size() > 1 ) {
			return Direction(lines[0]._points.at(0),lines[0]._points.at(1));
		}
	}
	return -1;
}

int	MPLine::directionGlobal(const int line_id) {
	point_int	_i1,_i2;
	point_t		_1,_2;
	int	v0;

	if( !getLine(line_id,_i1,_i2,v0) )
		return -1;

	_1.x = ( (double)_i1.x / 10000000.0 );
	_1.y = ( (double)_i1.y / 10000000.0 );

	_2.x = ( (double)_i2.x / 10000000.0 );
	_2.y = ( (double)_i2.y / 10000000.0 );

	return Direction(_1,_2);
}

bool MPLine::generateRoutingAtPoint(int line_id,int vertex) {
	sqlite3_stmt*	ppStmt;
	MPLine_segment	segment_1;
	MPLine_segment	segment_2;
	point_t			cross;
	std::vector< poly_t > lines_1;
	std::vector< poly_t > lines_2;
	int				vertex_2;
	vector_points::iterator i_l2;

	getSegment(line_id,vertex,segment_1,true);
	if( segment_1.node_id > -1 )
		return true;

	segment_1.node_id  = next_node_id++;
	getLine(line_id,0,&lines_1);
	if( lines_1[0]._points.size()-1 < vertex )
		return false;

	setSegment(line_id,vertex,segment_1);
	cross = lines_1[0]._points.at(vertex);

	s = sqlite3_mprintf("select id FROM MP_LINE where min_x <= %f and min_y <= %f and max_x >= %f and max_y >= %f and id <> %i;",cross.x,cross.y,cross.x,cross.y,line_id);
	int rc = sqlite3_prepare_v2(MPbase,s,-1,&ppStmt,NULL);
	sqlite3_free(s);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			getLine(sqlite3_column_int(ppStmt,0),0,&lines_2);
			
			vertex_2 = 0;
			for( i_l2 = lines_2[0]._points.begin(); i_l2 != lines_2[0]._points.end(); i_l2++,vertex_2++ ) {
				if( (*i_l2) == cross ) {
					//punkt styku
					getSegment(sqlite3_column_int(ppStmt,0),vertex_2,segment_2,true);
					segment_2.node_id = segment_1.node_id;
					setSegment(sqlite3_column_int(ppStmt,0),vertex_2,segment_2);
				}
			}

			rc = sqlite3_step( ppStmt );
		}
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );
			
	return true;
}

bool MPLine::getSegments(int line_id,std::vector<MPLine_segment>* segments) {
	sqlite3_stmt*	ppStmt;
	char			*s;
	bool			t_result = false;
	size_t			coord_size[10];

	s = sqlite3_mprintf("select node_id, bound, leftcity, leftRegion , leftCountry , " \
		" rightCity , rightRegion , rightCountry , leftZip , rightZip , leftStart , leftEnd , leftType , " \
		" rightStart , rightEnd , rightType, vertex from MP_SEGMENT where line_id = %i ORDER BY vertex;",line_id);
	int rc = sqlite3_prepare_v2(MPbase,s,-1,&ppStmt,NULL);
	sqlite3_free(s);

	segments->clear();

	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			MPLine_segment segment;
			segment.node_id = sqlite3_column_int(ppStmt,0);
			segment.bound = sqlite3_column_int(ppStmt,1);
			segment.leftCity = ( sqlite3_column_text(ppStmt,2) ? (char*)sqlite3_column_text(ppStmt,2) : "");
			segment.leftRegion = ( sqlite3_column_text(ppStmt,3) ? (char*)sqlite3_column_text(ppStmt,3) : "");
			segment.leftCountry = ( sqlite3_column_text(ppStmt,4) ? (char*)sqlite3_column_text(ppStmt,4) : "");
			segment.rightCity = ( sqlite3_column_text(ppStmt,5) ? (char*)sqlite3_column_text(ppStmt,5) : "");
			segment.rightRegion = ( sqlite3_column_text(ppStmt,6) ? (char*)sqlite3_column_text(ppStmt,6) : "");
			segment.rightCountry = ( sqlite3_column_text(ppStmt,7) ? (char*)sqlite3_column_text(ppStmt,7) : "");

			segment.leftZip = ( sqlite3_column_text(ppStmt,8) ? (char*)sqlite3_column_text(ppStmt,8) : "");
			segment.rightZip = ( sqlite3_column_text(ppStmt,9) ? (char*)sqlite3_column_text(ppStmt,9) : "");

			segment.leftStart = sqlite3_column_int(ppStmt,10);
			segment.leftEnd = sqlite3_column_int(ppStmt,11);
			segment.leftType = sqlite3_column_int(ppStmt,12);

			segment.rightStart = sqlite3_column_int(ppStmt,13);
			segment.rightEnd = sqlite3_column_int(ppStmt,14);
			segment.rightType = sqlite3_column_int(ppStmt,15);
			segment.vertex = sqlite3_column_int(ppStmt,16);

			t_result = true;

			segments->push_back(segment);

			rc = sqlite3_step( ppStmt );
		}
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

	return t_result;
}

bool MPLine::setSegments(int line_id,std::vector<MPLine_segment>* segments) {
	s = sqlite3_mprintf("DELETE FROM MP_SEGMENT WHERE line_id = %i;",line_id);
	int rc = sqlite3_exec(MPbase,s,NULL,NULL,NULL);
	sqlite3_free(s);

	for( vector<MPLine_segment>::iterator i_segment = segments->begin(); i_segment != segments->end(); i_segment++ ) {
		setSegment(line_id,(*i_segment).vertex,(*i_segment),true);	
	}
	return true;
}

bool MPLine::setSegment(int line_id,int vertex,MPLine_segment& segment,bool insert) {
	if( insert == false ) {
		s = sqlite3_mprintf("SELECT count(*) FROM MP_SEGMENT WHERE line_id = %i and vertex = %i;",line_id,vertex);
		insert = (getSelectCount(MPbase,s) == 0);
		sqlite3_free(s);
	}
	if( insert ) {
		s = sqlite3_mprintf("INSERT INTO MP_SEGMENT(node_id,bound,leftCity,leftRegion,leftCountry, " \
			    "rightCity,rightRegion,rightCountry,leftZip,rightZip,leftStart,leftEnd,leftType, " \
				" rightStart,rightEnd ,rightType,line_id,vertex) VALUES(%lld, %i,'%q','%q', '%q', " \
				" '%q', '%q', '%q', '%q', '%q', %i, %i, %i, " \
				"  %i, %i, %i, %i,%i) ",
				segment.node_id, segment.bound, 
				( segment.leftCity.size() ? segment.leftCity.c_str(): ""), 
				( segment.leftRegion.size() ? segment.leftRegion.c_str():""),
				( segment.leftCountry.size() ? segment.leftCountry.c_str():""),
				( segment.rightCity.size() ? segment.rightCity.c_str():""), 
				( segment.rightRegion.size() ? segment.rightRegion.c_str():""), 
				( segment.rightCountry.size() ? segment.rightCountry.c_str():""),
				( segment.leftZip.size() ? segment.leftZip.c_str():"" ),
				( segment.rightZip.size() ? segment.rightZip.c_str():""),
				segment.leftStart,segment.leftEnd, segment.leftType,
				segment.rightStart,segment.rightEnd, segment.rightType,
				line_id, segment.vertex);

	} else {
		s = sqlite3_mprintf("UPDATE MP_SEGMENT set node_id = %lld, bound = %i, leftCity = '%q', leftRegion = '%q', leftCountry = '%q', " \
				" rightCity = '%q', rightRegion = '%q', rightCountry = '%q', leftZip = '%q', rightZip = '%q', leftStart = %i, leftEnd = %i, leftType = %i, " \
				" rightStart = %i, rightEnd = %i, rightType = %i WHERE line_id = %i AND vertex =  %i; ",
				segment.node_id, segment.bound, 
				( segment.leftCity.size() ? segment.leftCity.c_str(): ""), 
				( segment.leftRegion.size() ? segment.leftRegion.c_str():""),
				( segment.leftCountry.size() ? segment.leftCountry.c_str():""),
				( segment.rightCity.size() ? segment.rightCity.c_str():""), 
				( segment.rightRegion.size() ? segment.rightRegion.c_str():""), 
				( segment.rightCountry.size() ? segment.rightCountry.c_str():""),
				( segment.leftZip.size() ? segment.leftZip.c_str():"" ),
				( segment.rightZip.size() ? segment.rightZip.c_str():""),
				segment.leftStart,segment.leftEnd, segment.leftType,
				segment.rightStart,segment.rightEnd, segment.rightType,
				line_id, segment.vertex);
	}
	int rc = sqlite3_exec(MPbase,s,NULL,NULL,NULL);
	sqlite3_free(s);
	return !rc?true:false;
}


bool MPLine::getSegment(int line_id,int vertex,MPLine_segment& segment,bool create_new) {
	sqlite3_stmt*	ppStmt;
	char			*s;
	bool			t_result = false;
	size_t			coord_size[10];

	s = sqlite3_mprintf("select node_id, bound, leftcity, leftRegion , leftCountry , " \
		" rightCity , rightRegion , rightCountry , leftZip , rightZip , leftStart , leftEnd , leftType , " \
		" rightStart , rightEnd , rightType, vertex from MP_SEGMENT where line_id = %i and vertex = %i;",line_id, vertex);
	int rc = sqlite3_prepare_v2(MPbase,s,-1,&ppStmt,NULL);
	sqlite3_free(s);

	rc = sqlite3_step( ppStmt );
	if ( rc == SQLITE_ROW ) {
		segment.node_id = sqlite3_column_int(ppStmt,0);
		segment.bound = sqlite3_column_int(ppStmt,1);
		segment.leftCity = ( sqlite3_column_text(ppStmt,2) ? (char*)sqlite3_column_text(ppStmt,2) : "");
		segment.leftRegion = ( sqlite3_column_text(ppStmt,3) ? (char*)sqlite3_column_text(ppStmt,3) : "");
		segment.leftCountry = ( sqlite3_column_text(ppStmt,4) ? (char*)sqlite3_column_text(ppStmt,4) : "");
		segment.rightCity = ( sqlite3_column_text(ppStmt,5) ? (char*)sqlite3_column_text(ppStmt,5) : "");
		segment.rightRegion = ( sqlite3_column_text(ppStmt,6) ? (char*)sqlite3_column_text(ppStmt,6) : "");
		segment.rightCountry = ( sqlite3_column_text(ppStmt,7) ? (char*)sqlite3_column_text(ppStmt,7) : "");

		segment.leftZip = ( sqlite3_column_text(ppStmt,8) ? (char*)sqlite3_column_text(ppStmt,8) : "");
		segment.rightZip = ( sqlite3_column_text(ppStmt,9) ? (char*)sqlite3_column_text(ppStmt,9) : "");

		segment.leftStart = sqlite3_column_int(ppStmt,10);
		segment.leftEnd = sqlite3_column_int(ppStmt,11);
		segment.leftType = sqlite3_column_int(ppStmt,12);

		segment.rightStart = sqlite3_column_int(ppStmt,13);
		segment.rightEnd = sqlite3_column_int(ppStmt,14);
		segment.rightType = sqlite3_column_int(ppStmt,15);
		segment.vertex = sqlite3_column_int(ppStmt,16);

		t_result = true;
	}
	sqlite3_finalize( ppStmt );

	if( t_result || !create_new )
		return t_result;

	//NEW!
	/*s = sqlite3_mprintf("select coordinates_size, coordinates from mp_line where id = %i;",line_id);
	rc = sqlite3_prepare_v2(MPbase,s,-1,&ppStmt,NULL);
	sqlite3_free(s);

	rc = sqlite3_step( ppStmt );
	if ( rc == SQLITE_ROW ) {
		memcpy(coord_size, sqlite3_column_blob(ppStmt, 0 ), sizeof( coord_size ) );
		if( coord_size[0] < vertex ) {
			sqlite3_finalize( ppStmt );
			return t_result;
		}
	}
	sqlite3_finalize( ppStmt );
*/
	s = sqlite3_mprintf("INSERT INTO MP_SEGMENT (line_id, vertex ) VALUES (%i, %i) ;",line_id, vertex);
	rc = sqlite3_exec(MPbase,s,NULL,NULL,NULL);
	sqlite3_free(s);
	if( rc )
		return false;

	segment.node_id = -1;
	segment.bound = 0;
	segment.leftCity = "";
	segment.leftRegion = "";
	segment.leftCountry = "";
	segment.rightCity = "";
	segment.rightRegion = "";
	segment.rightCountry = "";

	segment.leftZip = "";
	segment.rightZip = "";

	segment.leftStart = 0;
	segment.leftEnd = 0;
	segment.leftType = 0;

	segment.rightStart = 0;
	segment.rightEnd = 0;
	segment.rightType = 0;

	return true;
}

int	MPLine::connectionPoint(int line_id_1,int line_id_2) { //100 - brak, 10 - koniec 1, pocz¹tek 2, 11 - koniec z koncem etc..
	std::vector< poly_t > lines_1;
	std::vector< poly_t > lines_2;
	MPLine_segment	segment_1;
	MPLine_segment	segment_2;
	int				t_ret = 4;
	
	point_int	_s1,_e1,_s2,_e2;
	int			v1,v2;

	if( line_id_1 == line_id_2 )
		return 4;

	if( !getLine(line_id_1,_s1,_e1,v1) )
		return 4;
	if( !getLine(line_id_2,_s2,_e2,v2) )
		return 4;

	v1--;
	v2--;

	if( v1 + v2 > 60 ) {
		s = sqlite3_mprintf("select count(*) from MP_segment where line_id = %i or line_id = %i;",line_id_1,line_id_2);
		if( getSelectCount(MPbase,s) > 60 ) {
			sqlite3_free(s);
			return 4;
		}
		sqlite3_free(s);
	}

	if( _s1 == _s2 /*lines_1[0].at(0) == lines_2[0].at(0)*/ ) {
		if( getSegment(line_id_1,0,segment_1,false) && getSegment(line_id_2,0,segment_2,false) ) {
			if( segment_1.node_id != segment_2.node_id && segment_1.node_id > -1 && segment_2.node_id > -1 )
				return 4;
		}
		t_ret = 0; //00
	}

	if( _e1 == _s2 /*lines_1[0].at( lines_1[0].size() - 1 ) == lines_2[0].at(0)*/ ) {
		if( getSegment(line_id_1,v1,segment_1,false) && getSegment(line_id_2,0,segment_2,false) ) {
			if( segment_1.node_id != segment_2.node_id && segment_1.node_id > -1 && segment_2.node_id > -1 )
				return 4;
		}
		if( t_ret < 4 )
			return 4;
		t_ret = 2; //10
	}

	if( _e1 == _e2 /*lines_1[0].at( lines_1[0].size() - 1 ) == lines_2[0].at( lines_2[0].size() - 1 )*/ ){
		if( getSegment(line_id_1,v1,segment_1,false) && getSegment(line_id_2,v2,segment_2,false) ) {
			if( segment_1.node_id != segment_2.node_id && segment_1.node_id > -1 && segment_2.node_id > -1 )
				return 4;
		}
		if( t_ret < 4 )
			return 4;
		t_ret = 3; //11
	}

	if( _s1 == _e2 /*lines_1[0].at( 0) == lines_2[0].at( lines_2[0].size() - 1 )*/ ){
		if( getSegment(line_id_1,0,segment_1,false) && getSegment(line_id_2,v2,segment_2,false) ) {
			if( segment_1.node_id != segment_2.node_id && segment_1.node_id > -1 && segment_2.node_id > -1 )
				return 4;
		}
		if( t_ret < 4 )
			return 4;
		t_ret = 1; //01
	}

	return t_ret; //100
}

bool MPLine::selfIntersect(int line_id,bool split) {
	int		split_vertex;
	__int64	node_id;
	int		min_vertex, max_vertex;

	min_vertex = -1;
	max_vertex = -1;

	s = sqlite3_mprintf("select node_id from mp_segment where line_id = %i group by node_id having count(node_id)>1;",line_id);
	node_id = getSelectCount64(MPbase,s);
	sqlite3_free(s);
	if( node_id < 0 )
		return false;
	
	s = sqlite3_mprintf("select min(vertex) from mp_segment where line_id = %i and node_id = %lld;",line_id, node_id);
	min_vertex = getSelectCount(MPbase,s);
	sqlite3_free(s);

	s = sqlite3_mprintf("select max(vertex) from mp_segment where line_id = %i and node_id = %lld;",line_id, node_id);
	max_vertex = getSelectCount(MPbase,s);
	sqlite3_free(s);

	if( min_vertex > -1 && max_vertex > -1 ) {
		s = sqlite3_mprintf("select vertex from mp_segment where line_id = %i and vertex > %i and vertex < %i;",line_id, min_vertex, max_vertex);
		split_vertex = getSelectCount(MPbase,s);
		sqlite3_free(s);

		if( split_vertex < 0 ) { // brak wezla pomiedzy! 
			if( max_vertex - min_vertex == 1 ) {
				//po prostu usunac NOD
				s = sqlite3_mprintf("delete from mp_segment where line_id = %i and vertex = %i;",line_id, max_vertex);
				int rc = sqlite3_exec(MPbase,s,NULL,NULL,NULL);
				sqlite3_free(s);
				return false;
			}
			assert(false);
			return false;
		}
		int new_line_id;
		if( splitLine(line_id,split_vertex,new_line_id) ) {
			selfIntersect(new_line_id);
			selfIntersect(line_id);
		}
		return true;
	}
	assert(false);
	return false;
}

bool MPLine::selfIntersect(int line_id1, int line_id2) {
	//zliczenie wspolnych NOD - moze byc tylko jeden wiersz z podwojnym NODem
	bool	t_ret = false;
	
	s = sqlite3_mprintf("select count(*) from  (select count(node_id) from mp_segment where line_id = %i or line_id = %i group by node_id having count(node_id) > 1 );",line_id1, line_id2);
	if( getSelectCount(MPbase,s) > 1 )
		t_ret = true;
	sqlite3_free(s);
	return t_ret;
}

bool MPLine::mergeLine(int line_id_1,int line_id_2,int connection_type) {
	bool t_ret = false;
	static std::vector< poly_t > lines_1;
	static std::vector< poly_t > lines_2;
	vector_points::iterator	i_line;
	static vector<MPLine_segment> segments_1;
	static vector<MPLine_segment> segments_2;
	vector<MPLine_segment>::iterator segment_i;
	int	start_vertex;

	if( line_id_1 == line_id_2 )
		return false;
	if( !getLine(line_id_1,0,&lines_1) )
		return false;
	if( !getLine(line_id_2,0,&lines_2) )
		return false;

	if( lines_1.size() > 1 ) {
		//wszystkie po³¹czyæ w jedno!
		poly_t t_line;
		for( std::vector< poly_t >::iterator t_l = lines_1.begin(); t_l != lines_1.end(); t_l++ ) {
			t_line._points.insert(t_line._points.end(),(*t_l)._points.begin(), (*t_l)._points.end() );
		}
		lines_1.clear();
		lines_1.push_back(t_line);
	}

	if( lines_2.size() > 1 ) {
		//wszystkie po³¹czyæ w jedno!
		poly_t t_line;
		for( std::vector< poly_t >::iterator t_l = lines_2.begin(); t_l != lines_2.end(); t_l++ ) {
			t_line._points.insert(t_line._points.end(),(*t_l)._points.begin(), (*t_l)._points.end() );
		}
		lines_2.clear();
		lines_2.push_back(t_line);
	}


	if( connection_type < 0 )
		connection_type = connectionPoint(line_id_1,line_id_2);



	if( connection_type == 3 ) { // 11 koniec/koniec
		s = sqlite3_mprintf("SELECT one_way FROM mp_line WHERE id = %i;",line_id_1);
		if( getSelectCount(MPbase,s) < 0 ) {
			sqlite3_free(s);
			revert(line_id_1);
			connection_type = 1; //01
			if( !getLine(line_id_1,0,&lines_1) )
				return false;		
		} else {
			sqlite3_free(s);

			s = sqlite3_mprintf("SELECT one_way FROM mp_line WHERE id = %i;",line_id_2);
			if( getSelectCount(MPbase,s) < 0 ) {
				sqlite3_free(s);

				revert(line_id_2);
				connection_type = 2; //10
				if( !getLine(line_id_2,0,&lines_2) )
					return false;		
			} else
				sqlite3_free(s);
		}

		if( connection_type == 3 ) {
			revert(line_id_1);
			connection_type = 1; //01
			if( !getLine(line_id_1,0,&lines_1) )
				return false;		
		}
	}

	if( connection_type == 0 ) { // 00 pocz¹tek/pocz¹tek
		s = sqlite3_mprintf("SELECT one_way FROM mp_line WHERE id = %i;",line_id_1);
		if( getSelectCount(MPbase,s) < 0 ) {
			sqlite3_free(s);
			revert(line_id_1);
			connection_type = 2; //10
			if( !getLine(line_id_1,0,&lines_1) )
				return false;		
		} else {
			sqlite3_free(s);

			s = sqlite3_mprintf("SELECT one_way FROM mp_line WHERE id = %i;",line_id_2);
			if( getSelectCount(MPbase,s) < 0 ) {
				sqlite3_free(s);

				revert(line_id_2);
				connection_type = 1; //01
				if( !getLine(line_id_2,0,&lines_2) )
					return false;		
			} else
				sqlite3_free(s);
		}

		if( connection_type == 0 ) {
			revert(line_id_1);
			connection_type = 2; //10
			if( !getLine(line_id_1,0,&lines_1) )
				return false;		
		}
	}

	if( connection_type == 2 ) { // 10 koniec pierwszego/pocz¹tek 
		start_vertex = lines_1[0]._points.size() - 1;

		i_line = lines_2[0]._points.begin();
		i_line++;
		lines_1[0]._points.insert(lines_1[0]._points.end(), i_line,lines_2[0]._points.end());
		//segmenty
		getSegments(line_id_1,&segments_1);
		getSegments(line_id_2,&segments_2);

		for( segment_i = segments_2.begin(); segment_i < segments_2.end(); segment_i++ ) {
			(*segment_i).vertex = (*segment_i).vertex + start_vertex;			
		}

		if( segments_1.size() ) {
			if( segments_1.back().vertex == start_vertex ) 
				segments_1.erase(segments_1.end()-1);
		}

		if( segments_2.size() )
			segments_1.insert(segments_1.end(),segments_2.begin(),segments_2.end());
		setLine(line_id_1,0,&lines_1);
		setSegments(line_id_1,&segments_1);
	
		deleteLine(line_id_2);
	}

	if( connection_type == 1 ) { // 01 pocz¹tek/koniec 
		start_vertex = lines_2[0]._points.size() - 1;
		
		i_line = lines_1[0]._points.begin();
		i_line++;

		lines_2[0]._points.insert(lines_2[0]._points.end(), i_line,lines_1[0]._points.end());
		//segmenty
		getSegments(line_id_1,&segments_1);
		getSegments(line_id_2,&segments_2);
	
		for( segment_i = segments_1.begin(); segment_i < segments_1.end(); segment_i++ ) {
			(*segment_i).vertex = (*segment_i).vertex + start_vertex;			
		}

		if( segments_2.size() ) {
			if( segments_2.back().vertex == start_vertex ) 
				segments_2.erase(segments_2.end()-1);
		}

		if( segments_1.size() )
			segments_2.insert(segments_2.end(),segments_1.begin(),segments_1.end());
		setLine(line_id_1,0,&lines_2);
		setSegments(line_id_1,&segments_2);

		deleteLine(line_id_2);
	}
	return true;
}

void MPLine::deleteLine(int line_id) {

	//s = sqlite3_mprintf("DELETE FROM MP_SEGMENT WHERE line_id = %i;",line_id);
	s = sqlite3_mprintf("UPDATE MP_SEGMENT set line_id = -1 WHERE line_id = %i;",line_id);
	int rc = sqlite3_exec(MPbase,s,NULL,NULL,NULL);
	sqlite3_free(s);

	//s = sqlite3_mprintf("DELETE FROM MP_LINE WHERE id = %i;",line_id);
	s = sqlite3_mprintf("UPDATE MP_LINE set to_delete = 1 where id = %i;",line_id);
	rc = sqlite3_exec(MPbase,s,NULL,NULL,NULL);
	sqlite3_free(s);
}

void MPLine::revert(int line_id) {
	std::vector< poly_t > lines_1;
	poly_t t_line;
	poly_t t_rev_line;
	vector_points::iterator i_t_line;
	vector<MPLine_segment> segments;
	vector<MPLine_segment> new_segments;

	lines_1.clear();
	if( !getLine(line_id,0,&lines_1) ) 
		return;
	//wszystkie po³¹czyæ w jedno!
	for( std::vector< poly_t >::iterator t_l = lines_1.begin(); t_l != lines_1.end(); t_l++ ) {
		t_line._points.insert(t_line._points.end(),(*t_l)._points.begin(), (*t_l)._points.end() );
	}
	lines_1.clear();

	i_t_line = t_line._points.end()-1;
	while( i_t_line >= t_line._points.begin() ) {
		t_rev_line._points.push_back((*i_t_line));
		i_t_line--;
	}
	lines_1.push_back(t_rev_line);

	setLine(line_id,0,&lines_1);

	s = sqlite3_mprintf("SELECT one_way FROM mp_line WHERE id = %i;",line_id);
	int one_way = getSelectCount(MPbase,s);
	sqlite3_free(s);

	if( one_way != 0 ) {
		one_way = (one_way==-1)?1:-1;
		s = sqlite3_mprintf("UPDATE mp_line set one_way = %i WHERE id = %i;",one_way,line_id);
		sqlite3_exec(MPbase,s,NULL,NULL,NULL);
		sqlite3_free(s);
	}

	getSegments(line_id,&segments);
	if( segments.size() ) {
		int t_point = (int)t_rev_line._points.size()-1;
		vector<MPLine_segment>::iterator segment_i,segment_i2;
		vector<MPLine_segment> new_segments;
		MPLine_segment	tmp_segment;

		//1 - pierwszy i ostatni wezel musi miec zdefiniowane segmenty - jezeli brak,
		//    to trzeba dodac
		getSegment(line_id,0,tmp_segment,true);
		getSegment(line_id,t_point,tmp_segment,true);
		
		//2 - zamienienie nodeStart
		//  nodeStart = nodes - nodeStart
		for( segment_i = segments.begin(); segment_i < segments.end(); segment_i++ ) {
			MPLine_segment new_segment = (*segment_i);
			new_segment.vertex = t_point - (*segment_i).vertex;
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
			MPLine_segment new_segment = (*segment_i);
			segments.push_back( new_segment );
		}

		setSegments(line_id,&segments);
	}
	
}

int MPLine::duplicateLine(int line_id) {
	int new_line_id = -1;
	s = sqlite3_mprintf("insert into mp_line "\
		" (s_x, s_y, e_x, e_y, original_id, map_id ,road_id ,road_id_org ,min_X ,min_Y ,max_X ,max_Y ,coordinates,parts_size, parts, vertex0 , "\
		" rgnType ,type ,label,  label2 , label3 ,"\
	    " cityname , regionname , countryname , zip,speed , road_class ,one_way , toll ,restriction  ) "\
		" select "\
		" s_x, s_y, e_x, e_y, original_id, map_id ,road_id, road_id_org ,min_X ,min_Y ,max_X ,max_Y ,coordinates,parts_size, parts, vertex0 , "\
		" rgnType ,type ,label , label2 , label3 ,"\
	    " cityname , regionname , countryname , zip,speed , road_class ,one_way , toll ,restriction  "\
		" from mp_line where id = %i;",line_id);
	int rc = sqlite3_exec(MPbase,s,NULL,NULL,NULL);
	sqlite3_free(s);
	if( rc == 0 )
		new_line_id = (int)sqlite3_last_insert_rowid(MPbase);

	return new_line_id;
}

bool MPLine::splitLine(int line_id,int vertex,int &new_line_id) {
	sqlite3_stmt*	ppStmt;
	sqlite3_stmt*	stmt_idx = NULL;
	int				rc;	
	MPLine_segment	segment;
	std::vector< poly_t > lines;
	std::vector< poly_t > new_lines;
	bool			t_result = false;

	if( vertex == 0 )
		return false;

	if( !getLine(line_id,0,&lines) )
		return false;
	if( lines.size() > 1 ) {
		//wszystkie po³¹czyæ w jedno!
		poly_t t_line;
		for( std::vector< poly_t >::iterator t_l = lines.begin(); t_l != lines.end(); t_l++ ) {
			t_line._points.insert(t_line._points.end(),(*t_l)._points.begin(), (*t_l)._points.end() );
		}

		lines.clear();
		lines.push_back(t_line);
	}
	if( lines.size() != 1 )
		return false;

	if( !getSegment(line_id,vertex,segment,true) )
		return false;

	if( lines[0]._points.size() <= (vertex+1) )
		return false;

	/*if( segment.node_id < 0 )
		generateRoutingAtPoint(line_id,vertex);
	*/
	if( segment.node_id < 0 ) {
		segment.node_id = next_node_id++;
		setSegment(line_id,vertex,segment);
	}

	new_line_id = duplicateLine(line_id);
	if( new_line_id < 0 )
		return false;
	new_lines = lines;
	
	rc = lines[0]._points.size();
	
	lines[0]._points.erase(lines[0]._points.begin()+(vertex+1),lines[0]._points.end());
	new_lines[0]._points.erase(new_lines[0]._points.begin(),new_lines[0]._points.begin()+vertex);

	rc = lines[0]._points.size();
	rc = new_lines[0]._points.size();


	setLine(line_id,0,&lines);
	setLine(new_line_id,0,&new_lines);
/*
	//restrykcje
	int before_vertex = -1;
	int after_vertex = -1;

	if( segment.restriction_id > -1 ) {
		s = sqlite3_mprintf("SELECT count(*) FROM MP_SEGMENT, MP_RESTRICTIONS WHERE MP_SEGMENT.restriction_id = MP_RESTRICTIONS.restriction_id AND " \
			" vertex < %i AND line_id = %i AND restriction_id = %i;",vertex, line_id, segment.restriction_id );
		before_vertex = getSelectCount(MPbase,s); 
		sqlite3_free(s);

		s = sqlite3_mprintf("SELECT count(*) FROM MP_SEGMENT, MP_RESTRICTIONS WHERE MP_SEGMENT.restriction_id = MP_RESTRICTIONS.restriction_id AND " \
			" vertex > %i AND line_id = %i AND restriction_id = %i;",vertex, line_id, segment.restriction_id );
		after_vertex = getSelectCount(MPbase,s); 
		sqlite3_free(s);

*/
	//nadanie original_id - tylko jeœli nie jest ustawione jeszcze!
	s = sqlite3_mprintf("UPDATE MP_LINE set original_id = %i WHERE original_id IS NULL and id=%i;",line_id,new_line_id);
	rc = sqlite3_exec(MPbase,s,NULL,NULL,NULL);
	sqlite3_free(s);

	//zmiana przypisania segmentów
	s = sqlite3_mprintf("UPDATE MP_SEGMENT SET line_id = %i, vertex = vertex - %i WHERE vertex > %i AND line_id = %i;",new_line_id, vertex, vertex, line_id);
	rc = sqlite3_exec(MPbase,s,NULL,NULL,NULL);
	sqlite3_free(s);

	s = sqlite3_mprintf("INSERT INTO MP_SEGMENT (node_id, bound, leftcity, leftRegion , leftCountry , " \
		" rightCity , rightRegion , rightCountry , leftZip , rightZip , leftStart , leftEnd , leftType , " \
		" rightStart , rightEnd , rightType, vertex, line_id, restriction ) SELECT node_id, bound, leftcity, leftRegion , leftCountry , " \
		" rightCity , rightRegion , rightCountry , leftZip , rightZip , leftStart , leftEnd , leftType , " \
		" rightStart , rightEnd , rightType, 0, %i, restriction FROM MP_SEGMENT WHERE line_id = %i and vertex = %i;",new_line_id, line_id, vertex);
	rc = sqlite3_exec(MPbase,s,NULL,NULL,NULL);
	sqlite3_free(s);
	return true;
}

/********************/

bool MPProcess::updateRestrictions() {
	sqlite3_stmt*	ppStmt;
	char			*s;
	wxString		new_restriction;
	int				segment_id;

	int rc = sqlite3_prepare_v2(MPbase,"select line_id,restriction,id from mp_segment where restriction is not null;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			new_restriction.clear();

			segment_id = sqlite3_column_int(ppStmt,2);
			wxStringTokenizer st( wxString::FromUTF8((char*)(sqlite3_column_text(ppStmt,1))), _T(";"));
			while( st.HasMoreTokens() ) {
				new_restriction.append( updateRestriction(sqlite3_column_int(ppStmt,0), st.GetNextToken() ) );
			}

			s = sqlite3_mprintf("UPDATE MP_SEGMENT SET restriction = '%q' WHERE id = %i;",new_restriction.char_str(),segment_id);
			rc = sqlite3_exec(MPbase,s,NULL,NULL,NULL);
			sqlite3_free(s);

			rc = sqlite3_step( ppStmt );
		}
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );
	return true;
}

wxString MPProcess::updateRestriction(int line_id,wxString restriction) {
	//N1,2,3L1,2[Txxx]
	//N1,2,3,4L1,2,3
	//odtworzyæ na bazie segment_id oraz original_id
/*
	lina 1 - musi posiadac n1 i n2
	lina 2 - posiada n2 i n3
	opcja
	 lina 3 - posiada n3 i n4

L1 = (gdzie count=2)
select line_id,count(*) from mp_segment where node_id in( n1,n2) and line_id in (
select id from mp_line where original_id = L1 OR id = L1) group by line_id

L2 = (gdzie count=2)
select line_id,count(*) from mp_segment where node_id in( n2,n3) and line_id in (
select id from mp_line where original_id = L2 OR id = L2) group by line_id

opt.
L3 = (gdzie count=2)
select line_id,count(*) from mp_segment where node_id in( n3,n4) and line_id in (
select id from mp_line where original_id = L3 OR id = L3) group by line_id


*/
	wxString	lines_id,nodes_id,params;
	wxString	new_restriction;
	long		line1,line2,line3;
	long		node1,node2,node3,node4;
	char*		s;

	lines_id = restriction.BeforeFirst(wxT('N'));
	lines_id = lines_id.AfterFirst(wxT('L'));
	nodes_id = restriction.AfterFirst(wxT('N'));
	nodes_id = nodes_id.BeforeFirst(wxT('P'));
	params = restriction.AfterFirst(wxT('P'));
	
	if( !lines_id.Len() || !nodes_id.Len() )
		return new_restriction;

	line1,line2,line3 = -1;
	node1,node2,node3,node4 = -1;

	{
		wxStringTokenizer st(lines_id, _T(","));
		if( st.HasMoreTokens() ) st.GetNextToken().ToLong(&line1);
		if( st.HasMoreTokens() ) st.GetNextToken().ToLong(&line2);
		if( st.HasMoreTokens() ) st.GetNextToken().ToLong(&line3);
	}
	{
		wxStringTokenizer st(nodes_id, _T(","));
		if( st.HasMoreTokens() ) st.GetNextToken().ToLong(&node1);
		if( st.HasMoreTokens() ) st.GetNextToken().ToLong(&node2);
		if( st.HasMoreTokens() ) st.GetNextToken().ToLong(&node3);
		if( st.HasMoreTokens() ) st.GetNextToken().ToLong(&node4);
	}

	if( line1 > -1 && line2 > -1 && node1 > -1 && node2 > -1 && node3 > -1 ) {
		s = sqlite3_mprintf("select line_id from ( select line_id,count(*) as number from mp_segment where node_id in( %i,%i) and line_id in ( select id from mp_line where original_id = %i OR id = %i) group by line_id ) where number > 1 ", node1, node2, line1, line1 );

		line1 = getSelectCount(MPbase,s);
		sqlite3_free(s);

		s = sqlite3_mprintf("select line_id from ( select line_id,count(*) as number from mp_segment where node_id in( %i,%i) and line_id in ( select id from mp_line where original_id = %i OR id = %i) group by line_id ) where number > 1 ", node2, node3, line2, line2 );

		line2 = getSelectCount(MPbase,s);
		sqlite3_free(s);

		if( line3 > -1 && node4 > -1 ) {
			s = sqlite3_mprintf("select line_id from ( select line_id,count(*) as number from mp_segment where node_id in( %i,%i) and line_id in ( select id from mp_line where original_id = %i OR id = %i) group by line_id ) where number > 1 ", node3, node4, line3, line3 );
			line3 = getSelectCount(MPbase,s);
			sqlite3_free(s);
		}

		if( line3 > -1 ) 
			new_restriction = wxString::Format(_T("L%i,%i,%iN"),line1,line2,line3);
		else
			new_restriction = wxString::Format(_T("L%i,%iN"),line1,line2);

		new_restriction.append(nodes_id);
		new_restriction.append(_T("P"));
		new_restriction.append(params);
		new_restriction.append(_T(";"));

		if( line_id != line1 && line_id != line2 && line3 < 0 )
			new_restriction.clear();

		if( line_id != line1 && line_id != line2 && line3 != line_id )
			new_restriction.clear();
	}
	return new_restriction;
}

/*****************************************/

bool MPProcess::splitRoadsAtSegments(sqlite3* _MPbase,wxApp* mainApp,wxGauge* m_progress) {
	MPbase = _MPbase;

	sqlite3_stmt*	ppStmt;
	char			*s;
	int				line_id;
	int				new_line_id;
	int				t_range;
	int				t_max_id;
	int				t_progress = 0;
	MPLine			line;
	bool			next_segment;
	std::vector<MPLine_segment>	segments;
	std::vector<MPLine_segment>::iterator i_s;

	line.setDB(_MPbase);

	int rc = sqlite3_exec(MPbase,"BEGIN TRANSACTION",NULL,NULL,NULL);

	t_max_id = getSelectCount(MPbase,"SELECT MAX(id) FROM MP_LINE;");
	t_range = getSelectCount(MPbase,"select count(*) from mp_line where road_id > 0;");
	m_progress->SetRange(t_range);
	m_progress->SetValue(t_progress);
	t_range /= 1000;
	if( !t_range )
		t_range = 10;

	line.next_node_id = getSelectCount(MPbase,"SELECT MAX(node_id) FROM MP_SEGMENT;");
	line.next_node_id++;

	s = sqlite3_mprintf("select id from mp_line where road_id > 0 and id <= %i;",t_max_id);
	rc = sqlite3_prepare_v2(MPbase,s,-1,&ppStmt,NULL);
	sqlite3_free(s);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {

			m_progress->SetValue(++t_progress);
			if( (t_progress % t_range) == 0 )
				mainApp->Yield(true);
			
			do {
				line_id = sqlite3_column_int(ppStmt,0);
				line.getSegments(line_id,&segments);

				next_segment = false;
				if( segments.size() ) {
					i_s = segments.end();
					i_s--;
					for( ; i_s >= segments.begin(); i_s-- ) {
						next_segment = line.splitLine(line_id,(*i_s).vertex,new_line_id);
						if( next_segment )
							break;
					}
				}
			} while( next_segment );
			
			//line.revert(sqlite3_column_int(ppStmt,0));

			rc = sqlite3_step( ppStmt );
		}
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

	updateRestrictions();

	rc = sqlite3_exec(MPbase,"END TRANSACTION",NULL,NULL,NULL);
	return true;
}


//Lua
void MPProcess::registerLua(wxHtmlWindow* _m_htmlSQL) {
	m_htmlSQL = _m_htmlSQL;

	state->GetGlobals().RegisterDirect("printText", *this, &MPProcess::setPrintLuaText);
	state->GetGlobals().RegisterDirect("printInt", *this, &MPProcess::setPrintLuaInt);
	state->GetGlobals().RegisterDirect("sqlSelect", *this, &MPProcess::execSelect);
	state->GetGlobals().RegisterDirect("sqlExec", *this, &MPProcess::execQuery);

}

void MPProcess::setPrintLuaText(const char* text) {
	m_htmlSQL->AppendToPage( wxString::FromUTF8(text));
}

void MPProcess::setPrintLuaInt(int value) {
	m_htmlSQL->AppendToPage( wxString::Format(_T("%i"),value) );
}

int MPProcess::doString(const char* script) {
	int rc;
	rc = state->DoString(script);
	if( rc != 0 ) {
		m_htmlSQL->AppendToPage( _T("Error executing script") );
	}
	return 0;
}

int MPProcess::execQuery(const char* s) {
	return sqlite3_exec(MPbase,s,NULL,NULL,NULL);
}

void MPProcess::execSelect(const char* s) {
	wxString	t_ret;
	sqlite3_stmt*	ppStmtExec;
	bool		first = true;

	int rc = sqlite3_prepare_v2(MPbase,s,-1,&ppStmtExec,NULL);
	rc = sqlite3_step( ppStmtExec );
	if( rc != SQLITE_ROW )
		m_htmlSQL->AppendToPage( wxString::FromUTF8(sqlite3_errmsg(MPbase)) );
	do {
		if( rc == SQLITE_ROW ) {
			if( first ) {
				first = false;
				t_ret.Append(_T("<table><TR>"));

				for( int i = 0; i < sqlite3_column_count(ppStmtExec); i++ ) {
					t_ret.Append(_T("<TH>"));
					t_ret.Append(wxString::FromUTF8(sqlite3_column_name(ppStmtExec,i)));
					t_ret.Append(_T("</TH>"));
				}
				t_ret.Append(_T("</TR>"));
				m_htmlSQL->AppendToPage( t_ret );
				t_ret.clear();
			}

			t_ret.Append(_T("<TR>"));

			for( int i = 0; i < sqlite3_column_count(ppStmtExec); i++ ) {
				t_ret.Append(_T("<TD>"));
				t_ret.Append(wxString::FromUTF8( (char*)sqlite3_column_text(ppStmtExec,i)));
				t_ret.Append(_T("</TD>"));
			}
			t_ret.Append(_T("</TR></table>"));
			m_htmlSQL->AppendToPage( t_ret );

			rc = sqlite3_step( ppStmtExec );
		}
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmtExec );
}


/***********************************************/
void MPProcess::checkIntersect(wxApp* mainApp,wxGauge* m_progress) {
	sqlite3_stmt*	ppStmt;
	int				rc,line_id;
	int				t_progress = 0;
	int				t_range;
	MPLine			line;
	
	line.setDB(MPbase);

	t_range = getSelectCount(MPbase,"select count(*) from mp_line where road_id > 0;");
	m_progress->SetRange(t_range);
	m_progress->SetValue(t_progress);
	t_range /= 1000;
	if( !t_range )
		t_range = 10;

	rc = sqlite3_exec(MPbase,"BEGIN TRANSACTION",NULL,NULL,NULL);
	rc = sqlite3_prepare_v2(MPbase,"select line_id from mp_segment group by line_id;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			m_progress->SetValue(++t_progress);
			if( (t_progress % t_range) == 0 )
				mainApp->Yield(true);

			line_id = sqlite3_column_int(ppStmt,0);
			line.selfIntersect(line_id);

			t_progress++;
			rc = sqlite3_step( ppStmt );
		}
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

	rc = sqlite3_exec(MPbase,"END TRANSACTION;",NULL,NULL,NULL);
}

void MPProcess::mergeRoads(wxApp* mainApp,wxGauge* m_progress,MPMergeParams* merge_params) {
	sqlite3_stmt*	ppStmt;
	sqlite3_stmt*	ppStmt2;
	sqlite3_stmt*	ppStmt_sub;
	char			*s;
	int				line_id_1,line_id_2,line_id_last,line_id_next;
	int				line_an_1,line_an_2,line_an,connection;
	int				rc;
	int				one_way_1,one_way_2;
	int				t_progress = 0;
	int				t_range;
	bool			one_way_ok;
	
	MPLine			line;
	std::vector< poly_t > lines;
	//point_t			start_p,end_p;


	line.setDB(MPbase);
	
	t_range = getSelectCount(MPbase,"select count(*) from mp_line where road_id > 0;");
	m_progress->SetRange(t_range);
	m_progress->SetValue(t_progress);
	t_range /= 1000;
	if( !t_range )
		t_range = 10;
	rc = sqlite3_exec(MPbase,"DELETE FROM MP_ROUTABLE_ID",NULL,NULL,NULL);
	rc = sqlite3_exec(MPbase,"PRAGMA journal_mode = OFF;",NULL,NULL,NULL);

	try{

	rc = sqlite3_exec(MPbase,"BEGIN TRANSACTION",NULL,NULL,NULL);
	//dla ka¿dej drogi
	rc = sqlite3_prepare_v2(MPbase,"select id, one_way from mp_line where road_id > 0;",-1,&ppStmt,NULL);
/*
	rc = sqlite3_prepare_v2(MPbase,
		" select mp1.id, mp2.id as mp2_id, mp1.one_way as mp1_one_way, mp2.one_way as mp2_one_way from mp_line as mp1 inner join mp_line as mp2 on "
		" mp1.label = mp2.label and "
		" mp1.label2 = mp2.label2 and "
		" mp1.label3 = mp2.label3 and "
		" mp1.type = mp2.type and "
		" mp1.rgnType = mp2.rgnType and "
		" mp1.cityname = mp2.cityname and "
		" mp1.regionname = mp2.regionname and "
		" mp1.countryname = mp2.countryname and "
		" mp1.zip = mp2.zip and "
		" mp1.toll = mp2.toll and "
		//" mp1.one_way = mp2.one_way and "
		" mp1.min_x-0.0001 <= mp2.max_x+0.0001 and "
		" mp1.max_x+0.0001 >= mp2.min_x-0.0001 and "
		" mp1.min_y-0.0001 <= mp2.max_y+0.0001 and "
		" mp1.max_y+0.0001 >= mp2.min_y-0.0001 "
		" where mp1.id <> mp2.id and "		
		" mp1.road_id > 0 and mp2.road_id > 0;",-1,&ppStmt,NULL);*/
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			s = sqlite3_mprintf(
		" select mp1.id, mp2.id as mp2_id, mp1.one_way as mp1_one_way, mp2.one_way as mp2_one_way " 
		" from mp_line as mp1 inner join mp_line as mp2 on "
		" mp1.label = mp2.label and "
		" mp1.label2 = mp2.label2 and "
		" mp1.label3 = mp2.label3 and "
		" mp1.type = mp2.type and "
		" mp1.rgnType = mp2.rgnType and "
		" mp1.cityname = mp2.cityname and "
		" mp1.regionname = mp2.regionname and "
		" mp1.countryname = mp2.countryname and "
		" mp1.zip = mp2.zip and "
		" mp1.toll = mp2.toll and "
		" ( "
		" ( mp1.s_x = mp2.s_x and mp1.s_y = mp2.s_y ) "
		" or "
		" ( mp1.s_x = mp2.e_x and mp1.s_y = mp2.e_y ) "
		" or "
		" ( mp1.e_x = mp2.s_x and mp1.e_y = mp2.s_y ) "
		" or "
		" ( mp1.e_x = mp2.e_x and mp1.e_y = mp2.e_y ) "
		" ) "
		" where mp1.id < mp2.id and mp1.id = %i and mp2.road_id > 0;",
			sqlite3_column_int(ppStmt,0));

			rc = sqlite3_prepare_v2(MPbase,s,-1,&ppStmt2,NULL);
			sqlite3_free(s);

			rc = sqlite3_step( ppStmt2 );
			m_progress->SetValue(++t_progress);
			if( (t_progress % t_range) == 0 )
				mainApp->Yield(true);
			

			do {
				if ( rc == SQLITE_ROW ) {
					line_id_1 = min(sqlite3_column_int(ppStmt2,0),sqlite3_column_int(ppStmt2,1));
					line_id_2 = max(sqlite3_column_int(ppStmt2,0),sqlite3_column_int(ppStmt2,1));

					//s = sqlite3_mprintf("SELECT count(*) from MP_ROUTABLE_ID where first_id = %i and second_id = %i; ", line_id_1, line_id_2 );
					//if( getSelectCount(MPbase,s) == 0 ) {
					//	sqlite3_free(s);

						connection = line.connectionPoint(line_id_1,line_id_2);
						if( connection < 4 ) {

							line_an_1 = line.directionGlobal(line_id_1);
							line_an_2 = line.directionGlobal(line_id_2);

							//0 - brak, 10 - koniec 1, pocz¹tek 2, 11 - koniec z koncem etc..

							line_an = max(line_an_1,line_an_2) - min(line_an_1,line_an_2);
							if( connection == 3 || connection == 0) {
								//koniec z koñcem lub pocz¹tek z pocz¹tkiem
								line_an = 360 - line_an;
							}
							if( line_an < merge_params->max_angle ) {

								one_way_ok = false;
								if( sqlite3_column_int(ppStmt2,2) == 0 && sqlite3_column_int(ppStmt2,3) == 0 )
									one_way_ok = true;

								if( sqlite3_column_int(ppStmt2,2) == sqlite3_column_int(ppStmt2,3) && connection != 3 && connection != 0  )
									one_way_ok = true;

								if( sqlite3_column_int(ppStmt2,2) != 0 && sqlite3_column_int(ppStmt2,3) != 0 && sqlite3_column_int(ppStmt2,2) != sqlite3_column_int(ppStmt2,3) && connection !=2 && connection != 1)
									one_way_ok = true;

								if( connection < 4 && one_way_ok ) {
									s = sqlite3_mprintf("INSERT INTO MP_ROUTABLE_ID(first_id, second_id, angle, connection ) VALUES(%i, %i, %i, %i); ", line_id_1, line_id_2, line_an, connection);
									rc = sqlite3_exec(MPbase,s,NULL,NULL,NULL);
									sqlite3_free(s);
								}
							}
						}
						//tutaj wywo³anie funkcji która bêdzie ³¹czy³a linie
					//} else
					//	sqlite3_free(s);
					rc = sqlite3_step( ppStmt2 );
				}
			} while(rc == SQLITE_ROW);
			sqlite3_finalize( ppStmt2 );

			rc = sqlite3_step( ppStmt );
		}
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

	} catch( std::exception &e ) {
		;
	}

	t_range = getSelectCount(MPbase,"select count(*) from MP_LINE;");
	t_progress = 0;
	m_progress->SetRange(t_range);
	m_progress->SetValue(t_progress);
	t_range /= 1000;
	if( !t_range )
		t_range = 10;

	rc = sqlite3_prepare_v2(MPbase,"select id from mp_line where road_id > 0 order by id;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {

			m_progress->SetValue(++t_progress);
			if( (t_progress % t_range) == 0 )
				mainApp->Yield(true);

			line_id_1 = sqlite3_column_int(ppStmt,0);
			connection = 4;

			s = sqlite3_mprintf("select first_id, second_id from MP_ROUTABLE_ID WHERE connection < 4 "
				"and ( (first_id = %i and second_id > %i) or ( second_id = %i and first_id > %i) ) order by angle;",
				line_id_1,line_id_1,line_id_1,line_id_1);
			rc = sqlite3_prepare_v2(MPbase,s,-1,&ppStmt2,NULL);
			rc = sqlite3_step( ppStmt2 );
			sqlite3_free(s);
			do {
				if ( rc == SQLITE_ROW ) {
					if( sqlite3_column_int(ppStmt2,0) == line_id_1 ) {
						line_id_1 = min(sqlite3_column_int(ppStmt,0),sqlite3_column_int(ppStmt2,1));
						line_id_2 = max(sqlite3_column_int(ppStmt,0),sqlite3_column_int(ppStmt2,1));
					} else {
						line_id_1 = min(sqlite3_column_int(ppStmt,0),sqlite3_column_int(ppStmt2,0));
						line_id_2 = max(sqlite3_column_int(ppStmt,0),sqlite3_column_int(ppStmt2,0));
					}
					connection = line.connectionPoint(line_id_1,line_id_2);
					line_id_last = line_id_1;
					line_id_next = line_id_2;
				}
//				if( connection < 4 )
//					if( line.selfIntersect(line_id_1,line_id_2) )
//						connection = 4;
				if( connection < 4 )
					break;
				rc = sqlite3_step( ppStmt2 );
			} while( rc == SQLITE_ROW);
			sqlite3_finalize( ppStmt2 );

			do {
				if( connection < 4 ) {
					line.mergeLine(line_id_1,line_id_2,connection);
					connection = 4;

					s = sqlite3_mprintf("select first_id, second_id from MP_ROUTABLE_ID WHERE connection < 4 and "
						"(first_id = %i or second_id = %i) and first_id <> %i and second_id <> %i order by angle;",line_id_next,line_id_next,
						line_id_last, line_id_last);

					rc = sqlite3_prepare_v2(MPbase,s,-1,&ppStmt2,NULL);
					sqlite3_free(s);
					rc = sqlite3_step( ppStmt2 );
					if ( rc == SQLITE_ROW ) {
						if( sqlite3_column_int(ppStmt2,0) != line_id_2 ) {
							line_id_2 = sqlite3_column_int(ppStmt2,0);
						} else
							line_id_2 = sqlite3_column_int(ppStmt2,1);

						connection = line.connectionPoint(line_id_1,line_id_2);
						line_id_last = line_id_next;
						line_id_next = line_id_2;
					}
					sqlite3_finalize( ppStmt2 );
				}
			} while (connection < 4);

			/*	s = sqlite3_mprintf("UPDATE MP_ROUTABLE_ID set first_id = %i where first_id = %i;", line_id_1,line_id_2);
				rc = sqlite3_exec(MPbase,s,NULL,NULL,NULL);
				sqlite3_free(s);

				s = sqlite3_mprintf("UPDATE MP_ROUTABLE_ID set second_id = %i where second_id = %i;", line_id_1,line_id_2);
				rc = sqlite3_exec(MPbase,s,NULL,NULL,NULL);
				sqlite3_free(s);
			*/
			/*
				10
				1---2x  x1---2
				
			*/

			//10 - koniec 1, pocz¹tek 2, 11 - koniec z koncem etc..
			//usuniêcie pozosta³ych
			/*
			if( connection == 2 || connection == 3 ) {
				s = sqlite3_mprintf("DELETE from MP_ROUTABLE_ID where (first_id = %i and ( connection = 2 or connection = 3)) or "
					" ( second_id = %i and (connection = 1 or connection = 3) );",line_id_1,line_id_1);
				rc = sqlite3_exec(MPbase,s,NULL,NULL,NULL);
				sqlite3_free(s);
			}
			
			if( connection == 0 || connection == 1 ) {
				s = sqlite3_mprintf("DELETE from MP_ROUTABLE_ID where (first_id = %i and ( connection = 0 or connection = 1)) or "
					" ( second_id = %i and (connection = 0 or connection = 2) );",line_id_1,line_id_1);
				rc = sqlite3_exec(MPbase,s,NULL,NULL,NULL);
				sqlite3_free(s);
			}

			if( connection == 0 || connection == 2 ) {
				s = sqlite3_mprintf("DELETE from MP_ROUTABLE_ID where (first_id = %i and ( connection = 0 or connection = 1)) or "
					" ( second_id = %i and (connection = 0 or connection = 2) );",line_id_2,line_id_2);
				rc = sqlite3_exec(MPbase,s,NULL,NULL,NULL);
				sqlite3_free(s);
			}

			if( connection == 1 || connection == 3 ) {
				s = sqlite3_mprintf("DELETE from MP_ROUTABLE_ID where (first_id = %i and ( connection = 2 or connection = 3)) or "
					" ( second_id = %i and (connection = 1 or connection = 3) );",line_id_2,line_id_2);
				rc = sqlite3_exec(MPbase,s,NULL,NULL,NULL);
				sqlite3_free(s);
			}
			*/
			//}
			rc = sqlite3_step( ppStmt );
		}
	} while(rc == SQLITE_ROW);	
	sqlite3_finalize( ppStmt );

	rc = sqlite3_exec(MPbase,"DELETE FROM MP_LINE WHERE to_delete = 1;",NULL,NULL,NULL);
	rc = sqlite3_exec(MPbase,"DELETE FROM MP_SEGMENT WHERE line_id = -1;",NULL,NULL,NULL);
	rc = sqlite3_exec(MPbase,"END TRANSACTION;",NULL,NULL,NULL);

	rc = sqlite3_exec(MPbase,"PRAGMA journal_mode = DELETE;",NULL,NULL,NULL);
}
