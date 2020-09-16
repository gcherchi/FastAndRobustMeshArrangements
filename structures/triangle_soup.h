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

#ifndef TRIANGLESOUP_H
#define TRIANGLESOUP_H

#include "common.h"
#include "cinolib/geometry/vec3.h"
#include "implicit_point.h"

#include <vector>
#include <map>

struct Tri
{
    Tri(const uint &_t0_id, const uint &_t1_id, const uint &_t2_id, const Plane &_p) : p(_p)
    {
        v[0] = _t0_id;
        v[1] = _t1_id;
        v[2] = _t2_id;
    }

    uint v[3];
    Plane p;
};

typedef std::pair<uint, uint> Edge;

typedef struct
{
    double coords[3];
} Vptr;

class TriangleSoup
{
    public:
        TriangleSoup(){}

        TriangleSoup(const std::vector<double> &coords, std::vector<uint> &tris)
        {
            init(coords, tris);
        }

        ~TriangleSoup()
        {
            for(uint v = 0; v < impl_vertices.size(); v++)
                delete impl_vertices[v];
        }

        inline void init(const std::vector<double> &coords, const std::vector<uint> &tris);

        inline uint numVerts() const;
        inline uint numTris() const;
        inline uint numEdges() const;

        inline uint numOrigTriangles() const;

        // VERTICES
        inline const genericPoint* vert(const uint &v_id) const;

        inline const double* vertPtr(const uint &v_id) const;

        inline double vertX(const uint &v_id) const;
        inline double vertY(const uint &v_id) const;
        inline double vertZ(const uint &v_id) const;

        inline cinolib::vec3d vertCinolib(const uint &v_id) const;

        inline uint addVert(genericPoint* gp);

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

#include "triangle_soup.tpp"

#endif // TRIANGLESOUP_H
