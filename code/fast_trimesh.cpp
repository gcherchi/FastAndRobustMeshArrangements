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

#include "fast_trimesh.h"

#include <unordered_set>

inline void FastTrimesh::preAllocateSpace(const uint &estimated_num_verts)
{
    vertices.reserve(estimated_num_verts);
    rev_vtx_map.reserve(estimated_num_verts);
    edges.reserve(estimated_num_verts / 2);
    triangles.reserve(estimated_num_verts / 3);
    v2e.reserve(estimated_num_verts);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline uint FastTrimesh::numVerts() const
{
    return static_cast<uint>(vertices.size());
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline uint FastTrimesh::numEdges() const
{
    return static_cast<uint>(edges.size());
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline uint FastTrimesh::numTris() const
{
    return static_cast<uint>(triangles.size());
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline Plane FastTrimesh::refPlane() const
{
    return triangle_plane;
}

/************************************************************************************************
 *          VERTICES
 * *********************************************************************************************/

inline const genericPoint* FastTrimesh::vert(const uint &v_id) const
{
    assert(v_id < vertices.size() && "vtx id out of range");
    return vertices[v_id].p;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline uint FastTrimesh::vertOrigID(const uint &new_v_id) const
{
    assert(new_v_id < vertices.size() && "vtx id out of range");
    return vertices[new_v_id].orig_id;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline  uint FastTrimesh::vertNewID(const uint &orig_v_id) const
{
    auto it = rev_vtx_map.find(orig_v_id);
    assert(it != rev_vtx_map.end() && "vtx id not found in reverse map");

    return it->second;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline uint FastTrimesh::vertValence(const uint &v_id) const
{
    assert(v_id < vertices.size() && "vtx id out of range");
    return static_cast<uint>(v2e[v_id].size());
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline const std::vector<uint> &FastTrimesh::adjV2E(const uint &v_id) const
{
    assert(v_id < vertices.size() && "vtx id out of range");
    return v2e[v_id];
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline std::set<uint> FastTrimesh::adjV2T(const uint &v_id) const
{
    assert(v_id < vertices.size() && "vtx id out of range");
    std::set<uint> v2t;

    for(const uint &e_id : v2e[v_id])
    {
        for(const uint &t_id : e2t[e_id])
            v2t.insert(t_id);
    }

    return v2t;
}

/************************************************************************************************
 *          EDGES
 * *********************************************************************************************/

inline const std::pair<uint, uint> &FastTrimesh::edge(const uint &e_id) const
{
    assert(e_id < edges.size() && "edge id out of range");
    return edges[e_id].v;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline uint FastTrimesh::edgeVertID(const uint &e_id, const uint &off) const
{
    assert(e_id < edges.size() && "edge id out of range");
    if(off == 0) return edges[e_id].v.first;
    return edges[e_id].v.second;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline int FastTrimesh::edgeID(const uint &ev0_id, const uint &ev1_id) const
{
    assert(ev0_id != ev1_id && "edge with equal endpoints");
    assert((ev0_id < vertices.size() && ev1_id < vertices.size()) && "vtx id out of range");

    for(const uint &e_id : v2e[ev0_id])
    {
        if(edgeContainsVert(e_id, ev0_id) && edgeContainsVert(e_id, ev1_id))
            return static_cast<int>(e_id);
    }

    return -1;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline bool FastTrimesh::edgeIsConstr(const uint &e_id) const
{
    assert(e_id < edges.size() && "edge id out of range");
    return edges[e_id].constr;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline void FastTrimesh::setEdgeConstr(const uint &e_id)
{
    assert(e_id < edges.size() && "edge id out of range");
    edges[e_id].constr = true;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline uint FastTrimesh::edgeOppToVert(const uint &t_id, const uint &v_id) const
{
    assert(t_id < triangles.size() && "tri id out of range");
    assert(triContainsVert(t_id, v_id) && "tri doesn't contain vtx");

    int e_id = -1;
    if(triangles[t_id].v[0] == v_id) e_id = edgeID(triangles[t_id].v[1], triangles[t_id].v[2]); else
    if(triangles[t_id].v[1] == v_id) e_id = edgeID(triangles[t_id].v[0], triangles[t_id].v[2]); else
    if(triangles[t_id].v[2] == v_id) e_id = edgeID(triangles[t_id].v[0], triangles[t_id].v[1]);

    assert(e_id != -1 && "opposite edge not found in tri");
    return static_cast<uint>(e_id);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline bool FastTrimesh::edgeIsBoundary(const uint &e_id) const
{
    assert(e_id < edges.size() && "edge id out of range");
    return e2t[e_id].size() == 1;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline const std::vector<uint> &FastTrimesh::adjE2T(const uint &e_id) const
{
    assert(e_id < edges.size() && "edge id out of range");
    assert(e2t[e_id].size() <= 2 && "edge-triangle adj > 2 in manifold mesh");
    return e2t[e_id];
}

/************************************************************************************************
 *          TRIANGLES
 * *********************************************************************************************/

inline const uint *FastTrimesh::tri(const uint &t_id) const
{
    assert(t_id < triangles.size() && "tri id out of range");
    return triangles[t_id].v;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline int FastTrimesh::triID(const uint &tv0_id, const uint &tv1_id, const uint &tv2_id) const
{
    assert((tv0_id < vertices.size() && tv1_id < vertices.size() && tv2_id < vertices.size()) && "vtx id out of range");

    int e_id = edgeID(tv0_id, tv1_id);
    if(e_id == -1) return -1;

    for(const uint &t_id : e2t[static_cast<uint>(e_id)])
        if(triContainsVert(t_id, tv2_id))
            return static_cast<int>(t_id);

    return -1;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline uint FastTrimesh::triVertID(const uint &t_id, const uint &off) const
{
    assert(t_id < triangles.size() && "tri id out of range");
    return triangles[t_id].v[off];
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline const genericPoint *FastTrimesh::triVert(const uint &t_id, const uint &off) const
{
    assert(t_id < triangles.size() && "tri id out of range");
    return vertices[triangles[t_id].v[off]].p;
}


//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline int FastTrimesh::triEdgeID(const uint &t_id, const uint &off) const
{
    assert(t_id < triangles.size() && "tri id out of range");
    return edgeID(triVertID(t_id, off), triVertID(t_id, ((off +1) %3)));
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline uint FastTrimesh::triNodeID(const uint &t_id) const
{
    assert(t_id < triangles.size() && "tri id out of range");
    return triangles[t_id].node_id;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline void FastTrimesh::setTriNodeID(const uint &t_id, const uint &n_id)
{
    assert(t_id < triangles.size() && "tri id out of range");
    triangles[t_id].node_id = n_id;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline uint FastTrimesh::triVertOppositeTo(const uint &t_id, const uint &v0_id, const uint &v1_id) const
{
    assert(t_id < triangles.size() && "tri id out of range");
    assert(v0_id != v1_id && "verts are equal");
    assert((triContainsVert(t_id, v0_id) && triContainsVert(t_id, v0_id)) && "tri dosn't contain vtx");

    for(uint off = 0; off < 3; off++)
    {
        uint v_id = triangles[t_id].v[off];
        if(v_id != v0_id && v_id != v1_id) return v_id;
    }

    assert(false && "This should not happen");
    return 0; // warning killer
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline int FastTrimesh::triOppToEdge(const uint &e_id, const uint &t_id) const
{
    assert(e_id < edges.size() && "edge id out of range");
    assert(t_id < triangles.size() && "tri id out of range");
    assert(e2t[e_id].size() <= 2 && "no opposite tri found");

    if(e2t[e_id].size() == 1) return -1; // boundary edge

    if(e2t[e_id][0] == t_id)
        return static_cast<int>(e2t[e_id][1]);
    else if(e2t[e_id][1] == t_id)
        return static_cast<int>(e2t[e_id][0]);

    assert(false && "no opposite tri found");
    return -1; // warning killer
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline std::vector<uint> FastTrimesh::adjT2E(const uint &t_id) const
{
    assert(t_id < triangles.size() && "tri id out of range");

    return {static_cast<uint>(triEdgeID(t_id, 0)),
            static_cast<uint>(triEdgeID(t_id, 1)),
            static_cast<uint>(triEdgeID(t_id, 2))};
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline bool FastTrimesh::triVertsAreCCW(const uint &t_id, const uint &curr_v_id, const uint &prev_v_id) const
{
    uint prev_off = triVertOffset(t_id, prev_v_id);
    uint curr_off = triVertOffset(t_id, curr_v_id);
    if(curr_off == ((prev_off +1) %3)) return true;
    return false;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline int FastTrimesh::triOrientation(const uint &t_id) const
{
    assert(t_id < triangles.size() && "tri id out of range");
    switch (triangle_plane)
    {
        case XY: return genericPoint::orient2Dxy(*triVert(t_id, 0), *triVert(t_id, 1), *triVert(t_id, 2));
        case YZ: return genericPoint::orient2Dyz(*triVert(t_id, 0), *triVert(t_id, 1), *triVert(t_id, 2));
        case ZX: return genericPoint::orient2Dzx(*triVert(t_id, 0), *triVert(t_id, 1), *triVert(t_id, 2));
    }
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline bool FastTrimesh::triContainsVert(const uint &t_id, const uint &v_id) const
{
    assert(t_id < triangles.size() && "tri id out of range");

    if(triangles[t_id].v[0] == v_id) return true;
    if(triangles[t_id].v[1] == v_id) return true;
    if(triangles[t_id].v[2] == v_id) return true;
    return false;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline uint FastTrimesh::triVertOffset(const uint &t_id, const uint &v_id) const
{
    for(uint off = 0; off < 3; off++)
        if(triangles[t_id].v[off] == v_id) return off;

    assert(false && "This should not happen");
    return 0; // warning killer
}


/************************************************************************************************
 *          MESH MANIPULATION
 * **********************************************************************************************/

inline uint FastTrimesh::addVert(const genericPoint *v, const uint &orig_v_id)
{
    uint v_id = static_cast<uint>(vertices.size());
    vertices.emplace_back(v, orig_v_id);

    v2e.push_back(std::vector<uint>());
    rev_vtx_map[orig_v_id] = v_id;

    return v_id;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline uint FastTrimesh::addTri(const uint &tv0_id, const uint &tv1_id, const uint &tv2_id)
{
    assert((tv0_id < vertices.size() && tv1_id < vertices.size() && tv2_id < vertices.size()) && "vtx id out of range");
    assert((tv0_id != tv1_id && tv0_id != tv2_id && tv1_id != tv2_id) && "degenerate triangle");

    int t_id = triID(tv0_id, tv1_id, tv2_id);
    if(t_id != -1) return static_cast<uint>(t_id);

    t_id = static_cast<int>(triangles.size());

    triangles.emplace_back(tv0_id, tv1_id, tv2_id, 0);

    // adding missing edges
    int e0_id = addEdge(tv0_id, tv1_id);
    int e1_id = addEdge(tv1_id, tv2_id);
    int e2_id = addEdge(tv2_id, tv0_id);

    e2t[static_cast<uint>(e0_id)].push_back(static_cast<uint>(t_id));
    e2t[static_cast<uint>(e1_id)].push_back(static_cast<uint>(t_id));
    e2t[static_cast<uint>(e2_id)].push_back(static_cast<uint>(t_id));

    return static_cast<uint>(t_id);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline void FastTrimesh::removeEdge(const uint &e_id)
{
    assert(e_id < edges.size() && "edge id out of range");
    removeTris(e2t[e_id]);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline void FastTrimesh::removeTri(const uint &t_id)
{
    assert(t_id < triangles.size() && "tri id out of range");

    std::set< uint, std::greater<uint> > dangling_edges; // higher ids first

    uint e0_id = static_cast<uint>(triEdgeID(t_id, 0));
    uint e1_id = static_cast<uint>(triEdgeID(t_id, 1));
    uint e2_id = static_cast<uint>(triEdgeID(t_id, 2));
    removeFromVec(e2t[e0_id], t_id);
    removeFromVec(e2t[e1_id], t_id);
    removeFromVec(e2t[e2_id], t_id);

    if(e2t[e0_id].empty()) dangling_edges.insert(e0_id);
    if(e2t[e1_id].empty()) dangling_edges.insert(e1_id);
    if(e2t[e2_id].empty()) dangling_edges.insert(e2_id);

    for(const uint &e_id : dangling_edges)
    {
        uint v0_id = edges[e_id].v.first;
        uint v1_id = edges[e_id].v.second;
        if (v1_id > v0_id) std::swap(v0_id, v1_id);
        removeFromVec(v2e[v0_id], e_id);
        removeFromVec(v2e[v1_id], e_id);

        removeEdgeUnref(e_id);
    }

    removeTriUnref(t_id);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline void FastTrimesh::removeTris(const std::vector<uint> &t_ids)
{
    std::vector<uint> tmp_t_ids = t_ids;
    std::sort(tmp_t_ids.rbegin(), tmp_t_ids.rend());

    for(uint &t_id : tmp_t_ids) removeTri(t_id);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline void FastTrimesh::splitEdge(const uint  &e_id, const uint &v_id)
{
    assert(e_id < edges.size() && "edge id out of range");

    uint ev0_id = edges[e_id].v.first;
    uint ev1_id = edges[e_id].v.second;

    for(const uint &t_id : e2t[e_id])
    {
        uint v_opp = triVertOppositeTo(t_id, ev0_id, ev1_id);
        if(triVertsAreCCW(t_id, ev0_id, ev1_id)) std::swap(ev0_id, ev1_id);

        addTri(v_opp, ev0_id, v_id);
        addTri(v_opp, v_id, ev1_id);
    }

    removeTris(e2t[e_id]);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline void FastTrimesh::splitEdge(const uint  &e_id, const uint &v_id, Tree &tree)
{
    assert(e_id < edges.size() && "edge id out of range");

    uint ev0_id = edges[e_id].v.first;
    uint ev1_id = edges[e_id].v.second;

    for(const uint &t_id : e2t[e_id])
    {
        uint v_opp = triVertOppositeTo(t_id, ev0_id, ev1_id);
        if(triVertsAreCCW(t_id, ev0_id, ev1_id)) std::swap(ev0_id, ev1_id);

        uint t0_id = addTri(v_opp, ev0_id, v_id);
        uint t1_id = addTri(v_opp, v_id, ev1_id);

        uint n0_id = tree.addNode(v_opp, ev0_id, v_id);
        uint n1_id = tree.addNode(v_opp, v_id, ev1_id);

        uint node_id = triNodeID(t_id);
        tree.addChildren(node_id, n0_id, n1_id);

        setTriNodeID(t0_id, n0_id);
        setTriNodeID(t1_id, n1_id);
    }

    removeTris(e2t[e_id]);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline void FastTrimesh::splitTri(const uint &t_id, const uint &v_id)
{
    assert(t_id < triangles.size() && "tri id out of range");
    assert(v_id < vertices.size() && "vtx id out of range");

    addTri(triVertID(t_id, 0), triVertID(t_id, 1), v_id);
    addTri(triVertID(t_id, 1), triVertID(t_id, 2), v_id);
    addTri(triVertID(t_id, 2), triVertID(t_id, 0), v_id);

    removeTri(t_id);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline void FastTrimesh::splitTri(const uint &t_id, const uint &v_id, Tree &tree)
{
    assert(t_id < triangles.size() && "tri id out of range");
    assert(v_id < vertices.size() && "vtx id out of range");

    uint node_id = triNodeID(t_id);

    uint t0_id = addTri(triVertID(t_id, 0), triVertID(t_id, 1), v_id);
    uint t1_id = addTri(triVertID(t_id, 1), triVertID(t_id, 2), v_id);
    uint t2_id = addTri(triVertID(t_id, 2), triVertID(t_id, 0), v_id);

    uint n0_id = tree.addNode(triVertID(t_id, 0), triVertID(t_id, 1), v_id);
    uint n1_id = tree.addNode(triVertID(t_id, 1), triVertID(t_id, 2), v_id);
    uint n2_id = tree.addNode(triVertID(t_id, 2), triVertID(t_id, 0), v_id);
    tree.addChildren(node_id, n0_id, n1_id, n2_id);

    // the triangle label contains the node position
    setTriNodeID(t0_id, n0_id);
    setTriNodeID(t1_id, n1_id);
    setTriNodeID(t2_id, n2_id);

    removeTri(t_id);
}

/***********************************************************************************************
 *          PRIVATE METHODS
 * ********************************************************************************************/

inline int FastTrimesh::addEdge(const uint &ev0_id, const uint &ev1_id)
{
    int e_id = edgeID(ev0_id, ev1_id);
    if(e_id != -1) return e_id;

    e_id = static_cast<int>(edges.size());

    edges.emplace_back(ev0_id, ev1_id, false);

    e2t.push_back(std::vector<uint>());
    v2e[ev0_id].push_back(static_cast<uint>(e_id));
    v2e[ev1_id].push_back(static_cast<uint>(e_id));

    return e_id;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline bool FastTrimesh::edgeContainsVert(const uint &e_id, const uint &v_id) const
{
    if(edges[e_id].v.first == v_id) return true;
    if(edges[e_id].v.second == v_id) return true;
    return false;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline bool FastTrimesh::trisAreEqual(const uint t0[], const uint t1[]) const
{
    uint off;
    if(t0[0] == t1[0]) off = 0;
    else if(t0[0] == t1[1]) off = 1;
    else if(t0[0] == t1[2]) off = 2;
    else return false;

    if(t0[1] == t1[(off +1) %3] &&
       t0[2] == t1[(off +2) %3]) return true;
    return false;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline void FastTrimesh::removeFromVec(std::vector<uint> &vec, const uint &elem)
{
    vec.erase(std::remove(vec.begin(), vec.end(), elem), vec.end());
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline void FastTrimesh::triSwitch(const uint &t0_id, const uint &t1_id)
{
    if(t0_id == t1_id) return;

    std::swap(triangles[t0_id], triangles[t1_id]);

    std::unordered_set<int> edges_to_update;
    edges_to_update.insert(triEdgeID(t0_id, 0));
    edges_to_update.insert(triEdgeID(t0_id, 1));
    edges_to_update.insert(triEdgeID(t0_id, 2));
    edges_to_update.insert(triEdgeID(t1_id, 0));
    edges_to_update.insert(triEdgeID(t1_id, 1));
    edges_to_update.insert(triEdgeID(t1_id, 2));

    for(const int &e_id : edges_to_update)
    {
        if(e_id == -1) continue; // the triangle t_id could contain an already removed edge

        for(uint &t_id : e2t[static_cast<uint>(e_id)])
        {
            if(t_id == t0_id)  t_id = t1_id; else
            if(t_id == t1_id)  t_id = t0_id;
        }
    }
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline void FastTrimesh::edgeSwitch(const uint &e0_id, const uint e1_id)
{
    if(e0_id == e1_id) return;

    std::swap(edges[e0_id], edges[e1_id]);
    std::swap(e2t[e0_id], e2t[e1_id]);

    std::unordered_set<uint> verts_to_update;
    verts_to_update.insert(edges[e0_id].v.first);
    verts_to_update.insert(edges[e0_id].v.second);
    verts_to_update.insert(edges[e1_id].v.first);
    verts_to_update.insert(edges[e1_id].v.second);

    for(const uint &v_id : verts_to_update)
    {
        for(uint &e_id : v2e[v_id])
        {
            if(e_id == e0_id)  e_id = e1_id; else
            if(e_id == e1_id)  e_id = e0_id;
        }
    }
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline void FastTrimesh::removeEdgeUnref(const uint &e_id)
{
    e2t[e_id].clear();
    edgeSwitch(e_id, static_cast<uint>(edges.size() -1));
    edges.pop_back();
    e2t.pop_back();
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline void FastTrimesh::removeTriUnref(const uint &t_id)
{
    triSwitch(t_id, static_cast<uint>(triangles.size() -1));
    triangles.pop_back();
}

