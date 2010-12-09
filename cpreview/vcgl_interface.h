#ifndef vcgl_interfaceH
#define vcgl_interfaceH

#include <vector>
#include <algorithm>
#include "../../geograph.h"

using namespace std;

//: Return intersection of two polygons
void
vcgl_polygon_clip_intersect(vector< TinyPolygon* >const &a, vector< TinyPolygon* > const &b, vector< TinyPolygon* > &result, byte zoom_factor,int &num_fill, int &num_holes);

//: Return difference of two polygons: points in "a" and not in "b".
void vcgl_polygon_clip_subtract(vector< TinyPolygon* > const & a, vector< TinyPolygon* > const& b, vector< TinyPolygon* > &result, byte zoom_factor,int &num_fill, int &num_holes);

//: Return difference of two polygons: points in one and not in the other.
void vcgl_polygon_clip_xor(vector< TinyPolygon* > const& a, vector< TinyPolygon* > const& b, vector< TinyPolygon* > &result, byte zoom_factor,int &num_fill, int &num_holes);

//: Return union of polygons.
void vcgl_polygon_clip_union(vector< TinyPolygon* > const& a, vector< TinyPolygon* > const& b, vector< TinyPolygon* > &result, byte zoom_factor,int &num_fill, int &num_holes);

#endif
