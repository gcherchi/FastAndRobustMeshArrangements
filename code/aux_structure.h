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

#include "triangle_soup.h"

#include <set>
#include <unordered_set>
#include <unordered_map>

#include <mutex>

typedef std::pair<uint, uint> UIPair;

struct lessThanForMap
{
    bool operator()(const genericPoint* a, const genericPoint* b) const
    {
        return (genericPoint::lessThan(*a, *b) < 0);
    }
};

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

        inline AuxiliaryStructure() {}

        inline void initFromTriangleSoup(TriangleSoup &ts);

        inline std::set< std::pair<uint, uint> > &intersectionList();

        inline const std::set<std::pair<uint, uint> > &intersectionList() const;

        inline bool addVertexInTriangle(const uint &t_id, const uint &v_id);

        inline bool addVertexInEdge(const uint &e_id, const uint &v_id);

        inline bool addSegmentInTriangle(const uint &t_id, const UIPair &seg);

        inline void addTrianglesInSegment(const UIPair &seg, const uint &tA_id, const uint &tB_id);

        inline void splitSegmentInSubSegments(const uint &orig_v0, const uint &orig_v1, const uint &midpoint);

        inline void addCoplanarTriangles(const uint &ta, const uint &tb);

        inline const std::vector<uint> &coplanarTriangles(const uint &t_id) const;

        inline bool triangleHasCoplanars(const uint &t_id) const;

        inline void setTriangleHasIntersections(const uint &t_id);

        inline bool triangleHasIntersections(const uint &t_id) const;

        inline const std::set<uint> &trianglePointsList(const uint &t_id) const;

        inline const std::set<uint> &edgePointsList(const uint &e_id) const;

        inline const std::set<UIPair> &triangleSegmentsList(const uint &t_id) const;

        inline const std::set<uint> &segmentTrianglesList(const UIPair &seg) const;

        //inline std::pair<uint, bool> addVertexInSortedList(const std::pair<const genericPoint *, uint> &vtx_pair);
        inline std::pair<uint, bool> addVertexInSortedList(const genericPoint *v, const uint &pos);

        inline bool addVisitedPolygonPocket(const std::set<uint> &polygon);

        inline int addVisitedPolygonPocket(const std::set<uint> &polygon, const uint &pos);

        // mutex
        std::mutex tri_mutex;
        std::mutex tpi_mutex;


    private:

        uint    num_original_vtx;
        uint    num_original_tris;
        int     num_intersections;
        uint    num_tpi;

        std::set< std::pair<uint, uint> >    intersection_list;
        std::vector< std::vector<uint> >    coplanar_tris;

        std::vector< std::set<uint> >       tri2pts;
        std::vector< std::set<uint> >       edge2pts;
        std::vector< std::set<UIPair> >     tri2segs;
        CustomUnorderedPairSetMap           seg2tris;

        std::vector<bool>                   tri_has_intersections;

        std::map< const genericPoint*, uint, lessThanForMap> v_map;

        std::set< std::set<uint> >      visited_pockets;

        std::map< std::set<uint>, uint> pockets_map;


        inline UIPair uniquePair(const UIPair &uip) const;
};


#include "aux_structure.cpp"

#endif // INTERSECTIONS_GRAPH_H




