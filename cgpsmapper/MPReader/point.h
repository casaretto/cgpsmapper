#ifndef __POINT_H
#define __POINT_H

#include <vector>

class point_t {
public:
	// Longitude and latitude, in degrees.
	double x, y;

	point_t () {}
	point_t (double _x, double _y) : x (_x), y (_y) {}

	double ns() {return y;};
	double we() {return x;};
	void ns(double _y) {y = _y;};
	void we(double _x) {x = _x;};

	bool operator == (const point_t & _other) const {return x == _other.x && y == _other.y;}
};

class point_int {
public:
	int	x,y;
	point_int () {}
	point_int (int _x, int _y) : x (_x), y (_y) {}

	int ns() {return y;};
	int we() {return x;};
	void ns(int _y) {y = _y;};
	void we(int _x) {x = _x;};

	bool operator == (const point_int & _other) const {return x == _other.x && y == _other.y;}
};

inline point_t operator+ ( const point_t & _a,const point_t & _b) {
	return point_t (_a.x + _b.x, _a.y + _b.y);
}

inline point_t operator* (const point_t & _a,double _k) {
	return point_t (_a.x*_k, _a.y*_k);
}

inline point_t operator- (const point_t & _a,const point_t & _b) {
	return point_t (_a.x - _b.x, _a.y - _b.y);
}


typedef std::vector<point_t> vector_points;

class poly_t {
public:
	vector_points	_points;
	bool			hole;

	poly_t() : hole(false) {;};
};

bool IsRightOriented (const point_t * _pPoints, size_t _cPoints);
bool IsRightOriented (const vector_points & _points);
double GetArea    (const vector_points & _polygon);
void GetCentroid (const vector_points & _polygon, point_t & _ptCentroid);
double GetLength (double _x1, double _y1, double _x2, double _y2);
double GetLength (const point_t & _p1, const point_t & _p2);
double GetLength (const vector_points & _points);
double GetLength (const vector_points & _points, size_t _cBegin, size_t _cEnd);
double Distance (const point_t & _1, const point_t & _2);
int	Direction(const point_t & _1, const point_t & _2);

bool PointContainsPt    (const point_t & _point, const point_t & _pt, double _e, double _fCosY);

// Return index of point or -1.
size_t PointsContainPt  (const vector_points & _pts,  const point_t & _pt, double _e, double _fCosY);

bool PolylineContainsPt (const vector_points & _pts,  const point_t & _pt, double _e, double _fCosY, bool _bLastChain = false, point_t * _pPoint = NULL, size_t * _pcPointIndex = NULL);
bool SegmentContainsPt (const point_t & _a, const point_t & _b, const point_t & _pt, double _e, double _fCosY);

// Returns 1 if point is entirely within polygon, 0 if it is on its border and -1 if it is outside polygon.
int PolygonContainsPt   (const vector_points & _polygon, const point_t & _pt);

// Return number of points filled out in _pPointsOut.
size_t ShiftPolyline (
	const point_t * _pPointsIn,  size_t _cPointsIn,
	point_t       * _pPointsOut, size_t _cPointsOut,
	double _fCosY, double _fShift
);

bool AreThereRepeatingPoints (const vector_points & _points);
void RemoveRepeatingPoints (vector_points & _points);

double Distance2 (const point_t & _p0, const point_t & _p1, double _fCos2Y);
double DistanceToSegment2 (
	const point_t & _ptA, const point_t & _ptB, const point_t & _pt, double _fCosY,
	point_t & _ptNearest
);
double GetNearestPointOfPolyline (
	const vector_points & _points, size_t _cBegin, size_t _cEnd,
	const point_t & _pt,
	double _fCosY,
	point_t & _ptNearest, size_t & _cNearestSegment
);

double Distance2ToPolyline (const vector_points & _points, const point_t & _p, double _fCosY);
double Distance2ToPolygon  (const vector_points & _points, const point_t & _p, double _fCosY);


//
// Rectangle.
//
class rect_t 
{
public:
	// In degrees.
	double x0, y0, x1, y1;

	rect_t () : x0 (200), x1 (-200), y0 (200), y1 (-200) {}
	rect_t (double _x0, double _y0, double _x1, double _y1) : x0 (_x0), x1 (_x1), y0 (_y0), y1 (_y1) {}

	bool operator == (const rect_t & _other) const {
		return x0 == _other.x0 && x1 == _other.x1 && y0 == _other.y0 && y1 == _other.y1;
	}

	void swap (rect_t & _other) {
		std::swap (x0, _other.x0);
		std::swap (y0, _other.y0);
		std::swap (x1, _other.x1);
		std::swap (y1, _other.y1);
	}

	double Width  () const {return x1 > x0 ? x1 - x0 : 0;}
	double Height () const {return y1 > y0 ? y1 - y0 : 0;}

	double MeanX () const {return (x0 + x1)/2;}
	double MeanY () const {return (y0 + y1)/2;}

	bool Empty () const {return x1 <= x0 || y1 <= y0;}
	bool Invalid () const {return x1 < x0 || y1 < y0;}

	bool Intersects (const rect_t & _rect) const {
		if (x1 < _rect.x0 || x0 > _rect.x1)
			return false;
		if (y1 < _rect.y0 || y0 > _rect.y1)
			return false;
		return true;
	}

	bool Contains (double _x, double _y) const {
		return _x >= x0 && _x <= x1 && _y >= y0 && _y <= y1;
	}

	bool Contains (const point_t & _point) const {
		return _point.x >= x0 && _point.x <= x1 && _point.y >= y0 && _point.y <= y1;
	}

	bool Contains (const rect_t & _rect) const {
		return x0 <= _rect.x0 && x1 >= _rect.x1 && y0 <= _rect.y0 && y1 >= _rect.y1;
	}

	void Shift (double _x, double _y) {
		x0 += _x;
		x1 += _x;
		y0 += _y;
		y1 += _y;
	}

	void Extend (const point_t & _point) {
		if (_point.x < x0) x0 = _point.x;
		if (_point.x > x1) x1 = _point.x;
		if (_point.y < y0) y0 = _point.y;
		if (_point.y > y1) y1 = _point.y;
	}

	void Extend (const rect_t & _rect) {
		if (_rect.x0 < x0) x0 = _rect.x0;
		if (_rect.x1 > x1) x1 = _rect.x1;
		if (_rect.y0 < y0) y0 = _rect.y0;
		if (_rect.y1 > y1) y1 = _rect.y1;
	}

	void Extend (const poly_t & _points);
	void Extend (const vector_points & _points);

	static void Intersection (rect_t & _rectDest, const rect_t & _rectSrc1, const rect_t & _rectSrc2) {
		_rectDest.x0 = _rectSrc1.x0 > _rectSrc2.x0 ? _rectSrc1.x0 : _rectSrc2.x0;
		_rectDest.x1 = _rectSrc1.x1 < _rectSrc2.x1 ? _rectSrc1.x1 : _rectSrc2.x1;
		_rectDest.y0 = _rectSrc1.y0 > _rectSrc2.y0 ? _rectSrc1.y0 : _rectSrc2.y0;
		_rectDest.y1 = _rectSrc1.y1 < _rectSrc2.y1 ? _rectSrc1.y1 : _rectSrc2.y1;
	}
};

bool FrameContainsPt (const rect_t & _rect, const point_t & _pt, double _e, double _fCosY, size_t * _pcPointIndex = NULL);

bool RectIntersectsPolyline (const rect_t & _rect, const vector_points & _points, bool _bClosed);

bool SegmentIntersectsPolyline (const point_t & _a, const point_t & _b, const vector_points & _points, bool _bClosed);
bool SegmentIntersectsSegment  (const point_t & _a, const point_t & _b, const point_t & _c, const point_t & _d, point_t * _pIntersection = NULL);

struct pip_info_t {
	size_t cSegment1;
	size_t cSegment2;
};
bool PolylineIntersectsPolyline       (const vector_points & _points1, const vector_points & _points2, bool _bClosed, pip_info_t * _pInfo = NULL);
bool PolylineIntersectsSelf           (const vector_points & _points,                             bool _bClosed, pip_info_t * _pInfo = NULL);
bool PolylineIntersectsPolylineInNode (const vector_points & _points1, const vector_points & _points2,                pip_info_t * _pInfo = NULL);
bool PolylineIntersectsSelfInNode     (const vector_points & _points,                                            pip_info_t * _pInfo = NULL);
bool PolylinesWithContactIntersect    (const vector_points & _points1, const vector_points & _points2, int _nIdx1,    int _nIdx2, bool _bClosed);
bool PolygonIntersectsPolygon         (const vector_points & _points1, const vector_points & _points2,                pip_info_t * _pInfo = NULL);

#endif