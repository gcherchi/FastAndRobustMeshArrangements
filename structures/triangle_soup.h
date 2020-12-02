#ifndef TRIANGLESOUP_H
#define TRIANGLESOUP_H

#include "common.h"
#include <implicit_point.h>

#include <cinolib/geometry/vec3.h>

#include <vector>
#include <map>
#include <bitset>

struct Tri
{
    Tri(const uint &_t0_id, const uint &_t1_id, const uint &_t2_id, const std::bitset<NBIT> &_l, const Plane &_p) : label(_l), p(_p)
    {
        v[0] = _t0_id;
        v[1] = _t1_id;
        v[2] = _t2_id;
    }

    uint v[3];
    std::bitset<NBIT> label = 0;
    Plane p;
};

typedef std::pair<uint, uint> Edge;

class TriangleSoup
{
    public:

        inline TriangleSoup(){}

        inline TriangleSoup(const std::vector<double> &coords, const std::vector<uint> &tris, const std::vector< std::bitset<NBIT> > &labels)
        {
            init(coords, tris, labels);
        }

        inline ~TriangleSoup()
        {
            for(uint v = 0; v < impl_vertices.size(); v++)
                delete impl_vertices[v];
        }

        inline void init(const std::vector<double> &coords, const std::vector<uint> &tris, const std::vector< std::bitset<NBIT> > &labels);

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

        inline cinolib::vec3d vertCinolib(const uint &v_id) const;

        inline uint addVert(genericPoint* gp);

        inline void setVert(const uint &v_id, const double &x, const double &y, const double &z);

        // EDGES
        inline int edgeID(const uint &v0_id, const uint &v1_id) const;

        inline const genericPoint* edgeVert(const uint &e_id, const uint &off) const;

        inline const double* edgeVertPtr(const uint &e_id, const uint &off) const;

        inline uint edgeOppositeToVert(const uint &t_id, const uint &v_id) const;

        inline void addEdge(const uint &v0_id, const uint &v1_id);

        // TRIANGLES
        inline const uint* tri(const uint &t_id) const;

        inline uint triVertID(const uint &t_id, const uint &off) const;

        inline const genericPoint* triVert(const uint &t_id, const uint &off) const;

        inline const double* triVertPtr(const uint &t_id, const uint &off) const;

        inline cinolib::vec3d triVertCinolib(const uint &t_id, const uint &off) const;

        inline uint triEdgeID(const uint &t_id, const uint &off) const;

        inline Plane triPlane(const uint &t_id) const;

        inline bool triContainsVert(const uint &t_id, const uint &v_id) const;

        inline bool triContainsEdge(const uint t_id, const uint &ev0_id, const uint &ev1_id) const;

        inline void createDoubleVectorOfCoords(std::vector<double> &coords, const double &multiplier);

        inline void setTriLabel(const uint &t_id, const std::bitset<NBIT> &l);

        inline std::bitset<NBIT> triLabel(const uint &t_id) const;

        // JOLLY POINTS
        inline const genericPoint* jollyPoint(const uint &off) const;

        inline genericPoint* jollyPoint(const uint &off);

    private:

        std::vector<explicitPoint3D>  orig_vertices;
        std::vector<genericPoint*>    impl_vertices;
        std::vector<Edge>             edges;
        std::vector<Tri>              triangles;

        std::map<Edge, uint> edge_map;

        std::vector<explicitPoint3D>  jolly_points;

        uint num_orig_vtxs;
        uint num_orig_tris;

        // PRIVATE METHODS
        inline void initJollyPoints();

        inline Edge uniqueEdge(const uint &v0_id, const uint &v1_id) const;
};

#include "triangle_soup.cpp"

#endif // TRIANGLESOUP_H
