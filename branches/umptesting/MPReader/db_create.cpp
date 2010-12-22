#include <stdexcept>
#include <string>
#include <iostream>
#include "db_create.h"
#include "sql_helpers.h"
#include "error.h"

DBCreate::DBCreate(const char* _databaseName) {
	_constructor_error = false;
	if( sqlite3_open(_databaseName,&MPbase) != SQLITE_OK ) {
		sqlite3_close(MPbase);
		_constructor_error = true;
	}
}

DBCreate::~DBCreate() {
	sqlite3_close(MPbase);
}

sqlite3* DBCreate::getDB() {
	return MPbase;
}

void	DBCreate::createProcessDB() {
	int b_result;

	//czy istnieja juz tabele? - jesli tak - do usuniecia?

	//schemat tabel tak jak dane wynikowe :
	//LBL
	//	LBL_1
	//	LBL_2
	//	LBL_3
	//	LBL_4
	//	LBL_8
	//	
	//RGN
	//	RGN_02
	//	RGN_03
	//	RGN_04
	//	RGN_10
	//	RGN_20
	//	RGN_40
	//	RGN_80
	//TRE
	//NET
	//NOD
	//...

	//tabela label
	b_result = sqlite3_exec(MPbase,"CREATE TABLE DATA_LBL1(id integer primary key autoincrement, address int ,label unique text, counter int);",NULL,NULL,NULL);
	if( b_result ) throw(std::runtime_error(sqlite3_errmsg(MPbase)) ); 

	//tablea lbl2 - kraje
	b_result = sqlite3_exec(MPbase,"CREATE TABLE DATA_LBL2(id integer primary key autoincrement, address int ,lbl1_id int);",NULL,NULL,NULL);
	if( b_result ) throw(std::runtime_error(sqlite3_errmsg(MPbase)) ); 

	//tablea lbl3 - regiony
	b_result = sqlite3_exec(MPbase,"CREATE TABLE DATA_LBL3(id integer primary key autoincrement, address int ,lbl2_id int, lbl1_id int);",NULL,NULL,NULL);
	if( b_result ) throw(std::runtime_error(sqlite3_errmsg(MPbase)) ); 

	//tabela lbl4 - miasta
	b_result = sqlite3_exec(MPbase,"CREATE TABLE DATA_LBL4(id integer primary key autoincrement, address int ,lbl2_id int, lbl3_id int, rgnidx int, treidx int, lbl1_id int);",NULL,NULL,NULL);
	if( b_result ) throw(std::runtime_error(sqlite3_errmsg(MPbase)) ); 

	//tabela lbl5/7 - indeksy

	//tabela lbl6 - poi stary format
	b_result = sqlite3_exec(MPbase,"CREATE TABLE DATA_LBL6(id integer primary key autoincrement , address int, lbl1_id int, lbl9_id int, lbl9_flag int, lbl10_id int, );",NULL,NULL,NULL);
	if( b_result ) throw(std::runtime_error(sqlite3_errmsg(MPbase)) ); 


	//tabele RGN
	//rgn20 - typ dla czystych miast
	b_result = sqlite3_exec(MPbase,"CREATE TABLE DATA_RGN20(id integer primary key autoincrement , address int, rec_size int, coordinates blob, rgnType int,lbl1_id int, lbl6_id int);",NULL,NULL,NULL);
	if( b_result ) throw(std::runtime_error(sqlite3_errmsg(MPbase)) ); 
}

void	DBCreate::createInputDB() {
	int b_result;

	//czy istnieja juz tabele?
	if( getSelectCount(MPbase,"select count(*) from sqlite_master  where name = 'MP_ZIP_OBS'") )
		return;

	sqlite3_exec(MPbase,"PRAGMA temp_store = 2;",NULL,NULL,NULL);
	sqlite3_exec(MPbase,"PRAGMA cache_size = 100000;",NULL,NULL,NULL);
	b_result = sqlite3_exec(MPbase,"CREATE TABLE MP_ZIP_OBS(id int ,zip text);",NULL,NULL,NULL);
	if( b_result ) throw(std::runtime_error(sqlite3_errmsg(MPbase)) ); 

	b_result = sqlite3_exec(MPbase,"CREATE TABLE MP_COUNTRIES_OBS(id int ,country text);",NULL,NULL,NULL);
	if( b_result ) throw(std::runtime_error(sqlite3_errmsg(MPbase)) ); 

	b_result = sqlite3_exec(MPbase,"CREATE TABLE MP_REGIONS_OBS(id int ,region text, country_id int);",NULL,NULL,NULL);
	if( b_result ) throw(std::runtime_error(sqlite3_errmsg(MPbase)) ); 
	b_result = sqlite3_exec(MPbase,"CREATE INDEX MP_REGIONS_OBS_1 ON MP_CITIES_OBS (region);",NULL,NULL,NULL);
	b_result = sqlite3_exec(MPbase,"CREATE INDEX MP_REGIONS_OBS_2 ON MP_CITIES_OBS (country_id);",NULL,NULL,NULL);

	b_result = sqlite3_exec(MPbase,"CREATE TABLE MP_CITIES_OBS(id int ,city text, region_id int);",NULL,NULL,NULL);
	if( b_result ) throw(std::runtime_error(sqlite3_errmsg(MPbase)) ); 
	b_result = sqlite3_exec(MPbase,"CREATE INDEX MP_CITIES_OBS_1 ON MP_CITIES_OBS (city);",NULL,NULL,NULL);
	b_result = sqlite3_exec(MPbase,"CREATE INDEX MP_CITIES_OBS_2 ON MP_CITIES_OBS (region_id);",NULL,NULL,NULL);

	b_result = sqlite3_exec(MPbase,"CREATE TABLE MP_TYPES(rgn_type int,type int, name text);",NULL,NULL,NULL);
	if( b_result ) 
		throw(std::runtime_error(sqlite3_errmsg(MPbase)) ); 
	b_result = sqlite3_exec(MPbase,"CREATE INDEX MP_TYPES_1 ON MP_TYPES (name);",NULL,NULL,NULL);
	
	b_result = sqlite3_exec(MPbase,"CREATE TABLE MP_LEVELS(id int,level int, zoom int);",NULL,NULL,NULL);
	if( b_result ) 
		throw( std::runtime_error(sqlite3_errmsg(MPbase))); 

	b_result = sqlite3_exec(MPbase,"CREATE TABLE MP_ID(id int,lock int, fid int, productCode int, regionId int, polygonevaluate int,copyright text, " \
		" mapcopyright text, includelayers text, elevation float, drawPriority int, familyId int, marine int, preview int, mg int, mgnumbering int,  " \
		" routing int, defaultcitycountry text, defaultregioncountry text, simplifylevel float, preprocess int, " \
		" codepage int, lblcoding int, transparent int, transparentempty int, poionly int, poiindex int, poinumberfirst int, " \
		" poizipfirst int, name text, alignmethod float, tresize int, tremarigin int, datum text, rgnlimit int, levels int);",NULL,NULL,NULL);
	if( b_result ) 
		throw(std::runtime_error(sqlite3_errmsg(MPbase)) ); 	

	//Control data
	b_result = sqlite3_exec(MPbase,"CREATE TABLE MP_ROADID_MAPPING(id integer primary key autoincrement,new_id integer,road_id integer,new_road_id integer);",NULL,NULL,NULL);
	if( b_result ) 
		throw( std::runtime_error(sqlite3_errmsg(MPbase)) ); 	
	b_result = sqlite3_exec(MPbase,"CREATE INDEX MP_ROADID_MAPPING ON MP_LINE (new_id,road_id,new_road_id);",NULL,NULL,NULL);

	b_result = sqlite3_exec(MPbase,"CREATE TABLE MP_ROUTABLE_ID(id integer primary key autoincrement,first_id int, second_id int, angle int, connection int);",NULL,NULL,NULL);
	if( b_result ) 
		throw( std::runtime_error(sqlite3_errmsg(MPbase)) ); 	
	b_result = sqlite3_exec(MPbase,"CREATE INDEX MP_ROUTABLE_ID_1 ON MP_LINE (first_id,connection,angle);",NULL,NULL,NULL);
	b_result = sqlite3_exec(MPbase,"CREATE INDEX MP_ROUTABLE_ID_2 ON MP_LINE (second_id,connection,angle);",NULL,NULL,NULL);
	b_result = sqlite3_exec(MPbase,"CREATE INDEX MP_ROUTABLE_ID_3 ON MP_LINE (first_id,second_id);",NULL,NULL,NULL);
	b_result = sqlite3_exec(MPbase,"CREATE INDEX MP_ROUTABLE_ID_4 ON MP_LINE (first_id,connection);",NULL,NULL,NULL);
	b_result = sqlite3_exec(MPbase,"CREATE INDEX MP_ROUTABLE_ID_5 ON MP_LINE (second_id,connection);",NULL,NULL,NULL);
	b_result = sqlite3_exec(MPbase,"CREATE INDEX MP_ROUTABLE_ID_6 ON MP_LINE (first_id);",NULL,NULL,NULL);
	b_result = sqlite3_exec(MPbase,"CREATE INDEX MP_ROUTABLE_ID_7 ON MP_LINE (second_id);",NULL,NULL,NULL);
	b_result = sqlite3_exec(MPbase,"CREATE INDEX MP_ROUTABLE_ID_7 ON MP_LINE (angle);",NULL,NULL,NULL);
	b_result = sqlite3_exec(MPbase,"CREATE INDEX MP_ROUTABLE_ID_8 ON MP_LINE (connection,angle);",NULL,NULL,NULL);
	
	b_result = sqlite3_exec(MPbase,"CREATE TABLE MP_ROUTABLE_TYPES(id integer primary key autoincrement,type int);",NULL,NULL,NULL);
	if( b_result ) 
		throw( std::runtime_error(sqlite3_errmsg(MPbase)) ); 	
	b_result = sqlite3_exec(MPbase,"CREATE INDEX MP_ROUTABLE_TYPES_1 ON MP_LINE (type);",NULL,NULL,NULL);

	{
		char *s;
		for( int i=1; i<0x13; i++ ) {
			s = sqlite3_mprintf("INSERT INTO MP_ROUTABLE_TYPES (type) VALUES(%i); ", i);
			b_result = sqlite3_exec(MPbase,s,NULL,NULL,NULL);
			sqlite3_free(s);
		}
		s = sqlite3_mprintf("INSERT INTO MP_ROUTABLE_TYPES (type) VALUES(%i); ", 0x16);
		b_result = sqlite3_exec(MPbase,s,NULL,NULL,NULL);
		sqlite3_free(s);

		s = sqlite3_mprintf("INSERT INTO MP_ROUTABLE_TYPES (type) VALUES(%i); ", 0x1a);
		b_result = sqlite3_exec(MPbase,s,NULL,NULL,NULL);
		sqlite3_free(s);

		s = sqlite3_mprintf("INSERT INTO MP_ROUTABLE_TYPES (type) VALUES(%i); ", 0x1b);
		b_result = sqlite3_exec(MPbase,s,NULL,NULL,NULL);
		sqlite3_free(s);
	}

	//POI
	b_result = sqlite3_exec(MPbase,"CREATE TABLE MP_POI(id integer primary key autoincrement,map_id int,min_X float,min_Y float,max_X float,max_Y float,coordinates blob ,rgnType int,type int,label text , streetdesc text , phone text, alphaphone text," \
		                           " housenumber text , cityname text , regionname text , countryname text , zip text);",NULL,NULL,NULL);
	if( b_result ) 
		throw( std::runtime_error(sqlite3_errmsg(MPbase)) ); 	

	b_result = sqlite3_exec(MPbase,"CREATE VIRTUAL TABLE mp_poi_index USING RTREE( id, min_X, min_Y, max_X, max_Y );",NULL,NULL,NULL);
	if( b_result ) 
		throw( std::runtime_error(sqlite3_errmsg(MPbase))); 	

	//LINE
	b_result = sqlite3_exec(MPbase,"CREATE TABLE MP_LINE(id integer primary key autoincrement,to_delete int default 0,map_id int,original_id integer,road_id integer,road_id_org integer,min_X float,min_Y float,max_X float,max_Y float,s_x int,s_y int, e_x int, e_y int, coordinates blob,parts_size int, parts blob, vertex0 int,rgnType int,type int,label text , label2 text , label3 text ," \
		                           " cityname text , regionname text , countryname text , zip text, speed int, road_class int,one_way int, toll int,restriction text);",NULL,NULL,NULL);
	if( b_result ) 
		throw( std::runtime_error(sqlite3_errmsg(MPbase))); 	
	b_result = sqlite3_exec(MPbase,"CREATE INDEX MP_LINE_1 ON MP_LINE (map_id,road_id);",NULL,NULL,NULL);
	b_result = sqlite3_exec(MPbase,"CREATE INDEX MP_LINE_2 ON MP_LINE (road_id);",NULL,NULL,NULL);
	b_result = sqlite3_exec(MPbase,"CREATE INDEX MP_LINE_3 ON MP_LINE (original_id);",NULL,NULL,NULL);
	b_result = sqlite3_exec(MPbase,"CREATE INDEX MP_LINE_4 ON MP_LINE ( label, label2, label3, type, rgnType, cityname, regionname, countryname, zip, toll, id, road_id);",NULL,NULL,NULL);
	b_result = sqlite3_exec(MPbase,"CREATE INDEX MP_LINE_5 ON MP_LINE (s_x,s_y,e_x,e_y);",NULL,NULL,NULL);
	b_result = sqlite3_exec(MPbase,"CREATE INDEX MP_LINE_6 ON MP_LINE (to_delete);",NULL,NULL,NULL);

	b_result = sqlite3_exec(MPbase,"CREATE VIRTUAL TABLE mp_line_index USING RTREE( id, min_X, max_X, min_Y, max_Y );",NULL,NULL,NULL);
	if( b_result ) 
		throw( std::runtime_error(sqlite3_errmsg(MPbase)) ); 	

	//LINE - routing segments
	b_result = sqlite3_exec(MPbase,"CREATE TABLE MP_SEGMENT(id integer primary key autoincrement,line_id int,vertex int, node_id int default -1, bound int, " \
		" leftCity text, leftRegion text, leftCountry text, " \
		" rightCity text, rightRegion text, rightCountry text, leftZip text, rightZip text, leftStart int, leftEnd int, leftType int, " \
		" rightStart int, rightEnd int, rightType int, restriction text) ;",NULL,NULL,NULL);
	if( b_result ) 	throw( std::runtime_error(sqlite3_errmsg(MPbase)) ); 	
	b_result = sqlite3_exec(MPbase,"CREATE INDEX MP_SEGMENT_1 ON MP_SEGMENT (line_id,vertex);",NULL,NULL,NULL);
	b_result = sqlite3_exec(MPbase,"CREATE INDEX MP_SEGMENT_2 ON MP_SEGMENT (line_id);",NULL,NULL,NULL);
	b_result = sqlite3_exec(MPbase,"CREATE INDEX MP_SEGMENT_3 ON MP_SEGMENT (node_id);",NULL,NULL,NULL);
	b_result = sqlite3_exec(MPbase,"CREATE INDEX MP_SEGMENT_4 ON MP_SEGMENT (restriction_id);",NULL,NULL,NULL);
	
	//LINE = restrictions
	b_result = sqlite3_exec(MPbase,"CREATE TABLE MP_RESTRICTIONS(id integer primary key autoincrement, " \
		" line_id_list text, " \
		" node_id_list text, " \
		" params text, " \
		" type int) ;",NULL,NULL,NULL);
	if( b_result ) 	
		throw( std::runtime_error(sqlite3_errmsg(MPbase)) ); 	


	//POLY
	b_result = sqlite3_exec(MPbase,"CREATE TABLE MP_POLY(id integer primary key autoincrement,map_id int, " \
		" min_X float,min_Y float,max_X float,max_Y float, " \
		" coordinates blob,parts_size int, parts blob,rgnType int,type int,label text );",NULL,NULL,NULL);
	if( b_result ) 
		throw( std::runtime_error(sqlite3_errmsg(MPbase))); 	

	b_result = sqlite3_exec(MPbase,"CREATE VIRTUAL TABLE mp_poly_index USING RTREE( id, min_X, min_Y, max_X, max_Y );",NULL,NULL,NULL);
	if( b_result ) 
		throw( std::runtime_error(sqlite3_errmsg(MPbase)) ); 	
}
