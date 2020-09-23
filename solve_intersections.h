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

#ifndef SOLVE_INTERSECTIONS_H
#define SOLVE_INTERSECTIONS_H

#include "preprocessing/pre_processing.h"
#include "structures/aux_structure.h"
#include "structures/triangle_soup.h"
#include "intersections_detection/intersection_classification.h"
#include "triangulation/triangulation.h"

#include <chrono>

inline void report_elapsed(const char *what, std::chrono::system_clock::time_point& chrono_start, std::chrono::system_clock::time_point& end)
{
    end = std::chrono::system_clock::now();
    std::cout << what << ": " << (end - chrono_start).count() / 10000000.0 << " seconds\n";
    chrono_start = end;
}

void solveIntersections(const std::vector<double>& in_coords, const std::vector< unsigned int>& in_tris,
    std::vector<double>& out_coords, std::vector< unsigned int>& out_tris, bool verbose =false)
{
    T_MESH::TMesh::init();
    initFPU();
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // make sure "." is the decimal separator

    T_MESH::nonManifoldPLC tm;
    TriangleSoup ts;
    AuxiliaryStructure g;
    const double multiplier = 67108864.0;

    if (verbose)
    {
        std::chrono::system_clock::time_point chrono_start, start, end = std::chrono::system_clock::now();
        chrono_start = start = end;

        fillTMeshStructure(tm, in_coords, in_tris, multiplier);
        report_elapsed("fillTMeshStructure", chrono_start, end);

        detectIntersectionsBSP(tm, g.intersectionList());
        report_elapsed("detectIntersectionsBSP", chrono_start, end);

        convertTMeshToTriangleSoup(tm, ts);
        tm.clear();
        report_elapsed("convertTMeshToTriangleSoup", chrono_start, end);

        g.initFromTriangleSoup(ts);
        report_elapsed("initFromTriangleSoup", chrono_start, end);

        classifyIntersections(ts, g);
        report_elapsed("classifyIntersections", chrono_start, end);

        triangulation(ts, g, out_tris);
        report_elapsed("triangulation", chrono_start, end);

        ts.createDoubleVectorOfCoords(out_coords, multiplier);
        report_elapsed("createDoubleVectorOfCoords", chrono_start, end);

        report_elapsed("TOTAL elapsed", start, end);
    }
    else
    {
        fillTMeshStructure(tm, in_coords, in_tris, multiplier);
        detectIntersectionsBSP(tm, g.intersectionList());
        convertTMeshToTriangleSoup(tm, ts);
        tm.clear();
        g.initFromTriangleSoup(ts);
        classifyIntersections(ts, g);
        triangulation(ts, g, out_tris);
        ts.createDoubleVectorOfCoords(out_coords, multiplier);
    }

}

#endif // SOLVE_INTERSECTIONS_H
