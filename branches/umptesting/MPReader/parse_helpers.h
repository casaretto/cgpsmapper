#ifndef __PARSE_HELPERS
#define __PARSE_HELPERS

#include <string>
#include "point.h"

namespace TXTparser {

	const char * getValue (const char * _pKey, const char * _pEnd);
	bool parseDecU (const char * _pValue, const char * _pEnd, const char ** _ppEnd, __int64 & _ulRet);
	bool parseHex (const char * _pValue, const char * _pEnd, const char ** _ppEnd, __int64 & _ulRet);

	bool parseDecInt (const char * _pValue, const char * _pEnd, const char ** _ppEnd, int & _ulRet);
	bool parseDecUL (const char * _pValue, const char * _pEnd, const char ** _ppEnd, unsigned long & _ulRet);
	bool parseHexUL (const char * _pValue, const char * _pEnd, const char ** _ppEnd, unsigned long & _ulRet);


	bool parseType (const char * _pValue, const char * _pEnd, const char ** _ppEnd, unsigned long & _ulType);
	bool parseType (const char * _pValue, const char * _pEnd, const char ** _ppEnd, __int64 & _ulType);

	bool parseBool (const char * _pValue, const char * _pEnd, bool & _ulRet);
	bool parseChar (const char * _pValue, const char * _pEnd, char & _cRet);
	bool parseReal (const char * _pValue, const char * _pEnd, const char ** _ppEnd, double & _fRet);
	bool parseReal (const char * _pValue, const char * _pEnd, const char ** _ppEnd, float & _fRet);
	bool parsePoint (const char *_pValue, const char *_pEnd, point_t &_pt, const char **_ppEnd);
	bool parseString (const char terminator, const char *_pValue, const char *_pEnd, std::string &_s, const char **_ppEnd);
	bool skipChar (const char terminator, const char *_pValue, const char *_pEnd, const char **_ppEnd);
	void parsePoints(const char * _pValue, const char * _pEnd, vector_points & _points);
	char toLower (char _c);
	bool __fastcall hasPrefix (const char * _pBegin, const char * _pEnd, const char * _pPrefix, size_t _cPrefixLen);
}
#endif