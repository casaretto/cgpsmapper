#include <stdexcept>
#include <string>
#include <iostream>
#include <time.h>
#include "point.h"
#include "mp_read.h"
#include "sql_helpers.h"
#include "error.h"
#include "parse_helpers.h"
#include "filexor.h"

using namespace boost::interprocess;
using namespace TXTparser;

void MPReader::setDB(sqlite3* _MPbase) {
	MPbase = _MPbase;
}

MPReader::MPReader() {
	_mp_file = NULL;
}

MPReader::MPReader(const char* _fileName) {
	std::string a;
	file_name = _fileName;
	_mp_file = NULL;
	_constructor_error = false;
	try {
		_mp_file = new file_mapping(_fileName,read_only);
	} catch (...) {
		_constructor_error = true;
	}
}

MPReader::~MPReader() {
	if( _mp_file )
		delete _mp_file;
}

const char * MPReader::findNextLine (const char * _pBegin, const char * _pEnd) {
	for (const char * p = _pBegin; p < _pEnd; ++ p)
		if (* p == '\n')
			return p + 1;

	return _pEnd;
}

int	MPReader::calculateLineNumber(const char *_pLine) {
	int	lineNumber = 1;
	if( _pLine > _endPointer )
		return -1;
	const char* _pTmp = findNextLine(_beginPointer, _pLine);

	while( _pTmp <= _pLine ) {
		lineNumber++;
		_pTmp = findNextLine(_pTmp, _endPointer);
	}

	return lineNumber;
}
size_t MPReader::getLineLen (const char * _pBegin, const char * _pNextLine) {
	if (_pNextLine < _pBegin)
		return 0;
	size_t cLen = _pNextLine - _pBegin;

	while (
		cLen > 0 && (
			_pBegin [cLen - 1] == '\r' ||
			_pBegin [cLen - 1] == '\n' ||
			_pBegin [cLen - 1] == ' '  ||
			_pBegin [cLen - 1] == '\t'
		)
	)
	-- cLen;
	return cLen;	
}


/******************************************************************************/

bool MPReader::readMP() {
	boost::interprocess::mapped_region	_mp_memory(*_mp_file,read_only);
	const char*	p_line;
	const char*	p_nextLine;
	size_t		line_size;
	_mp_section	mp_section;

	_beginPointer = (char*)_mp_memory.get_address();
	p_line = (char*)_mp_memory.get_address();
	_endPointer = p_line + _mp_memory.get_size();

	sqlite3_exec(MPbase,"BEGIN TRANSACTION",NULL,NULL,NULL);

	while( p_line < _endPointer ) {
		p_nextLine = findNextLine(p_line,_endPointer);

		if(p_line[0]=='[') {
			switch( selectSection(p_line,p_nextLine) ) {
				case _imgid:
					p_nextLine = readImgId(p_nextLine,_endPointer);
					break;
				case _dictionary:
					break;
				case _poi:
					p_nextLine = readPoi(detectType(p_line,p_nextLine),p_nextLine,_endPointer);
					break;
				case _polyline:
					p_nextLine = readLine(detectType(p_line,p_nextLine),p_nextLine,_endPointer);
					break;
				case _polygon:
					p_nextLine = readPoly(detectType(p_line,p_nextLine),p_nextLine,_endPointer);
					break;
				case _shp:
					p_nextLine = readShp(0,p_nextLine,_endPointer);
					break;
				case _countries:
					p_nextLine = readCountries(p_nextLine,_endPointer);
					break;
				case _regions:
					p_nextLine = readRegions(p_nextLine,_endPointer);
					break;
				case _cities:
					p_nextLine = readCities(p_nextLine,_endPointer);
					break;
				case _zip:
					p_nextLine = readZip(p_nextLine,_endPointer);
					break;
				case _restriction:
					p_nextLine = readRestrict(p_nextLine,_endPointer);
					break;
			}
		}

		p_line = p_nextLine;
	}	

	//post steps

	assignResstrictions();

	sqlite3_exec(MPbase,"END TRANSACTION",NULL,NULL,NULL);

	return true;
}

int MPReader::detectType(const char* _pLine,const char* _pEnd) {
	if( _img_attr_n.marine == 1 || _img_attr_n.marine == 2 ) {
		if( hasPrefix(_pLine,_pEnd,"[rgn10]",7) ) return 0x04; 
		if( hasPrefix(_pLine,_pEnd,"[rgn20]",7) ) return 0x04; 
		if( hasPrefix(_pLine,_pEnd,"[rgn40]",7) ) return 0x03; 
		if( hasPrefix(_pLine,_pEnd,"[rgn80]",7) ) return 0x02;
	} else {
		if( hasPrefix(_pLine,_pEnd,"[rgn10]",7) ) return 0x10; 
		if( hasPrefix(_pLine,_pEnd,"[rgn20]",7) ) return 0x20; 
		if( hasPrefix(_pLine,_pEnd,"[rgn40]",7) ) return 0x40; 
		if( hasPrefix(_pLine,_pEnd,"[rgn80]",7) ) return 0x80;
	}
	if( hasPrefix(_pLine,_pEnd,"[rgn02]",7) ) return 0x02;  //0x80
	if( hasPrefix(_pLine,_pEnd,"[rgn03]",7) ) return 0x03;  //0x40
	if( hasPrefix(_pLine,_pEnd,"[rgn04]",7) ) return 0x04;  //0x10,0x20
	if( hasPrefix(_pLine,_pEnd,"[rgn2]",6) ) return 0x02; 
	if( hasPrefix(_pLine,_pEnd,"[rgn3]",6) ) return 0x03; 
	if( hasPrefix(_pLine,_pEnd,"[rgn4]",6) ) return 0x04; 

	//0x20 - tylko miasta,
	//0x10 - poi 
	if( hasPrefix(_pLine,_pEnd,"[poi]",5) ) return 0x10;
	if( hasPrefix(_pLine,_pEnd,"[polyline]",10) ) return 0x40;
	if( hasPrefix(_pLine,_pEnd,"[polygon]",9) ) return 0x80;

	return 0;//wszystkie inne - nieznane na tym etapie
}

_mp_section	MPReader::selectSection(const char* _pLine,const char* _pEnd) {
	if( hasPrefix(_pLine,_pEnd,"[img id]",8) ) return _imgid;
	if( hasPrefix(_pLine,_pEnd,"[dictionary]",12) ) return _dictionary;
	
	if( hasPrefix(_pLine,_pEnd,"[poi]",5) ) return _poi; //0x10
	if( hasPrefix(_pLine,_pEnd,"[rgn10]",7) ) return _poi; //0x10
	if( hasPrefix(_pLine,_pEnd,"[rgn20]",7) ) return _poi; //city - 0x20
	if( hasPrefix(_pLine,_pEnd,"[rgn04]",7) ) return _poi; //0x04
	if( hasPrefix(_pLine,_pEnd,"[rgn4]",6) ) return _poi; //0x04

	if( hasPrefix(_pLine,_pEnd,"[rgn40]",7) ) return _polyline;
	if( hasPrefix(_pLine,_pEnd,"[polyline]",10) ) return _polyline;
	if( hasPrefix(_pLine,_pEnd,"[rgn80]",7) ) return _polygon;
	if( hasPrefix(_pLine,_pEnd,"[polygon]",9) ) return _polygon;
	
	if( hasPrefix(_pLine,_pEnd,"[shp]",5) ) return _shp;

	if( hasPrefix(_pLine,_pEnd,"[countries]",11) ) return _countries;
	if( hasPrefix(_pLine,_pEnd,"[regions]",9) ) return _regions;
	if( hasPrefix(_pLine,_pEnd,"[cities]",8) ) return _cities;
	if( hasPrefix(_pLine,_pEnd,"[zipcodes]",10) ) return _zip;

	if( hasPrefix(_pLine,_pEnd,"[restrict]",10) ) return _restriction;

	return _none;
}

bool MPReader::detectObjectType(const char * _pValue, const char * _pEnd, unsigned long & _ulRet) {
	const char* pEnd = NULL;
	
	if( parseType(_pValue,_pEnd,&pEnd,_ulRet) )
		return true;

	std::string	typeName;
	typeName.assign(_pValue, getLineLen(_pValue,_pEnd) );
	return false;
}

void MPReader::assignResstrictions() {
	sqlite3_stmt*	ppStmt;
	__int64			line1,line2,line3;
	__int64			node1,node2,node3,node4;
	wxString		line_id,node_id,params;
	std::string		restriction;
	int				segment_id;
	char*			s;

	int rc = sqlite3_prepare_v2(MPbase,"SELECT line_id_list, node_id_list, params from MP_RESTRICTIONS;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			line1=line2=line3=-1;
			node1=node2=node3=node4=-1;

			line_id = wxString::FromUTF8((char*)sqlite3_column_text(ppStmt,0));
			node_id = wxString::FromUTF8((char*)sqlite3_column_text(ppStmt,1));
			params = wxString::FromUTF8((char*)sqlite3_column_text(ppStmt,2));

			{
				wxStringTokenizer st(line_id, _T(","));
				if( st.HasMoreTokens() ) st.GetNextToken().ToLongLong(&line1);
				if( st.HasMoreTokens() ) st.GetNextToken().ToLongLong(&line2);
				if( st.HasMoreTokens() ) st.GetNextToken().ToLongLong(&line3);
			}
			{
				wxStringTokenizer st(node_id, _T(","));
				if( st.HasMoreTokens() ) st.GetNextToken().ToLongLong(&node1);
				if( st.HasMoreTokens() ) st.GetNextToken().ToLongLong(&node2);
				if( st.HasMoreTokens() ) st.GetNextToken().ToLongLong(&node3);
				if( st.HasMoreTokens() ) st.GetNextToken().ToLongLong(&node4);
			}
			
			if( node1 > -1 && node2 > -1 && node3 > -1 && line1 > -1 && line2 > -1 ) {
				s = sqlite3_mprintf("SELECT MP_SEGMENT.restriction,MP_SEGMENT.id FROM MP_SEGMENT, MP_LINE WHERE MP_SEGMENT.node_id = %lld AND MP_SEGMENT.line_id = MP_LINE.id AND MP_LINE.road_id = %lld;",node2,line1);
				getSelectStringLong(MPbase,s,restriction,segment_id);
				sqlite3_free(s);

				if( segment_id > -1 ) {

					s = sqlite3_mprintf("SELECT id from MP_LINE where road_id = %lld;",line1);
					line1 = getSelectCount(MPbase,s);
					sqlite3_free(s);

					s = sqlite3_mprintf("SELECT id from MP_LINE where road_id = %lld;",line2);
					line2 = getSelectCount(MPbase,s);
					sqlite3_free(s);

					if( line3 > -1 ) {
						s = sqlite3_mprintf("SELECT id from MP_LINE where road_id = %lld;",line3);
						line3 = getSelectCount(MPbase,s);
						sqlite3_free(s);
					}
					line_id.clear();
					if( line3 < 0 )
						line_id = wxString::Format(_T("%lld,%lld"),line1,line2);
					else
						line_id = wxString::Format(_T("%lld,%lld,%lld"),line1,line2,line3);
					
					if( restriction.size() )
						restriction.append(";");
					restriction.append("L");
					restriction.append(line_id.ToUTF8());
					restriction.append("N");
					restriction.append(node_id.ToUTF8());
					restriction.append("P");
					restriction.append(params.ToUTF8());

					s = sqlite3_mprintf("UPDATE MP_SEGMENT set restriction = '%q' WHERE id = %i;",restriction.c_str(),segment_id);
					rc = sqlite3_exec(MPbase,s,NULL,NULL,NULL);
					sqlite3_free(s);
				}
			}

			rc = sqlite3_step( ppStmt );
		}
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

	s = sqlite3_mprintf("DELETE from MP_RESTRICTIONS;");
	rc = sqlite3_exec(MPbase,s,NULL,NULL,NULL);
	sqlite3_free(s);
}

/***********************************************************************************/
const char*	MPReader::readZip(const char* _pLine,const char* _pEnd) {
	const char*	p_nextLine;
	const char* p_value;
	const char* pEnd = NULL;
	int _v;
	std::string	text;
	char *s;
	int	b_result;

	do {
		try {
			p_nextLine = findNextLine(_pLine,_endPointer);
			p_value = getValue(_pLine,p_nextLine);

			text = "";
			if( hasPrefix(_pLine,_pEnd,"zipcode",7 ) ) {
				parseDecInt(&_pLine[7],p_value,&pEnd,_v);				
				text.append(p_value, getLineLen(p_value,p_nextLine) );

				s = sqlite3_mprintf("INSERT INTO MP_ZIP_OBS (id,zip) " \
					" VALUES (%i, '%q'); ", _v,text.c_str());
				b_result = sqlite3_exec(MPbase,s,NULL,NULL,NULL);
				sqlite3_free(s);
			}

		} catch(std::exception& e) {
			throw std::invalid_argument("wrong arguments in [-section-]");
		}
		_pLine = p_nextLine;
	} while( !hasPrefix(_pLine,findNextLine(_pLine,_endPointer),"[end",4) );
	return _pLine;
}

const char*	MPReader::readCountries(const char* _pLine,const char* _pEnd) {
	const char*	p_nextLine;
	const char* p_value;
	const char* pEnd = NULL;
	unsigned long _v;
	std::string	text;
	char *s;
	int	b_result;

	do {
		try {
			p_nextLine = findNextLine(_pLine,_endPointer);
			p_value = getValue(_pLine,p_nextLine);

			text = "";
			if( hasPrefix(_pLine,_pEnd,"country",7 ) ) {
				parseDecUL(&_pLine[7],p_value,&pEnd,_v);				
				text.append(p_value, getLineLen(p_value,p_nextLine) );

				s = sqlite3_mprintf("INSERT INTO MP_COUNTRIES_OBS (id,country) " \
					" VALUES (%i, '%q'); ", _v,text.c_str());
				b_result = sqlite3_exec(MPbase,s,NULL,NULL,NULL);
				sqlite3_free(s);
			}

		} catch(std::exception& e) {
			throw std::invalid_argument("wrong arguments in [-section-]");
		}
		_pLine = p_nextLine;
	} while( !hasPrefix(_pLine,findNextLine(_pLine,_endPointer),"[end",4) );
	return _pLine;
}

const char*	MPReader::readRegions(const char* _pLine,const char* _pEnd) {
	const char*	p_nextLine;
	const char* p_value;
	const char* pEnd = NULL;
	unsigned long	_v,_w;
	std::string	text;
	char *s;
	int	b_result;

	do {
		try {
			p_nextLine = findNextLine(_pLine,_endPointer);
			p_value = getValue(_pLine,p_nextLine);

			if( hasPrefix(_pLine,_pEnd,"region",6 ) ) {
				parseDecUL(&_pLine[6],p_value,&pEnd,_v);				
				text.append(p_value, getLineLen(p_value,p_nextLine) );
			}
			if( hasPrefix(_pLine,_pEnd,"countryidx",10 ) ) {
				parseDecUL(p_value,p_nextLine,&pEnd,_w);				

				s = sqlite3_mprintf("INSERT INTO MP_REGIONS_OBS (id,region,country_id) " \
					" VALUES (%i, '%q', %i); ", _v, text.c_str(), _w);
				b_result = sqlite3_exec(MPbase,s,NULL,NULL,NULL);
				sqlite3_free(s);

				text = "";
			}

		} catch(std::exception& e) {
			throw std::invalid_argument("wrong arguments in [-section-]");
		}
		_pLine = p_nextLine;
	} while( !hasPrefix(_pLine,findNextLine(_pLine,_endPointer),"[end",4) );
	return _pLine;
}

const char*	MPReader::readCities(const char* _pLine,const char* _pEnd) {
	const char*	p_nextLine;
	const char* p_value;
	const char* pEnd = NULL;
	unsigned long	_v,_w;
	std::string	text;
	char *s;
	int	b_result;

	do {
		try {
			p_nextLine = findNextLine(_pLine,_endPointer);
			p_value = getValue(_pLine,p_nextLine);

			if( hasPrefix(_pLine,_pEnd,"city",4 ) ) {
				parseDecUL(&_pLine[4],p_value,&pEnd,_v);				
				text.append(p_value, getLineLen(p_value,p_nextLine) );
			}
			if( hasPrefix(_pLine,_pEnd,"regionidx",9 ) ) {
				parseDecUL(p_value,p_nextLine,&pEnd,_w);				

				s = sqlite3_mprintf("INSERT INTO MP_CITIES_OBS (id,city,region_id) " \
					" VALUES (%i, '%q', %i); ", _v, text.c_str(), _w);
				b_result = sqlite3_exec(MPbase,s,NULL,NULL,NULL);
				sqlite3_free(s);
				
				text = "";
			}

		} catch(std::exception& e) {
			throw std::invalid_argument("wrong arguments in [-section-]");
		}
		_pLine = p_nextLine;
	} while( !hasPrefix(_pLine,findNextLine(_pLine,_endPointer),"[end",4) );
	return _pLine;
}

const char*	MPReader::readShp(int ,const char* _pLine,const char* _pEnd) {
	const char*	p_nextLine;
	const char* p_value;
	const char* pEnd = NULL;

	struct shp_dbf {
		std::string	dbf_layer;
		std::string	name;
		std::string	type;
		std::string	dbf_label;
		std::string	dbf_streetDesc;
		std::string	dbf_label3 ;
		std::string	dbf_dirInd ;
		std::string	dbf_type ;
		std::string	dbf_subtype;

		std::string	def_level;
		std::string	end_level;

		std::string	def_type;
		std::string	dbf_cityName ;
		std::string	dbf_regionName;
		std::string	dbf_countryName;
		std::string	dbf_zip ;
		std::string	dbf_exit;

		std::string	dbf_cityIdx ;
		std::string	dbf_houseNumber ;
		std::string	dbf_phoneNumber;
		std::string	dbf_zipIdx ;

		std::string	dbf_exitIdx;
		std::string	dbf_overnightPark ;

		std::string	dbf_exit_start ;
		std::string	dbf_exit_end ;

		std::string	dbf_StreetLeftTypeField ;
		std::string	dbf_StreetLeftStartField;
		std::string	dbf_StreetLeftEndField ;

		std::string	dbf_StreetRightTypeField ;
		std::string	dbf_StreetRightStartField;
		std::string	dbf_StreetRightEndField ;

		std::string	dbf_StreetRoadID ;
		std::string	dbf_StreetSpeed ;
		std::string	dbf_StreetRoadClass ;
		std::string	dbf_StreetVE ;
		std::string	dbf_StreetVD ;
		std::string	dbf_StreetVC ;
		std::string	dbf_StreetVB ;
		std::string	dbf_StreetVT ;
		std::string	dbf_StreetVP ;
		std::string	dbf_StreetVI ;
		std::string	dbf_StreetVR ;
		std::string	dbf_StreetOneWay ;
		std::string	dbf_StreetToll ;

		//MARINE
		std::string	dbf_text_start;
		std::string	dbf_text_end ;
		std::string	dbf_text_lines;

		std::string	dbf_textFile ;

		std::string	dbf_color ;
		std::string	dbf_style ;
		std::string	dbf_depth ;//height
		std::string	dbf_depthUnit;
		std::string	dbf_position;
		std::string	dbf_depthFlag;
		std::string	dbf_foundationColor ;
		std::string	dbf_light ;
		std::string	dbf_lightType ;
		std::string	dbf_note ;
		std::string	dbf_localDesignator ;
		std::string	dbf_internationalDesignator ;
		std::string	dbf_period ;
		std::string	dbf_height2;
		std::string	dbf_height2unit ;
		std::string	dbf_height1;
		std::string	dbf_height1unit;
		std::string	dbf_leadingAngle;
		std::string	dbf_racon;
		std::string	dbf_doubleLights;
		std::string	dbf_doubleLightsHorizontal;
		std::string	dbf_facilityPoint;
	} _shp_dbf;

	//_shp_attr_depth.unit = _img_attr_n.elevation;
/*
	do {
		p_nextLine = findNextLine(_pLine,_endPointer);
		p_value = getValue(_pLine,p_nextLine);

		try {
			//if( hasPrefix(_pLine,_pEnd,"layers",6 ) ) _shp_attr_s.layers.assign(p_value, getLineLen(p_value,p_nextLine) );

			if( hasPrefix(_pLine,_pEnd,"layer",5)) _shp_dbf.dbg_layer.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"name",4)) _shp_dbf.name.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"type",4) {
				if( !parseType(p_value,p_nextLine,&pEnd,_shp_dbf.type) ) {
					if( hasPrefix(p_value,_pEnd,"rgn04",5) ) _shp_dbf.type = 4;
					if( hasPrefix(p_value,_pEnd,"rgn03",5) ) _shp_dbf.type = 3;
					if( hasPrefix(p_value,_pEnd,"rgn02",5) ) _shp_dbf.type = 2;
					if( hasPrefix(p_value,_pEnd,"rgn10",5) ) _shp_dbf.type = 16;
					if( hasPrefix(p_value,_pEnd,"rgn20",5) ) _shp_dbf.type = 32;
					if( hasPrefix(p_value,_pEnd,"rgn40",5) ) _shp_dbf.type = 64;
					if( hasPrefix(p_value,_pEnd,"rgn80",5) ) _shp_dbf.type = 128;
				}
			}
			if( hasPrefix(_pLine,_pEnd,"labelfield",10) )	_shp_dbf.dbg_label.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"label2field",11) )	_shp_dbf.dbg_streetDesc.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"label3field",11) )	_shp_dbf.dbg_label3.assign(p_value, getLineLen(p_value,p_nextLine) );
			//if( hasPrefix(_pLine,_pEnd,"DIRFIELD") )	_shp_dbf.dbg_dirInd.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"typefield",9) )	_shp_dbf.dbg_type.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"subtypefield",12) )	_shp_dbf.dbg_subtype.assign(p_value, getLineLen(p_value,p_nextLine) );

			if( hasPrefix(_pLine,_pEnd,"level",5)) parseType(p_value,p_nextLine,&pEnd,_shp_dbf.def_level);
			if( hasPrefix(_pLine,_pEnd,"levelsnumber",12) || hasPrefix(_pLine,_pEnd,"endlevel",8) )	parseType(p_value,p_nextLine,&pEnd,_shp_dbf.def_endLevel);

			if( hasPrefix(_pLine,_pEnd,"defaulttype",11) {
				if( !detectObjectType(p_value,p_nextLine,_shp_dbf.def_type) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	
			}
			if( hasPrefix(_pLine,_pEnd,"cityname",8))	_shp_dbf.dbg_cityName.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"regionname",10)) _shp_dbf.dbg_regionName.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"countryname",11)) _shp_dbf.dbg_countryName.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"zip=",4)) _shp_dbf.dbg_zip.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"highway",7)) _shp_dbf.dbg_exit.assign(p_value, getLineLen(p_value,p_nextLine) );

			if( hasPrefix(_pLine,_pEnd,"cityidx",7)) _shp_dbf.dbg_cityIdx.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"housenumber",11)) _shp_dbf.dbg_houseNumber.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"streetdesc",10)) _shp_dbf.dbg_streetDesc.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"phonenumber",11)) _shp_dbf.dbg_phoneNumber.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"zipidx",6)) _shp_dbf.dbg_zipIdx.assign(p_value, getLineLen(p_value,p_nextLine) );

			if( hasPrefix(_pLine,_pEnd,"highwayidx",10))	_shp_dbf.dbg_exitIdx.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"overnightparking",16))	_shp_dbf.dbg_overnightPark.assign(p_value, getLineLen(p_value,p_nextLine) );

			if( hasPrefix(_pLine,_pEnd,"exitservicesstart",17))	_shp_dbf.dbg_exit_start.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"exitservicesend",15))	_shp_dbf.dbg_exit_end.assign(p_value, getLineLen(p_value,p_nextLine) );

			if( hasPrefix(_pLine,_pEnd,"lefttype",8))	_shp_dbf.dbg_StreetLeftTypeField.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"leftstart",9)) _shp_dbf.dbf_StreetLeftStartField .assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"leftend",7)) _shp_dbf.dbf_StreetLeftEndField.assign(p_value, getLineLen(p_value,p_nextLine) );

			if( hasPrefix(_pLine,_pEnd,"righttype",9))	_shp_dbf.dbf_StreetRightTypeField.assign(p_value, getLineLen(p_value,p_nextLine) ); 
			if( hasPrefix(_pLine,_pEnd,"rightstart",10))	_shp_dbf.dbf_StreetRightStartField.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"rightend",8))	_shp_dbf.dbf_StreetRightEndField.assign(p_value, getLineLen(p_value,p_nextLine) ); 

			if( hasPrefix(_pLine,_pEnd,"roadid",6))	_shp_dbf.dbf_StreetRoadID.assign(p_value, getLineLen(p_value,p_nextLine) ); 
			if( hasPrefix(_pLine,_pEnd,"speedtype",9)) _shp_dbf.dbf_StreetSpeed.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"roadclass",9)) _shp_dbf.dbf_StreetRoadClass.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"vehiclee",8)) _shp_dbf.dbf_StreetVE.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"vehicled",8))	_shp_dbf.dbf_StreetVD.assign(p_value, getLineLen(p_value,p_nextLine) ); 
			if( hasPrefix(_pLine,_pEnd,"vehiclec",8))	_shp_dbf.dbf_StreetVC.assign(p_value, getLineLen(p_value,p_nextLine) ); 
			if( hasPrefix(_pLine,_pEnd,"vehicleb",8))	_shp_dbf.dbf_StreetVB.assign(p_value, getLineLen(p_value,p_nextLine) ); 
			if( hasPrefix(_pLine,_pEnd,"vehiclet",8))	_shp_dbf.dbf_StreetVT.assign(p_value, getLineLen(p_value,p_nextLine) ); 
			if( hasPrefix(_pLine,_pEnd,"vehiclep",8))	_shp_dbf.dbf_StreetVP.assign(p_value, getLineLen(p_value,p_nextLine) ); 
			if( hasPrefix(_pLine,_pEnd,"vehiclei",8))	_shp_dbf.dbf_StreetVI.assign(p_value, getLineLen(p_value,p_nextLine) ); 
			if( hasPrefix(_pLine,_pEnd,"vehicler",8))	_shp_dbf.dbf_StreetVR.assign(p_value, getLineLen(p_value,p_nextLine) ); 
			if( hasPrefix(_pLine,_pEnd,"oneway",6))	_shp_dbf.dbf_StreetOneWay.assign(p_value, getLineLen(p_value,p_nextLine) ); 
			if( hasPrefix(_pLine,_pEnd,"toll",4)) _shp_dbf.dbf_StreetToll.assign(p_value, getLineLen(p_value,p_nextLine) );

			//MARINE
			if( hasPrefix(_pLine,_pEnd,"textstart",9)) _shp_dbf.dbf_text_start.assign(p_value, getLineLen(p_value,p_nextLine) ); 
			if( hasPrefix(_pLine,_pEnd,"textend",7))	_shp_dbf.dbf_text_end.assign(p_value, getLineLen(p_value,p_nextLine) ); 
			if( hasPrefix(_pLine,_pEnd,"textfilelines",13))	_shp_dbf.dbf_text_lines.assign(p_value, getLineLen(p_value,p_nextLine) );

			if( hasPrefix(_pLine,_pEnd,"textfile",8))	_shp_dbf.dbf_textFile.assign(p_value, getLineLen(p_value,p_nextLine) );

			if( hasPrefix(_pLine,_pEnd,"color",5))	_shp_dbf.dbf_color.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"style",5))	_shp_dbf.dbf_style.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"depth",5) || hasPrefix(_pLine,_pEnd,"height",6) ) _shp_dbf.dbf_depth.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"heightunit",10) || hasPrefix(_pLine,_pEnd,"depthunit",9)) _shp_dbf.dbf_depthUnit.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"position",8)) _shp_dbf.dbf_position.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"depthflag",9))	_shp_dbf.dbf_depthFlag.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"foundationcolor",15))	_shp_dbf.dbf_foundationColor.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"light",5))	_shp_dbf.dbf_light.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"lighttype",9))	_shp_dbf.dbf_lightType.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"note",4))	_shp_dbf.dbf_note.assign(p_value, getLineLen(p_value,p_nextLine) ); 
			if( hasPrefix(_pLine,_pEnd,"localdesignator",15))	_shp_dbf.dbf_localDesignator.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"internationaldesignator",23))	_shp_dbf.dbf_internationalDesignator.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"period",6))	_shp_dbf.dbf_period.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"heightabovefoundation",21))	_shp_dbf.dbf_height2.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"heightabovefoundationunit",25))	_shp_dbf.dbf_height2unit.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"heightabovedatum",16))	_shp_dbf.dbf_height1.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"heightabovedatumunit",20))	_shp_dbf.dbf_height1unit.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"leadingangle",12))	_shp_dbf.dbf_leadingAngle.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"racon",5))	_shp_dbf.dbf_racon.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"doublelights",12))	_shp_dbf.dbf_doubleLights.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"doublelightshorizontal",22))	_shp_dbf.dbf_doubleLightsHorizontal.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"facilitypoint",13))	_shp_dbf.dbf_facilityPoint.assign(p_value, getLineLen(p_value,p_nextLine) );
			//MARINE END

		} catch(std::exception& e) {
			throw std::invalid_argument("wrong arguments in [-section-]");
		}
		_pLine = p_nextLine;
	} while( !hasPrefix(_pLine,findNextLine(_pLine,_endPointer),"[end",4) );
*/
	//data check
	/*if( _shp_attr_n.type < 0xff ) {
		_shp_attr_n.type <<= 8 | _shp_attr_n.subType;
	}*/
	
//	if( _shp_dbf.type < 0x10 ) _shp_dbf.type | 0x010000;
	
	char *s;
	const char *tail;
/*
	s = sqlite3_mprintf("INSERT INTO MP_POLY (map_id, min_X,min_Y,max_X,max_Y,coordinates_size, coordinates,rgnType,type,label) " \
		" VALUES (%i, %f, %f, %f, %f, ?, ?, %i, %i, '%q'); ", 
		_img_attr_n.id,float(_poly_coord.approx.x0),float(_poly_coord.approx.y0),float(_poly_coord.approx.x1),float(_poly_coord.approx.y1),
		_poly_attr_n.rgnType,_poly_attr_n.type,_poly_attr_s.label.c_str());

	//prepare coordinates
	size_t coord_size[10];
	int t_total_size = 0;
	size_t t_coord_ptr = 0;
	for( int t_l =0; t_l< 10; t_l++ ) {
		coord_size[t_l] = _poly_coord.coord[t_l].size();
		t_total_size += coord_size[t_l];
	}
	double* coord_array = new double[t_total_size*2];

	//copy coordinates

	for( int t_l =0; t_l< 10; t_l++ ) {
		for( vector_points::iterator t_i = _poly_coord.coord[t_l].begin(); t_i < _poly_coord.coord[t_l].end(); t_i++ ) {
			coord_array[t_coord_ptr] = t_i->x;
			coord_array[t_coord_ptr+1] = t_i->y;
			t_coord_ptr+=2;
		}
	}


	sqlite3_stmt *stmt_idx = NULL;
	int b_result = sqlite3_prepare_v2(MPbase, s, -1, &stmt_idx, &tail); 
	b_result = sqlite3_bind_blob(stmt_idx, 1, coord_size , sizeof( coord_size ), SQLITE_STATIC);
	b_result = sqlite3_bind_blob(stmt_idx, 2, coord_array , t_total_size*2*sizeof( double ), SQLITE_STATIC);
	b_result = sqlite3_step(stmt_idx);
	sqlite3_free(s);

	delete []coord_array;
*/
	return _pLine;
}

const char*	MPReader::readPoi(int defaultType,const char* _pLine,const char* _pEnd) {
	const char*	p_nextLine;
	const char* p_value;
	const char* pEnd = NULL;

	struct poi_a {
		unsigned long	rgnType;
		unsigned long	type;
		unsigned long	subType;
		__int64			roadId;
		unsigned long	endLevel;

		unsigned long	zipIdx;
		unsigned long	cityIdx;
	} _poi_attr_n;
	struct poi_s {
		std::string		label;
		std::string		layers;

		std::string		streetDesc;
		std::string		phone;
		std::string		alphaphone;
		std::string		houseNumber;
		std::string		cityName;
		std::string		regionName;
		std::string		countryName;
		std::string		zip;
	} _poi_attr_s;

	struct poi_m {
		std::string		text;
		unsigned long	color;
	} _poi_attr_m;

	struct poi_d {
		float			depth;
		float			unit;
	} _poi_attr_depth;

	struct poi_o {
		float			depth;
		float			unit;
		unsigned long	position;
		unsigned long	depthFlag;
	} _poi_attr_obstruction;

	struct poi_coord {
		rect_t	approx; // approx - coordinates
		point_t coord[10];
	} _poi_coord;

	memset(&_poi_attr_n,0,sizeof _poi_attr_n);
	for( int t_i =0; t_i < 10; t_i++ ) {
		_poi_coord.coord[t_i].x = -1.0;
		_poi_coord.coord[t_i].y = -1.0;
	}

	_poi_attr_n.rgnType = defaultType;
	_poi_attr_depth.unit = _img_attr_n.elevation;
	_poi_attr_obstruction.unit = _img_attr_n.elevation;

	do {
		p_nextLine = findNextLine(_pLine,_endPointer);
		p_value = getValue(_pLine,p_nextLine);

		try {
			if( hasPrefix(_pLine,_pEnd,"layers",6 ) ) _poi_attr_s.layers.assign(p_value, getLineLen(p_value,p_nextLine) );
			
			if( hasPrefix(_pLine,_pEnd,"city",4 ) ) {
				char	t_marine;
				if( parseChar(p_value,p_nextLine,t_marine) ) {
					if( toLower(t_marine) == '1' || toLower(t_marine) == 'y') _poi_attr_n.rgnType = 0x20;
				}
			}

			if( hasPrefix(_pLine,_pEnd,"marine",6 ) ) {
				char	t_marine;
				if( !parseChar(p_value,p_nextLine,t_marine) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");
				else {
					if( toLower(t_marine) == '1' || toLower(t_marine) == 'y') _poi_attr_n.rgnType = 0x04;
				}
			}

			if( hasPrefix(_pLine,_pEnd,"roadid",6 ) )
				if( !parseType(p_value,p_nextLine,&pEnd,_poi_attr_n.roadId) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	

			if( hasPrefix(_pLine,_pEnd,"type",4 ) ) {
				if( !detectObjectType(p_value,p_nextLine,_poi_attr_n.type) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	
			}

			if( hasPrefix(_pLine,_pEnd,"subtype",7 ) )
				if( !parseType(p_value,p_nextLine,&pEnd,_poi_attr_n.subType) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	

			//marine part
			if( hasPrefix(_pLine,_pEnd,"text",4 ) ) {
				if( _poi_attr_m.text.size() )
					_poi_attr_m.text += '\n';
				_poi_attr_m.text.append(p_value, getLineLen(p_value,p_nextLine) );
			}
			if( hasPrefix(_pLine,_pEnd,"color",5 ) )
				if( !parseType(p_value,p_nextLine,&pEnd,_poi_attr_m.color) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	
	
			if( hasPrefix(_pLine,_pEnd,"textfile",8 ) ) {
				std::string	_file;
				_file.assign(p_value, getLineLen(p_value,p_nextLine) );
				
				xor_fstream* text_file = new xor_fstream(_file.c_str(),"r");

				try {
					text_file->SetXorMask(0);
					if( _poi_attr_m.text.size() )
						_poi_attr_m.text += '\n';
					_poi_attr_m.text.append(text_file->ReadString());
				} catch(...) {
					errorClass::showError(-1,_file.c_str(),"W001");
				}
				delete text_file;
			}

			if( hasPrefix(_pLine,_pEnd,"depth",5 ) || hasPrefix(_pLine,_pEnd,"height",6 ) ) {
				if( !parseReal (p_value, p_nextLine, &pEnd,_poi_attr_depth.depth) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	
				_poi_attr_obstruction.depth = _poi_attr_depth.depth;
			}
			
			if( hasPrefix(_pLine,_pEnd,"depthunit",9 ) || hasPrefix(_pLine,_pEnd,"heightunit",10 )) {
				char	t_marine;
				if( !parseChar(p_value,p_nextLine,t_marine) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");
				else {
					if( toLower(t_marine) == 'm' ) _poi_attr_depth.unit = 0.3048006;
					if( toLower(t_marine) == 'f' ) _poi_attr_depth.unit = 1;
				}
				_poi_attr_obstruction.unit = _poi_attr_depth.unit;
			}

			if( hasPrefix(_pLine,_pEnd,"position",8 ) )
				if( !parseType(p_value,p_nextLine,&pEnd,_poi_attr_obstruction.position) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	

			if( hasPrefix(_pLine,_pEnd,"depthflag",9 ) )
				if( !parseType(p_value,p_nextLine,&pEnd,_poi_attr_obstruction.depthFlag) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	
/*
		if( key == "FOUNDATIONCOLOR" && t_read == 2 ) {
			navaid.setFoundationColor(strtol(value.c_str(),NULL,0) );
		}
		if( key == "LIGHT" && t_read == 2 ) {
			ParseLight(value,navaid);
		}
		if( key == "FACILITYPOINT" && t_read == 2) {
			marineFacilities.setFlag(strtol(value.c_str(),NULL,0) );
		}
		if( key == "LIGHTTYPE" && t_read == 2 ) {
			if( value.size() == 1 ) {
				//morse?
				if( char(*value.c_str()) >= 'A' && char(*value.c_str()) <= 'Z' )
					navaid.setLightType(char(*value.c_str()));
				else
				navaid.setLightType(strtol(value.c_str(),NULL,0) );
			} else
				navaid.setLightType(strtol(value.c_str(),NULL,0) );
		}
		if( key == "NOTE" && t_read == 2 ) {
			if( lblcoding == 6 )
				navaid.setNote(legalize_to_lbl(value));
			else
				navaid.setNote(value);
		}
		if( key == "LOCALDESIGNATOR" && t_read == 2 ) {
			if( lblcoding == 6 )
				navaid.setLocalDesignator(legalize_to_lbl(value));
			else
				navaid.setLocalDesignator(value);
		} 
		if( key == "INTERNATIONALDESIGNATOR" && t_read == 2 ) {
			if( lblcoding == 6 )
				navaid.setInternationalDesignator(legalize_to_lbl(value));
			else
				navaid.setInternationalDesignator(value);
		} 
		if( key == "BLACKRED" && t_read == 2 ) {
			if( upper_case(value) == "Y" || upper_case(value) == "1"  )
				navaid.setBlackRed(1);
		}
		if( key == "PERIOD" && t_read == 2) {
			ParsePeriod(value,navaid);
		}
		if( key == "HEIGHTABOVEFOUNDATION" && t_read == 2) {
			const char *p  = value.c_str();
			navaid.setHeight2( ParseFloat( value.c_str(),(value.c_str() + value.length()),&p));
		}
		if( key == "HEIGHTABOVEFOUNDATIONUNIT" && t_read == 2) {
			if( upper_case(value) == "M" ) navaid.setHeight2Meters();
			if( upper_case(value) == "F" ) navaid.setHeight2Feet();
		}
		if( key == "HEIGHTABOVEDATUM" && t_read == 2) {
			const char *p  = value.c_str();
			navaid.setHeight1( ParseFloat( value.c_str(),(value.c_str() + value.length()),&p));
		}
		if( key == "HEIGHTABOVEDATUMUNIT" && t_read == 2) {
			if( upper_case(value) == "M" ) navaid.setHeight1Meters();
			if( upper_case(value) == "F" ) navaid.setHeight1Feet();
		}
		if( key == "LEADINGANGLE" && t_read == 2 ) {
			const char *p  = value.c_str();
			navaid.setLeadingAngle( ParseFloat( value.c_str(),(value.c_str() + value.length()),&p));
		}
		if( key == "RACON" && t_read == 2 ) {
			if( upper_case(value) == "Y" || upper_case(value) == "1"  ) navaid.setRacon();
		}
		if( key == "DOUBLELIGHTS" && t_read == 2 ) {
			navaid.setDoubleLights(strtol(value.c_str(),NULL,0) );
		}
		if( key == "DOUBLELIGHTSHORIZONTAL" && t_read == 2 ) {
			if( upper_case(value) == "Y" || upper_case(value) == "1"  ) navaid.setDoubleLightsOrientationHorizontal();
		}
		//END MARINE


		if(key == "FACILITIES" && t_read == 2)
			facilities = strtol(value.c_str(), NULL, 0);
		if(key == "VHFMONITORED" && t_read == 2)
			vhf_monitored = atoi(value.c_str());
		if(key == "VHFWORKING" && t_read == 2)
			vhf_working = atoi(value.c_str());
		if(key == "PHONEMARINE" && t_read == 2) {
			if( lblcoding == 6 )
				secondaryPhone = legalize_to_lbl(value);
			else
				secondaryPhone = value;
		}
		if(key == "WEBPAGE" && t_read == 2) {
			if( lblcoding == 6 )	webPage = legalize_to_lbl(value);
			else					webPage = value;
		}
*/

			if( hasPrefix(_pLine,_pEnd,"streetdesc",10 ) ) 
				_poi_attr_s.streetDesc.assign(p_value, getLineLen(p_value,p_nextLine) );

			if( hasPrefix(_pLine,_pEnd,"phone",5 ) ) 
				_poi_attr_s.phone.assign(p_value, getLineLen(p_value,p_nextLine) );

			if( hasPrefix(_pLine,_pEnd,"phonealpha",10 ) ) 
				_poi_attr_s.alphaphone.assign(p_value, getLineLen(p_value,p_nextLine) );

			if( hasPrefix(_pLine,_pEnd,"housenumber",11 ) ) 
				_poi_attr_s.houseNumber.assign(p_value, getLineLen(p_value,p_nextLine) );

			if( hasPrefix(_pLine,_pEnd,"cityname",8 ) ) 
				_poi_attr_s.cityName.assign(p_value, getLineLen(p_value,p_nextLine) );

			if( hasPrefix(_pLine,_pEnd,"regionname",10 ) ) 
				_poi_attr_s.regionName.assign(p_value, getLineLen(p_value,p_nextLine) );

			if( hasPrefix(_pLine,_pEnd,"countryname",11 ) ) 
				_poi_attr_s.countryName.assign(p_value, getLineLen(p_value,p_nextLine) );

			if( hasPrefix(_pLine,_pEnd,"zipidx",6 ) )
				if( !parseType(p_value,p_nextLine,&pEnd,_poi_attr_n.zipIdx) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	

			if( hasPrefix(_pLine,_pEnd,"zip=",4 ) ) 
				_poi_attr_s.zip.assign(p_value, getLineLen(p_value,p_nextLine) );

			if( hasPrefix(_pLine,_pEnd,"cityidx",7 ) )
				if( !parseType(p_value,p_nextLine,&pEnd,_poi_attr_n.cityIdx) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	

			if( hasPrefix(_pLine,_pEnd,"levels",6 ) || hasPrefix(_pLine,_pEnd,"endlevel",8 ) )
				if( !parseType(p_value,p_nextLine,&pEnd,_poi_attr_n.endLevel) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	

			if( hasPrefix(_pLine,_pEnd,"label",5 ) ) 
				_poi_attr_s.label.assign(p_value, getLineLen(p_value,p_nextLine) );

			//geograph
			if( hasPrefix(_pLine,_pEnd,"origin",6 ) ) {
				if( _pLine[6] >= '0' && _pLine[6] <= '9' ) {
					if( !parsePoint(p_value,p_nextLine,_poi_coord.coord[ _pLine[6] - '0' ],&pEnd) )
						errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"E017");						
					else
						_poi_coord.approx.Extend(_poi_coord.coord[ _pLine[6] - '0' ]);
				}
			}

			if( hasPrefix(_pLine,_pEnd,"data",4 ) ) {
				if( _pLine[4] >= '0' && _pLine[4] <= '9' ) {
					if( !parsePoint(p_value,p_nextLine,_poi_coord.coord[ _pLine[4] - '0' ],&pEnd) )
						errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"E017");
					else
						_poi_coord.approx.Extend(_poi_coord.coord[ _pLine[4] - '0' ]);
				}
			}
/*
		if(key == "OVERNIGHTPARKING" && t_read == 2)
			overnightPark = (strtol(value.c_str(), NULL, 0) == 1);
		if(key == "HIGHWAYIDX" && t_read == 2)
			exitIdx = strtol(value.c_str(), NULL, 0);
		if(key == "HIGHWAY" && t_read == 2) {
			if( lblcoding == 6 )
				highwayName = legalize_to_lbl(value);
			else
				highwayName = upper_case(value);
		}

		if(key.find("EXIT") < key.length() && t_read == 2) {
			exit_order_number = atoi(key.substr(4, key.length() - 4).c_str());
			while(static_cast<int>(list_exits_name.size()) < exit_order_number) {
				list_exits_name.push_back("");
				list_exits_type_orientation.push_back(0);
				list_exits_facilities.push_back(0);
			}

			// value do podzielenia
			exit_label = ParseExit(value, exit_type, exit_facility, exit_orientation);

			if( lblcoding == 6 )
				list_exits_name[exit_order_number - 1] = legalize_to_lbl(exit_label);
			else
				list_exits_name[exit_order_number - 1] = exit_label;
			
			list_exits_type_orientation[exit_order_number - 1] |= exit_type;
			list_exits_type_orientation[exit_order_number - 1] |= (exit_orientation << 4);
			list_exits_facilities[exit_order_number - 1] = exit_facility;
		}

*/
		} catch(std::exception& e) {
			throw std::invalid_argument("wrong arguments in [-section-]");
		}
		_pLine = p_nextLine;
	} while( !hasPrefix(_pLine,findNextLine(_pLine,_endPointer),"[end",4) );

	//rgnType 0x20 - tylko jeœli type <= 0x11 ! w przeciwnym wypadku 0x10

	//data check
	if( _poi_attr_n.type < 0xff ) {
		_poi_attr_n.type <<= 8 | _poi_attr_n.subType;
	}
	if( defaultType < 0x10 ) _poi_attr_n.type |= 0x010000;
	if( _poi_attr_n.rgnType < 0x10 ) _poi_attr_n.type |= 0x010000;
	
	//0x20 or 0x10?
	if( !(_poi_attr_n.type & 0x010000) && (defaultType >= 0x10) ) {
		if( (_poi_attr_n.type>>8) <= 0x11 )
			_poi_attr_n.rgnType = 0x20;
		else
			_poi_attr_n.rgnType = 0x10;
	}
	if( _poi_attr_n.type & 0x010000 ) 
		_poi_attr_n.rgnType = 0x04;

	char *s;
	const char *tail;

	if( _poi_attr_n.cityIdx > 0 ) {
		s = sqlite3_mprintf("SELECT MP_CITIES_OBS.city, MP_REGIONS_OBS.region, MP_COUNTRIES_OBS.country " \
		" FROM MP_COUNTRIES_OBS, MP_REGIONS_OBS,  MP_CITIES_OBS WHERE " \
		" MP_CITIES_OBS.id = %i AND MP_CITIES_OBS.region_id = MP_REGIONS_OBS.id AND MP_REGIONS_OBS.country_id = MP_COUNTRIES_OBS.id;",
		_poi_attr_n.cityIdx);
	
		getSelectString3(MPbase,s,_poi_attr_s.cityName,_poi_attr_s.regionName,_poi_attr_s.countryName);
		sqlite3_free(s);
	}
	if( _poi_attr_n.zipIdx > 0 ) {
		s = sqlite3_mprintf("SELECT MP_ZIP_OBS.zip " \
		" FROM MP_ZIP_OBS WHERE " \
		" MP_ZIP_OBS.id = %i;",
		_poi_attr_n.zipIdx);
	
		getSelectString(MPbase,s,_poi_attr_s.zip);
		sqlite3_free(s);
	}

	s = sqlite3_mprintf("INSERT INTO MP_POI (map_id,min_X,min_Y,max_X,max_Y,coordinates,rgnType,type,label, streetdesc, phone, alphaphone, housenumber,cityname,regionname,countryname, zip) " \
		" VALUES (%i, %f, %f, %f, %f, ?, %i, %i, '%q', '%q', '%q', '%q', '%q', '%q', '%q', '%q', '%q'); ", 
		_img_attr_n.id, float(_poi_coord.approx.x0),float(_poi_coord.approx.y0),float(_poi_coord.approx.x1),float(_poi_coord.approx.y1),
		_poi_attr_n.rgnType,_poi_attr_n.type,_poi_attr_s.label.c_str(), _poi_attr_s.streetDesc.c_str(), _poi_attr_s.phone.c_str(), _poi_attr_s.alphaphone.c_str(), 
		_poi_attr_s.houseNumber.c_str(),_poi_attr_s.cityName.c_str(),_poi_attr_s.regionName.c_str(),_poi_attr_s.countryName.c_str(), _poi_attr_s.zip.c_str());

	sqlite3_stmt *stmt_idx = NULL;
	int b_result = sqlite3_prepare_v2(MPbase, s, -1, &stmt_idx, &tail); 
	b_result = sqlite3_bind_blob(stmt_idx, 1, _poi_coord.coord , sizeof( _poi_coord.coord ), SQLITE_STATIC);
	b_result = sqlite3_step(stmt_idx);
	sqlite3_free(s);
	sqlite3_finalize(stmt_idx);

	return _pLine;
}

const char*	MPReader::readLine(int defaultType,const char* _pLine,const char* _pEnd) {
	const char*	p_nextLine;
	const char* p_value;
	const char* pEnd = NULL;
	unsigned long	line_id;

	static struct line_a {
		unsigned long	rgnType;
		unsigned long	type;
		unsigned long	subType;
		__int64			roadId;
		unsigned long	endLevel;

		unsigned long	zipIdx;
		unsigned long	cityIdx;
	} _line_attr_n;
	struct line_s {
		std::string		label;
		std::string		label2;
		std::string		label3;
		std::string		layers;

		std::string		cityName;
		std::string		regionName;
		std::string		countryName;
		std::string		zip;
	} _line_attr_s;

	struct line_m {
		std::string		text;
		unsigned long	color;
	} _line_attr_m;

	static struct line_d {
		float			depth;
		float			unit;
	} _line_attr_depth;

	struct line_coord {
		std::vector< poly_t > lines;
	};
	MPLine_route _route_param;

	rect_t	approx; // approx - coordinates
	line_coord _line_coord[10];

	std::vector<MPLine_segment> _line_nodes;


	//clear data
	memset(&_line_attr_n,0,sizeof _line_attr_n);
	_line_attr_depth.depth = 0.0;
	_line_attr_depth.unit = 0.0

	_line_attr_n.rgnType = defaultType;
	_line_attr_depth.unit = _img_attr_n.elevation;

	do {
		p_nextLine = findNextLine(_pLine,_endPointer);
		p_value = getValue(_pLine,p_nextLine);

		try {
			if( hasPrefix(_pLine,_pEnd,"layers",6 ) ) _line_attr_s.layers.assign(p_value, getLineLen(p_value,p_nextLine) );
			
			if( hasPrefix(_pLine,_pEnd,"marine",6 ) ) {
				char	t_marine;
				if( !parseChar(p_value,p_nextLine,t_marine) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");
				else {
					if( toLower(t_marine) == '1' || toLower(t_marine) == 'y') _line_attr_n.rgnType = 0x03;
				}
			}

			if( hasPrefix(_pLine,_pEnd,"roadid",6 ) )
				if( !parseType(p_value,p_nextLine,&pEnd,_line_attr_n.roadId) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	

			if( hasPrefix(_pLine,_pEnd,"type",4 ) ) {
				if( !detectObjectType(p_value,p_nextLine,_line_attr_n.type) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	
			}

			if( hasPrefix(_pLine,_pEnd,"subtype",7 ) )
				if( !parseType(p_value,p_nextLine,&pEnd,_line_attr_n.subType) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	

			//marine part
			if( hasPrefix(_pLine,_pEnd,"color",5 ) )
				if( !parseType(p_value,p_nextLine,&pEnd,_line_attr_m.color) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	
			
			if( hasPrefix(_pLine,_pEnd,"label2",6 ) ) 
				_line_attr_s.label2.assign(p_value, getLineLen(p_value,p_nextLine) );

			if( hasPrefix(_pLine,_pEnd,"label3",6 ) ) 
				_line_attr_s.label3.assign(p_value, getLineLen(p_value,p_nextLine) );

			if( hasPrefix(_pLine,_pEnd,"cityname",8 ) ) 
				_line_attr_s.cityName.assign(p_value, getLineLen(p_value,p_nextLine) );

			if( hasPrefix(_pLine,_pEnd,"regionname",10 ) ) 
				_line_attr_s.regionName.assign(p_value, getLineLen(p_value,p_nextLine) );

			if( hasPrefix(_pLine,_pEnd,"countryname",11 ) ) 
				_line_attr_s.countryName.assign(p_value, getLineLen(p_value,p_nextLine) );

			if( hasPrefix(_pLine,_pEnd,"zipidx",6 ) )
				if( !parseType(p_value,p_nextLine,&pEnd,_line_attr_n.zipIdx) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	

			if( hasPrefix(_pLine,_pEnd,"zip=",4 ) ) 
				_line_attr_s.zip.assign(p_value, getLineLen(p_value,p_nextLine) );

			if( hasPrefix(_pLine,_pEnd,"cityidx",7 ) )
				if( !parseType(p_value,p_nextLine,&pEnd,_line_attr_n.cityIdx) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	

			if( hasPrefix(_pLine,_pEnd,"levels",6 ) || hasPrefix(_pLine,_pEnd,"endlevel",8 ) )
				if( !parseType(p_value,p_nextLine,&pEnd,_line_attr_n.endLevel) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	

			if( hasPrefix(_pLine,_pEnd,"label",5 ) ) 
				_line_attr_s.label.assign(p_value, getLineLen(p_value,p_nextLine) );

			if( hasPrefix(_pLine,_pEnd,"numbers",7 ) ) {
				MPLine_segment l_n;				
				l_n.vertex = -1;
				l_n.node_id = -1;

				parseNumbering(p_value,p_nextLine,l_n) ;
				_line_nodes.push_back(l_n);
			}

			if( hasPrefix(_pLine,_pEnd,"routeparam",10 ) ) {
				parseRouteParam(p_value,p_nextLine,_route_param) ;
			}

			if( hasPrefix(_pLine,_pEnd,"nod",3 ) ) {
				MPLine_segment l_n;				
				l_n.vertex = -1;
				l_n.node_id = -1;
				
				parseNOD(p_value,p_nextLine,l_n) ;
				_line_nodes.push_back(l_n);
			}

			//geograph
			if( hasPrefix(_pLine,_pEnd,"data",4 ) ) {
				if( _pLine[4] >= '0' && _pLine[4] <= '9' ) {
					poly_t new_poly;
					parsePoints (p_value,p_nextLine, new_poly._points);

					approx.Extend( new_poly );
					_line_coord[_pLine[4] - '0'].lines.push_back(new_poly);
				}
			}

		} catch(std::exception& e) {
			throw std::invalid_argument("wrong arguments in [-section-]");
		}
		_pLine = p_nextLine;
	} while( !hasPrefix(_pLine,findNextLine(_pLine,_endPointer),"[end",4) );

	//data check
	if( _line_attr_n.type < 0xff ) {
		_line_attr_n.type <<= 8 | _line_attr_n.subType;
	}
	if( defaultType < 0x10 ) _line_attr_n.type |= 0x010000;
	if( _line_attr_n.rgnType < 0x10 ) _line_attr_n.type |= 0x010000;
	
	if( _line_attr_n.type & 0x010000 ) 
		_line_attr_n.rgnType = 0x03;

	char *s;
	const char *tail;

	if( _line_attr_n.cityIdx > 0 ) {
		s = sqlite3_mprintf("SELECT MP_CITIES_OBS.city, MP_REGIONS_OBS.region, MP_COUNTRIES_OBS.country " \
		" FROM MP_COUNTRIES_OBS, MP_REGIONS_OBS,  MP_CITIES_OBS WHERE " \
		" MP_CITIES_OBS.id = %i AND MP_CITIES_OBS.region_id = MP_REGIONS_OBS.id AND MP_REGIONS_OBS.country_id = MP_COUNTRIES_OBS.id;",
		_line_attr_n.cityIdx);
	
		getSelectString3(MPbase,s,_line_attr_s.cityName,_line_attr_s.regionName,_line_attr_s.countryName);
		sqlite3_free(s);
	}

	if( _line_attr_n.zipIdx > 0 ) {
		s = sqlite3_mprintf("SELECT MP_ZIP_OBS.zip " \
		" FROM MP_ZIP_OBS WHERE " \
		" MP_ZIP_OBS.id = %i;",
		_line_attr_n.zipIdx);
	
		getSelectString(MPbase,s,_line_attr_s.zip);
		sqlite3_free(s);
	}

	int parts_size = 0;
	for( int a = 0; a<10; a++ ) {
		parts_size += 2 * _line_coord[a].lines.size(); //layer + no of vertex
	}

	int*	parts_array = new int[parts_size];
	int		t_total_size = 0;
	int		t_coord_ptr = 0;
	int		part = 0;
	int		s_x,s_y,e_x,e_y;
	std::vector< poly_t >::iterator t_single_line;

	if( _line_coord[0].lines.size() ) {
		t_single_line = _line_coord[0].lines.begin();

		s_x = ((*(*t_single_line)._points.begin()).x * 10000000.0);
		s_y = ((*(*t_single_line)._points.begin()).y * 10000000.0);

		t_single_line = _line_coord[0].lines.end();
		t_single_line--;

		e_x = ((*  ((*t_single_line)._points.end()-1) ).x * 10000000.0);
		e_y = ((*  ((*t_single_line)._points.end()-1) ).y * 10000000.0);
	} else
		s_x = s_y = e_x = e_y = 10000000.0 * 200;

	for( int a = 0; a<10; a++ ) {
		for( std::vector< poly_t >::iterator i_p = _line_coord[a].lines.begin(); i_p != _line_coord[a].lines.end(); i_p ++ ) {
			parts_array[part*2] = a;
			parts_array[part*2+1] = (*i_p)._points.size();
			part++;
			t_total_size += (*i_p)._points.size();
		}
	}

	s = sqlite3_mprintf("INSERT INTO MP_LINE (map_id,road_id,s_x,s_y,e_x,e_y,min_X,min_Y,max_X,max_Y, coordinates ,parts_size , parts, vertex0, rgnType,type,label, label2, label3 , cityname,regionname,countryname, zip, speed , road_class ,one_way , toll ,restriction ) " \
		" VALUES (%i, %lld, %i, %i, %i, %i, %f, %f, %f, %f, ?, %i, ?, %i,  %i, %i, '%q', '%q', '%q', '%q', '%q', '%q' , '%q', %i, %i, %i, %i, '%q'); ",
		_img_attr_n.id,_line_attr_n.roadId,
		s_x,s_y,e_x,e_y,
		float(approx.x0),float(approx.y0),float(approx.x1),float(approx.y1),
		parts_size, _line_coord[0].lines.size()?_line_coord[0].lines[0]._points.size():0,
		_line_attr_n.rgnType,_line_attr_n.type,_line_attr_s.label.c_str(), _line_attr_s.label2.c_str(), _line_attr_s.label3.c_str(),
		_line_attr_s.cityName.c_str(),_line_attr_s.regionName.c_str(),_line_attr_s.countryName.c_str(), _line_attr_s.zip.c_str(),
		_route_param.speed , _route_param.road_class ,_route_param.one_way , _route_param.toll ,_route_param.restriction.c_str() );

	//prepare coordinates
	double* coord_array = new double[t_total_size*2];

	//copy coordinates

	vector_points::iterator t_i;
	for( int t_l =0; t_l< 10; t_l++ ) {
		for( t_single_line = _line_coord[t_l].lines.begin(); t_single_line != _line_coord[t_l].lines.end(); t_single_line ++ ) {
			for( t_i = (*t_single_line)._points.begin(); t_i < (*t_single_line)._points.end(); t_i++ ) {
				coord_array[t_coord_ptr] = t_i->x;
				t_coord_ptr++;
			}
			for( t_i = (*t_single_line)._points.begin(); t_i < (*t_single_line)._points.end(); t_i++ ) {
				coord_array[t_coord_ptr] = t_i->y;
				t_coord_ptr++;
			}
		}
	}

	sqlite3_stmt *stmt_idx = NULL;
	int b_result = sqlite3_prepare_v2(MPbase, s, -1, &stmt_idx, &tail); 
	b_result = sqlite3_bind_blob(stmt_idx, 1, coord_array , t_total_size*2*sizeof( double ), SQLITE_STATIC);
	b_result = sqlite3_bind_blob(stmt_idx, 2, parts_array , parts_size * sizeof(int), SQLITE_STATIC);
	b_result = sqlite3_step(stmt_idx);
	sqlite3_finalize(stmt_idx);
	sqlite3_free(s);

	delete []parts_array;
	delete []coord_array;

	if( b_result == SQLITE_DONE) {
		line_id = (unsigned long)sqlite3_last_insert_rowid(MPbase);


		if( _line_nodes.size() ) {
			//TODO: spawdzenie b³êdów
			for( std::vector<MPLine_segment>::iterator il_n = _line_nodes.begin(); il_n != _line_nodes.end(); il_n++ ) {
				//MP_SEGMENT(id integer primary key autoincrement,line_id int,vertex int, node_id int, bound int, " 

				//exist?
				s = sqlite3_mprintf("SELECT count(*) FROM MP_SEGMENT WHERE line_id = %i AND vertex =  %i; ",line_id, (*il_n).vertex);
				if( getSelectCount(MPbase,s) > 0 ) {
					sqlite3_free(s);
					//update

					if( (*il_n).node_id > -1 ) {
						//routing
						s = sqlite3_mprintf("UPDATE MP_SEGMENT set node_id = %lld, bound = %i WHERE line_id = %i AND vertex =  %i; ",(*il_n).node_id, (*il_n).bound, line_id, (*il_n).vertex);
						b_result = sqlite3_exec(MPbase,s,NULL,NULL,NULL);
						sqlite3_free(s);
					} else {
						//numbering
						s = sqlite3_mprintf("UPDATE MP_SEGMENT set leftCity = '%q', leftRegion = '%q', leftCountry = '%q', " \
							" rightCity = '%q', rightRegion = '%q', rightCountry = '%q', leftZip = '%q', rightZip = '%q', leftStart = %i, leftEnd = %i, leftType = %i, " \
							" rightStart = %i, rightEnd = %i, rightType = %i WHERE line_id = %i AND vertex =  %i; ",
							( (*il_n).leftCity.size() ? (*il_n).leftCity.c_str(): _line_attr_s.cityName.c_str()), 
							( (*il_n).leftRegion.size() ? (*il_n).leftRegion.c_str():_line_attr_s.regionName.c_str()),
							( (*il_n).leftCountry.size() ? (*il_n).leftCountry.c_str():_line_attr_s.countryName.c_str() ),
							( (*il_n).rightCity.size() ? (*il_n).rightCity.c_str():_line_attr_s.cityName.c_str()), 
							( (*il_n).rightRegion.size() ? (*il_n).rightRegion.c_str():_line_attr_s.regionName.c_str()), 
							( (*il_n).rightCountry.size() ? (*il_n).rightCountry.c_str():_line_attr_s.countryName.c_str()),

							( (*il_n).leftZip.size() ? (*il_n).leftZip.c_str():_line_attr_s.zip.c_str() ),
							( (*il_n).rightZip.size() ? (*il_n).rightZip.c_str():_line_attr_s.zip.c_str() ),
							(*il_n).leftStart,(*il_n).leftEnd, (*il_n).leftType,
							(*il_n).rightStart,(*il_n).rightEnd, (*il_n).rightType,
							line_id, (*il_n).vertex);
						b_result = sqlite3_exec(MPbase,s,NULL,NULL,NULL);
						sqlite3_free(s);
					}
				} else {
					sqlite3_free(s);

					s = sqlite3_mprintf("INSERT INTO MP_SEGMENT(line_id,vertex,node_id,bound, " \
						" leftCity , leftRegion , leftCountry , " \
						" rightCity , rightRegion , rightCountry , leftZip , rightZip , leftStart , leftEnd , leftType , " \
						" rightStart , rightEnd , rightType ) " \
						" VALUES( %i, %i, %lld, %i, '%q', '%q','%q','%q','%q','%q','%q','%q',%i,%i,%i,%i,%i,%i); ",						
						line_id, (*il_n).vertex, (*il_n).node_id, (*il_n).bound,
							( (*il_n).leftCity.size() ? (*il_n).leftCity.c_str(): _line_attr_s.cityName.c_str()), 
							( (*il_n).leftRegion.size() ? (*il_n).leftRegion.c_str():_line_attr_s.regionName.c_str()),
							( (*il_n).leftCountry.size() ? (*il_n).leftCountry.c_str():_line_attr_s.countryName.c_str() ),
							( (*il_n).rightCity.size() ? (*il_n).rightCity.c_str():_line_attr_s.cityName.c_str()), 
							( (*il_n).rightRegion.size() ? (*il_n).rightRegion.c_str():_line_attr_s.regionName.c_str()), 
							( (*il_n).rightCountry.size() ? (*il_n).rightCountry.c_str():_line_attr_s.countryName.c_str()),

							( (*il_n).leftZip.size() ? (*il_n).leftZip.c_str():_line_attr_s.zip.c_str() ),
							( (*il_n).rightZip.size() ? (*il_n).rightZip.c_str():_line_attr_s.zip.c_str() ),

						(*il_n).leftStart,(*il_n).leftEnd, (*il_n).leftType,
						(*il_n).rightStart,(*il_n).rightEnd, (*il_n).rightType);
					b_result = sqlite3_exec(MPbase,s,NULL,NULL,NULL);
					sqlite3_free(s);
				}
			}
		}
	}

	return _pLine;
}

const char* MPReader::parseNOD(const char* p_value,const char* p_nextLine,MPLine_segment& l_n) {
	const char* pEnd;
	__int64 _v;

	parseDecInt(p_value,p_nextLine,&pEnd,l_n.vertex);
	skipChar(',',pEnd, p_nextLine, &pEnd);

	parseDecU(pEnd,p_nextLine,&pEnd,_v);
	l_n.node_id = _v;

	if( skipChar(',',pEnd, p_nextLine, &pEnd) ) {
		parseDecInt(pEnd,p_nextLine,&pEnd,l_n.bound);
	}
	return pEnd;
}

const char* MPReader::parseNumbering(const char* _pLine,const char* _pEnd,MPLine_segment& l_n) {
	const char* pEnd;
	std::string		city,region,country,temp_value;

	parseDecInt(_pLine,_pEnd,&pEnd,l_n.vertex); // do zmiany
	if( !skipChar(',',pEnd, _pEnd, &pEnd) )
		return pEnd;

	if( hasPrefix(pEnd,_pEnd,"o",1) || hasPrefix(pEnd,_pEnd,"2",1) ) l_n.leftType = 2;
	if( hasPrefix(pEnd,_pEnd,"e",1) || hasPrefix(pEnd,_pEnd,"1",1) ) l_n.leftType = 1;
	if( hasPrefix(pEnd,_pEnd,"b",1) || hasPrefix(pEnd,_pEnd,"3",1) ) l_n.leftType = 3;

	if( !skipChar(',',pEnd, _pEnd, &pEnd) )
		return pEnd;

	parseDecInt(pEnd,_pEnd,&pEnd,l_n.leftStart);
	if( !skipChar(',',pEnd, _pEnd, &pEnd) )
		return pEnd;

	parseDecInt(pEnd,_pEnd,&pEnd,l_n.leftEnd);
	if( !skipChar(',',pEnd, _pEnd, &pEnd) )
		return pEnd;


	if( hasPrefix(pEnd,_pEnd,"o",1) || hasPrefix(pEnd,_pEnd,"2",1) ) l_n.rightType = 2;
	if( hasPrefix(pEnd,_pEnd,"e",1) || hasPrefix(pEnd,_pEnd,"1",1) ) l_n.rightType = 1;
	if( hasPrefix(pEnd,_pEnd,"b",1) || hasPrefix(pEnd,_pEnd,"3",1) ) l_n.rightType = 3;

	if( !skipChar(',',pEnd, _pEnd, &pEnd) )
		return pEnd;

	parseDecInt(pEnd,_pEnd,&pEnd,l_n.rightStart);
	if( !skipChar(',',pEnd, _pEnd, &pEnd) )
		return pEnd;

	parseDecInt(pEnd,_pEnd,&pEnd,l_n.rightEnd);
	if( !skipChar(',',pEnd, _pEnd, &pEnd) )
		return pEnd;

	if( !parseString(',', pEnd, _pEnd, l_n.leftZip, &pEnd) )
		return pEnd;

	if( !parseString(',', pEnd, _pEnd, l_n.rightZip, &pEnd) )
		return pEnd;

	if( !parseString(',', pEnd, _pEnd, l_n.leftCity, &pEnd) ) return pEnd;
	if( !parseString(',', pEnd, _pEnd, l_n.leftRegion, &pEnd) ) return pEnd;
	if( !parseString(',', pEnd, _pEnd, l_n.leftCountry, &pEnd) ) return pEnd;

	if( !parseString(',', pEnd, _pEnd, l_n.rightCity, &pEnd) ) return pEnd;
	if( !parseString(',', pEnd, _pEnd, l_n.rightRegion, &pEnd) ) return pEnd;
	if( !parseString(',', pEnd, _pEnd, l_n.rightCountry, &pEnd) ) return pEnd;

	return pEnd;
}

const char* MPReader::parseCarRestriction(const char* _pLine,const char* _pEnd,MPLine_car_restriction& _route) {
	const char* pEnd;
	__int64 _v;

	parseDecU(_pLine,_pEnd,&pEnd,_v);
	if( _v == 1 )
		_route.setDenied_emergency();
	if( !skipChar(',',pEnd, _pEnd, &pEnd) )
		return pEnd;

	parseDecU(pEnd,_pEnd,&pEnd,_v);
	if( _v == 1 )
		_route.setDenied_delivery();
	if( !skipChar(',',pEnd, _pEnd, &pEnd) )
		return pEnd;

	parseDecU(pEnd,_pEnd,&pEnd,_v);
	if( _v == 1 )
		_route.setDenied_car();
	if( !skipChar(',',pEnd, _pEnd, &pEnd) )
		return pEnd;

	parseDecU(pEnd,_pEnd,&pEnd,_v);
	if( _v == 1 )
		_route.setDenied_bus();
	if( !skipChar(',',pEnd, _pEnd, &pEnd) )
		return pEnd;

	parseDecU(pEnd,_pEnd,&pEnd,_v);
	if( _v == 1 )
		_route.setDenied_taxi();
	if( !skipChar(',',pEnd, _pEnd, &pEnd) )
		return pEnd;

	parseDecU(pEnd,_pEnd,&pEnd,_v);
	if( _v == 1 )
		_route.setDenied_pedestrian();
	if( !skipChar(',',pEnd, _pEnd, &pEnd) )
		return pEnd;

	parseDecU(pEnd,_pEnd,&pEnd,_v);
	if( _v == 1 )
		_route.setDenied_bicycle();
	if( !skipChar(',',pEnd, _pEnd, &pEnd) )
		return pEnd;

	parseDecU(pEnd,_pEnd,&pEnd,_v);
	if( _v == 1 )
		_route.setDenied_truck();

	return pEnd;
}

const char* MPReader::parseRouteParam(const char* _pLine,const char* _pEnd,MPLine_route& _route) {
	const char* pEnd;
	std::string		city,region,country,temp_value;

	parseDecInt(_pLine,_pEnd,&pEnd,_route.speed);
	if( !skipChar(',',pEnd, _pEnd, &pEnd) )
		return pEnd;

	parseDecInt(pEnd,_pEnd,&pEnd,_route.road_class);
	if( !skipChar(',',pEnd, _pEnd, &pEnd) )
		return pEnd;

	parseDecInt(pEnd,_pEnd,&pEnd,_route.one_way);
	if( !skipChar(',',pEnd, _pEnd, &pEnd) )
		return pEnd;

	parseDecInt(pEnd,_pEnd,&pEnd,_route.toll);
	if( !skipChar(',',pEnd, _pEnd, &pEnd) )
		return pEnd;

	return parseCarRestriction(pEnd,_pEnd,_route);
}

const char*	MPReader::readRestrict(const char* _pLine,const char* _pEnd) {
	MPLine_restriction restr;
	const char*	p_nextLine;
	const char* p_value;
	const char* pEnd = NULL;

	restr.type = 5; //sprawdziæ!
	do {
		p_nextLine = findNextLine(_pLine,_endPointer);
		p_value = getValue(_pLine,p_nextLine);

		try {
			if( hasPrefix(_pLine,_pEnd,"traffpoints",11 ) ) 
				restr.points.assign(p_value, getLineLen(p_value,p_nextLine) );

			if( hasPrefix(_pLine,_pEnd,"traffroads",10 ) ) 
				restr.roads.assign(p_value, getLineLen(p_value,p_nextLine) );

			if( hasPrefix(_pLine,_pEnd,"restrparam",10 ) ) 
				parseCarRestriction(p_value, p_nextLine,restr);
				//restr.roads.assign(p_value, getLineLen(p_value,p_nextLine) );
			
		} catch(std::exception& e) {
			throw std::invalid_argument("wrong arguments in [-section-]");
		}
		_pLine = p_nextLine;
	} while( !hasPrefix(_pLine,findNextLine(_pLine,_endPointer),"[end",4) );

	char* s = sqlite3_mprintf("INSERT INTO MP_RESTRICTIONS (line_id_list, node_id_list, params ) VALUES ('%q','%q','%q'); ", restr.roads.c_str(), restr.points.c_str(), restr.restriction.c_str() );
	int b_result = sqlite3_exec(MPbase,s,NULL,NULL,NULL);
	sqlite3_free(s);

	return _pLine;
}


const char*	MPReader::readPoly(int defaultType,const char* _pLine,const char* _pEnd) {
	const char*	p_nextLine;
	const char* p_value;
	const char* pEnd = NULL;

	static struct poly_a {
		unsigned long	rgnType;
		unsigned long	type;
		unsigned long	subType;
		__int64			roadId;
		unsigned long	endLevel;
	} _poly_attr_n;
	struct poly_s {
		std::string		label;
		std::string		layers;
	} _poly_attr_s;

	struct poly_m {
		std::string		text;
		unsigned long	color;
	} _poly_attr_m;

	static struct poly_d {
		float			depth;
		float			unit;
	} _poly_attr_depth;

	struct poly_coord {
		std::vector< poly_t > polygons;
	};

	rect_t	approx; // approx - coordinates
	poly_coord _poly_coord[10];

	memset(&_poly_attr_n,0,sizeof _poly_attr_n);
	_poly_attr_depth.depth = 0.0;
	_poly_attr_depth.unit = 0.0;

	_poly_attr_n.rgnType = defaultType;
	_poly_attr_depth.unit = _img_attr_n.elevation;

	do {
		p_nextLine = findNextLine(_pLine,_endPointer);
		p_value = getValue(_pLine,p_nextLine);

		try {
			if( hasPrefix(_pLine,_pEnd,"layers",6 ) ) _poly_attr_s.layers.assign(p_value, getLineLen(p_value,p_nextLine) );
			
			if( hasPrefix(_pLine,_pEnd,"marine",6 ) ) {
				char	t_marine;
				if( !parseChar(p_value,p_nextLine,t_marine) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");
				else {
					if( toLower(t_marine) == '1' || toLower(t_marine) == 'y') _poly_attr_n.rgnType = 0x02;
				}
			}

			if( hasPrefix(_pLine,_pEnd,"type",4 ) ) {
				if( !detectObjectType(p_value,p_nextLine,_poly_attr_n.type) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	
			}

			if( hasPrefix(_pLine,_pEnd,"subtype",7 ) )
				if( !parseType(p_value,p_nextLine,&pEnd,_poly_attr_n.subType) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	

			//marine part
			if( hasPrefix(_pLine,_pEnd,"color",5 ) )
				if( !parseType(p_value,p_nextLine,&pEnd,_poly_attr_m.color) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	
			
			if( hasPrefix(_pLine,_pEnd,"levels",6 ) || hasPrefix(_pLine,_pEnd,"endlevel",8 ) )
				if( !parseType(p_value,p_nextLine,&pEnd,_poly_attr_n.endLevel) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	

			if( hasPrefix(_pLine,_pEnd,"label",5 ) ) 
				_poly_attr_s.label.assign(p_value, getLineLen(p_value,p_nextLine) );

			//geograph
			if( hasPrefix(_pLine,_pEnd,"data",4 ) ) {
				if( _pLine[4] >= '0' && _pLine[4] <= '9' ) {
					poly_t new_poly;
					parsePoints (p_value,p_nextLine, new_poly._points);

					approx.Extend( new_poly );
					_poly_coord[_pLine[4] - '0'].polygons.push_back(new_poly);
				}
			}

		} catch(std::exception& e) {
			throw std::invalid_argument("wrong arguments in [-section-]");
		}
		_pLine = p_nextLine;
	} while( !hasPrefix(_pLine,findNextLine(_pLine,_endPointer),"[end",4) );

	//data check
	if( _poly_attr_n.type < 0xff ) {
		_poly_attr_n.type <<= 8 | _poly_attr_n.subType;
	}
	if( defaultType < 0x10 ) _poly_attr_n.type |= 0x010000;
	if( _poly_attr_n.rgnType < 0x10 ) _poly_attr_n.type |= 0x010000;
	
	if( _poly_attr_n.type & 0x010000 ) 
		_poly_attr_n.rgnType = 0x02;

	char *s;
	const char *tail;


	//parts - tablica o strukturze:
	//char - level (0-9)
	//int - ilosc punktow dla polygona (NIE sizeof z double)
	int parts_size = 0;
	int t_total_size = 0;
	int	t_coord_ptr = 0;

	for( int a = 0; a<10; a++ ) {
		parts_size += 2 * _poly_coord[a].polygons.size(); //layer + no of vertex
	}
	int*	parts_array = new int[parts_size];
	int		part = 0;
	for( int a = 0; a<10; a++ ) {
		for( std::vector< poly_t >::iterator i_p = _poly_coord[a].polygons.begin(); i_p != _poly_coord[a].polygons.end(); i_p ++ ) {
			parts_array[part*2] = a;
			parts_array[part*2+1] = (*i_p)._points.size();
			part++;
			t_total_size += (*i_p)._points.size();
		}
	}

	s = sqlite3_mprintf("INSERT INTO MP_POLY (map_id, min_X,min_Y,max_X,max_Y, coordinates, parts_size , parts  ,rgnType,type,label) " \
		" VALUES (%i, %f, %f, %f, %f, ?, %i, ?, %i, %i, '%q'); ", 
		_img_attr_n.id,float(approx.x0),float(approx.y0),float(approx.x1),float(approx.y1),
		parts_size, _poly_attr_n.rgnType,_poly_attr_n.type,_poly_attr_s.label.c_str());

	double* coord_array = new double[t_total_size*2];

	//copy coordinates

	std::vector< poly_t >::iterator t_single_poly;
	vector_points::iterator t_i;
	for( int t_l =0; t_l< 10; t_l++ ) {
		for( t_single_poly = _poly_coord[t_l].polygons.begin(); t_single_poly != _poly_coord[t_l].polygons.end(); t_single_poly ++ ) {
			for( t_i = (*t_single_poly)._points.begin(); t_i < (*t_single_poly)._points.end(); t_i++ ) {
				coord_array[t_coord_ptr] = t_i->x;
				t_coord_ptr++;
			}
			for( t_i = (*t_single_poly)._points.begin(); t_i < (*t_single_poly)._points.end(); t_i++ ) {
				coord_array[t_coord_ptr] = t_i->y;
				t_coord_ptr++;
			}
		}
	}


	sqlite3_stmt *stmt_idx = NULL;
	int b_result = sqlite3_prepare_v2(MPbase, s, -1, &stmt_idx, &tail); 
	b_result = sqlite3_bind_blob(stmt_idx, 1, coord_array , t_total_size*2*sizeof( double ), SQLITE_STATIC);
	b_result = sqlite3_bind_blob(stmt_idx, 2, parts_array , parts_size * sizeof(int), SQLITE_STATIC);
	b_result = sqlite3_step(stmt_idx);
	sqlite3_finalize(stmt_idx);
	sqlite3_free(s);

	delete[] parts_array;
	delete[] coord_array;

	return _pLine;
}

const char* MPReader::readImgId(const char* _pLine,const char* _pEnd) {
	const char*	p_nextLine;
	const char* p_value;
	const char* pEnd = NULL;
	int	b_result;

	memset(&_img_attr_n,0,sizeof _img_attr_n);

	//set defaults
	_img_attr_n.elevation = 1;
	_img_attr_n.drawPriority = 20;
	_img_attr_n.simplifyLevel = 1;
	_img_attr_n.lblCoding = 9;
	_img_attr_n.codePage = 1252;
	_img_attr_n.level[0] = 24;
	_img_attr_n.level[1] = 22;
	_img_attr_n.level[2] = 20;
	_img_attr_n.level[3] = 18;
	_img_attr_n.level[4] = 16;
	_img_attr_n.level[5] = 14;
	_img_attr_n.level[6] = 12;
	_img_attr_n.level[7] = 10;
	_img_attr_n.level[8] = 8;
	_img_attr_n.level[9] = 6;

	_img_attr_n.zoom[0] = 1;
	_img_attr_n.zoom[1] = 2;
	_img_attr_n.zoom[2] = 3;
	_img_attr_n.zoom[3] = 4;
	_img_attr_n.zoom[4] = 5;
	_img_attr_n.zoom[5] = 6;
	_img_attr_n.zoom[6] = 7;
	_img_attr_n.zoom[7] = 8;
	_img_attr_n.zoom[8] = 9;
	_img_attr_n.zoom[9] = 10;

	_img_attr_s.defaultCityCountry = "country";
	_img_attr_s.defaultRegionCountry = "region";

	do {
		p_nextLine = findNextLine(_pLine,_endPointer);
		p_value = getValue(_pLine,p_nextLine);

		try {
			if( hasPrefix(_pLine,_pEnd,"id",2) ) {
				if( hasPrefix(p_value,_pEnd,"random",6) ) {
					char	t_buf[10];
					_img_attr_n.id = (int) time(NULL) % 16999999;
				} else {
					if( !parseType(p_value,p_nextLine,&pEnd,_img_attr_n.id) )
						errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"E029");
				}
				if( _img_attr_n.id < 0x10000 ) 
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W018");
				else if( _img_attr_n.id > 0x0FFFFFFF ) 
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"E036");	
			}
			if( hasPrefix(_pLine,_pEnd,"lock",4 ) )
				if( !parseBool (p_value,p_nextLine,_img_attr_n.lock) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	

			if( hasPrefix(_pLine,_pEnd,"fid",3 ) )
				if( !parseType(p_value,p_nextLine,&pEnd,_img_attr_n.fid) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	

			if( hasPrefix(_pLine,_pEnd,"productcode",11 ) )
				if( !parseType(p_value,p_nextLine,&pEnd,_img_attr_n.productCode) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	

			if( hasPrefix(_pLine,_pEnd,"regionid",8 ) )
				if( !parseType(p_value,p_nextLine,&pEnd,_img_attr_n.regionId) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	

			if( hasPrefix(_pLine,_pEnd,"copyright",9 ) ) _img_attr_s.copyright.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"mapcopyright",12 ) ) _img_attr_s.mapCopyright.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"includelayers",13 ) ) _img_attr_s.includeLayers.assign(p_value, getLineLen(p_value,p_nextLine) );

			if( hasPrefix(_pLine,_pEnd,"elevation",9 ) ) {
				char	t_elevation;
				if( !parseChar(p_value,p_nextLine,t_elevation) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");
				else
					if( toLower(t_elevation) == 'm' )
						_img_attr_n.elevation = 0.3048006;
			}

			if( hasPrefix(_pLine,_pEnd,"drawpriority",12 ) )
				if( !parseType(p_value,p_nextLine,&pEnd,_img_attr_n.drawPriority) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	
/*
			if( hasPrefix(_pLine,_pEnd,"familyid",8 ) )
				if( !parseType(p_value,p_nextLine,&pEnd,_img_attr_n.familyId) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	
*/
			if( hasPrefix(_pLine,_pEnd,"marine",6 ) ) {
				char	t_marine;
				if( !parseChar(p_value,p_nextLine,t_marine) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");
				else {
					if( toLower(t_marine) == '1' || toLower(t_marine) == 'y') _img_attr_n.marine = 1;
					if( toLower(t_marine) == '2' || toLower(t_marine) == 'b') _img_attr_n.marine = 2;
					if( toLower(t_marine) == '3' || toLower(t_marine) == 'x') _img_attr_n.marine = 3;
				}
			}

			if( hasPrefix(_pLine,_pEnd,"preview",7 ) )
				if( !parseBool (p_value,p_nextLine,_img_attr_n.preview) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	

			if( hasPrefix(_pLine,_pEnd,"mg",2 ) )
				if( !parseBool (p_value,p_nextLine,_img_attr_n.mg) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	

			if( hasPrefix(_pLine,_pEnd,"mgnumbering",11 ) || hasPrefix(_pLine,_pEnd,"numbering",9 ) )
				if( !parseBool (p_value,p_nextLine,_img_attr_n.mgNumbering) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	

			if( hasPrefix(_pLine,_pEnd,"routing",7 ) )
				if( !parseBool (p_value,p_nextLine,_img_attr_n.routing) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	

			if( hasPrefix(_pLine,_pEnd,"defaultcitycountry",18 ) || hasPrefix(_pLine,_pEnd,"countryname",11 ) ) 
				_img_attr_s.defaultCityCountry.assign(p_value, getLineLen(p_value,p_nextLine) );

			if( hasPrefix(_pLine,_pEnd,"defaultregioncountry",20 ) || hasPrefix(_pLine,_pEnd,"regionname",10 ) ) 
				_img_attr_s.defaultRegionCountry.assign(p_value, getLineLen(p_value,p_nextLine) );

			if( hasPrefix(_pLine,_pEnd,"simplifylevel",13 ) )
				if( !parseReal (p_value, p_nextLine, &pEnd,_img_attr_n.simplifyLevel) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	

			if( hasPrefix(_pLine,_pEnd,"alignmethod",11 ) )
				if( !parseReal (p_value, p_nextLine, &pEnd,_img_attr_n.alignMethod) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	

			if( hasPrefix(_pLine,_pEnd,"tresize",7 ) )
				if( !parseType(p_value,p_nextLine,&pEnd,_img_attr_n.treSize) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	

			if( hasPrefix(_pLine,_pEnd,"preprocess",10 ) ) {
				char	t_option;
				if( !parseChar(p_value,p_nextLine,t_option) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");
				else {
					if( toLower(t_option) == 'f' || toLower(t_option) == 'y') _img_attr_n.preProcess = 3;
					if( toLower(t_option) == 'g') _img_attr_n.preProcess = 2;
					if( toLower(t_option) == 'p') _img_attr_n.preProcess = 7;
				}
			}

			if( hasPrefix(_pLine,_pEnd,"codepage",8 ) )
				if( !parseType(p_value,p_nextLine,&pEnd,_img_attr_n.codePage) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	

			if( hasPrefix(_pLine,_pEnd,"lblcoding",9 ) )
				if( !parseType(p_value,p_nextLine,&pEnd,_img_attr_n.lblCoding) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	

			if( hasPrefix(_pLine,_pEnd,"transparent",11 ) ) {
				char	t_option;
				if( !parseChar(p_value,p_nextLine,t_option) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");
				else {
					if( toLower(t_option) == 'y') _img_attr_n.transparent = 1;
					if( toLower(t_option) == 's') _img_attr_n.transparent = 2;
				}
			}

			if( hasPrefix(_pLine,_pEnd,"poiindex",8 ) )
				if( !parseBool (p_value,p_nextLine,_img_attr_n.poiIndex) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	

			if( hasPrefix(_pLine,_pEnd,"poinumberfirst",14 ) )
				if( !parseBool (p_value,p_nextLine,_img_attr_n.poiNumberFirst) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	

			if( hasPrefix(_pLine,_pEnd,"poizipfirst",11 ) )
				if( !parseBool (p_value,p_nextLine,_img_attr_n.poiZipFirst) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	

			if( hasPrefix(_pLine,_pEnd,"name",4 ) ) _img_attr_s.name.assign(p_value, getLineLen(p_value,p_nextLine) );
			if( hasPrefix(_pLine,_pEnd,"datum",5 ) ) _img_attr_s.datum.assign(p_value, getLineLen(p_value,p_nextLine) );

			if( hasPrefix(_pLine,_pEnd,"rgnlimit",8 ) )
				if( !parseType(p_value,p_nextLine,&pEnd,_img_attr_n.rgnLimit) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	

			if( hasPrefix(_pLine,_pEnd,"levels",6 ) ) {
				if( !parseType(p_value,p_nextLine,&pEnd,_img_attr_n.levels) )
					errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	
				else {
					if( pEnd,_img_attr_n.levels > 8 )
						errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"E020");	
					if( pEnd,_img_attr_n.levels < 1 )
						errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"E033");	
				}
			}

			if( hasPrefix(_pLine,_pEnd,"level",5 ) ) {
				if( _pLine[5] >= '0' && _pLine[5] <= '9' ) {
					if( !parseType(p_value,p_nextLine,&pEnd,_img_attr_n.level[ _pLine[5] - '0' ] ) )
						errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	
					else {
						if( _img_attr_n.level[ _pLine[5] - '0' ] > 24 )
							errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	
					}
				}
			}

			if( hasPrefix(_pLine,_pEnd,"zoom",4 ) ) {
				if( _pLine[5] >= '0' && _pLine[5] <= '9' ) {
					if( !parseType(p_value,p_nextLine,&pEnd,_img_attr_n.zoom[ _pLine[5] - '0' ] ) )
						errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"W025");	
				}
			}

		} catch(std::exception& e) {
			throw std::invalid_argument("wrong arguments in [IMG ID]");
		}
		_pLine = p_nextLine;
	} while( !hasPrefix(_pLine,findNextLine(_pLine,_endPointer),"[end",4) );

	//error checks
	if( !_img_attr_n.id )
		errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"E029");

	for(int t_read = 0; (t_read+1) < _img_attr_n.levels; t_read++) {
		if((_img_attr_n.level[t_read] & 0x3f) <= (_img_attr_n.level[t_read + 1] & 0x3f)) {
			errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"E021");				
		}
	}
	for(int t_read = 0; (t_read+1) < _img_attr_n.levels; t_read++) {
		if( _img_attr_n.zoom[t_read] >= _img_attr_n.zoom[t_read + 1]) {
			errorClass::showError(calculateLineNumber(p_value),file_name.c_str(),"E025");
		}
	}

	//DBstore
	char *s;
	
	s = sqlite3_mprintf("INSERT INTO MP_ID (id ,lock , fid , productCode , regionId , copyright , " \
		" mapcopyright , includelayers , elevation , drawPriority , familyId , marine , preview , mg , mgnumbering ,  " \
		" routing , DEFAULTCITYCOUNTRY , DEFAULTREGIONCOUNTRY , SIMPLIFYLEVEL , PREPROCESS , " \
		" CODEPAGE , LBLCODING , TRANSPARENT , TRANSPARENTEMPTY , POIONLY , POIINDEX , POINUMBERFIRST , " \
		" POIZIPFIRST , NAME , ALIGNMETHOD , TRESIZE , TREMARIGIN , DATUM , RGNLIMIT , LEVELS ) " \
		" VALUES " \
		" (%i,%i, %i, %i, %i, '%q', " \
		" '%q', '%q', %f, %i, %i, %i, %i, %i, %i,  " \
		" %i, '%q', '%q', %f, %i, " \
		" %i, %i, %i, %i, %i,%i, %i, " \
		" %i, '%q', %f, %i, %i, '%q', %i, %i) ",
		_img_attr_n.id ,_img_attr_n.lock , _img_attr_n.fid , _img_attr_n.productCode , _img_attr_n.regionId ,_img_attr_s.copyright.c_str() , 
		_img_attr_s.mapCopyright.c_str() , _img_attr_s.includeLayers.c_str() , _img_attr_n.elevation , _img_attr_n.drawPriority , _img_attr_n.familyId , _img_attr_n.marine , _img_attr_n.preview , _img_attr_n.mg , _img_attr_n.mgNumbering ,  
		_img_attr_n.routing , _img_attr_s.defaultCityCountry.c_str() , _img_attr_s.defaultRegionCountry.c_str() , _img_attr_n.simplifyLevel , _img_attr_n.preProcess , 
		_img_attr_n.codePage , _img_attr_n.lblCoding , _img_attr_n.transparent , 0 , 0, _img_attr_n.poiIndex, _img_attr_n.poiNumberFirst , 
		_img_attr_n.poiZipFirst , _img_attr_s.name.c_str() , _img_attr_n.alignMethod, _img_attr_n.treSize, 0, _img_attr_s.datum.c_str(), 1024 , _img_attr_n.levels);

	b_result = sqlite3_exec(MPbase,s,NULL,NULL,NULL);
	sqlite3_free(s);

	for( int t_tmp =0;  t_tmp < _img_attr_n.levels; t_tmp++ ) {
		s = sqlite3_mprintf("INSERT INTO MP_LEVELS(id ,level , zoom ) VALUES( %i, %i, %i );",_img_attr_n.id,_img_attr_n.level[t_tmp],_img_attr_n.zoom[t_tmp]);
		b_result = sqlite3_exec(MPbase,s,NULL,NULL,NULL);
		sqlite3_free(s);
	}

	return _pLine;
}

