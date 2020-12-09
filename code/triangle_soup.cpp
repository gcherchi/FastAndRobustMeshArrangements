#include "triangle_soup.h"

inline void TriangleSoup::init(const double &multiplier)
{
    num_orig_vtxs = static_cast<uint>(vertices.size());
    num_orig_tris = static_cast<uint>(triangles.size() / 3);

    edges.reserve(numVerts() + numTris());
    tri_planes.resize(numTris());

    //vertices
    for(uint v_id = 0; v_id < num_orig_vtxs; v_id++)
    {
        const explicitPoint3D &e = vertices[v_id]->toExplicit3D();
        vertices[v_id]->toExplicit3D().set(e.X() * multiplier, e.Y() * multiplier, e.Z() * multiplier);
    }

    // traingles
    for(uint t_id = 0; t_id < num_orig_tris; t_id++)
    {
        uint v0_id = triVertID(t_id, 0), v1_id = triVertID(t_id, 1), v2_id = triVertID(t_id, 2);

        tri_planes[t_id] = intToPlane(genericPoint::maxComponentInTriangleNormal(vertX(v0_id), vertY(v0_id), vertZ(v0_id),
                                                                                 vertX(v1_id), vertY(v1_id), vertZ(v1_id),
                                                                                 vertX(v2_id), vertY(v2_id), vertZ(v2_id)));

        // edges
        addEdge(v0_id, v1_id);
        addEdge(v1_id, v2_id);
        addEdge(v2_id, v0_id);
    }

    initJollyPoints(multiplier);
}

/*******************************************************************************************************
 *      VERTICES
 * ****************************************************************************************************/

inline uint TriangleSoup::numVerts() const
{
    return static_cast<uint>(vertices.size());
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline uint TriangleSoup::numTris() const
{
    return static_cast<uint>(triangles.size() / 3);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline uint TriangleSoup::numEdges() const
{
    return static_cast<uint>(edges.size());
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline uint TriangleSoup::numOrigTriangles() const
{
    return num_orig_tris;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline const genericPoint* TriangleSoup::vert(const uint &v_id) const
{
    assert(v_id < numVerts() && "vtx id out of range");
    return vertices[v_id];
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline const double* TriangleSoup::vertPtr(const uint &v_id) const
{
    assert(v_id < num_orig_vtxs && "vtx id out of range of original points");
    return vertices[v_id]->toExplicit3D().ptr();
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline double TriangleSoup::vertX(const uint &v_id) const
{
    assert(v_id < num_orig_vtxs && "vtx id out of range");
    return vertices[v_id]->toExplicit3D().X();
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline double TriangleSoup::vertY(const uint &v_id) const
{
    assert(v_id < num_orig_vtxs && "vtx id out of range");
    return vertices[v_id]->toExplicit3D().Y();
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline double TriangleSoup::vertZ(const uint &v_id) const
{
    assert(v_id < num_orig_vtxs && "vtx id out of range");
    return vertices[v_id]->toExplicit3D().Z();
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline uint TriangleSoup::addImplVert(genericPoint* gp)
{
    vertices.push_back(gp);
    return static_cast<uint>(vertices.size() -1);
}

/*******************************************************************************************************
 *      EDGES
 * ****************************************************************************************************/

inline int TriangleSoup::edgeID(const uint &v0_id, const uint &v1_id) const
{
    auto it = edge_map.find(uniqueEdge(v0_id, v1_id));

    if(it == edge_map.end()) return -1;
    return static_cast<int>(it->second); // edge id
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline const genericPoint* TriangleSoup::edgeVert(const uint &e_id, const uint &off) const
{
    assert(e_id < edges.size() && "e_id out of range");
    if(off == 0) return vert(edges[e_id].first);
    else         return vert(edges[e_id].second);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline const double* TriangleSoup::edgeVertPtr(const uint &e_id, const uint &off) const
{
    assert(e_id < edges.size() && "e_id out of range");
    if(off == 0) return vertPtr(edges[e_id].first);
    else         return vertPtr(edges[e_id].second);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline uint TriangleSoup::edgeOppositeToVert(const uint &t_id, const uint &v_id) const
{
    assert(t_id < numTris() && "t_id out of range");
    assert(v_id < numVerts() && "vtx id out of range");

    int e_id = -1;

    if     (triVertID(t_id, 0) == v_id) e_id = edgeID(triVertID(t_id, 1), triVertID(t_id, 2));
    else if(triVertID(t_id, 1) == v_id) e_id = edgeID(triVertID(t_id, 0), triVertID(t_id, 2));
    else if(triVertID(t_id, 2) == v_id) e_id = edgeID(triVertID(t_id, 0), triVertID(t_id, 1));

    assert(e_id >= 0 && "Opposite edge not found");
    return static_cast<uint>(e_id);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline void TriangleSoup::addEdge(const uint &v0_id, const uint &v1_id)
{
    uint tmp_id = static_cast<uint>(edges.size());
    Edge e = uniqueEdge(v0_id, v1_id);

    auto it = edge_map.insert({e, tmp_id});

    if(it.second) // new edge
        edges.push_back(e);
}

/*******************************************************************************************************
 *      TRIANGLES
 * ****************************************************************************************************/

const std::vector<uint>& TriangleSoup::trisVector() const
{
    return triangles;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline const uint* TriangleSoup::tri(const uint &t_id) const
{
    assert(t_id < numTris() && "t_id out of range");
    return &triangles[ 3 * t_id];
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline uint TriangleSoup::triVertID(const uint &t_id, const uint &off) const
{
    assert(t_id < numTris() && "t_id out of range");
    return triangles[3 * t_id + off];
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline const genericPoint* TriangleSoup::triVert(const uint &t_id, const uint &off) const
{
    assert(t_id < numTris() && "t_id out of range");
    return vert(triangles[3 * t_id + off]);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline const double* TriangleSoup::triVertPtr(const uint &t_id, const uint &off) const
{
    assert(t_id < numTris() && "t_id out of range");
    return vertPtr(triangles[3 * t_id + off]);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline uint TriangleSoup::triEdgeID(const uint &t_id, const uint &off) const
{
    assert(t_id < numTris() && "t_id out of range");
    int e_id = edgeID(triangles[3 * t_id + off],
                      triangles[3 * t_id + ((off +1) %3)]);

    assert(e_id >= 0 && "no triangle edge found");
    return static_cast<uint>(e_id);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline Plane TriangleSoup::triPlane(const uint &t_id) const
{
    assert(t_id < numTris() && "t_id out of range");
    return tri_planes[t_id];
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline bool TriangleSoup::triContainsVert(const uint &t_id, const uint &v_id) const
{
    assert(t_id < numTris() && "t_id out of range");
    assert(v_id < numVerts() && "v_id out of range");

    if(triangles[3 * t_id]     == v_id) return true;
    if(triangles[3 * t_id + 1] == v_id) return true;
    if(triangles[3 * t_id + 2] == v_id) return true;
    return false;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline bool TriangleSoup::triContainsEdge(const uint t_id, const uint &ev0_id, const uint &ev1_id) const
{
    return (triContainsVert(t_id, ev0_id) && triContainsVert(t_id, ev1_id));
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline std::bitset<NBIT> TriangleSoup::triLabel(const uint &t_id) const
{
    assert(t_id < numTris() && "t_id out of range");
    return tri_labels[t_id];
}

/*******************************************************************************************************
 *      JOLLY POINTS
 * ****************************************************************************************************/

inline const genericPoint* TriangleSoup::jollyPoint(const uint &off) const
{
    assert(off < 4 && "jolly point id out of range");
    return vertices[num_orig_vtxs + off];
}

/********************************************************************************************************
 *              PRIVATE METHODS
 * ****************************************************************************************************/

inline void TriangleSoup::initJollyPoints(const double &multiplier)
{
    vertices.push_back(new explicitPoint3D(0.94280904158 * multiplier, 0.0 * multiplier, -0.333333333 * multiplier));
    vertices.push_back(new explicitPoint3D(-0.47140452079 * multiplier, 0.81649658092 * multiplier, -0.333333333 * multiplier));
    vertices.push_back(new explicitPoint3D(-0.47140452079 * multiplier, -0.81649658092 * multiplier, -0.333333333 * multiplier));
    vertices.push_back(new explicitPoint3D(0.0 * multiplier, 0.0 * multiplier, 1.0 * multiplier));
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline Edge TriangleSoup::uniqueEdge(const uint &v0_id, const uint &v1_id) const
{
    if(v0_id < v1_id) return {v0_id, v1_id};
    return {v1_id, v0_id};
}

