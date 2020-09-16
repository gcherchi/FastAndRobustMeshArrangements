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

#ifndef FASTTRIMESH_H
#define FASTTRIMESH_H

#include "implicit_point.h"
#include "structures/tree.h"
#include "common.h"

#include <vector>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

struct iVtx
{
    iVtx(const genericPoint *_p, const uint &_orig_id) : p(_p), orig_id(_orig_id){}

    const genericPoint* p;
    uint orig_id;
};

struct iEdge
{
    iEdge(const uint &_v0, const uint &_v1, const bool _b) : v(_v0, _v1), constr(_b){}

    std::pair<uint, uint> v;
    bool constr = false;
};

struct iTri
{
    iTri(const uint &_v0, const uint &_v1, const uint &_v2, const uint &_n) : node_id(_n)
    {
        v[0] = _v0;
        v[1] = _v1;
        v[2] = _v2;
    }

    uint v[3];
    uint node_id; // used only if the triangulation uses the Tree
};

class FastTrimesh
{
    public:
        FastTrimesh(){}

        FastTrimesh(const genericPoint* tv0, const genericPoint* tv1, const genericPoint *tv2,
                    const uint *tv_id, const Plane &ref_p)
        {
            addVert(tv0, tv_id[0]);
            addVert(tv1, tv_id[1]);
            addVert(tv2, tv_id[2]);
            addTri(0, 1, 2);

            triangle_plane = ref_p;
        }

        inline void preAllocateSpace(const uint &estimated_num_verts);

        inline uint numVerts() const;
        inline uint numEdges() const;
        inline uint numTris() const;
        inline Plane refPlane() const;

        // VERTICES
        inline const genericPoint* vert(const uint &v_id) const;

        inline uint vertOrigID(const uint &new_v_id) const; // from new id to original mesh id

        inline uint vertNewID(const uint &orig_v_id) const; // from original mesh id to new id

        inline uint vertValence(const uint &v_id) const;

        inline const std::vector<uint> &adjV2E(const uint &v_id) const;

        inline std::set<uint> adjV2T(const uint &v_id) const;

        // EDGES
        inline const std::pair<uint, uint> &edge(const uint &e_id) const;

        inline uint edgeVertID(const uint &e_id, const uint &off) const;

        inline int edgeID(const uint &ev0_id, const uint &ev1_id) const;

        inline bool edgeIsConstr(const uint &e_id) const;

        inline void setEdgeConstr(const uint &e_id);

        inline uint edgeOppToVert(const uint &t_id, const uint &v_id) const;

        inline bool edgeIsBoundary(const uint &e_id) const;

        inline const std::vector<uint> &adjE2T(const uint &e_id) const;

        // TRIANGLES
        inline const uint *tri(const uint &t_id) const;

        inline int triID(const uint &tv0_id, const uint &tv1_id, const uint &tv2_id) const;

        inline uint triVertID(const uint &t_id, const uint &off) const;

        inline const genericPoint *triVert(const uint &t_id, const uint &off) const;

        inline int triEdgeID(const uint &t_id, const uint &off) const;

        inline uint triNodeID(const uint &t_id) const;

        inline void setTriNodeID(const uint &t_id, const uint &n_id);

        inline uint triVertOppositeTo(const uint &t_id, const uint &v0_id, const uint &v1_id) const;

        inline int triOppToEdge(const uint &e_id, const uint &t_id) const;

        inline std::vector<uint> adjT2E(const uint &t_id) const;

        inline bool triVertsAreCCW(const uint &t_id, const uint &curr_v_id, const uint &prev_v_id) const;

        inline int triOrientation(const uint &t_id) const;

        inline bool triContainsVert(const uint &t_id, const uint &v_id) const;

        inline uint triVertOffset(const uint &t_id, const uint &v_id) const;


        // MESH MANIPULATION
        inline uint addVert(const genericPoint *v, const uint &orig_v_id);

        inline uint addTri(const uint &tv0_id, const uint &tv1_id, const uint &tv2_id);

        inline void removeEdge(const uint &e_id);

        inline void removeTri(const uint &t_id);

        inline void removeTris(const std::vector<uint> &t_ids);

        inline void splitEdge(const uint  &e_id, const uint &v_id);

        inline void splitEdge(const uint  &e_id, const uint &v_id, Tree &tree);

        inline void splitTri(const uint &t_id, const uint &v_id);

        inline void splitTri(const uint &t_id, const uint &v_id, Tree &tree);


    private:
        std::vector<iVtx>    vertices;
        std::vector<iEdge>   edges;
        std::vector<iTri>    triangles;

        std::vector< std::vector<uint> >    v2e;
        std::vector< std::vector<uint> >    e2t;

        std::unordered_map<uint, uint> rev_vtx_map;

        Plane triangle_plane;

        // PRIVATE METHODS
        inline int addEdge(const uint &ev0_id, const uint &ev1_id);

        inline bool edgeContainsVert(const uint &e_id, const uint &v_id) const;

        inline bool trisAreEqual(const uint t0[], const uint t1[]) const;

        inline void removeFromVec(std::vector<uint> &vec, const uint &elem);

        inline void triSwitch(const uint &t0_id, const uint &t1_id);

        inline void edgeSwitch(const uint &e0_id, const uint e1_id);

        inline void removeEdgeUnref(const uint &e_id);

        inline void removeTriUnref(const uint &t_id);

};

#include "fast_trimesh.tpp"

#endif // FASTTRIMESH_H


