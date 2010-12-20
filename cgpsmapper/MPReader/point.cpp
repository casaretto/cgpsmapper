#include "point.h"
#include "m_const.h"
#include <assert.h>

using namespace std;

const double c_A = 6378137;
const double c_a = 1/298.257223563;
const double c_e2 = 2*c_a - c_a*c_a;
const double DE2RA = 0.01745329252;
const double RA2DE = 57.2957795129;

double Distance (const point_t & _1, const point_t & _2) {
	const double fSinB1 = ::sin (_1.y*c_PI/180);
	const double fCosB1 = ::cos (_1.y*c_PI/180);
	const double fSinL1 = ::sin (_1.x*c_PI/180);
	const double fCosL1 = ::cos (_1.x*c_PI/180);

	const double N1 = c_A/::sqrt (1 - c_e2*fSinB1*fSinB1);

	const double X1 = N1*fCosB1*fCosL1;
	const double Y1 = N1*fCosB1*fSinL1;
	const double Z1 = (1 - c_e2)*N1*fSinB1;

	const double fSinB2 = ::sin (_2.y*c_PI/180);
	const double fCosB2 = ::cos (_2.y*c_PI/180);
	const double fSinL2 = ::sin (_2.x*c_PI/180);
	const double fCosL2 = ::cos (_2.x*c_PI/180);

	const double N2 = c_A/::sqrt (1 - c_e2*fSinB2*fSinB2);

	const double X2 = N2*fCosB2*fCosL2;
	const double Y2 = N2*fCosB2*fSinL2;
	const double Z2 = (1 - c_e2)*N2*fSinB2;

	const double D = ::sqrt ((X1 - X2)*(X1 - X2) + (Y1 - Y2)*(Y1 - Y2) + (Z1 - Z2)*(Z1 - Z2));

	const double R = N1;
	const double D2 = 2*R*::asin (.5f*D/R);

	return D2;
}

int	Direction(const point_t & _1, const point_t & _2) {
	//double GCAzimuth(double lat1, double lon1, double lat2, double lon2)
	//lat -y - ns
	//lon -x - we
	double result = 0.0;

	int ilat1 = (int)(0.50 + _1.y  * 360000.0);
	int ilat2 = (int)(0.50 + _2.y  * 360000.0);
	int ilon1 = (int)(0.50 + _1.x  * 360000.0);
	int ilon2 = (int)(0.50 + _2.x  * 360000.0);

	point_t loc_1 = _1 * DE2RA;
	point_t loc_2 = _2 * DE2RA;

	if ((ilat1 == ilat2) && (ilon1 == ilon2))
	{
		return result;
	}
	else if (ilon1 == ilon2)
	{
		if (ilat1 > ilat2)
			result = 180.0;
	}
	else
	{
		double c = acos(sin(loc_2.y)*sin(loc_1.y) +
			cos(loc_2.y)*cos(loc_1.y)*cos((loc_2.x-loc_1.x)));
		double A = asin(cos(loc_2.y)*sin((loc_2.x-loc_1.x))/sin(c));
		result = (A * RA2DE);

		if ((ilat2 > ilat1) && (ilon2 > ilon1))
		{
		}
		else if ((ilat2 < ilat1) && (ilon2 < ilon1))
		{
			result = 180.0 - result;
		}
		else if ((ilat2 < ilat1) && (ilon2 > ilon1))
		{
			result = 180.0 - result;
		}
		else if ((ilat2 > ilat1) && (ilon2 < ilon1))
		{
			result += 360.0;
		}
	}

	return result;
}

bool IsRightOriented (const vector_points & _points) {
	return IsRightOriented (& * _points.begin (), _points.size ());
}

bool IsRightOriented (const point_t * _pPoints, size_t _cPoints) {
	double fArea2 = 0;

	for (size_t cPoint = 0; cPoint < _cPoints; ++ cPoint) {
		const point_t & p0 = _pPoints [cPoint];
		const point_t & p1 = _pPoints [cPoint + 1 == _cPoints ? 0 : cPoint + 1];

		fArea2 += (p1.x - p0.x)*(p0.y + p1.y);
	}

	return fArea2 > 0;
}

double GetArea (const vector_points & _polygon) {
	const size_t cPoints = _polygon.size ();
	if (cPoints <= 2)
		return 0;

	double fArea = 0;
	for (size_t cPoint = 0; cPoint < cPoints; ++ cPoint) {
		const point_t & p0 = _polygon [cPoint];
		const point_t & p1 = _polygon [cPoint + 1 == cPoints ? 0 : cPoint + 1];

		fArea += (p1.x - p0.x)*(p0.y + p1.y);
	}

	return ::fabs (fArea)*.5;
}

void GetCentroid (const vector_points & _polygon, point_t & _ptCentroid) {
	const size_t cPoints = _polygon.size ();
	if (cPoints == 0)
		return;
	else if (cPoints == 1) {
		_ptCentroid = _polygon [0];
		return;
	} else if (cPoints == 2) {
		_ptCentroid.x = (_polygon [0].x + _polygon [1].x)/2;
		_ptCentroid.y = (_polygon [0].y + _polygon [1].y)/2;
		return;
	}

	double xc = 0;
	double yc = 0;
	double fA = 0;

	for (size_t cPoint = 0; cPoint < cPoints; ++ cPoint) {
		const point_t & p0 = _polygon [cPoint];
		const point_t & p1 = _polygon [cPoint + 1 == cPoints ? 0 : cPoint + 1];

		xc += (p0.x + p1.x)*(double (p0.x)*p1.y - double (p1.x)*p0.y);
		yc += (p0.y + p1.y)*(double (p0.x)*p1.y - double (p1.x)*p0.y);

		fA += (p1.x - p0.x)*double (p0.y + p1.y);
	}

	fA *= .5;
	fA = -fA;

	if (::fabs (fA) < 1e-11) {
		rect_t rectBound;
		rectBound.Extend (_polygon);
		_ptCentroid.x = rectBound.MeanX ();
		_ptCentroid.y = rectBound.MeanY ();
		return;
	}

	_ptCentroid.x = xc/(6*fA);
	_ptCentroid.y = yc/(6*fA);
}

double GetLength (double _x1, double _y1, double _x2, double _y2) {
	const double fCosY = ::cos (_y1*c_PI/180.);
	const double fCos2Y = fCosY*fCosY;

	const double dx = _x1 - _x2;
	const double dy = _y1 - _y2;
	return ::sqrt (dx*dx*fCos2Y + dy*dy);
}

double GetLength (const point_t & _p1, const point_t & _p2) {
	return GetLength (_p1.x, _p1.y, _p2.x, _p2.y);
}

double GetLength (const vector_points & _points) {
	return GetLength (_points, 0, _points.size ());
}

double GetLength (const vector_points & _points, size_t _cBegin, size_t _cEnd) {
	const size_t cPoints = _points.size ();
	if (cPoints <= 1)
		return 0;

	if (_cEnd > cPoints)
		_cEnd = cPoints;

	const double fCosY = ::cos (_points [0].y*c_PI/180.);
	const double fCosY2 = fCosY*fCosY;

	double fLength = 0;
	for (size_t cPoint = _cBegin; cPoint + 1 < _cEnd; ++ cPoint) {
		const point_t & p0 = _points [cPoint];
		const point_t & p1 = _points [cPoint + 1];

		fLength += ::sqrt ((p1.x - p0.x)*(p1.x - p0.x)*fCosY2 + (p1.y - p0.y)*(p1.y - p0.y));
	}

	return fLength;
}

////////////////////////////////////////////////

double Distance2 (const point_t & _p0, const point_t & _p1, double _fCos2Y) {
	return (_p0.x - _p1.x)*(_p0.x - _p1.x)*_fCos2Y + (_p0.y - _p1.y)*(_p0.y - _p1.y);
}

double DistanceToSegment2 (
	const point_t & _ptA, const point_t & _ptB, const point_t & _pt, double _fCosY,
	point_t & _ptNearest
) {
	const double dx = (_ptB.x - _ptA.x)*_fCosY;
	const double dy = _ptB.y - _ptA.y;

	const double D2 = dx*dx + dy*dy;
	if (D2 < 1e-12) {
		_ptNearest = _ptA;
		return Distance2 (_pt, _ptA, _fCosY*_fCosY);
	}

	const double dxa = (_pt.x - _ptA.x)*_fCosY;
	const double dya = _pt.y - _ptA.y;

	double t = (dxa*dx + dya*dy)/D2;
	if (t < 0)
		t = 0;
	else if (t > 1)
		t = 1;

	_ptNearest.x = _ptA.x + dx*t/_fCosY;
	_ptNearest.y = _ptA.y + dy*t;

	return Distance2 (_pt, _ptNearest, _fCosY*_fCosY);
}

double GetNearestPointOfPolyline (
	const vector_points & _points, size_t _cBegin, size_t _cEnd,
	const point_t & _pt,
	double _fCosY,
	point_t & _ptNearest, size_t & _cNearestSegment
) {
	if (_points.empty ())
		return -1;

	if (_cEnd >= _points.size ())
		_cEnd = _points.size ();
	if (_cBegin >= _cEnd)
		return -1;

	double fMinD2 = -1;
	for (size_t c = _cBegin; c + 1 < _cEnd; ++ c) {
		const point_t & ptA = _points [c];
		const point_t & ptB = _points [c + 1];

		point_t ptNearest;
		const double fD2 = DistanceToSegment2 (ptA, ptB, _pt, _fCosY, ptNearest);
		if (fMinD2 > fD2 || fMinD2 < 0) {
			fMinD2           = fD2;
			_ptNearest       = ptNearest;
			_cNearestSegment = c;
		}
	}
	return fMinD2;
}

double Distance2ToPolyline (const vector_points & _points, const point_t & _p, double _fCosY) {
	point_t ptNearest;
	size_t cNearestSegment;
	return GetNearestPointOfPolyline (_points, 0, _points.size (), _p, _fCosY, ptNearest, cNearestSegment);
}

double Distance2ToPolygon (const vector_points & _points, const point_t & _p, double _fCosY) {
	if (PolygonContainsPt (_points, _p) >= 0)
		return 0;

	const double fD2_1 = Distance2ToPolyline (_points, _p, _fCosY);
	const size_t cPoints = _points.size ();
	if (cPoints <= 2)
		return fD2_1;

	point_t ptNearest;
	const double fD2_2 = DistanceToSegment2 (_points [0], _points [cPoints - 1], _p, _fCosY, ptNearest);
	return __min (fD2_1, fD2_2);
}

///////////////////////////////////////////////////////////

bool AreThereRepeatingPoints (const vector_points & _points) {
	const size_t cPoints = _points.size ();
	for (size_t c = 0; c + 1 < cPoints; ++ c) {
		if (_points [c] == _points [c + 1])
			return true;
	}

	return false;
}

void RemoveRepeatingPoints (vector_points & _points) {
	for (vector_points::iterator i = _points.begin (); i != _points.end (); ++ i) {
		const vector_points::iterator i_next = i + 1;
		vector_points::iterator i2 = i_next;
		while (i2 != _points.end () && * i == * i2)
			++ i2;
		if (i2 != i_next)
			_points.erase (i_next, i2);
	}
}

void rect_t::Extend (const poly_t & _points) {
	const size_t cPoints = _points._points.size ();
	for (size_t cPoint = 0; cPoint < cPoints; ++ cPoint)
		Extend (_points._points[cPoint]);
}

void rect_t::Extend (const vector_points & _points) {
	const size_t cPoints = _points.size ();
	for (size_t cPoint = 0; cPoint < cPoints; ++ cPoint)
		Extend (_points[cPoint]);
}

////////////////////////////////////////////////

inline
bool IsInRange (double _x, double _x0, double _x1) {
    return (_x0 <= _x && _x <= _x1) || (_x1 <= _x && _x <= _x0);
}

static
bool RectIntersectsLine (const rect_t & _rect, double _x1, double _y1, double _x2, double _y2) {
	if (_x2 != _x1) {
		const double y1 = (_y2 - _y1)/(_x2 - _x1)*(_rect.x0 - _x1) + _y1;
        if (IsInRange (y1, _rect.y0, _rect.y1) && IsInRange (_rect.x0, _x1, _x2))
            return true;

		const double y2 = (_y2 - _y1)/(_x2 - _x1)*(_rect.x1 - _x1) + _y1;
        if (IsInRange (y2, _rect.y0, _rect.y1) && IsInRange (_rect.x1, _x1, _x2))
            return true;
	}
	if (_y2 != _y1) {
		const double x1 = (_x2 - _x1)/(_y2 - _y1)*(_rect.y0 - _y1) + _x1;
        if (IsInRange (x1, _rect.x0, _rect.x1) && IsInRange (_rect.y0, _y1, _y2))
            return true;

		const double x2 = (_x2 - _x1)/(_y2 - _y1)*(_rect.y1 - _y1) + _x1;
        if (IsInRange (x2, _rect.x0, _rect.x1) && IsInRange (_rect.y1, _y1, _y2))
            return true;
	}
    return _rect.Contains (_x1, _y1);
}

bool RectIntersectsPolyline (const rect_t & _rect, const vector_points & _pts, bool _bClosed) {
	if (_pts.empty ())
		return false;

	const size_t cPoints = _pts.size ();
	const size_t cPoints2 = _bClosed ? cPoints : cPoints - 1;
	for (size_t c = 0; c < cPoints2; ++ c) {
		assert (c < cPoints);
		const point_t & a = _pts [c];
		const point_t & b = _pts [c + 1 < cPoints ? c + 1 : 0];

        if (_rect.Contains (a))
            return true;
        if (c == cPoints2 - 1 && _rect.Contains (b))
            return true;
        if (RectIntersectsLine (_rect, a.x, a.y, b.x, b.y))
            return true;
    }
    return false;
}

////////////////////////////////////////////////

bool PointContainsPt (const point_t & _point, const point_t & _pt, double _e, double _fCosY) {
	assert (0 < _fCosY && _fCosY <= 1);
	return ::fabs (_point.x - _pt.x)*_fCosY <= _e && ::fabs (_point.y - _pt.y) <= _e;
}

size_t PointsContainPt (const vector_points & _pts, const point_t & _pt, double _e, double _fCosY) {
	if (_pts.empty ())
		return (size_t) -1;
	const size_t cPoints = _pts.size ();
	for (size_t c = 0; c < cPoints; ++ c)
        if (::fabs (_pts [c].x - _pt.x)*_fCosY <= _e && ::fabs (_pts [c].y - _pt.y) <= _e)
            return c;
    return (size_t) -1;
}

//////////////////////////////////

bool SegmentContainsPt (const point_t & _ptA, const point_t & _ptB, const point_t & _pt, double _e, double _fCosY) {
	point_t ptNearest;
	const double fD2 = DistanceToSegment2 (_ptA, _ptB, _pt, _fCosY, ptNearest);
	return fD2 < _e*_e;
}

bool PolylineContainsPt (const vector_points & _pts, const point_t & _pt, double _e, double _fCosY, bool _bLastLeg, point_t * _pPoint, size_t * _pcPointIndex) {
	if (_pts.empty ())
		return false;

	assert (0 < _fCosY && _fCosY <= 1);

	const size_t cPoints = _pts.size ();
	const size_t cPoints2 = _bLastLeg ? cPoints : cPoints - 1;
	for (size_t c = 0; c < cPoints2; ++ c) {
		assert (c < cPoints);

		// TODO: ::sqrt() is slow. Use SegmentContainsPt() or DistanceToSegment2() instead of this bulky code!

		const point_t & a = _pts [c];
		const point_t & b = _pts [c + 1 < cPoints ? c + 1 : 0];

		const double dx = (b.x - a.x)*_fCosY;
		const double dy = b.y - a.y;
		const double r = ::sqrt (dx*dx + dy*dy);
		const double fSinA = dy/r;
		const double fCosA = dx/r;
		const double t = (_pt.x - a.x)*_fCosY*fCosA + (_pt.y - a.y)       *fSinA;
		const double d = (_pt.y - a.y)       *fCosA - (_pt.x - a.x)*_fCosY*fSinA;
		if (-_e < d && d < _e && -_e < t && t < r + _e) {
			if (_pPoint) {
				if (t < _e) {
					_pPoint->x = a.x;
					_pPoint->y = a.y;
				} else if (t > r - _e) {
					_pPoint->x = b.x;
					_pPoint->y = b.y;
				} else {
					_pPoint->x = a.x + t*fCosA/_fCosY;
					_pPoint->y = a.y + t*fSinA;
				}
			}
            if (_pcPointIndex) 
                * _pcPointIndex = c;

			return true;  
		}
	}

	return false;
}

bool FrameContainsPt (const rect_t & _rect, const point_t & _pt, double _e, double _fCosY, size_t * _pcPointIndex) {
	vector_points points (4);
	points [0] = point_t (_rect.x0, _rect.y0);
	points [1] = point_t (_rect.x0, _rect.y1);
	points [2] = point_t (_rect.x1, _rect.y1);
	points [3] = point_t (_rect.x1, _rect.y0);
	return PolylineContainsPt (points, _pt, _e, _fCosY, true, NULL, _pcPointIndex);
}

///////////////////////////////////////////////////////////////
//
// The algorithm was obtained from pGPSMap.js written by Olexa Ryznik (http://www.olexa.ua)
//
// Returns 1 if point is entirely within polygon,
// 0 if it is on its border and
// -1 if it is outside polygon.
//
int PolygonContainsPt (const vector_points & _polygon, const point_t & _p) {
	size_t cc = 0;

	int iLastTouchingLinkSide = 0;
	size_t cFirstPoint = 0;
	bool bFirstPointPassed = false;

	size_t cNodesWithEqualY = 0;

	const size_t cPoints = _polygon.size ();
	for (size_t cPoint = 1; cPoint != cFirstPoint + 1 || ! bFirstPointPassed; ++ cPoint) {
		// Get the first point of leg.
		size_t cPoint0 = cPoint - 1;
		while (cPoint0 >= cPoints)
			cPoint0 -= cPoints;
		const point_t & p0 = _polygon [cPoint0];

		// Get the second point of leg.
		while (cPoint >= cPoints)
			cPoint -= cPoints;
		const point_t & p1 = _polygon [cPoint];

		if (p0 == p1) {
			// Infinite loop protection.
			++ cNodesWithEqualY;
			if (cNodesWithEqualY > cPoints)
				return -1;

			continue;
		}

		if (
			(p0.y < _p.y && _p.y < p1.y) ||
			(p0.y > _p.y && _p.y > p1.y)
		) {
			// Leg crosses point's latitude.
			const double x = p0.x + (_p.y - p0.y)*(p1.x - p0.x)/(p1.y - p0.y);
			if (x == _p.x)
				// Leg crosses the point.
				return 0;
			else if (x < _p.x)
				// Leg passes under the point.
				++ cc;

			bFirstPointPassed = true;
		} else if (p0.y == _p.y && p1.y == _p.y) {
			// Leg is entirely within point's latitude.
			if (
				(p0.x <= _p.x && p1.x >= _p.x) ||
				(p0.x >= _p.x && p1.x <= _p.x)
			)
				// Leg crosses the point.
				return 0;

			if (cFirstPoint == cPoint - 1 && p1.x < _p.x)
				// There was no any link that crosses point's latitude or finishes at it yet.
				++ cFirstPoint;

			// Infinite loop protection.
			assert (p0.y == p1.y);
			++ cNodesWithEqualY;
			if (cNodesWithEqualY > cPoints)
				return -1;
		} else if (p0.y != _p.y && p1.y == _p.y) {
			// Leg finishes at point's latitude.
			if (p1.x == _p.x)
				// Leg crosses the point.
				return 0;
			else if (p1.x < _p.x)
				// Remember last touching leg side.
				iLastTouchingLinkSide = p0.y < _p.y ? -1 : 1;
			bFirstPointPassed = true;
		} else if (p0.y == _p.y && p1.y != _p.y) {
			// Leg starts at point's latitude.
			if (p0.x == _p.x)
				// Leg crosses the point.
				return 0;
			else if (p0.x < _p.x)
				if (iLastTouchingLinkSide == 0)
					// There was no touching leg yet.
					// We should loop through the polygon 'till this point.
					cFirstPoint = cPoint;
				else if (
					iLastTouchingLinkSide == -1 && p1.y > _p.y ||
					iLastTouchingLinkSide == 1  && p1.y < _p.y
				)
					// This links with previous touching leg together cross point's latitude.
					++ cc;
		} else
			// Leg does not cross point's latitude.
			bFirstPointPassed = true;
	}
	return (cc & 0x1) ? 1 : -1;
}

/////////////////////////////////////////////

size_t ShiftPolyline (
	const point_t * _pPointsIn,  size_t _cPointsIn,
	point_t       * _pPointsOut, size_t _cPointsOut,
	double _fCosY, double _fShift
) {
	if (_cPointsIn == 0)
		return 0;
	if (_cPointsIn == 1) {
		_pPointsOut [0] = _pPointsIn [0];
		return 1;
	}

	// Lengths of segments.
	double   * const l = reinterpret_cast<double   *> (alloca (sizeof (double)*_cPointsIn));
	// Normals to segments.
	point_t * const n = reinterpret_cast<point_t *> (alloca (sizeof (point_t)*_cPointsIn));
	for (size_t c = 0; c < _cPointsIn - 1; ++ c) {
		const double dx = (_pPointsIn [c + 1].x - _pPointsIn [c].x)*_fCosY;
		const double dy = (_pPointsIn [c + 1].y - _pPointsIn [c].y);
		const double len = ::_hypot (dx, dy);

		l [c]   = len;
		n [c].x = len ?  dy/len : 0;
		n [c].y = len ? -dx/len : 0;
	}
	l [_cPointsIn - 1] = 0;
	n [_cPointsIn - 1] = n [_cPointsIn - 2];

	// Find first non-zero length segment.
	size_t cIn = 0;
	while (cIn < _cPointsIn && l [cIn] == 0)
		++ cIn;

	// Shift the first point.
	size_t cOut = 0;
	if (cOut < _cPointsOut) {
		_pPointsOut [cOut].x = _pPointsIn [cIn].x + n [cIn].x*_fShift/_fCosY;
		_pPointsOut [cOut].y = _pPointsIn [cIn].y + n [cIn].y*_fShift;
		++ cOut;
	}

	// Shift all other points.
	while (cIn < _cPointsIn) {
		// Find next non-zero length segment.
		while (cIn < _cPointsIn && l [cIn] == 0)
			++ cIn;

		const double nx = cIn + 1 < _cPointsIn ? n [cIn].x : 0;
		const double ny = cIn + 1 < _cPointsIn ? n [cIn].y : 0;

		// Find next non-zero length segment.
		size_t cNext = cIn + 1;
		while (cNext < _cPointsIn && l [cNext] == 0)
			++ cNext;

		const double mx = cNext + 1 < _cPointsIn ? n [cNext].x : nx;
		const double my = cNext + 1 < _cPointsIn ? n [cNext].y : ny;

		const double nmx = nx + mx;
		const double nmy = ny + my;
		const double nml2 = nmx*nmx + nmy*nmy;
		const double nmd = nx*my - ny*mx;

		if (cOut < _cPointsOut && nml2 != 0) {
			_pPointsOut [cOut] = _pPointsIn [cIn + 1];
			_pPointsOut [cOut].x += 2*_fShift*nmx/nml2/_fCosY;
			_pPointsOut [cOut].y += 2*_fShift*nmy/nml2;
			++ cOut;
		}

		cIn = cNext;
	}

	return cOut;
}

/////////////////////////////////////////////

bool SegmentIntersectsSegment (
	const point_t & _a, const point_t & _b,
	const point_t & _c, const point_t & _d,
	point_t * _pIntersection
) {
	const double c_fE = 1e-10f;

	const double t = double (_d.x - _c.x)*(_b.y - _a.y) - double (_d.y - _c.y)*(_b.x - _a.x);
	if (::fabs (t) < c_fE)
		return false;

	const double p = (double (_c.y - _a.y)*(_b.x - _a.x) - double (_c.x - _a.x)*(_b.y - _a.y));
	if (p/t <= c_fE || p/t >= 1 - c_fE)
		return false;

	const double q = (double (_c.y - _a.y)*(_d.x - _c.x) - double (_c.x - _a.x)*(_d.y - _c.y));
	if (q/t <= c_fE || q/t >= 1 - c_fE)
		return false;

	if (_pIntersection)
		* _pIntersection = _a + (_b - _a)*(q/t);

	return true;
}

bool SegmentIntersectsPolyline (const point_t & _a, const point_t & _b, const vector_points & _points, bool _bClosed) {
	const size_t cPoints = _points.size ();
	const size_t cPoints_e = _bClosed ? cPoints : cPoints - 1;

	for (size_t n = 0; n < cPoints_e; ++ n) {
		const point_t & c = _points [n];
		const point_t & d = _points [n + 1 < cPoints ? n + 1 : 0];

		if (SegmentIntersectsSegment (_a, _b, c, d))
			return true;
	}

	return false;
}

static
double _SegmentLengthSq (const point_t & _pt1, const point_t & _pt2) { 
	return (_pt1.x - _pt2.x)*(_pt1.x - _pt2.x) + (_pt1.y - _pt2.y)*(_pt1.y - _pt2.y);
} 

static
double _SegmentLengthsSum (const point_t & _pt1, const point_t & _pt2) {
	return ::fabs (_pt1.x - _pt2.x) + ::fabs (_pt1.y - _pt2.y);
}

// polylines 1 and 2 are supposed to have common point (contact). a bypass line is drawn trough
// the points of pre-contact and post-contact segments of polyline 2, opposing the contact point.
// the pre-contact and post-contact segments of polyline 1 are tested to be intersected by 
// this bypass
static
bool _PolylineBypassIntersectsOnePolylineRayInCommonVertex (
	point_t pt1contact, point_t pt1pre, point_t pt1post, point_t pt2contact, point_t pt2pre, point_t pt2post, 
	point_t & _ptInters 
) { 
	bool bRet = false;	
	const double fLengthSq1Pre  = _SegmentLengthSq (pt1contact, pt1pre),
		        fLengthSq1Post = _SegmentLengthSq (pt1contact, pt1post);
	const double fLengthSq2Pre  = _SegmentLengthSq (pt2pre,  pt2contact),
		        fLengthSq2Post = _SegmentLengthSq (pt2post, pt2contact);
	const double fSegmentLengthSq2Max = __max (fLengthSq2Pre, fLengthSq2Post);

	double fExpansionFactorPre = 0, fExpansionFactorPost = 0; 
	point_t pt1preRayEnd = pt1pre, pt1postRayEnd = pt1post;

	// form sufficiently long "rays" of pre- and post-contact segments of polyline 1
	if (fSegmentLengthSq2Max > fLengthSq1Pre) { 
		fExpansionFactorPre = (fSegmentLengthSq2Max == fLengthSq2Pre) ? 
			fExpansionFactorPre = 2 * fLengthSq2Pre / fLengthSq1Pre : 2 * fLengthSq2Post / fLengthSq1Pre;
	} 
	if (fSegmentLengthSq2Max > fLengthSq1Post) { 
		fExpansionFactorPost = (fSegmentLengthSq2Max == fLengthSq2Pre) ? 
			fExpansionFactorPost = 2*fLengthSq2Pre/fLengthSq1Post : 2*fLengthSq2Post/fLengthSq1Post;
	} 

	if (fExpansionFactorPre > 0) {
		 pt1preRayEnd.x = pt1pre.x + (pt1pre.x - pt1contact.x)*fExpansionFactorPre;
		 pt1preRayEnd.y = pt1pre.y + (pt1pre.y - pt1contact.y)*fExpansionFactorPre;
	}
	if (fExpansionFactorPost > 0) {
		 pt1postRayEnd.x = pt1post.x + (pt1post.x - pt1contact.x)*fExpansionFactorPost;  
		 pt1postRayEnd.y = pt1post.y + (pt1post.y - pt1contact.y)*fExpansionFactorPost;  
	}

	point_t ptInters;

	// check if the "rays" are intersected by the bypass
	bool bPre1RayIntersected = 
		SegmentIntersectsSegment (pt2pre, pt2post, pt1preRayEnd, pt1contact, & ptInters) && 
		! (ptInters == pt2pre || ptInters == pt2post);
	bool bPost1RayIntersected = 
		SegmentIntersectsSegment (pt2pre, pt2post, pt1contact, pt1postRayEnd, & ptInters) &&
		! (ptInters == pt2pre || ptInters == pt2post);

	// we are interested only in 1 intersection
	bRet = (bPre1RayIntersected && !bPost1RayIntersected) || (!bPre1RayIntersected && bPost1RayIntersected);

	_ptInters = ptInters;

	return bRet;
} 

bool PolylinesWithContactIntersect (const vector_points & _points1, const vector_points & _points2, int _nIdx1, int _nIdx2, bool _bClosed) {
	//	 _points2[_nIdx2] lies between	_points1[_nIdx1] and  _points1[next_idx(_nIdx1)]
	const int nPoints1 = _points1.size ();
	const int nPoints2 = _points2.size ();
	const point_t & a = _points1 [_nIdx1];
	int bIdx = _nIdx1 + 1 < nPoints1 ? _nIdx1 + 1 : 0;
	const point_t & b = _points1 [bIdx];
	const point_t & pt2contact = _points2 [_nIdx2];

	bool bRet = false; 
	//double fShift = 0.01;

	int nContactPtIdx1 = -1;
	
	if (pt2contact == a)
		nContactPtIdx1 = _nIdx1;
	else if (pt2contact == b)
		nContactPtIdx1 = bIdx;
	
	const point_t & pt2pre  = _points2 [_nIdx2 - 1 < 0 ? nPoints2 - 1 : _nIdx2 - 1];
	const point_t & pt2post = _points2 [_nIdx2 + 1 < nPoints2 ? _nIdx2 + 1 : 0];
	const double fLengthSq2Pre  = _SegmentLengthSq (pt2pre,  pt2contact);
	const double fLengthSq2Post = _SegmentLengthSq (pt2post, pt2contact);
	const double fLengthSqAB    = _SegmentLengthSq (a, b);
	const double fSegmentLengthSq2Max = __max (fLengthSq2Pre, fLengthSq2Post);
	point_t ptInters1 (-1, -1),
	        ptInters2 (-1, -1);

	if (nContactPtIdx1 != -1) { 
		const point_t & pt1contact = _points1 [nContactPtIdx1];
		const point_t & pt1pre  = _points1 [nContactPtIdx1 - 1 < 0 ? nPoints1 - 1 : nContactPtIdx1 - 1];
		const point_t & pt1post = _points1 [nContactPtIdx1 + 1 < nPoints1 ? nContactPtIdx1 + 1 : 0];
		
		const bool bFirstIntersection = _PolylineBypassIntersectsOnePolylineRayInCommonVertex (
			pt1contact, pt1pre, pt1post, pt2contact, pt2pre, pt2post, ptInters1
		);
		const bool bSecondIntersection = bFirstIntersection && _PolylineBypassIntersectsOnePolylineRayInCommonVertex (
			pt2contact, pt2pre, pt2post, pt1contact, pt1pre, pt1post, ptInters2
		);

		bRet = 
			(bFirstIntersection && bSecondIntersection) || 
			(_bClosed && bFirstIntersection  && PolygonContainsPt (_points2, ptInters1) == 1) ||
			(_bClosed && bSecondIntersection && PolygonContainsPt (_points1, ptInters2) == 1);
	} else
		bRet = _PolylineBypassIntersectsOnePolylineRayInCommonVertex (pt2contact, a, b, pt2contact, pt2pre, pt2post, ptInters1);

	return bRet;
}

bool PolygonIntersectsPolygon (const vector_points & _points1, const vector_points & _points2, pip_info_t * _pInfo) {
	const size_t cPoints1 = _points1.size ();
	const size_t cPoints1e = cPoints1;

	const size_t cPoints2 = _points2.size ();
	const size_t cPoints2e = cPoints2;
	const double c_fE2 = 1e-22f;

	const double fCosY = _points1.empty () ? 1 : ::cos (_points1 [0].y*c_PI/180);

	for (size_t c1 = 0; c1 < cPoints1e; ++ c1) {
		const point_t & a = _points1 [c1];
		const point_t & b = _points1 [c1 + 1 < cPoints1 ? c1 + 1 : 0];

		for (size_t c2 = 0; c2 < cPoints2e; ++ c2) {
			const point_t & c = _points2 [c2];
			const point_t & d = _points2 [c2 + 1 < cPoints2 ? c2 + 1 : 0];

			point_t ptNearest;
			if (
				SegmentIntersectsSegment (a, b, c, d) || 
				(
					DistanceToSegment2 (a, b, c, fCosY, ptNearest) < c_fE2 &&
					PolylinesWithContactIntersect (_points1, _points2, c1, c2, true)
				) || (
					DistanceToSegment2 (c, d, a, fCosY, ptNearest) < c_fE2 &&
					PolylinesWithContactIntersect (_points2, _points1, c2, c1, true)
				)
			) {
				if (_pInfo) {
					_pInfo->cSegment1 = c1;
					_pInfo->cSegment2 = c2;
				}
				return true;
			}
		}
	}
		
	return false;
}

bool PolylineIntersectsPolyline (const vector_points & _points1, const vector_points & _points2, bool _bClosed, pip_info_t * _pInfo) {
	const size_t cPoints1 = _points1.size ();
	const size_t cPoints1e = _bClosed ? cPoints1 : cPoints1 - 1;

	const size_t cPoints2 = _points2.size ();
	const size_t cPoints2e = _bClosed ? cPoints2 : cPoints2 - 1;

	for (size_t c1 = 0; c1 < cPoints1e; ++ c1) {
		const point_t & a = _points1 [c1];
		const point_t & b = _points1 [c1 + 1 < cPoints1 ? c1 + 1 : 0];

		for (size_t c2 = 0; c2 < cPoints2e; ++ c2) {
			const point_t & c = _points2 [c2];
			const point_t & d = _points2 [c2 + 1 < cPoints2 ? c2 + 1 : 0];

			if (SegmentIntersectsSegment (a, b, c, d)) {
				if (_pInfo) {
					_pInfo->cSegment1 = c1;
					_pInfo->cSegment2 = c2;
				}
				return true;
			}
		}
	}

	return false;
}

bool PolylineIntersectsSelf (const vector_points & _points, bool _bClosed, pip_info_t * _pInfo) {
	return PolylineIntersectsPolyline (_points, _points, _bClosed, _pInfo);
}

bool PolylineIntersectsPolylineInNode (const vector_points & _points1, const vector_points & _points2, pip_info_t * _pInfo) {
	const size_t cPoints1 = _points1.size ();
	const size_t cPoints2 = _points2.size ();
	for (size_t c1 = 0; c1 < cPoints1; ++ c1) {
		const point_t & pt1 = _points1 [c1];

		for (size_t c2 = 0; c2 < cPoints2; ++ c2) {
			const point_t & pt2 = _points2 [c2];

			if (pt1 == pt2) {
				if (_pInfo) {
					_pInfo->cSegment1 = c1;
					_pInfo->cSegment2 = c2;
				}
				return true;
			}
		}
	}
	return false;
}

bool PolylineIntersectsSelfInNode (const vector_points & _points, pip_info_t * _pInfo) {
	const size_t cPoints = _points.size ();
	for (size_t c1 = 0; c1 < cPoints; ++ c1) {
		const point_t & pt1 = _points [c1];

		for (size_t c2 = c1 + 1; c2 < cPoints; ++ c2) {
			const point_t & pt2 = _points [c2];

			if (pt1 == pt2) {
				if (_pInfo) {
					_pInfo->cSegment1 = c1;
					_pInfo->cSegment2 = c2;
				}
				return true;
			}
		}
	}

	return false;
}

