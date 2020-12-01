#ifndef INTERSECTION_CLASSIFICATION_H
#define INTERSECTION_CLASSIFICATION_H

#include "structures/triangle_soup.h"
#include "structures/aux_structure.h"

#include <cinolib/predicates.h>

#pragma GCC diagnostic ignored "-Wfloat-equal"

void classifyIntersections(TriangleSoup &ts, AuxiliaryStructure &g);

void checkTriangleTriangleIntersections(TriangleSoup &ts, AuxiliaryStructure &g, const uint &tA_id, const uint &tB_id);

uint addEdgeCrossEdgeInters(TriangleSoup &ts, const uint &e0_id, const uint &e1_id, AuxiliaryStructure &g);

uint addEdgeCrossEdgeInters(TriangleSoup &ts, const uint &e0_id, const uint &e1_id, const uint &t_id, AuxiliaryStructure &g);

uint addEdgeCrossTriInters(TriangleSoup &ts, const uint &e_id, const uint &t_id, AuxiliaryStructure &g);

void addSymbolicSegment(const TriangleSoup &ts, const uint &v0_id, const uint &v1_id, const uint &tA_id, const uint &tB_id, AuxiliaryStructure &g);

uint noCoplanarJollyPointID(const TriangleSoup &ts, const double *v0, const double *v1, const double *v2);

void checkSingleCoplanarEdgeIntersections(TriangleSoup &ts, const uint &e_v0, const uint &e_v1,
                                          const uint &e_t_id, const uint &o_t_id, AuxiliaryStructure &g, std::set<uint> &il);

void checkSingleNoCoplanarEdgeIntersection(TriangleSoup &ts, const uint &e_id, const uint &t_id,
                                           std::set<uint> &v_tmp, AuxiliaryStructure &g, std::set<uint> &li);

void checkVtxInTriangleIntersection(TriangleSoup &ts, const uint &v_id, const uint &t_id, std::set<uint> &v_tmp, AuxiliaryStructure &g, std::set<uint> &li);

void propagateCoplanarTrianlesIntersections(TriangleSoup &ts, AuxiliaryStructure &g);

void normalizeOrientations(double o[]);

bool sameOrientation(const double &o1, const double &o2);

// 1 if all edges are coplanar to the triangle, -1 otherwise
bool allCoplanarEdges(const double o[]);

// if there is a coplanar edge return its id, -1 otherwise
int singleCoplanarEdge(const double o[]);

// if there is a vertex in the plane and the opposite edge doesn't intersect the plane return the vtx id, -1 otherwise
int vtxInPlaneAndOppositeEdgeOnSameSide(const double o[]);

// if there is a vertex in the plane and the opposite edge intersect the plane return the vtx id, -1 otherwise
int vtxInPlaneAndOppositeEdgeCrossPlane(const double o[]);

// if there is a vertex on one side and the opposite edge on the other return the relative informations, -1 otherwise
int vtxOnASideAndOppositeEdgeOnTheOther(const double o[], uint &opp_v0, uint &opp_v1);


bool genericPointInsideTriangle(const TriangleSoup &ts, const uint &p_id, const uint &t_id, const bool &strict);


#endif // INTERSECTION_CLASSIFICATION_H
