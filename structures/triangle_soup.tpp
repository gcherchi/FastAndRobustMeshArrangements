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

#ifndef TRIANGLE_SOUP_TPP
#define TRIANGLE_SOUP_TPP

#include "triangle_soup.h"

inline void TriangleSoup::init(const std::vector<double> &coords, const std::vector<uint> &tris)
{
    num_orig_vtxs = static_cast<uint>(coords.size() /3);
    orig_vertices.resize(num_orig_vtxs);
    triangles.reserve(tris.size() / 3);
    edges.reserve(tris.size());

    // vertices
    for(uint v_id = 0; v_id < num_orig_vtxs; v_id++)
    {
        uint off = v_id *3;
        orig_vertices[v_id] = explicitPoint3D(coords[off], coords[off +1], coords[off +2]);
    }

    // traingles
    for(uint t_id = 0; t_id < tris.size(); t_id += 3)
    {
        uint v0_id = tris[t_id], v1_id = tris[t_id + 1], v2_id = tris[t_id + 2];
        if(v0_id == v1_id && v1_id == v2_id && v2_id == v0_id) continue; // degenerate triangle

        triangles.emplace_back(v0_id, v1_id, v2_id,
                               intToPlane(genericPoint::maxComponentInTriangleNormal(vertX(v0_id), vertY(v0_id), vertZ(v0_id),
                                                                                     vertX(v1_id), vertY(v1_id), vertZ(v1_id),
                                                                                     vertX(v2_id), vertY(v2_id), vertZ(v2_id))));
        // edges
        addEdge(v0_id, v1_id);
        addEdge(v1_id, v2_id);
        addEdge(v2_id, v0_id);
    }

    num_orig_tris = static_cast<uint>(triangles.size());

    initJollyPoints();
}

/*******************************************************************************************************
 *      VERTICES
 * ****************************************************************************************************/

inline uint TriangleSoup::numVerts() const
{
    return (num_orig_vtxs + static_cast<uint>(impl_vertices.size()));
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline uint TriangleSoup::numTris() const
{
    return static_cast<uint>(triangles.size());
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
    assert(v_id < (num_orig_vtxs + impl_vertices.size()) && "vtx id out of range");
    if(v_id < num_orig_vtxs) return &orig_vertices[v_id];
    return impl_vertices[v_id - num_orig_vtxs];
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline const double* TriangleSoup::vertPtr(const uint &v_id) const
{
    assert(v_id < num_orig_vtxs && "vtx id out of range of original points");
    return orig_vertices[v_id].ptr();
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline double TriangleSoup::vertX(const uint &v_id) const
{
    assert(v_id < num_orig_vtxs && "vtx id out of range");
    return orig_vertices[v_id].X();
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline double TriangleSoup::vertY(const uint &v_id) const
{
    assert(v_id < num_orig_vtxs && "vtx id out of range");
    return orig_vertices[v_id].Y();
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline double TriangleSoup::vertZ(const uint &v_id) const
{
    assert(v_id < num_orig_vtxs && "vtx id out of range");
    return orig_vertices[v_id].Z();
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline cinolib::vec3d TriangleSoup::vertCinolib(const uint &v_id) const
{
    assert(v_id < (num_orig_vtxs + impl_vertices.size()) && "vtx id out of range of original points");
    if(v_id < num_orig_vtxs)
        return cinolib::vec3d(vertX(v_id), vertY(v_id), vertZ(v_id));

    double x, y, z;
    impl_vertices[v_id - num_orig_vtxs]->getApproxXYZCoordinates(x, y, z);
    return cinolib::vec3d(x, y, z);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline uint TriangleSoup::addVert(genericPoint* gp)
{
    impl_vertices.push_back(gp);
    return static_cast<uint>(num_orig_vtxs + impl_vertices.size() -1);
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
    assert(t_id < triangles.size() && "t_id out of range");
    assert(v_id < (num_orig_vtxs + impl_vertices.size()) && "vtx id out of range");

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

inline const uint* TriangleSoup::tri(const uint &t_id) const
{
    assert(t_id < triangles.size() && "t_id out of range");
    return triangles[t_id].v;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline uint TriangleSoup::triVertID(const uint &t_id, const uint &off) const
{
    assert(t_id < triangles.size() && "t_id out of range");
    return triangles[t_id].v[off];
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline const genericPoint* TriangleSoup::triVert(const uint &t_id, const uint &off) const
{
    assert(t_id < triangles.size() && "t_id out of range");
    return vert(triangles[t_id].v[off]);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline const double* TriangleSoup::triVertPtr(const uint &t_id, const uint &off) const
{
    assert(t_id < triangles.size() && "t_id out of range");
    return vertPtr(triangles[t_id].v[off]);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline cinolib::vec3d TriangleSoup::triVertCinolib(const uint &t_id, const uint &off) const
{
    assert(t_id < triangles.size() && "t_id out of range");
    return vertCinolib(triangles[t_id].v[off]);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline uint TriangleSoup::triEdgeID(const uint &t_id, const uint &off) const
{
    assert(t_id < triangles.size() && "t_id out of range");
    int e_id = edgeID(triangles[t_id].v[off], triangles[t_id].v[(off +1) %3]);
    assert(e_id >= 0 && "no triangle edge found");
    return static_cast<uint>(e_id);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline Plane TriangleSoup::triPlane(const uint &t_id) const
{
    assert(t_id < triangles.size() && "t_id out of range");
    return triangles[t_id].p;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline bool TriangleSoup::triContainsVert(const uint &t_id, const uint &v_id) const
{
    assert(t_id < triangles.size() && "t_id out of range");
    assert(v_id < (num_orig_vtxs + impl_vertices.size()) && "v_id out of range");

    if(triangles[t_id].v[0] == v_id) return true;
    if(triangles[t_id].v[1] == v_id) return true;
    if(triangles[t_id].v[2] == v_id) return true;
    return false;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline bool TriangleSoup::triContainsEdge(const uint t_id, const uint &ev0_id, const uint &ev1_id) const
{
    return (triContainsVert(t_id, ev0_id) && triContainsVert(t_id, ev1_id));
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline void TriangleSoup::createDoubleVectorOfCoords(std::vector<double> &coords, const double &multiplier)
{
    coords.clear();
    coords.reserve((num_orig_vtxs + impl_vertices.size()) *3);

    for(auto &v : orig_vertices)
    {
        coords.push_back(v.X() / multiplier);
        coords.push_back(v.Y() / multiplier);
        coords.push_back(v.Z() / multiplier);
    }

    for(auto &v : impl_vertices)
    {
        double x, y, z;
        v->getApproxXYZCoordinates(x, y, z);

        coords.push_back(x / multiplier);
        coords.push_back(y / multiplier);
        coords.push_back(z / multiplier);
    }
}

/*******************************************************************************************************
 *      JOLLY POINTS
 * ****************************************************************************************************/

inline const genericPoint* TriangleSoup::jollyPoint(const uint &off) const
{
    assert(off < 4 && "jolly point id out of range");
    return &jolly_points[off];
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline genericPoint* TriangleSoup::jollyPoint(const uint &off)
{
    assert(off < 4 && "jolly point id out of range");
    return &jolly_points[off];
}

/********************************************************************************************************
 *              PRIVATE METHODS
 * ****************************************************************************************************/

inline void TriangleSoup::initJollyPoints()
{
    jolly_points.resize(4);
    jolly_points[0] = explicitPoint3D(0.94280904158, 0.0, -0.333333333);
    jolly_points[1] = explicitPoint3D(-0.47140452079, 0.81649658092, -0.333333333);
    jolly_points[2] = explicitPoint3D(-0.47140452079, -0.81649658092, -0.333333333);
    jolly_points[3] = explicitPoint3D(0.0, 0.0, 1.0);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline Edge TriangleSoup::uniqueEdge(const uint &v0_id, const uint &v1_id) const
{
    if(v0_id < v1_id) return {v0_id, v1_id};
    return {v1_id, v0_id};
}


#endif // TRIANGLE_SOUP_TPP
