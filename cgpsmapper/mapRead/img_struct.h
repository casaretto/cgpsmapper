/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#ifndef __img_structures_h
#define __img_structures_h

const float c_fGarminUnit = 180.0 / (1UL << 23);

typedef unsigned char B2_t [2];
typedef unsigned char B3_t [3];
typedef unsigned char B4_t [4];

struct t_map_levels {
	unsigned char	level[10];	
	bool			empty[10];
	unsigned long	level_address[10];
	unsigned int	level_start_index[10];
	char			map_levels;
	unsigned int	last_tre_idx;
};

struct t_tre_coords {
	B3_t	y1;
	B3_t	x1;
	B3_t	y0;
	B3_t	x0;
};	

struct tre_header {
	B4_t	tre_levels_start;
	B4_t	tre_levels_len;
	B4_t	tre_regions_start;
	B4_t	tre_regions_len;
};

#pragma pack(1)
struct t_tre_parent {
	B2_t		tre_child;
	unsigned	rgn_end : 28;
	unsigned	types : 4;
};

struct t_tre_child {
	unsigned	rgn_end : 28;
	unsigned	types : 4;
};

struct tre {
	struct {
		unsigned rgn_start : 28; //3.5 byte
		unsigned types : 4;//rgn types
	};
	B3_t			x,y;
	B2_t			x_span;
	B2_t			y_span;
	union {
		t_tre_parent	parent;
		t_tre_child		child;
	};	
};
#pragma pack()


struct rgn_table {
	char	types;
	unsigned long	rgn_address[5];
};

struct rgn_extended {
	unsigned long	rgn2_address;
	unsigned long	rgn3_address;
	unsigned long	rgn4_address;

	unsigned long	rgn2_address_end;
	unsigned long	rgn3_address_end;
	unsigned long	rgn4_address_end;
};

//Conversion independent of the processor type - hi-lo or lo-hi
inline unsigned __int16 ConvertB2ToUInt16 (const unsigned char* _data) {
    return (static_cast<unsigned __int16>(_data [1]) << 8) + static_cast<unsigned __int16>(_data [0]);
}

inline __int16 ConvertB2ToInt16 (const unsigned char* _data) {
    return (static_cast<__int16>(_data [1]) << 8) + static_cast<__int16> (_data [0]);
}

inline __int16 ConvertB2ToInt16 (const B2_t& _data) {
    return (static_cast<__int16> (_data [1]) << 8) + static_cast<__int16> (_data [0]);
}

inline unsigned __int32 ConvertB3ToUInt (const B3_t& _data) {
    return (static_cast<unsigned __int32> (_data [2]) << 16) + (static_cast<unsigned __int32> (_data [1]) << 8) + static_cast<unsigned __int32> (_data [0]);
}

inline __int32 ConvertB3ToInt (const B3_t& _data) {
    __int32 l = (static_cast<unsigned __int32> (_data [2]) << 16) + (static_cast<unsigned __int32> (_data [1]) << 8) + static_cast<unsigned __int32> (_data [0]);
    if (l >= 0x800000)
        l -= 0x1000000;
	return l;    
}

inline unsigned __int32 ConvertB4ToUInt32 (const unsigned char* _data) {
    return (static_cast<unsigned __int32> (_data [3]) << 24) + (static_cast<unsigned __int32> (_data [2]) << 16) + (static_cast<unsigned __int32> (_data [1]) << 8) + static_cast<unsigned __int32> (_data [0]);
}

inline float ConvertB3ToDeg (const B3_t& _data) {
    int l = ConvertB3ToInt (_data);	
	return l*c_fGarminUnit;
}

inline long ConvertB2ToGU (const char & _cShiftBits, long _2b, long _3b0) {
    if (_2b >= 0x8000)
        _2b -= 0x10000;

	if (_3b0 >= 0x800000)
		_3b0 -= 0x1000000;

	return _3b0 + (_2b << _cShiftBits);
}

//some basic structures and operations
struct t_coord {
	// Longitude and latitude, in degrees.
	float we, ns;

	t_coord () {}
	t_coord (float _we, float _ns) : we (_we), ns (_ns) {}

	bool operator == (const t_coord & _other) const {return we == _other.we && ns == _other.ns;}
};

struct t_coord_i {
	// Longitude and latitude, in degrees.
	long we, ns;

	t_coord_i () {}
	t_coord_i (const long _we, const long _ns) : we (_we), ns (_ns) {}

	bool operator == (const t_coord_i & _other) const {return we == _other.we && ns == _other.ns;}
};

//road structure
struct t_segment : public t_coord {
	int	left_start,right_start,left_end,right_end;
	int	city;
	int	zip;
};

//road additional attributes
struct t_road_data {
	int	one_way;
	int road_class;
};


//rect structure
struct t_rect {
	// In degrees.
	float x0, y0, x1, y1;

	t_rect () : x0 (200), x1 (-200), y0 (200), y1 (-200) {}
	
	float Width  () const {return x1 > x0 ? x1 - x0 : 0;}
	float Height () const {return y1 > y0 ? y1 - y0 : 0;}

	float MeanX () const {return (x0 + x1)/2;}
	float MeanY () const {return (y0 + y1)/2;}

	//check if two rectangles do intersect
	bool Intersects (const t_rect & _rect) const {
		if (x1 < _rect.x0 || x0 > _rect.x1)
			return false;
		if (y1 < _rect.y0 || y0 > _rect.y1)
			return false;
		return true;
	}

	bool Contains (float _x, float _y) const {
		return _x >= x0 && _x <= x1 && _y >= y0 && _y <= y1;
	}

	bool Contains (const t_coord & _point) const {
		return _point.we >= x0 && _point.we <= x1 && _point.ns >= y0 && _point.ns <= y1;
	}

	bool Contains (const t_rect & _rect) const {
		return x0 <= _rect.x0 && x1 >= _rect.x1 && y0 <= _rect.y0 && y1 >= _rect.y1;
	}

	void Extend (const t_coord & _point) {
		if (_point.we < x0) x0 = _point.we;
		if (_point.we > x1) x1 = _point.we;
		if (_point.ns < y0) y0 = _point.ns;
		if (_point.ns > y1) y1 = _point.ns;
	}

	void Extend (const t_rect & _rect) {
		if (_rect.x0 < x0) x0 = _rect.x0;
		if (_rect.x1 > x1) x1 = _rect.x1;
		if (_rect.y0 < y0) y0 = _rect.y0;
		if (_rect.y1 > y1) y1 = _rect.y1;
	}

	void Intersection (t_rect & _rectDest, const t_rect & _rectSrc1, const t_rect & _rectSrc2) {
		_rectDest.x0 = _rectSrc1.x0 > _rectSrc2.x0 ? _rectSrc1.x0 : _rectSrc2.x0;
		_rectDest.x1 = _rectSrc1.x1 < _rectSrc2.x1 ? _rectSrc1.x1 : _rectSrc2.x1;
		_rectDest.y0 = _rectSrc1.y0 > _rectSrc2.y0 ? _rectSrc1.y0 : _rectSrc2.y0;
		_rectDest.y1 = _rectSrc1.y1 < _rectSrc2.y1 ? _rectSrc1.y1 : _rectSrc2.y1;
	}
	
};

#endif