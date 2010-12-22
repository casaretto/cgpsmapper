#ifndef __MP_READ
#define __MP_READ

#include <boost\interprocess\file_mapping.hpp>
#include <boost\interprocess\mapped_region.hpp>
#include <string>
#include "typeTranslation.h"
#include ".\..\sqlite-source\sqlite3.h"
#include "mp_types.h"
#include "mp_process.h"
#include "wx\wx.h"
#include "wx\tokenzr.h"


class MPReader {
private:
	bool		_constructor_error;
	sqlite3		*MPbase;
	boost::interprocess::file_mapping*	_mp_file;
	std::string	file_name;
	typeReader	type_translation;


	const char * findNextLine (const char * _pBegin, const char * _pEnd);
	size_t	getLineLen (const char * _pBegin, const char * _pNextLine);

	int		calculateLineNumber(const char *_pLine);

	int		detectType(const char* _pLine,const char* _pEnd);
	bool	detectObjectType(const char * _pValue, const char * _pEnd, unsigned long & _ulRet);

	const char* parseNOD(const char* _pLine,const char* _pEnd,MPLine_segment& segment);
	const char* parseRouteParam(const char* _pLine,const char* _pEnd,MPLine_route& _route);
	const char* parseCarRestriction(const char* _pLine,const char* _pEnd,MPLine_car_restriction& _restr);
	const char* parseNumbering(const char* _pLine,const char* _pEnd,MPLine_segment& segment);

	const char*	readImgId(const char* _pBegin,const char* _pEnd);
	const char*	readCountries(const char* _pBegin,const char* _pEnd);
	const char*	readRegions(const char* _pBegin,const char* _pEnd);
	const char*	readCities(const char* _pBegin,const char* _pEnd);
	const char*	readZip(const char* _pBegin,const char* _pEnd);
	const char*	readPoi(int defaultType,const char* _pBegin,const char* _pEnd);
	const char*	readLine(int defaultType,const char* _pBegin,const char* _pEnd);
	const char*	readPoly(int defaultType,const char* _pBegin,const char* _pEnd);
	const char*	readShp(int defaultType,const char* _pBegin,const char* _pEnd);
	const char*	readRestrict(const char* _pBegin,const char* _pEnd);

	void assignResstrictions();

	const char* _beginPointer;
	const char*	_endPointer;

	struct {
		unsigned long id;
		bool lock;
		unsigned long fid;
		unsigned long productCode;
		unsigned long regionId;
		float elevation;
		unsigned long drawPriority;
		unsigned long familyId;
		int marine;
		bool preview;
		bool mg;
		bool mgNumbering;
		bool routing;
		float simplifyLevel;
		int preProcess;
		unsigned long codePage;
		unsigned long lblCoding;
		int transparent;
		bool poiIndex;
		bool poiNumberFirst;
		bool poiZipFirst;
		float alignMethod;
		unsigned long treSize;
		int TREMARIGIN;
		unsigned long rgnLimit;
		unsigned long levels;
		unsigned long level[10];
		unsigned long zoom[10];
	} _img_attr_n;
	struct img_attr {
		std::string name;
		std::string datum;
		std::string defaultCityCountry;
		std::string defaultRegionCountry;
		std::string copyright;
		std::string mapCopyright;
		std::string includeLayers;
	} _img_attr_s;

public:
	MPReader();
	MPReader(const char* _fileName);
	~MPReader();

	bool	constructorError() { return _constructor_error; };
	bool	isOK() {return !_constructor_error;};

	_mp_section	selectSection(const char* _pBegin, const char* pEnd);


	//MAIN - reading MP format
	bool	readMP();

	void	setDB(sqlite3* _MPbase);
};


#endif