#ifndef TRIANGULATION_H
#define TRIANGULATION_H

#include "aux_structure.h"
#include "triangle_soup.h"
#include "fast_trimesh.h"
#include "tree.h"

#pragma GCC diagnostic ignored "-Wfloat-equal"

typedef unsigned int uint;

typedef std::unordered_map< UIPair, UIPair, PairHash  > CustomUnorderedPairPairMap;


// lessThan(a,b) =
// -1 - if a < b
// 0  - if a == b
// 1  - if a > b
inline bool lessThanP(std::pair<const genericPoint*, uint> &a, std::pair<const genericPoint*, uint> &b)
{
    return (genericPoint::lessThan(*a.first, *b.first) < 0);
}


inline void triangulation(TriangleSoup &ts, AuxiliaryStructure &g, std::vector<uint> &new_tris, std::vector<std::bitset<NBIT> > &new_labels);

inline void triangulateSingleTriangle(TriangleSoup &ts, FastTrimesh &subm, const uint &t_id, AuxiliaryStructure &g, std::vector<uint> &new_tris, std::vector<std::bitset<NBIT> > &new_labels);

inline void splitSingleTriangle(const TriangleSoup &ts, FastTrimesh &subm, const std::set<uint> &points);

inline void splitSingleTriangleWithTree(const TriangleSoup &ts, FastTrimesh &subm, const std::set<uint> &points);

inline int findContainingTriangle(const FastTrimesh &subm, const uint &p_id);

inline const Node &findContainingTriangleInTree(const FastTrimesh &subm, const uint &p_id, const Tree &tree);
inline const Node &innerFindContainingTriangleInTree(const Tree &tree, const uint &root_id, const FastTrimesh &subm, const genericPoint *p);

inline void splitSingleEdge(const TriangleSoup &ts, FastTrimesh &subm, const uint &v0_id, const uint &v1_id, std::vector<uint> &points);

inline void addConstraintSegmentsInSingleTriangle(TriangleSoup &ts, FastTrimesh &subm, AuxiliaryStructure &g, std::vector<UIPair> &segment_list);

inline void addConstraintSegment(TriangleSoup &ts, FastTrimesh &subm, const uint &v0_id, const uint &v1_id, const int orientation,
                                 AuxiliaryStructure &g, std::vector<UIPair> &segment_list, CustomUnorderedPairPairMap &sub_segs_map);

inline void findIntersectingElements(TriangleSoup &ts, FastTrimesh &subm, const uint &v_start, const uint &v_stop, std::vector<uint> &intersected_edges, std::vector<uint> &intersected_tris,
                                     AuxiliaryStructure &g, std::vector<UIPair> &segment_list, CustomUnorderedPairPairMap &sub_seg_map);

template<typename iterator>
inline void boundaryWalker(const FastTrimesh &subm, const uint &v_start, const uint &v_stop, iterator curr_p, iterator curr_e, std::vector<uint> &h);

inline void earcut(const FastTrimesh &subm, std::vector<uint> &poly, std::vector<uint> &tris, const Plane &ref_p, const int &orientation);

inline void earcutLinear(const FastTrimesh &subm, const std::vector<uint> &poly, std::vector<uint> &tris, const int &orientation);

inline uint createTPI(TriangleSoup &ts, FastTrimesh &subm, const UIPair &e0, const UIPair &e1, AuxiliaryStructure &g, const CustomUnorderedPairPairMap &sub_segs_map);

inline std::vector<const genericPoint *> computeTriangleOfSegment(const TriangleSoup &ts, const UIPair &seg, std::vector<uint> &ref_t,
                                                                  const AuxiliaryStructure &g, const CustomUnorderedPairPairMap &sub_segs_map);

inline std::vector<const genericPoint *> computeTriangleOfSegmentInCoplanarCase(const TriangleSoup &ts, const UIPair &seg, const std::set<uint> &tris, const std::vector<uint> &ref_t);

inline bool vectorsAreEqual(std::vector<uint> &v0, std::vector<uint> &v1);

inline bool fastPointOnLine(const FastTrimesh &subm, const uint &e_id, const uint &p_id);

inline bool segmentsIntersectInside(const FastTrimesh &subm, const uint &e00_id, const uint &e01_id, const uint &e10_id, const uint &e11_id);

inline bool pointInsideSegment(const FastTrimesh &subm, const uint &ev0_id, const uint &ev1_id, const uint &p_id);

inline void splitSegmentInSubSegments(const uint &v_start, const uint &v_stop, const uint &mid_point, CustomUnorderedPairPairMap &sub_segments_map);

inline const std::set<uint> &segmentTrianglesList(const UIPair &seg, const CustomUnorderedPairPairMap &sub_segments_map, const AuxiliaryStructure &g);

inline void solvePocketsInCoplanarTriangle(const FastTrimesh &subm, AuxiliaryStructure &g, std::vector<uint> &new_tris,
                                           std::vector<std::bitset<NBIT> > &new_labels, const std::bitset<NBIT> &label);

inline void findPocketsInTriangle(const FastTrimesh &subm, std::vector<std::vector<uint> > &tri_pockets, std::vector<std::set<uint> > &polygons);

inline int customOrient2D(const genericPoint *p0, const genericPoint *p1, const genericPoint *p2, const Plane &ref_p);

inline void sortedVertexListAlongSegment(const TriangleSoup &ts, const std::set<uint> &point_list, const uint &v0_id, const uint &v1_id, std::vector<uint> &res);


#include "triangulation.cpp"

#endif // TRIANGULATION_H
