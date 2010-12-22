#include "polygons.h"

void PolyProcess::detectMasterAndHoles(MPPoly* poly) {
/*
	if( poly->

	vector<TinyPolygon *>::iterator		i;
	vector<TinyPolygon *>::iterator		outer_element;

	if( Region.size() == 1 )
		return;
	outer_element = Region.begin();
	
	for( i = Region.begin(); i != Region.end(); i++ ) {
		(*i)->GetExtent();
		if( i != outer_element ) {
			//sprawdzenie punktów
			if( IsHoleOf(outer_element,i) ) {
				//(*outer_element)->hole = false;
				(*i)->hole = true;
			} else {
				(*i)->hole = false;
				outer_element = i;
			}
		}
	}	*/
}
/*
bool ImportSHP::IsHoleOf(const vector<TinyPolygon*>::iterator& outer, const vector<TinyPolygon*>::iterator& inner) {
	vector<TinyCoordinates>::iterator t_cord;

	if( (*outer)->Area < 1.e-10f || (*outer)->Area < (*inner)->Area 
	||	(*inner)->MinNS < (*outer)->MinNS
	||	(*inner)->MinWE < (*outer)->MinWE
	||	(*inner)->MaxNS > (*outer)->MaxNS
	||	(*inner)->MaxWE > (*outer)->MaxWE
	)
		return false;

	for( t_cord = (*inner)->Nodes.begin(); t_cord < (*inner)->Nodes.end(); t_cord++ ) {
		
		const int iContains = PolygonContainsPt ( (*outer)->Nodes, (*t_cord) );
		if (iContains < 0)
			return false;
		else if (iContains > 0)
			return true;
	}
	return false;
}

int ImportSHP::PolygonContainsPt ( const vector<TinyCoordinates>& _polygon, const TinyCoordinates& _p ) {
	size_t cc = 0;

	//x - we
	//y - ne

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
		const TinyCoordinates & p0 = _polygon [cPoint0];

		// Get the second point of leg.
		while (cPoint >= cPoints)
			cPoint -= cPoints;
		const TinyCoordinates & p1 = _polygon [cPoint];

		if (p0 == p1) {
			// Infinite loop protection.
			++ cNodesWithEqualY;
			if (cNodesWithEqualY > cPoints)
				return -1;

			continue;
		}

		if (
			(p0.ns < _p.ns && _p.ns < p1.ns) ||
			(p0.ns > _p.ns && _p.ns > p1.ns)
		) {
			// Leg crosses point's latitude.
			const double x = p0.we + (_p.ns - p0.ns)*(p1.we - p0.we)/(p1.ns - p0.ns);
			if (x == _p.we)
				// Leg crosses the point.
				return 0;
			else if (x < _p.we)
				// Leg passes under the point.
				++ cc;

			bFirstPointPassed = true;
		} else if (p0.ns == _p.ns && p1.ns == _p.ns) {
			// Leg is entirely within point's latitude.
			if (
				(p0.we <= _p.we && p1.we >= _p.we) ||
				(p0.we >= _p.we && p1.we <= _p.we)
			)
				// Leg crosses the point.
				return 0;

			if (cFirstPoint == cPoint - 1 && p1.we < _p.we)
				// There was no any link that crosses point's latitude or finishes at it yet.
				++ cFirstPoint;

			// Infinite loop protection.
			assert (p0.ns == p1.ns);
			++ cNodesWithEqualY;
			if (cNodesWithEqualY > cPoints)
				return -1;
		} else if (p0.ns != _p.ns && p1.ns == _p.ns) {
			// Leg finishes at point's latitude.
			if (p1.we == _p.we)
				// Leg crosses the point.
				return 0;
			else if (p1.we < _p.we)
				// Remember last touching leg side.
				iLastTouchingLinkSide = p0.ns < _p.ns ? -1 : 1;
			bFirstPointPassed = true;
		} else if (p0.ns == _p.ns && p1.ns != _p.ns) {
			// Leg starts at point's latitude.
			if (p0.we == _p.we)
				// Leg crosses the point.
				return 0;
			else if (p0.we < _p.we)
				if (iLastTouchingLinkSide == 0)
					// There was no touching leg yet.
					// We should loop through the polygon 'till this point.
					cFirstPoint = cPoint;
				else if (
					iLastTouchingLinkSide == -1 && p1.ns > _p.ns ||
					iLastTouchingLinkSide == 1  && p1.ns < _p.ns
				)
					// This links with previous touching leg together cross point's latitude.
					++ cc;
		} else
			// Leg does not cross point's latitude.
			bFirstPointPassed = true;
	}
	return (cc & 0x1) ? 1 : -1;
}
*/