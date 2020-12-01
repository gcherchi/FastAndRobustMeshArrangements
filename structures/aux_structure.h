#ifndef INTERSECTIONS_GRAPH_H
#define INTERSECTIONS_GRAPH_H

#include "structures/triangle_soup.h"

#include <set>
#include <unordered_set>
#include <unordered_map>

typedef std::pair<uint, uint> UIPair;

struct lessThanForSet
{
    bool operator()(const std::pair<const genericPoint*, uint> &a, const std::pair<const genericPoint*,uint> &b) const
    {
        return (genericPoint::lessThan(*a.first, *b.first) < 0);
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

        inline std::vector< std::vector<uint> > &intersectionList();

        inline const std::vector<uint> &triangleIntersectionList(const uint &t_id) const;

        inline void clearStructure();

        inline bool addVertexInTriangle(const uint &t_id, const uint &v_id);

        inline bool addVertexInEdge(const uint &e_id, const uint &v_id);

        inline bool addSegmentInTriangle(const uint &t_id, const UIPair &seg);

        inline void addTrianglesInSegment(const UIPair &seg, const uint &tA_id, const uint &tB_id);

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

        inline int addVisitedPolygonPocket(const std::set<uint> &polygon, const uint &pos);

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

        std::map< std::set<uint>, uint> pockets_map;

        inline UIPair uniquePair(const UIPair &uip) const;
};


#include "aux_structure.cpp"

#endif // INTERSECTIONS_GRAPH_H




