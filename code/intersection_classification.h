#ifndef INTERSECTION_CLASSIFICATION_H
#define INTERSECTION_CLASSIFICATION_H

#include "triangle_soup.h"
#include "aux_structure.h"

#include <cinolib/predicates.h>

#pragma GCC diagnostic ignored "-Wfloat-equal"



inline void detectIntersections(const TriangleSoup &ts, std::set<std::pair<uint, uint> > &intersection_list);

inline void classifyIntersections(TriangleSoup &ts, AuxiliaryStructure &g);

inline void checkTriangleTriangleIntersections(TriangleSoup &ts, AuxiliaryStructure &g, const uint &tA_id, const uint &tB_id);

inline uint addEdgeCrossEdgeInters(TriangleSoup &ts, const uint &e0_id, const uint &e1_id, AuxiliaryStructure &g);

inline uint addEdgeCrossEdgeInters(TriangleSoup &ts, const uint &e0_id, const uint &e1_id, const uint &t_id, AuxiliaryStructure &g);

inline uint addEdgeCrossTriInters(TriangleSoup &ts, const uint &e_id, const uint &t_id, AuxiliaryStructure &g);

inline void addSymbolicSegment(const TriangleSoup &ts, const uint &v0_id, const uint &v1_id, const uint &tA_id, const uint &tB_id, AuxiliaryStructure &g);

inline uint noCoplanarJollyPointID(const TriangleSoup &ts, const double *v0, const double *v1, const double *v2);

inline void checkSingleCoplanarEdgeIntersections(TriangleSoup &ts, const uint &e_v0, const uint &e_v1,
                                                 const uint &e_t_id, const uint &o_t_id, AuxiliaryStructure &g, std::set<uint> &il);

inline void checkSingleNoCoplanarEdgeIntersection(TriangleSoup &ts, const uint &e_id, const uint &t_id,
                                                  std::set<uint> &v_tmp, AuxiliaryStructure &g, std::set<uint> &li);

inline void checkVtxInTriangleIntersection(TriangleSoup &ts, const uint &v_id, const uint &t_id, std::set<uint> &v_tmp, AuxiliaryStructure &g, std::set<uint> &li);

inline void propagateCoplanarTrianlesIntersections(TriangleSoup &ts, AuxiliaryStructure &g);

inline void normalizeOrientations(double o[]);

inline bool sameOrientation(const double &o1, const double &o2);

// 1 if all edges are coplanar to the triangle, -1 otherwise
inline bool allCoplanarEdges(const double o[]);

// if there is a coplanar edge return its id, -1 otherwise
inline int singleCoplanarEdge(const double o[]);

// if there is a vertex in the plane and the opposite edge doesn't intersect the plane return the vtx id, -1 otherwise
inline int vtxInPlaneAndOppositeEdgeOnSameSide(const double o[]);

// if there is a vertex in the plane and the opposite edge intersect the plane return the vtx id, -1 otherwise
inline int vtxInPlaneAndOppositeEdgeCrossPlane(const double o[]);

// if there is a vertex on one side and the opposite edge on the other return the relative informations, -1 otherwise
inline int vtxOnASideAndOppositeEdgeOnTheOther(const double o[], uint &opp_v0, uint &opp_v1);

inline bool genericPointInsideTriangle(const TriangleSoup &ts, const uint &p_id, const uint &t_id, const bool &strict);


#include "intersection_classification.cpp"

#endif // INTERSECTION_CLASSIFICATION_H
