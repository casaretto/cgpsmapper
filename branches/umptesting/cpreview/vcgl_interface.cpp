#include "vcgl_interface.h"
#include "gpc.h"
#include "../simplify.h"
#include "../../geograph.h"

/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
gpc_polygon from(const vector< TinyPolygon* > &a)
{
  gpc_polygon p;

  p.num_contours = static_cast<int>(a.size());

  p.hole = (int*)malloc(p.num_contours * sizeof(int));//,"hole flag array creation");
  p.contour = (gpc_vertex_list*)malloc(p.num_contours * sizeof(gpc_vertex_list));//, "contour creation");

  for(int c = 0; c < p.num_contours; ++c) {
    int n = static_cast<int>(a[c]->Nodes.size());
    p.contour[c].num_vertices = n;
    p.hole[c] = a[c]->hole?1:0;

    p.contour[c].vertex = (gpc_vertex*)malloc(n  * sizeof(gpc_vertex));//, "vertex creation");

    for(int v = 0; v < n; ++v) {
      p.contour[c].vertex[v].x = a[c]->Nodes[v].we;
      p.contour[c].vertex[v].y = a[c]->Nodes[v].ns;
    }
  }
  return p;
}


class sort_comp
{
public: // note, the operator must be public
  bool operator()(const TinyPolygon *x, const TinyPolygon *y) const
  { return x->Area > y->Area; }
};

bool check_if_same(TinyCoordinates coord1,TinyCoordinates coord2,byte zoom_factor)
{
  TinyCoordinates refCoords = TinyCoordinates(10.0,10.0);

  coord1 = Simplify::AlignToGrid(coord1,refCoords,zoom_factor);
  coord2 = Simplify::AlignToGrid(coord2,refCoords,zoom_factor);

  return coord1 == coord2;
}

void from(gpc_polygon const& a, vector< TinyPolygon* > &p, byte zoom_factor,int &num_fill, int &num_holes)
{
  num_fill = 0;
  num_holes = 0;

  for(int c = 0; c < a.num_contours; ++c) {
    //if( a.hole[c] == 0 ) {
      TinyPolygon* p1 = new TinyPolygon();
      if( a.hole[c] == 1 ) {
  	num_holes++;
        p1->hole = true;
      }
      else {
        num_fill++;
        p1->hole = false;
      }

      int n = a.contour[c].num_vertices;
      p1->Nodes.clear();
      for(int v = 0; v < n; ++v) {
//        if( v > 0 ) {
//          if( check_if_same( TinyCoordinates(a.contour[c].vertex[v].y, a.contour[c].vertex[v].x),  TinyCoordinates(a.contour[c].vertex[v-1].y, a.contour[c].vertex[v-1].x), zoom_factor ) == false ) {
//	    p1->Nodes.push_back( TinyCoordinates(a.contour[c].vertex[v].y, a.contour[c].vertex[v].x) );
//          }
//        } else
        p1->Nodes.push_back( TinyCoordinates(a.contour[c].vertex[v].y, a.contour[c].vertex[v].x) );
      }

      if( p1->Nodes[0] == p1->Nodes[p1->Nodes.size()-1] )
      	p1->Nodes.pop_back();

      if( p1->Nodes.size() < 3 ) {
      	delete p1;
      } else {
	p1->GetExtent();
      	p.push_back(p1);
      }
    //}
  }
/*
  for(int c = 0; c < a.num_contours; ++c) {
    if( a.hole[c] ) {
      any_holes = true;
      TinyPolygon* p1 = new TinyPolygon();
      p1->hole = true;

      int n = a.contour[c].num_vertices;
      p1->Nodes.clear();
      for(int v = 0; v < n; ++v)
        p1->Nodes.push_back( TinyCoordinates(a.contour[c].vertex[v].y, a.contour[c].vertex[v].x) );
      p1->GetExtent();
      p.push_back(p1);
    }
  }
*/
  sort( p.begin(), p.end(), sort_comp() );
}

//: Return intersection of two polygons
static
void go(gpc_op op, vector< TinyPolygon* > const& a, vector< TinyPolygon* > const& b, vector< TinyPolygon* > &result,byte zoom_factor,int &num_fill, int &num_holes)
{
  gpc_polygon ga = from(a);
  gpc_polygon gb = from(b);

  gpc_polygon gret;

  //vcgl_gpc gpc;
  gpc_polygon_clip(op, &ga, &gb, &gret);

  from(gret,result,zoom_factor,num_fill,num_holes);

  gpc_free_polygon(&ga);
  gpc_free_polygon(&gb);
  gpc_free_polygon(&gret);
}

//: Return intersection of two polygons
void vcgl_polygon_clip_intersect(vector< TinyPolygon* > const& a, vector< TinyPolygon* > const& b, vector< TinyPolygon* > &result, byte zoom_factor,int &num_fill, int &num_holes)
{
  go(GPC_INT, a, b, result, zoom_factor, num_fill,num_holes);
}

//: Return difference of two polygons: points in "a" and not in "b".
void vcgl_polygon_clip_subtract(vector< TinyPolygon* > const& a, vector< TinyPolygon* > const& b, vector< TinyPolygon* > &result, byte zoom_factor,int &num_fill, int &num_holes)
{
  go(GPC_DIFF, a, b, result, zoom_factor,num_fill,num_holes);
}


//: Return difference of two polygons: points in one and not in the other.
void vcgl_polygon_clip_xor(vector< TinyPolygon* > const& a, vector< TinyPolygon* > const& b, vector< TinyPolygon* > &result, byte zoom_factor,int &num_fill, int &num_holes)
{
  go(GPC_XOR, a, b, result, zoom_factor,num_fill,num_holes);
}


//: Return union of polygons.
void vcgl_polygon_clip_union(vector< TinyPolygon* > const& a, vector< TinyPolygon* > const& b, vector< TinyPolygon* > &result, byte zoom_factor,int &num_fill, int &num_holes)
{
  go(GPC_UNION, a, b,result, zoom_factor, num_fill,num_holes);
}
