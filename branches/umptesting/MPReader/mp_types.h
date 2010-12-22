#ifndef __MP_TYPES
#define __MP_TYPES

enum _mp_section {_none, _imgid, _dictionary, _poi, _polyline, _polygon, _shp, _countries, _regions, _cities, _zip, _restriction};

struct _value_type {
	double		double_v;
	char*		string_v;
};

#define MAX_LBL_LENGTH 80

#endif