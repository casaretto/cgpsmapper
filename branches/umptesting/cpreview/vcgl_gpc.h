#ifndef vcgl_gpc_h_
#define vcgl_gpc_h_
// <begin copyright notice>
// ==========================================================================
//
// Project:   Generic Polygon Clipper
//
//            Based on the algorithm:
//            Bala R. Vatti, "A Generic Solution to Polygon Clipping",
//            Communications of the ACM, 35(7), July 1992, pp.56-63.
//
// File:      gpc.h
// Author:    Alan Murta (amurta@cs.man.ac.uk)
// Version:   2.00
// Date:      25th November 1997
//
// Copyright: (C) 1997, Advanced Interfaces Group, University of Manchester.
//            All rights reserved.
//
//            This software may be freely copied, modified, and redistributed
//            provided that this copyright notice is preserved on all copies.
//            The intellectual property rights of the algorithms used reside
//            with the University of Manchester Advanced Interfaces Group.
//
//            You may not distribute this software, in whole or in part, as
//            part of any commercial product without the express consent of
//            the author.
//
//            There is no warranty or other guarantee of fitness of this
//            software for any purpose. It is provided solely "as is".
//
// ==========================================================================
// <end copyright notice>

//:
// \file
// \brief Set operations on 2d polygons.
// \author Translated to C++ by Robin Y. Flatland from Alan Murta's gpc library version 2.20 written in C.
//
// A library for calculating the difference, intersection, exclusive-or,
//  and union of arbitrary 2D polygons.
//
// See PolygonClipper.h for the TargetJr interface to this library.
// Last Modified: June 1998

class vcgl_gpc
{
public:
    enum gpc_op                         // Set operation type
    {
      GPC_DIFF,                         // Difference
      GPC_INT,                          // Intersection
      GPC_XOR,                          // Exclusive or
      GPC_UNION                         // Union
    };

    struct gpc_vertex                   // Polygon vertex structure
    {
      double              x;            // Vertex x component
      double              y;            // vertex y component
    };

    struct gpc_vertex_list              // Vertex list structure
    {
      int                 num_vertices; // Number of vertices in list
      gpc_vertex         *vertex;       // Vertex array pointer
    };

    struct gpc_polygon                  // Polygon set structure
    {
      int                 num_contours; // Number of contours in polygon
      gpc_vertex_list    *contour;      // Contour array pointer
    };

    struct gpc_tristrip                 // Tristrip set structure
    {
      int                 num_strips;   // Number of tristrips
      gpc_vertex_list    *strip;        // Tristrip array pointer
    };

    void gpc_add_contour         (gpc_polygon     *polygon,
                                  gpc_vertex_list *contour);

    void gpc_polygon_clip        (gpc_op           set_operation,
                                  gpc_polygon     *subject_polygon,
                                  gpc_polygon     *clip_polygon,
                                  gpc_polygon     *result_polygon);

    void gpc_tristrip_clip       (gpc_op           set_operation,
                                  gpc_polygon     *subject_polygon,
                                  gpc_polygon     *clip_polygon,
                                  gpc_tristrip    *result_tristrip);

    void gpc_polygon_to_tristrip (gpc_polygon     *polygon,
                                  gpc_tristrip    *tristrip);

    void gpc_free_polygon        (gpc_polygon     *polygon);

    void gpc_free_tristrip       (gpc_tristrip    *tristrip);


public:

    enum vertex_type                    // Edge intersection classes
    {
      NUL,                              // Empty non-intersection
      EMX,                              // External maximum
      ELI,                              // External left intermediate
      TED,                              // Top edge
      ERI,                              // External right intermediate
      RED,                              // Right edge
      IMM,                              // Internal maximum and minimum
      IMN,                              // Internal minimum
      EMN,                              // External minimum
      EMM,                              // External maximum and minimum
      LED,                              // Left edge
      ILI,                              // Internal left intermediate
      BED,                              // Bottom edge
      IRI,                              // Internal right intermediate
      IMX,                              // Internal maximum
      FUL                               // Full non-intersection
    };

    enum h_state                        // Horizontal edge states
    {
      NH,                               // No horizontal edge
      BH,                               // Bottom horizontal edge
      TH                                // Top horizontal edge
    };

    struct vertex_node                  // Internal vertex list datatype
    {
      double              x;            // X coordinate component
      double              y;            // Y coordinate component
      vertex_node        *next;         // Pointer to next vertex in list
    };

    struct polygon_node                 // Internal polygon / tristrip type
    {
      int                 active;       // Active flag / vertex count
      vertex_node       **proxy;        // Pointer to actual v[] list in use
      vertex_node        *v[2];         // Left and right vertex list ptrs
      polygon_node       *next;         // Pointer to next polygon contour
    };

    struct edge_node
    {
      gpc_vertex          vertex;       // Piggy-backed contour vertex data
      gpc_vertex          bot;          // Edge lower (x, y) coordinate
      gpc_vertex          top;          // Edge upper (x, y) coordinate
      double              delx;         // Change in x for a unit y increase
      int                 type;         // Clip / subject edge flag
      int                 head;         // Head of bundle flag
      int                 bundle[2][2]; // Bundle edge flags
      int                 bside[2];     // Bundle left / right indicators
      polygon_node       *outp;         // Output polygon / tristrip ptr
      struct edge_node   *prev;         // Previous edge in the AET
      struct edge_node   *next;         // Next edge in the AET
      struct edge_node   *pred;         // Edge connected at the lower end
      struct edge_node   *succ;         // Edge connected at the upper end
      struct edge_node   *next_bound;   // Pointer to next bound in LMT
    };

    struct lmt_node                     // Local minima table
    {
      double              y;            // Y coordinate at local minimum
      edge_node          *first_bound;  // Pointer to bound list
      lmt_node           *next;         // Pointer to next local minimum
    };

    struct sb_tree                      // Scanbeam tree
    {
      double              y;            // Scanbeam node y value
      sb_tree            *less;         // Pointer to nodes with lower y
      sb_tree            *more;         // Pointer to nodes with higher y
    };

    struct it_node                      // Intersection table
    {
      edge_node          *ie[2];        // Intersecting edge (bundle) pair
      gpc_vertex          point;        // Point of intersection
      it_node            *next;         // The next intersection table node
    };

    struct st_node                      // Sorted edge table
    {
      edge_node          *e;            // Pointer to AET edge
      double              xtop;         // X coordinate at top of scanbeam
      st_node            *prev;         // Previous edge in sorted list
    };

private:
    // Minimum difference for equality determination
    double gpc_epsilon;

    // Horizontal edge state transitions within scanbeam boundary
    static const h_state next_h_state[3][6];

    void reset_it(it_node **it);
    void reset_lmt(lmt_node **lmt);
    void insert_bound(edge_node **b, edge_node *e);
    edge_node **bound_list(lmt_node **lmt, double y);
    void add_to_sbtree(int *entries, sb_tree **sbtree, double *y);
    void build_sbt(int *entries, double *sbt, sb_tree *sbtree);
    void free_sbtree(sb_tree **sbtree);
    int count_optimal_vertices(gpc_vertex_list c);
    edge_node *build_lmt(lmt_node **lmt, sb_tree **sbtree,
                                int *sbt_entries, gpc_polygon *p, int type,
                                gpc_op op);
    void add_edge_to_aet(edge_node **aet, edge_node *edge, edge_node *prev);
    void add_intersection(it_node **it, edge_node *edge0, edge_node *edge1,
                                 double x, double y);
    void add_st_edge(st_node **st, it_node **it, edge_node *edge,
                            double xtop, double dy, double yt);
    void build_intersection_table(it_node **it, edge_node *aet,
                                         double dy, double yt);
    int count_vertices(vertex_node *vertex);
    int count_contours(polygon_node *polygon);
    void add_left(polygon_node *P, double x, double y);
    void merge_left(polygon_node *P, polygon_node *Q, polygon_node *list);
    void add_right(polygon_node *P, double x, double y);
    void merge_right(polygon_node *P, polygon_node *Q, polygon_node *list);
    void add_local_min(polygon_node **P, edge_node *edge,
                              double x, double y);
    int count_tristrips(polygon_node *tn);
    void add_vertex(vertex_node **t, double x, double y);
    void new_tristrip(polygon_node **tn, edge_node *edge,
                             double x, double y);
    int set_gpc_epsilon(gpc_polygon *sp, gpc_polygon *cp);
};

#endif

