/*****************************************************************************************
 *              MIT License                                                              *
 *                                                                                       *
 * Copyright (c) 2020 Gianmarco Cherchi, Marco Livesu, Riccardo Scateni e Marco Attene   *
 *                                                                                       *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this  *
 * software and associated documentation files (the "Software"), to deal in the Software *
 * without restriction, including without limitation the rights to use, copy, modify,    *
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to    *
 * permit persons to whom the Software is furnished to do so, subject to the following   *
 * conditions:                                                                           *
 *                                                                                       *
 * The above copyright notice and this permission notice shall be included in all copies *
 * or substantial portions of the Software.                                              *
 *                                                                                       *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,   *
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A         *
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT    *
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION     *
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE        *
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                *
 *                                                                                       *
 * Authors:                                                                              *
 *      Gianmarco Cherchi (g.cherchi@unica.it)                                           *
 *      https://people.unica.it/gianmarcocherchi/                                        *
 *                                                                                       *
 *      Marco Livesu (marco.livesu@ge.imati.cnr.it)                                      *
 *      http://pers.ge.imati.cnr.it/livesu/                                              *
 *                                                                                       *
 *      Riccardo Scateni (riccardo@unica.it)                                             *
 *      https://people.unica.it/riccardoscateni/                                         *
 *                                                                                       *
 *      Marco Attene (marco.attene@ge.imati.cnr.it)                                      *
 *      https://www.cnr.it/en/people/marco.attene/                                       *
 *                                                                                       *
 * ***************************************************************************************/

#ifndef TRIANGULATION_H
#define TRIANGULATION_H

#include "structures/aux_structure.h"
#include "structures/triangle_soup.h"
#include "structures/fast_trimesh.h"
#include "structures/tree.h"

#include <vector>
#include <stack>
#include <numeric>

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


void triangulation(TriangleSoup &ts, AuxiliaryStructure &g, std::vector<uint> &new_tris);

void triangulateSingleTriangle(TriangleSoup &ts, FastTrimesh &subm, const uint &t_id, AuxiliaryStructure &g, std::vector<uint> &new_tris);

void splitSingleTriangle(const TriangleSoup &ts, FastTrimesh &subm, const std::set<uint> &points);

void splitSingleTriangleWithTree(const TriangleSoup &ts, FastTrimesh &subm, const std::set<uint> &points);

int findContainingTriangle(const FastTrimesh &subm, const uint &p_id);

const Node &findContainingTriangleInTree(const FastTrimesh &subm, const uint &p_id, const Tree &tree);
const Node &innerFindContainingTriangleInTree(const Tree &tree, const uint &root_id, const FastTrimesh &subm, const genericPoint *p);

void splitSingleEdge(const TriangleSoup &ts, FastTrimesh &subm, const uint &v0_id, const uint &v1_id, std::vector<uint> &points);

void addConstraintSegmentsInSingleTriangle(TriangleSoup &ts, FastTrimesh &subm, AuxiliaryStructure &g, std::vector<UIPair> &segment_list);

void addConstraintSegment(TriangleSoup &ts, FastTrimesh &subm, const uint &v0_id, const uint &v1_id, const int orientation,
                          AuxiliaryStructure &g, std::vector<UIPair> &segment_list, CustomUnorderedPairPairMap &sub_segs_map);

void findIntersectingElements(TriangleSoup &ts, FastTrimesh &subm, const uint &v_start, const uint &v_stop, std::vector<uint> &intersected_edges, std::vector<uint> &intersected_tris,
                              AuxiliaryStructure &g, std::vector<UIPair> &segment_list, CustomUnorderedPairPairMap &sub_seg_map);

template<typename iterator>
void boundaryWalker(const FastTrimesh &subm, const uint &v_start, const uint &v_stop, iterator curr_p, iterator curr_e, std::vector<uint> &h);

void earcut(const FastTrimesh &subm, std::vector<uint> &poly, std::vector<uint> &tris, const Plane &ref_p, const int &orientation);

void earcutLinear(const FastTrimesh &subm, const std::vector<uint> &poly, std::vector<uint> &tris, const int &orientation);

uint createTPI(TriangleSoup &ts, FastTrimesh &subm, const UIPair &e0, const UIPair &e1, AuxiliaryStructure &g, const CustomUnorderedPairPairMap &sub_segs_map);

std::vector<const genericPoint *> computeTriangleOfSegment(const TriangleSoup &ts, const UIPair &seg, std::vector<uint> &ref_t,
                                                           const AuxiliaryStructure &g, const CustomUnorderedPairPairMap &sub_segs_map);

std::vector<const genericPoint *> computeTriangleOfSegmentInCoplanarCase(const TriangleSoup &ts, const UIPair &seg, const std::set<uint> &tris, const std::vector<uint> &ref_t);

bool vectorsAreEqual(std::vector<uint> &v0, std::vector<uint> &v1);

bool fastPointOnLine(const FastTrimesh &subm, const uint &e_id, const uint &p_id);

bool segmentsIntersectInside(const FastTrimesh &subm, const uint &e00_id, const uint &e01_id, const uint &e10_id, const uint &e11_id);

bool pointInsideSegment(const FastTrimesh &subm, const uint &ev0_id, const uint &ev1_id, const uint &p_id);

void splitSegmentInSubSegments(const uint &v_start, const uint &v_stop, const uint &mid_point, CustomUnorderedPairPairMap &sub_segments_map);

const std::set<uint> &segmentTrianglesList(const UIPair &seg, const CustomUnorderedPairPairMap &sub_segments_map, const AuxiliaryStructure &g);

void solvePocketsInCoplanarTriangle(const FastTrimesh &subm, AuxiliaryStructure &g, std::vector<bool> &valid_tris);

void findPocketsInTriangle(const FastTrimesh &subm, std::vector<std::vector<uint> > &tri_pockets, std::vector<std::set<uint> > &polygons);

int customOrient2D(const genericPoint *p0, const genericPoint *p1, const genericPoint *p2, const Plane &ref_p);

void sortedVertexListAlongSegment(const TriangleSoup &ts, const std::set<uint> &point_list,
                                  const uint &v0_id, const uint &v1_id, std::vector<uint> &res);


#endif // TRIANGULATION_H
