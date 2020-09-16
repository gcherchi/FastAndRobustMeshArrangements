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

#ifndef INTERSECTIONS_GRAPH_H
#define INTERSECTIONS_GRAPH_H

#include <vector>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include "structures/triangle_soup.h"

typedef unsigned int uint;

typedef std::pair<uint, uint> UIPair;


struct lessThanForSet
{
    bool operator()(const std::pair<const genericPoint*, uint> &a, const std::pair<const genericPoint*,uint> &b) const
    {
        return (genericPoint::lessThan(*a.first, *b.first) < 0);
    }
};


struct VectorHash
{
    inline size_t operator()(const std::vector<uint>& v) const
    {
        std::hash<uint> hasher;
        size_t seed = 0;
        for (uint i : v) seed ^= hasher(i) + 0x9e3779b9 + (seed<<6) + (seed>>2);

        return seed;
    }
};

typedef std::unordered_set< std::vector<uint>, VectorHash > CustomUnorderedVectorSet;


struct PairHash
{
    inline size_t operator()(const UIPair &p) const
    {
        std::hash<uint> hasher;
        size_t seed = 0;
        seed ^= hasher(p.first) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        seed ^= hasher(p.second) + 0x9e3779b9 + (seed<<6) + (seed>>2);

        return seed;
    }
};


typedef std::unordered_map< UIPair, std::set<uint>, PairHash  > CustomUnorderedPairSetMap;


class AuxiliaryStructure
{
    public:

        AuxiliaryStructure() {}

        inline void initFromTriangleSoup(TriangleSoup &ts);

        inline const std::vector<uint> &triangleIntersectionList(const uint &t_id) const;

        inline std::vector<std::vector<uint> > &intersectionList();

        inline void clearStructure();

        inline bool addVertexInTriangle(const uint &t_id, const uint &v_id);

        inline bool addVertexInEdge(const uint &e_id, const uint &v_id);

        inline bool addSegmentInTriangle(const uint &t_id, const UIPair &seg);

        inline void addTrianglesInSegment(const UIPair &seg, const std::vector<uint> &triangles);
        inline void addTrianglesInSegment(const UIPair &seg, const std::set<uint> &triangles);

        inline void splitSegmentInSubSegments(const uint &orig_v0, const uint &orig_v1, const uint &midpoint);

        inline void addCoplanarTriangles(const uint &ta, const uint &tb);

        inline const std::vector<uint> &coplanarTriangles(const uint &t_id) const;

        inline bool triangleHasCoplanars(const uint &t_id) const;

        inline bool triangleHasIntersections(const uint &t_id) const;

        inline const std::vector< std::vector<uint> > &intersectionList() const;

        inline const std::set<uint> &trianglePointsList(const uint &t_id) const;

        inline const std::set<uint> &edgePointsList(const uint &e_id) const;

        inline const std::set<UIPair> &triangleSegmentsList(const uint &t_id) const;

        inline const std::set<uint> &segmentTrianglesList(const UIPair &seg) const;

        inline std::pair<uint, bool> addVertexInSortedList(const std::pair<const genericPoint *, uint> &vtx_pair);

        inline bool addVisitedPolygonPocket(const std::set<uint> &polygon);

        inline uint numIntersections() const;

        inline void incrementNumTPI(const uint &num);

        inline void incrementNumIntersections(const uint &num);

        inline uint numTPI() const;


    private:

        uint num_original_vtx;
        uint num_original_tris;
        int num_intersections;
        uint num_tpi;

        std::vector< std::vector<uint> > coplanar_tris;

        std::vector< std::set<uint> > tri2pts;
        std::vector< std::set<uint> > edge2pts;
        std::vector< std::vector<uint> > tri2tri;
        std::vector< std::set<UIPair> > tri2segs;
        CustomUnorderedPairSetMap seg2tris;

        std::set< std::pair<const genericPoint*, uint> , lessThanForSet> sorted_vtx;

        std::set< std::set<uint> > visited_pockets;

        inline UIPair uniquePair(const UIPair &uip) const;
};


#include "aux_structure.tpp"


#endif // INTERSECTIONS_GRAPH_H
