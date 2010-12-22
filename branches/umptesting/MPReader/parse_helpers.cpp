#include <memory>
#include <assert.h>
#include "parse_helpers.h"

namespace TXTparser {

	//Reader parsers
	const char * getValue (const char * _pKey, const char * _pEnd) {
		if (_pKey >= _pEnd)
			return _pEnd;
		const char * const p = reinterpret_cast<const char *> (::memchr (_pKey, '=', _pEnd - _pKey));
		return p ? p + 1 : _pEnd;
	}

	bool parseType (const char * _pValue, const char * _pEnd, const char ** _ppEnd, unsigned long & _ulType) {
		const char * p = _pValue;
		// Omit spaces.
		while (p < _pEnd && * p == ' ')
			++ p;
		if (p + 1 < _pEnd && p [0] == '0' && p [1] == 'x')
			return parseHexUL (p + 2, _pEnd, _ppEnd, _ulType);
		else
			return parseDecUL (p, _pEnd, _ppEnd, _ulType);
	}

	bool parseType (const char * _pValue, const char * _pEnd, const char ** _ppEnd, __int64 & _ulType) {
		const char * p = _pValue;
		// Omit spaces.
		while (p < _pEnd && * p == ' ')
			++ p;
		if (p + 1 < _pEnd && p [0] == '0' && p [1] == 'x')
			return parseHex (p + 2, _pEnd, _ppEnd, _ulType);
		else
			return parseDecU (p, _pEnd, _ppEnd, _ulType);
	}

	bool parseBool (const char * _pValue, const char * _pEnd, bool & _ulRet) {
		const char * p = _pValue;
		_ulRet = false;
		// Omit spaces.
		while (p < _pEnd && * p == ' ')
			++ p;
		if( p[0] == '0' || toLower(p[0]) == 'n')
			_ulRet = false;
		else if( p[0] == '1' || toLower(p[0]) == 'y')
			_ulRet = true;
		else
			return false;
		return true;
	}

	bool parseChar (const char * _pValue, const char * _pEnd, char & _cRet) {
		const char * p = _pValue;
		_cRet = 0;
		// Omit spaces.
		while (p < _pEnd && * p == ' ')
			++ p;
		if( p[0] >= 32 )
			_cRet = p[0];
		else
			return false;
		return true;
	}

	bool parseDecUL (const char * _pValue, const char * _pEnd, const char ** _ppEnd, unsigned long & _ulRet){
		unsigned long r = 0;
		bool bHasDigits = false;
		const char * p = _pValue;

		// Omit spaces.
		while (p < _pEnd && * p == ' ')
			++ p;

		while (p < _pEnd && '0' <= * p && * p <= '9') {
			r *= 10;
			r += (* p) - '0';
			++ p;
			bHasDigits = true;
		}

		if (bHasDigits && _ppEnd)
			* _ppEnd = p;
		_ulRet = r;

		return bHasDigits;
	}
	bool parseHexUL (const char * _pValue, const char * _pEnd, const char ** _ppEnd, unsigned long & _ulRet) {
		unsigned long r = 0;
		bool bHasDigits = false;
		const char * p = _pValue;

		// Omit spaces.
		while (p < _pEnd && * p == ' ')
			++ p;

		while (p < _pEnd) {
			if ('0' <= * p && * p <= '9') {
				r *= 16;
				r += (* p) - '0';
			} else if ('a' <= * p && * p <= 'f') {
				r *= 16;
				r += (* p) - 'a' + 10;
			} else if ('A' <= * p && * p <= 'F') {
				r *= 16;
				r += (* p) - 'A' + 10;
			} else
				break;
			++ p;
			bHasDigits = true;
		}

		if (bHasDigits && _ppEnd)
			* _ppEnd = p;
		_ulRet = r;
		return bHasDigits;
	}

	bool parseHex (const char * _pValue, const char * _pEnd, const char ** _ppEnd, __int64 & _ulRet) {
		__int64 r = 0;
		bool bHasDigits = false;
		const char * p = _pValue;

		// Omit spaces.
		while (p < _pEnd && * p == ' ')
			++ p;

		while (p < _pEnd) {
			if ('0' <= * p && * p <= '9') {
				r *= 16;
				r += (* p) - '0';
			} else if ('a' <= * p && * p <= 'f') {
				r *= 16;
				r += (* p) - 'a' + 10;
			} else if ('A' <= * p && * p <= 'F') {
				r *= 16;
				r += (* p) - 'A' + 10;
			} else
				break;
			++ p;
			bHasDigits = true;
		}

		if (bHasDigits && _ppEnd)
			* _ppEnd = p;
		_ulRet = r;
		return bHasDigits;
	}

	bool skipChar (const char terminator, const char *_pValue, const char *_pEnd, const char **_ppEnd) {
		bool bHasTerminator = false;
		const char * p = _pValue;
		
		while (p < _pEnd && terminator != * p ) {
			++ p;
		}
		if( terminator == * p && p < _pEnd) {
			bHasTerminator = true;
			++ p;
		}

		if (bHasTerminator && _ppEnd)
			* _ppEnd = p;
		return bHasTerminator;
	}

	bool parseDecInt (const char * _pValue, const char * _pEnd, const char ** _ppEnd, int & _ulRet) {
		int r = 0;
		bool bHasDigits = false;
		const char * p = _pValue;

		// Omit spaces.
		while (p < _pEnd && * p == ' ')
			++ p;

		while (p < _pEnd && '0' <= * p && * p <= '9') {
			r *= 10;
			r += (* p) - '0';
			++ p;
			bHasDigits = true;
		}

		if (bHasDigits && _ppEnd)
			* _ppEnd = p;
		_ulRet = r;

		return bHasDigits;
	}

	bool parseDecU (const char * _pValue, const char * _pEnd, const char ** _ppEnd, __int64 & _ulRet) {
		__int64 r = 0;
		bool bHasDigits = false;
		const char * p = _pValue;

		// Omit spaces.
		while (p < _pEnd && * p == ' ')
			++ p;

		while (p < _pEnd && '0' <= * p && * p <= '9') {
			r *= 10;
			r += (* p) - '0';
			++ p;
			bHasDigits = true;
		}

		if (bHasDigits && _ppEnd)
			* _ppEnd = p;
		_ulRet = r;
		return bHasDigits;
	}
	inline char toLower (char _c) {
		if ('A' <= _c && _c <= 'Z')
			return _c - 'A' + 'a';
		else
			return _c;
	}

	bool __fastcall hasPrefix (const char * _pBegin, const char * _pEnd, const char * _pPrefix, size_t _cPrefixLen) {
		assert( strlen(_pPrefix) == _cPrefixLen );
		if (_pBegin + _cPrefixLen > _pEnd)
			return false;

		if (toLower (_pBegin [0]) != toLower (_pPrefix [0]))
			return false;

		++ _pPrefix;
		for (const char * p = _pBegin + 1; p < _pBegin + _cPrefixLen; ++ p, ++ _pPrefix) {
			if (* p == * _pPrefix)
				continue;

			if (toLower (* p) == toLower (* _pPrefix))
				continue;

			return false;
		}

		return true;
	}
	bool parseReal (const char * _pValue, const char * _pEnd, const char ** _ppEnd, float & _fRet) {
		float r = 0;
		bool bMinus = false;
		bool bHasDigits = false;
		const char * p = _pValue;

		// Omit spaces.
		while (p < _pEnd && * p == ' ')
			++ p;

		// Get sign.
		if (p < _pEnd) {
			if (* p == '-') {
				++ p;
				bMinus = true;
			} else if (* p == '+')
				++ p;
		}

		// Get whole part of the digit.
		while (p < _pEnd && '0' <= * p && * p <= '9') {
			r *= 10;
			r += (* p) - '0';
			++ p;
			bHasDigits = true;
		}

		// Get decimal dot.
		if (p == _pEnd || (p < _pEnd && * p != '.')) {
			if (bHasDigits && _ppEnd)
				* _ppEnd = p;
			_fRet = bMinus ? -r : r;
			return bHasDigits;
		}
		++ p;
		const char * pFraction = p;

		// Get fractional part of the digit.
		float f = 0;
		float pw = 0.1f;
		while (p < _pEnd && '0' <= * p && * p <= '9') {
			f += ((* p) - '0')*pw;
			++ p;
			pw *= .1f;
			bHasDigits = true;
		}

		if (bHasDigits && _ppEnd)
			* _ppEnd = p;
		_fRet = bMinus ? -(r + f) : (r + f);
		return bHasDigits;
	}

	bool parseReal (const char * _pValue, const char * _pEnd, const char ** _ppEnd, double & _fRet) {
		double r = 0;
		bool bMinus = false;
		bool bHasDigits = false;
		const char * p = _pValue;

		// Omit spaces.
		while (p < _pEnd && * p == ' ')
			++ p;

		// Get sign.
		if (p < _pEnd) {
			if (* p == '-') {
				++ p;
				bMinus = true;
			} else if (* p == '+')
				++ p;
		}

		// Get whole part of the digit.
		while (p < _pEnd && '0' <= * p && * p <= '9') {
			r *= 10;
			r += (* p) - '0';
			++ p;
			bHasDigits = true;
		}

		// Get decimal dot.
		if (p == _pEnd || (p < _pEnd && * p != '.')) {
			if (bHasDigits && _ppEnd)
				* _ppEnd = p;
			_fRet = bMinus ? -r : r;
			return bHasDigits;
		}
		++ p;
		const char * pFraction = p;

		// Get fractional part of the digit.
		double f = 0;
		double pw = 0.1f;
		while (p < _pEnd && '0' <= * p && * p <= '9') {
			f += ((* p) - '0')*pw;
			++ p;
			pw *= .1f;
			bHasDigits = true;
		}

		if (bHasDigits && _ppEnd)
			* _ppEnd = p;
		_fRet = bMinus ? -(r + f) : (r + f);
		return bHasDigits;
	}
	bool parseString (const char terminator, const char *_pValue, const char *_pEnd, std::string &_s, const char **_ppEnd) {
		const char * p = _pValue;
		bool	bHasTerminator = false;
		_s="";

		while (p < _pEnd && * p != terminator ) {
			_s += *p;
			++ p;
		}
		if( *p == terminator ) {
			bHasTerminator = true;
			++ p;
		}
		* _ppEnd = p;
		return bHasTerminator;
	}

	bool parsePoint (const char * _pValue, const char * _pEnd, point_t & _pt, const char ** _ppEnd) {
		const char * p = _pValue;

		// Omit spaces.
		while (p < _pEnd && * p == ' ')
			++ p;
		// Get opening bracket.
		if (p >= _pEnd || * p != '(') {
			* _ppEnd = _pEnd;
			return false;
		}
		++ p;
		while (p < _pEnd && * p == ' ')
			++ p;

		// Get latitude.
		if (! parseReal (p, _pEnd, & p, _pt.y)) { //NS
			//ReportWarning (m_cOffset, "Latitude value is expected. The point will be ignored.");
			* _ppEnd = _pEnd;
			return false;
		}
		// Omit spaces.
		while (p < _pEnd && * p == ' ')
			++ p;
		// Get delimiting comma.
		if (p >= _pEnd || * p != ',') {
			//ReportWarning (m_cOffset, "Comma between latitude and longitude is expected. The point will be ignored.");
			* _ppEnd = _pEnd;
			return false;
		}
		++ p;

		// Omit spaces.
		while (p < _pEnd && * p == ' ')
			++ p;

		// Get longitude.
		if (! parseReal (p, _pEnd, & p, _pt.x)) {
			//ReportWarning (m_cOffset, "Longitude value is expected. The point will be ignored.");
			* _ppEnd = _pEnd;
			return false;
		}

		// Omit spaces.
		while (p < _pEnd && * p == ' ')
			++ p;

		// Get closing bracket.
		if (p >= _pEnd || * p != ')') {
			//ReportWarning (m_cOffset, "Closing bracket for point is expected. The point will be ignored.");
			* _ppEnd = _pEnd;
			return false;
		}
		++ p;

		* _ppEnd = p;
		/*
		// Check coordinates.
		if (90 <= _pt.y || _pt.y <= -90) {
		ReportWarning (m_cOffset, "Invalid latitude value: %0.5f. The point will be ignored.", _pt.y);
		return false;
		}
		if (180 < _pt.x || _pt.x < -180) {
		ReportWarning (m_cOffset, "Invalid longitude value: %0.5f. The point will be ignored.", _pt.x);
		return false;
		}
		*/
		return true;
	}

	void parsePoints (const char * _pValue, const char * _pEnd, vector_points & _points) 
	{
		for (const char * p = _pValue; p < _pEnd; ++ p) {
			point_t pt;
			if (parsePoint (p, _pEnd, pt, & p))
				_points.push_back (pt);

			// Get point-delimiting comma.
			while (p < _pEnd && * p != ',')
				++ p;
		}
	}

}