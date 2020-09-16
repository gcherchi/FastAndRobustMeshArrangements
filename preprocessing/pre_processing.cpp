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

#include "pre_processing.h"

void fillTMeshStructure(T_MESH::nonManifoldPLC &tm, const std::vector<double> &coords, const std::vector<uint> &tris, const double &multiplier)
{
    uint num_verts = static_cast<uint>(coords.size() / 3);

    for(uint v_id = 0; v_id < num_verts; v_id++)
        tm.V.push_back(new T_MESH::nmVertex(coords[3 * v_id] * multiplier, coords[(3 * v_id) +1] * multiplier, coords[(3 * v_id) +2] * multiplier));

    for(unsigned int t_id = 0; t_id < tris.size(); t_id += 3)
    {
        if(*tm.V[tris[t_id]] != *tm.V[tris[t_id+1]] &&
           *tm.V[tris[t_id+1]] != *tm.V[tris[t_id+2]] &&
           *tm.V[tris[t_id]] != *tm.V[tris[t_id+2]])
                tm.CreateTriangle(tm.V[tris[t_id]], tm.V[tris[t_id+1]], tm.V[tris[t_id+2]]);
    }

    tm.mergeEdges();
    tm.mergeTriangles();

}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void detectIntersectionsBSP(T_MESH::nonManifoldPLC &tm, std::vector< std::vector<uint> > &intersection_list)
{
    tm.assignIntersectingTriangles();

    intersection_list.resize(tm.T.size());

    uint32_t i = 0;
    for (T_MESH::nmTriangle *t : tm.T)
    {
        std::vector<uint32_t> *its = static_cast<std::vector<uint32_t> *>(t->info);

        if (!its->empty())
        {
            intersection_list[i].reserve(its->size());

            for (uint32_t j : *its)
                intersection_list[i].push_back(static_cast<uint>(j));
        }

        i++;
        delete its;
        t->info = nullptr;
    }
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void detectIntersectionsOctree(TriangleSoup &ts, std::vector<std::vector<uint> > &intersection_list)
{
    cinolib::Octree octree;
    std::vector< std::vector<cinolib::vec3d> > tmp_tris(ts.numTris());
    intersection_list.resize(ts.numTris());

    for(uint t_id = 0; t_id < ts.numTris(); t_id++)
    {
        tmp_tris[t_id] = {ts.triVertCinolib(t_id, 0), ts.triVertCinolib(t_id, 1), ts.triVertCinolib(t_id, 2)};
        octree.add_triangle(t_id, tmp_tris[t_id]);
    }

    octree.build();

    for(uint t_id = 0; t_id < ts.numTris(); t_id++)
    {
        std::unordered_set<uint> list;

        bool b = octree.intersects_triangle(tmp_tris[t_id].data(), true, list);

        if(b) intersection_list[t_id] = std::vector<uint>(list.begin(), list.end());
    }
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void convertTMeshToTriangleSoup(T_MESH::nonManifoldPLC &tm, TriangleSoup &ts)
{
    std::vector<double> coords;
    coords.reserve(tm.V.size() *3);
    std::vector<uint> tris(tm.T.size() *3);
    uint32_t n;
    T_MESH::nmVertex *v;
    T_MESH::nmTriangle *t;

    // Vertices
    for (n = 0, v = tm.V[n]; n < tm.V.size(); n++, v = tm.V[n])
    {
        coords.push_back(v->x);
        coords.push_back(v->y);
        coords.push_back(v->z);
        v->info = (void *)((int)n);
    }

    // Triangles
    for (n = 0, t = tm.T[n]; n < tm.T.size(); n++, t = tm.T[n])
    {
        uint off = n *3;
        tris[off] = static_cast<uint>((intptr_t)t->v1()->info);
        tris[off +1] = static_cast<uint>((intptr_t)t->v2()->info);
        tris[off +2] = static_cast<uint>((intptr_t)t->v3()->info);
    }

    ts = TriangleSoup(coords, tris);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

