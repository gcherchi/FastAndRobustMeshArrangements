#ifndef TRIANGLESOUP_H
#define TRIANGLESOUP_H

#include "common.h"
#include <implicit_point.h>

#include <cinolib/geometry/vec3.h>

#include <vector>
#include <map>
#include <bitset>

typedef std::pair<uint, uint> Edge;

class TriangleSoup
{
    public:

        inline TriangleSoup(std::vector<genericPoint*> &in_vertices, std::vector<uint> &in_tris, std::vector< std::bitset<NBIT> > &labels, const double &multiplier)
            : vertices(in_vertices), triangles(in_tris), tri_labels(labels)
        {
            init(multiplier);
        }

        inline ~TriangleSoup()
        {
//            for(uint v = 0; v < impl_vertices.size(); v++)
//                delete impl_vertices[v];
        }

        inline void init(const double &multiplier);

        inline uint numVerts() const;
        inline uint numTris() const;
        inline uint numEdges() const;

        //inline uint numOrigVertices() const;
        inline uint numOrigTriangles() const;

        // VERTICES
        inline const genericPoint* vert(const uint &v_id) const;

        inline const double* vertPtr(const uint &v_id) const;

        inline double vertX(const uint &v_id) const;
        inline double vertY(const uint &v_id) const;
        inline double vertZ(const uint &v_id) const;

        inline uint addImplVert(genericPoint* gp);

        // EDGES
        inline int edgeID(const uint &v0_id, const uint &v1_id) const;

        inline const genericPoint* edgeVert(const uint &e_id, const uint &off) const;

        inline const double* edgeVertPtr(const uint &e_id, const uint &off) const;

        inline uint edgeOppositeToVert(const uint &t_id, const uint &v_id) const;

        inline void addEdge(const uint &v0_id, const uint &v1_id);

        // TRIANGLES
        inline const std::vector<uint>& trisVector() const;

        inline const uint* tri(const uint &t_id) const;

        inline uint triVertID(const uint &t_id, const uint &off) const;

        inline const genericPoint* triVert(const uint &t_id, const uint &off) const;

        inline const double* triVertPtr(const uint &t_id, const uint &off) const;

        inline uint triEdgeID(const uint &t_id, const uint &off) const;

        inline Plane triPlane(const uint &t_id) const;

        inline bool triContainsVert(const uint &t_id, const uint &v_id) const;

        inline bool triContainsEdge(const uint t_id, const uint &ev0_id, const uint &ev1_id) const;

        inline std::bitset<NBIT> triLabel(const uint &t_id) const;

        // JOLLY POINTS
        inline const genericPoint* jollyPoint(const uint &off) const;

        inline void appendJollyPoints();

    private:

        std::vector<genericPoint*>      &vertices;

        std::vector<Edge>               edges;

        std::vector<uint>               &triangles;
        std::vector<std::bitset<NBIT>>  &tri_labels;
        std::vector<Plane>              tri_planes;

        std::vector<genericPoint*>      jolly_points;

        std::map<Edge, uint> edge_map;

        uint num_orig_vtxs;
        uint num_orig_tris;

        // PRIVATE METHODS
        inline void initJollyPoints(const double &multiplier);

        inline Edge uniqueEdge(const uint &v0_id, const uint &v1_id) const;
};

#include "triangle_soup.cpp"

#endif // TRIANGLESOUP_H
