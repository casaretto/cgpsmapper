#include <algorithm>
#include <iomanip> 
#include "mp_export.h"
#include "mp_process.h"
#include "sql_helpers.h"
#include ".\shapelib\shapefil.h"
#include "wx/tokenzr.h"

using namespace std;

bool MPExport::exportSHP(sqlite3* _MPbase,_mp_section section,const char* _fileName) {
	MPbase = _MPbase;

	return exportLines(_fileName);
}

bool MPExport::exportRestrictionsMP(sqlite3* _MPbase,const char* _fileName) {
	MPbase = _MPbase;

	int			rc;
	wxString	restrict,lines_id,nodes_id,params;
	sqlite3_stmt*	ppStmt;
	MPLine		line;
	long		line_id1,line_id2,line_id3;
	long		node_id1,node_id2,node_id3,node_id4;
	std::vector< poly_t > line_1;
	std::vector< poly_t > line_2;
	std::vector< poly_t > line_3;
	int			vertex;
	char		*s;

	line.setDB(MPbase);

	ofstream mp(_fileName,ios::app);
	mp<<setprecision(7);

	rc = sqlite3_prepare_v2(MPbase,"SELECT restriction FROM MP_SEGMENT where restriction is NOT NULL;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {


			wxStringTokenizer st( wxString::FromUTF8((char*)(sqlite3_column_text(ppStmt,0))), _T(";"));
			while( st.HasMoreTokens() ) {
				restrict = st.GetNextToken();

				line_id1 =line_id2 = line_id3 = -1;

				lines_id = restrict.BeforeFirst(wxT('N'));
				lines_id = lines_id.AfterFirst(wxT('L'));
				nodes_id = restrict.AfterFirst(wxT('N'));
				nodes_id = nodes_id.BeforeFirst(wxT('P'));
				params = restrict.AfterFirst(wxT('P'));

				wxStringTokenizer ln( lines_id, _T(","));
				if( ln.HasMoreTokens() ) ln.GetNextToken().ToLong(&line_id1);
				if( ln.HasMoreTokens() ) ln.GetNextToken().ToLong(&line_id2);
				if( ln.HasMoreTokens() ) ln.GetNextToken().ToLong(&line_id3);

				wxStringTokenizer nd( nodes_id, _T(","));
				if( nd.HasMoreTokens() ) nd.GetNextToken().ToLong(&node_id1);
				if( nd.HasMoreTokens() ) nd.GetNextToken().ToLong(&node_id2);
				if( nd.HasMoreTokens() ) nd.GetNextToken().ToLong(&node_id3);
				if( nd.HasMoreTokens() ) nd.GetNextToken().ToLong(&node_id4);

				if( line_id1 > -1 && line_id2 > -1 ) {

					line.getLine(line_id1,0,&line_1);
					line.getLine(line_id2,0,&line_2);
					if( line_id3 > -1 )
						line.getLine(line_id3,0,&line_3);

					mp<<endl<<"[RGN40]"<<endl;
					mp<<"Type=0x27"<<endl;
					mp<<"DirIndicator=1"<<endl;
					mp<<"Label="<<(char*)(sqlite3_column_text(ppStmt,0))<<endl;
					mp<<"Data0=";

				
					s = sqlite3_mprintf("select vertex from mp_segment where line_id = %i and node_id = %i;", line_id1,node_id1);
					vertex = getSelectCount(MPbase,s);
					sqlite3_free(s);
					mp<<"("<<line_1[0]._points.at(vertex).y<<","<<line_1[0]._points.at(vertex).x<<"),";

					s = sqlite3_mprintf("select vertex from mp_segment where line_id = %i and node_id = %i;", line_id1,node_id2);
					vertex = getSelectCount(MPbase,s);
					sqlite3_free(s);
					mp<<"("<<line_1[0]._points.at(vertex).y<<","<<line_1[0]._points.at(vertex).x<<"),";
					
					s = sqlite3_mprintf("select vertex from mp_segment where line_id = %i and node_id = %i;", line_id2,node_id3);
					vertex = getSelectCount(MPbase,s);
					sqlite3_free(s);
					mp<<"("<<line_2[0]._points.at(vertex).y<<","<<line_2[0]._points.at(vertex).x<<")";

					if( line_id3 > -1 ) {
						s = sqlite3_mprintf("select vertex from mp_segment where line_id = %i and node_id = %i;", line_id3,node_id4);
						vertex = getSelectCount(MPbase,s);
						sqlite3_free(s);
						if( line_3[0]._points.size() > vertex ) {
							mp<<",("<<line_3[0]._points.at(vertex).y<<","<<line_3[0]._points.at(vertex).x<<")";
						}
					}
					mp<<endl;

					mp<<"[END-RGN40]"<<endl;

				}
			}
			rc = sqlite3_step( ppStmt );
		}
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );
	return true;
}


bool MPExport::exportMP(sqlite3* _MPbase,const char* _fileName) {
	MPbase = _MPbase;

	ofstream mp(_fileName,ios::out);

	mp<<";*******************************************************************************"<<endl;
	mp<<";cGPSmapper a Generic routing generator"<<endl;
	mp<<";Copyright(C) Stanislaw Kozicki, 2000-2004"<<endl;
	mp<<";*******************************************************************************"<<endl;
	mp<<";REMEMBER TO ADD A VALID [IMG ID] SECTION"<<endl;
	mp<<endl<<endl;
	mp<<"[IMG ID]"<<endl<<"ID=90000000"<<endl<<"Name=MapRoute generated map"<<endl<<"Routing=Y"<<endl;
	mp<<"Levels=5"<<endl<<"Level0=24"<<endl<<"Level1=22"<<endl<<"Level2=20"<<endl;
	mp<<"Level3=19"<<endl<<"Level4=18"<<endl<<"[END]"<<endl;
	mp<<endl<<";Followind DICTIONARY section can/should be modified to meet your requirements"<<endl<<endl;

	mp<<"[DICTIONARY]"<<endl;
	mp<<";           0              1               2               3               4               5               6              "<<endl;
	mp<<";           123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcde"<<endl;
	mp<<"Level1Rgn10=10000100001111000001011111110101111111110111111100000000000000011111111111111111111110101000110000011100000000"<<endl;
	mp<<"Level2Rgn10=00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
	mp<<"Level3Rgn10=00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
	mp<<"Level4Rgn10=00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
	mp<<"Level5Rgn10=00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
	mp<<"Level6Rgn10=00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
	mp<<"Level7Rgn10=00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
	mp<<"Level8Rgn10=00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
	mp<<"Level9Rgn10=00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
	mp<<"Level1Rgn20=11111111111111111111000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
	mp<<"Level2Rgn20=11111111111111111000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
	mp<<"Level3Rgn20=11111111111100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
	mp<<"Level4Rgn20=11111111111100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
	mp<<"Level5Rgn20=11111111111100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
	mp<<"Level6Rgn20=11111111111100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
	mp<<"Level7Rgn20=11111111111100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
	mp<<"Level8Rgn20=11111111111100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
	mp<<"Level9Rgn20=11111111111100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
	mp<<"Level1Rgn40=11111110000000000001000101111110110000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
	mp<<"Level2Rgn40=11111000000000000000000000011110010000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
	mp<<"Level3Rgn40=11100000000000000000000000010100000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
	mp<<"Level4Rgn40=11100000000000000000000000010100000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
	mp<<"Level5Rgn40=11100000000000000000000000010100000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
	mp<<"Level6Rgn40=11100000000000000000000000010100000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
	mp<<"Level7Rgn40=11100000000000000000000000010100000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
	mp<<"Level8Rgn40=11100000000000000000000000010100000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
	mp<<"Level9Rgn40=11100000000000000000000000010100000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
	mp<<"Level1Rgn80=11100000000000000001111001000110000000010000000001000000000111111111011110010001000000000000000000000000000000"<<endl;
	mp<<"Level2Rgn80=11100000000000000001110000000000000000010000000001000000000111100000010000000001000000000000000000000000000000"<<endl;
	mp<<"Level3Rgn80=11100000000000000001110000000000000000010000000001000000000111100000010000000001000000000000000000000000000000"<<endl;
	mp<<"Level4Rgn80=11100000000000000001110000000000000000010000000001000000000111100000010000000001000000000000000000000000000000"<<endl;
	mp<<"Level5Rgn80=11100000000000000001110000000000000000010000000001000000000111100000010000000001000000000000000000000000000000"<<endl;
	mp<<"Level6Rgn80=11100000000000000001110000000000000000010000000001000000000111100000010000000001000000000000000000000000000000"<<endl;
	mp<<"Level7Rgn80=11100000000000000001110000000000000000010000000001000000000111100000010000000001000000000000000000000000000000"<<endl;
	mp<<"Level8Rgn80=11100000000000000001110000000000000000010000000001000000000111100000010000000001000000000000000000000000000000"<<endl;
	mp<<"Level9Rgn80=11100000000000000001110000000000000000010000000001000000000111100000010000000001000000000000000000000000000000"<<endl;
	mp<<"[END-DICTIONARY]"<<endl<<endl;

	mp<<setprecision(7);

	int			rc;
	int			element = 0;
	int			a;

	std::vector< poly_t > polygons;
	std::vector< poly_t >::iterator i_p;
	vector_points::iterator i_r;
	vector_points::iterator i_r2;
	vector<MPLine_segment> segments;
	std::vector<MPLine_segment>::iterator i_s;
	wxString	restrict,lines_id,nodes_id,params;


	sqlite3_stmt*	ppStmt;
	MPLine		line;
	MPPoly		poly;
	MPLine_route	route;


	line.setDB(MPbase);
	poly.setDB(MPbase);

	//restrictions
	rc = sqlite3_prepare_v2(MPbase,"SELECT restriction FROM MP_SEGMENT where restriction is NOT NULL;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {


			wxStringTokenizer st( wxString::FromUTF8((char*)(sqlite3_column_text(ppStmt,0))), _T(";"));
			while( st.HasMoreTokens() ) {
				restrict = st.GetNextToken();

				lines_id = restrict.BeforeFirst(wxT('N'));
				lines_id = lines_id.AfterFirst(wxT('L'));
				nodes_id = restrict.AfterFirst(wxT('N'));
				nodes_id = nodes_id.BeforeFirst(wxT('P'));
				params = restrict.AfterFirst(wxT('P'));

				mp<<"[RESTRICT]"<<endl;
				mp<<"TraffPoints="<<nodes_id.char_str()<<endl;
				mp<<"TraffRoads="<<lines_id.char_str()<<endl;
				mp<<"[END-RESTRICT]"<<endl;
			}
			rc = sqlite3_step( ppStmt );
		}
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

	rc = sqlite3_prepare_v2(MPbase,"SELECT id, rgnType, type, label, label2, label3, cityname, regionname, countryname, zip, speed , road_class ,one_way , toll ,restriction, road_id  FROM MP_LINE where vertex0 > 0;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			mp<<"[RGN40]"<<endl;
			mp<<"Type=0x"<<hex<<( sqlite3_column_int(ppStmt,2)>>8) <<dec<<endl;
			mp<<"RoadID="<<sqlite3_column_int64(ppStmt,0)<<endl;
			mp<<"Label="<<(char*)sqlite3_column_text(ppStmt,3)<<endl;
			mp<<"CityName="<<(char*)sqlite3_column_text(ppStmt,6)<<endl;
			mp<<"RegionName="<<(char*)sqlite3_column_text(ppStmt,7)<<endl;
			mp<<"CountryName="<<(char*)sqlite3_column_text(ppStmt,8)<<endl;
			if( sqlite3_column_int(ppStmt,12) != 0 )
				mp<<"DirIndicator=1"<<endl;
			if( sqlite3_column_int(ppStmt,15) != 0 ) {
				mp<<"RouteParam=";
				mp<<sqlite3_column_int(ppStmt,10)<<","<<sqlite3_column_int(ppStmt,11)<<","<<sqlite3_column_int(ppStmt,12)<<","<<sqlite3_column_int(ppStmt,13)<<",";
				route.setRestrictions((char*)sqlite3_column_text(ppStmt,14));
				mp<<(route.getDenied_emergency()? "1,":"0,");
				mp<<(route.getDenied_delivery()? "1,":"0,");
				mp<<(route.getDenied_car()? "1,":"0,");
				mp<<(route.getDenied_bus()? "1,":"0,");
				mp<<(route.getDenied_taxi()? "1,":"0,");
				mp<<(route.getDenied_pedestrian()? "1,":"0,");
				mp<<(route.getDenied_bicycle()? "1,":"0,");
				mp<<(route.getDenied_truck()? "1":"0");
				mp<<endl;
			}
			line.getLine(sqlite3_column_int(ppStmt,0),0,&polygons);
			for( i_p = polygons.begin(); i_p != polygons.end(); i_p++ ) {

				i_r2 = (*i_p)._points.begin();
				i_r2++;
				mp<<"Data0=";
				for( i_r = (*i_p)._points.begin() ; i_r != (*i_p)._points.end() ; i_r++,i_r2++ ) {
					mp<<"("<< (*i_r).y<<","<<(*i_r).x<<")";
					if( i_r2 != (*i_p)._points.end() )
						mp<<",";
				}
				mp<<endl;
			}

			if( line.getSegments(sqlite3_column_int(ppStmt,0),&segments) ) {
				for( a = 1, i_s = segments.begin(); i_s != segments.end(); i_s++, a++ ) {
					mp<<"Nod"<<a<<"="<<(*i_s).vertex<<","<<(*i_s).node_id<<","<<(*i_s).bound<<endl;
				}
				for( a = 1, i_s = segments.begin(); i_s != segments.end(); i_s++, a++ ) {
					mp<<"Numbers"<<a<<"="<<(*i_s).vertex<<",";
					if( (*i_s).leftType==1 ) 
						mp<<"e";
					else if( (*i_s).leftType==2 ) 
						mp<<"o";
					else if( (*i_s).leftType==3 ) 
						mp<<"b";
					else
						mp<<"n";
					mp<<","<<(*i_s).leftStart<<","<<(*i_s).leftEnd<<",";

					if( (*i_s).rightType==1 ) 
						mp<<"e";
					else if( (*i_s).rightType==2 ) 
						mp<<"o";
					else if( (*i_s).rightType==3 ) 
						mp<<"b";
					else
						mp<<"n";
					mp<<","<<(*i_s).rightStart<<","<<(*i_s).rightEnd<<",";

					mp<<(*i_s).leftZip<<","<<(*i_s).rightZip<<","<<(*i_s).leftCity<<","<<(*i_s).leftRegion<<","<<(*i_s).leftCountry<<",";
					mp<<(*i_s).rightCity<<","<<(*i_s).rightRegion<<","<<(*i_s).rightCountry<<",";
					
					mp<<endl;
				}
			}
			mp<<"[END-RGN40]"<<endl<<endl;
			
			rc = sqlite3_step( ppStmt );
			element++;
		}
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

/**********************/

	//rgn80
	rc = sqlite3_prepare_v2(MPbase,"SELECT id, rgnType ,type ,label FROM MP_POLY;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {

			poly.getPoly(sqlite3_column_int(ppStmt,0),0,&polygons);

			mp<<"[RGN80]"<<endl;
			mp<<"Type=0x"<<hex<<( sqlite3_column_int(ppStmt,2)>>8) <<dec<<endl;
			mp<<"Label="<<(char*)sqlite3_column_text(ppStmt,3)<<endl;

			for( i_p = polygons.begin(); i_p != polygons.end(); i_p++ ) {

				i_r2 = (*i_p)._points.begin();
				i_r2++;
				mp<<"Data0=";
				for( i_r = (*i_p)._points.begin() ; i_r != (*i_p)._points.end() ; i_r++,i_r2++ ) {
					mp<<"("<< (*i_r).y<<","<<(*i_r).x<<")";
					if( i_r2 != (*i_p)._points.end() )
						mp<<",";
				}
				mp<<endl;
			}
			mp<<"[END-RGN80]"<<endl<<endl;

			rc = sqlite3_step( ppStmt );
			element++;
		}
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

	/**********************/
	//rgn10,20
	double x,y;
	rc = sqlite3_prepare_v2(MPbase,"SELECT coordinates,rgnType ,type ,label , streetdesc , phone,housenumber , cityname , regionname , countryname, zip  FROM MP_POI;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			memcpy(&x, sqlite3_column_text(ppStmt,0), sizeof( double ) );
			memcpy(&y, &sqlite3_column_text(ppStmt,0)[sizeof( double )], sizeof( double ) );

			mp<<"[RGN10]"<<endl;
			mp<<"Type=0x"<<hex<<( sqlite3_column_int(ppStmt,2)>>8) <<dec<<endl;
			mp<<"SubType=0x"<<hex<<( sqlite3_column_int(ppStmt,2) && 0xff ) <<dec<<endl;
			mp<<"Label="<<(char*)sqlite3_column_text(ppStmt,3)<<endl;
			mp<<"CityName="<<(char*)sqlite3_column_text(ppStmt,7)<<endl;
			mp<<"RegionName="<<(char*)sqlite3_column_text(ppStmt,8)<<endl;
			mp<<"CountryName="<<(char*)sqlite3_column_text(ppStmt,9)<<endl;
			mp<<"Data0=("<<y<<","<<x<<")";
			mp<<endl;
			mp<<"[END-RGN10]"<<endl<<endl;
			
			rc = sqlite3_step( ppStmt );
			element++;
		}
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

	return true;
}

bool MPExport::exportLines(const char* _fileName) {
//vector<RGNxx*> *list_Import,vector<lbl_ZipCode>* list_ZipCodes,vector<lbl_Country>* list_Countries,vector<lbl_Region>* list_Regions,vector<lbl_City>* list_Cities,map<int, nod_ID >* list_NODES,map<int, nod_Traffic >* list_TRAFFIC) {
	SHPHandle	shp_handle;
	DBFHandle	dbf_handle;
	SHPObject*	shp_object;
	MPLine_segment	segment;
	MPLine_segment	segment_end;
	int			rc;
	int			element = 0;
	std::vector< poly_t > lines;
	vector_points::iterator i_p;
	MPLine		line;
	sqlite3_stmt*	ppStmt;
	MPLine_route	route;

	size_t coord_size = 1000;
	size_t c;
	double* x = new double[coord_size];
	double* y = new double[coord_size];

	line.setDB(MPbase);

	shp_handle = SHPCreate(_fileName, SHPT_ARC);
	dbf_handle = DBFCreate(_fileName);

	DBFAddField(dbf_handle, "type", FTInteger, 5, 0);
	DBFAddField(dbf_handle, "oldtype", FTInteger, 5, 0);
	DBFAddField(dbf_handle, "oldstype", FTInteger, 5, 0);
	DBFAddField(dbf_handle, "link_id", FTInteger, 5, 0);
	DBFAddField(dbf_handle, "st_name", FTString, MAX_LBL_LENGTH, 0);

	DBFAddField(dbf_handle, "l_refaddr", FTInteger, 5, 0);
	DBFAddField(dbf_handle, "l_nrefaddr", FTInteger, 5, 0);
	DBFAddField(dbf_handle, "r_refaddr", FTInteger, 5, 0);
	DBFAddField(dbf_handle, "r_nrefaddr", FTInteger, 5, 0);
	DBFAddField(dbf_handle, "ref_in_id", FTInteger, 5, 0);
	DBFAddField(dbf_handle, "nref_in_id", FTInteger, 5, 0); //10

	DBFAddField(dbf_handle, "func_class", FTInteger, 2, 0);
	DBFAddField(dbf_handle, "speed_cat", FTInteger, 2, 0);
	DBFAddField(dbf_handle, "dir_travel", FTString, 2, 0);

	DBFAddField(dbf_handle, "ar_auto", FTString, 2, 0);//14
	DBFAddField(dbf_handle, "ar_bus", FTString, 2, 0);
	DBFAddField(dbf_handle, "ar_taxis", FTString, 2, 0);
	DBFAddField(dbf_handle, "ar_pedest", FTString, 2, 0);
	DBFAddField(dbf_handle, "ar_trucks", FTString, 2, 0);
	DBFAddField(dbf_handle, "ar_deliv", FTString, 2, 0);
	DBFAddField(dbf_handle, "ar_emerveh", FTString, 2, 0);
	DBFAddField(dbf_handle, "tollway", FTString, 2, 0);


//int max_coord_size = getSelectCount("SELECT MAX(coordinates_size) FROM MP_LINE;");
//																				5		6			7		8			9
	rc = sqlite3_prepare_v2(MPbase,"SELECT id, rgnType, type, label, speed , road_class ,one_way , toll ,restriction, road_id FROM MP_LINE where vertex0 > 0;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {

			route.setRestrictions((char*)sqlite3_column_text(ppStmt,8));
			line.getLine(sqlite3_column_int(ppStmt,0),0,&lines);
			line.getSegment(sqlite3_column_int(ppStmt,0),0,segment);
			line.getSegment(sqlite3_column_int(ppStmt,0),lines[0]._points.size()-1,segment_end);

			if( lines[0]._points.size() > coord_size ) {
				delete[] x;
				delete[] y;

				coord_size = lines[0]._points.size();
				x = new double[coord_size];
				y = new double[coord_size];
			}

			for( c = 0,i_p = lines[0]._points.begin(); i_p != lines[0]._points.end(); i_p++,c++ ) {
				x[c] = (*i_p).x;
				y[c] = (*i_p).y;
			}
			c = lines[0]._points.size();

			shp_object = SHPCreateSimpleObject( SHPT_ARC, (int)c, x, y, NULL );
			SHPComputeExtents( shp_object );
			SHPWriteObject( shp_handle, -1, shp_object );
			SHPDestroyObject( shp_object );

			DBFWriteIntegerAttribute (dbf_handle,element,0, sqlite3_column_int(ppStmt,2) ); //type
			DBFWriteIntegerAttribute (dbf_handle,element,1, (sqlite3_column_int(ppStmt,2)>>8) ); //type
			DBFWriteIntegerAttribute (dbf_handle,element,2, (sqlite3_column_int(ppStmt,2)&0xff) ); //type
			DBFWriteIntegerAttribute (dbf_handle,element,3, sqlite3_column_int(ppStmt,0) ); //link_id
			DBFWriteStringAttribute (dbf_handle,element,4,(char*)sqlite3_column_text(ppStmt,3) );

			DBFWriteIntegerAttribute (dbf_handle,element,11, sqlite3_column_int(ppStmt,5) );
			DBFWriteIntegerAttribute (dbf_handle,element,12, sqlite3_column_int(ppStmt,4) );

			DBFWriteStringAttribute (dbf_handle,element,14,(route.getDenied_car()?"N":"Y") );
			DBFWriteStringAttribute (dbf_handle,element,15,(route.getDenied_bus()?"N":"Y") );
			DBFWriteStringAttribute (dbf_handle,element,16,(route.getDenied_taxi()?"N":"Y") );
			DBFWriteStringAttribute (dbf_handle,element,17,(route.getDenied_pedestrian()?"N":"Y") );
			DBFWriteStringAttribute (dbf_handle,element,18,(route.getDenied_truck()?"N":"Y") );
			DBFWriteStringAttribute (dbf_handle,element,19,(route.getDenied_delivery()?"N":"Y") );
			DBFWriteStringAttribute (dbf_handle,element,20,(route.getDenied_emergency()?"N":"Y") );
			DBFWriteStringAttribute (dbf_handle,element,21,(sqlite3_column_int(ppStmt,7)?"Y":"N") );

			if( lines[0]._points.at(0).y < lines[0]._points.at(lines[0]._points.size()-1).y ||
				lines[0]._points.at(0).y == lines[0]._points.at(lines[0]._points.size()-1).y &&
				lines[0]._points.at(0).x < lines[0]._points.at(lines[0]._points.size()-1).x ) {

				DBFWriteIntegerAttribute (dbf_handle,element,5, segment.leftStart );
				DBFWriteIntegerAttribute (dbf_handle,element,6, segment.leftEnd );
				DBFWriteIntegerAttribute (dbf_handle,element,7, segment.rightStart );
				DBFWriteIntegerAttribute (dbf_handle,element,8, segment.rightEnd );

				DBFWriteIntegerAttribute (dbf_handle,element,9, static_cast<int>(segment.node_id)); //ref_in_id
				DBFWriteIntegerAttribute (dbf_handle,element,10,static_cast<int>(segment_end.node_id)); //nref_in_id

				if( sqlite3_column_int(ppStmt,6) == 0 )
					DBFWriteStringAttribute (dbf_handle,element,13,"B");
				else if( sqlite3_column_int(ppStmt,6) == 1 )
					DBFWriteStringAttribute (dbf_handle,element,13,"F");
				else if( sqlite3_column_int(ppStmt,6) == -1 )
					DBFWriteStringAttribute (dbf_handle,element,13,"T");
			} else {
				DBFWriteIntegerAttribute (dbf_handle,element,5, segment.leftEnd );
				DBFWriteIntegerAttribute (dbf_handle,element,6, segment.leftStart );
				DBFWriteIntegerAttribute (dbf_handle,element,7, segment.rightEnd );
				DBFWriteIntegerAttribute (dbf_handle,element,8, segment.rightStart );

				DBFWriteIntegerAttribute (dbf_handle,element,9, static_cast<int>(segment_end.node_id)); //ref_in_id
				DBFWriteIntegerAttribute (dbf_handle,element,10, static_cast<int>(segment.node_id)); //nref_in_id

				if( sqlite3_column_int(ppStmt,6) == 0 )
					DBFWriteStringAttribute (dbf_handle,element,13,"B");
				else if( sqlite3_column_int(ppStmt,6) == 1 )
					DBFWriteStringAttribute (dbf_handle,element,13,"T");
				else if( sqlite3_column_int(ppStmt,6) == -1 )
					DBFWriteStringAttribute (dbf_handle,element,13,"F");
			}

			rc = sqlite3_step( ppStmt );
			element++;
		}
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

	delete[] x;
	delete[] y;

	//restrictions
	long		cond_id = 1;
	long		line_id;
	long		line_id1;
	long		line_id2;
	long		line_id3;
	int			element_rdms = 0;
	int			element_cdms = 0;
	int			seq_number;
	MPLine_restriction	restriction;
	wxString	restrict,lines_id,nodes_id,params;

	DBFHandle	rdms_handle;
	DBFHandle	cdms_handle;
	wxString	rdms_filename;
	wxString	cdms_filename;



	rdms_filename = wxString::FromUTF8(_fileName);
	cdms_filename = wxString::FromUTF8(_fileName);
	
	rdms_filename.append(wxT("RDMS"));
	cdms_filename.append(wxT("CDMS"));

	rdms_handle = DBFCreate(rdms_filename.char_str());
	cdms_handle = DBFCreate(cdms_filename.char_str());

	DBFAddField(rdms_handle, "link_id", FTInteger, 10, 0);
	DBFAddField(rdms_handle, "cond_id", FTInteger, 10, 0);
	DBFAddField(rdms_handle, "man_linkid", FTInteger, 10, 0);
	DBFAddField(rdms_handle, "seq_number", FTInteger, 4, 0);

	DBFAddField(cdms_handle, "link_id", FTInteger, 10, 0);
	DBFAddField(cdms_handle, "cond_id", FTInteger, 10, 0);
	DBFAddField(cdms_handle, "cond_type", FTInteger, 5, 0);
	DBFAddField(cdms_handle, "cond_val1", FTString, 30, 0);
	DBFAddField(cdms_handle, "cond_val2", FTString, 15, 0);
	DBFAddField(cdms_handle, "cond_val3", FTString, 30, 0);
	DBFAddField(cdms_handle, "cond_val4", FTInteger, 5, 0);
	DBFAddField(cdms_handle, "end_of_lk", FTString, 1, 0);
	DBFAddField(cdms_handle, "AR_Auto", FTString, 1, 0);
	DBFAddField(cdms_handle, "AR_Bus", FTString, 1, 0);
	DBFAddField(cdms_handle, "AR_Taxis", FTString, 1, 0);
	DBFAddField(cdms_handle, "AR_Carpool", FTString, 1, 0);
	DBFAddField(cdms_handle, "AR_Pedest", FTString, 1, 0);
	DBFAddField(cdms_handle, "AR_Trucks", FTString, 1, 0);
	DBFAddField(cdms_handle, "AR_ThruTr", FTString, 1, 0);
	DBFAddField(cdms_handle, "AR_Deliver", FTString, 1, 0);
	DBFAddField(cdms_handle, "AR_EmerVeh", FTString, 1, 0);
	
	rc = sqlite3_prepare_v2(MPbase,"SELECT restriction, line_id, vertex FROM MP_SEGMENT where restriction is NOT NULL;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {

			wxStringTokenizer st( wxString::FromUTF8((char*)(sqlite3_column_text(ppStmt,0))), _T(";"));
			while( st.HasMoreTokens() ) {
				restrict = st.GetNextToken();

				lines_id = restrict.BeforeFirst(wxT('N'));
				lines_id = lines_id.AfterFirst(wxT('L'));
				nodes_id = restrict.AfterFirst(wxT('N'));
				nodes_id = nodes_id.BeforeFirst(wxT('P'));
				params = restrict.AfterFirst(wxT('P'));

				restriction.setRestrictions( string(params.char_str()) );

				line_id1 = -1;
				line_id2 = -1;
				line_id3 = -1;

				wxStringTokenizer ln( lines_id, _T(","));
				if( ln.HasMoreTokens() ) ln.GetNextToken().ToLong(&line_id1);
				if( ln.HasMoreTokens() ) ln.GetNextToken().ToLong(&line_id2);
				if( ln.HasMoreTokens() ) ln.GetNextToken().ToLong(&line_id3);

				if( line_id1 > -1 && line_id2 > -1 ) {
					seq_number = 1;
					DBFWriteIntegerAttribute (rdms_handle,element_rdms,0,line_id1 );
					DBFWriteIntegerAttribute (rdms_handle,element_rdms,1,cond_id );
					DBFWriteIntegerAttribute (rdms_handle,element_rdms,2,line_id2 );
					DBFWriteIntegerAttribute (rdms_handle,element_rdms,3,seq_number );
					element_rdms++;
				}
				if( line_id2 > -1 && line_id3 > -1 ) {
					seq_number = 2;
					DBFWriteIntegerAttribute (rdms_handle,element_rdms,0,line_id2 );
					DBFWriteIntegerAttribute (rdms_handle,element_rdms,1,cond_id );
					DBFWriteIntegerAttribute (rdms_handle,element_rdms,2,line_id3 );
					DBFWriteIntegerAttribute (rdms_handle,element_rdms,3,seq_number );
					element_rdms++;
				}

				line.getLine(sqlite3_column_int(ppStmt,1),0,&lines);

				/*Blank - Condition applies to whole link
				N - Condition applies to non-reference end of link
				R - Condition applies to reference end of link*/
				if( lines[0]._points.at(0).y < lines[0]._points.at(lines[0]._points.size()-1).y ||
					lines[0]._points.at(0).y == lines[0]._points.at(lines[0]._points.size()-1).y &&
					lines[0]._points.at(0).x < lines[0]._points.at(lines[0]._points.size()-1).x ) {

					//vertex
					if(sqlite3_column_int(ppStmt,2) == 0 ) 
						DBFWriteStringAttribute(cdms_handle,element_cdms,7,"R" ); 
					else
						DBFWriteStringAttribute(cdms_handle,element_cdms,7,"N" ); 
				} else {
					//vertex
					if(sqlite3_column_int(ppStmt,2) == 0 ) 
						DBFWriteStringAttribute(cdms_handle,element_cdms,7,"N" ); 
					else
						DBFWriteStringAttribute(cdms_handle,element_cdms,7,"R" ); 
				}

				DBFWriteIntegerAttribute (cdms_handle,element_cdms,0,line_id1 );
				DBFWriteIntegerAttribute (cdms_handle,element_cdms,1,cond_id );
				DBFWriteIntegerAttribute (cdms_handle,element_cdms,2,7 );
				DBFWriteStringAttribute  (cdms_handle,element_cdms,3,"LEGAL" );
				
				if( restriction.getDenied_car() ) 
					DBFWriteStringAttribute (cdms_handle,element_cdms,8,"Y" );
				else
					DBFWriteStringAttribute (cdms_handle,element_cdms,8,"N" );

				if( restriction.getDenied_bus() ) 
					DBFWriteStringAttribute (cdms_handle,element_cdms,9,"Y" );
				else
					DBFWriteStringAttribute (cdms_handle,element_cdms,9,"N" );

				if( restriction.getDenied_taxi() ) 
					DBFWriteStringAttribute (cdms_handle,element_cdms,10,"Y" );
				else
					DBFWriteStringAttribute (cdms_handle,element_cdms,10,"N" );
				
				//carpool
				DBFWriteStringAttribute (cdms_handle,element_cdms,11,"Y" );

				if( restriction.getDenied_pedestrian() ) 
					DBFWriteStringAttribute (cdms_handle,element_cdms,12,"Y" );
				else
					DBFWriteStringAttribute (cdms_handle,element_cdms,12,"N" );

				if( restriction.getDenied_truck() ) 
					DBFWriteStringAttribute (cdms_handle,element_cdms,13,"Y" );
				else
					DBFWriteStringAttribute (cdms_handle,element_cdms,13,"N" );

				//thrutr
				DBFWriteStringAttribute (cdms_handle,element_cdms,14,"Y" );

				if( restriction.getDenied_delivery() ) 
					DBFWriteStringAttribute (cdms_handle,element_cdms,15,"Y" );
				else
					DBFWriteStringAttribute (cdms_handle,element_cdms,15,"N" );

				if( restriction.getDenied_emergency() ) 
					DBFWriteStringAttribute (cdms_handle,element_cdms,16,"Y" );
				else
					DBFWriteStringAttribute (cdms_handle,element_cdms,16,"N" );

				element_cdms++;
				cond_id++;

				/*
				mp<<"[RESTRICT]"<<endl;
				mp<<"TraffPoints="<<nodes_id.char_str()<<endl;
				mp<<"TraffRoads="<<lines_id.char_str()<<endl;
				mp<<"[END-RESTRICT]"<<endl;
*/
			}
			rc = sqlite3_step( ppStmt );
		}
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

	DBFClose(rdms_handle);
	DBFClose(cdms_handle);

	SHPClose(shp_handle);
	DBFClose(dbf_handle);
	return true;
}