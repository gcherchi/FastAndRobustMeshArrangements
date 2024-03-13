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

#ifdef _MSC_VER // Workaround for known bugs and issues on MSVC
#define _HAS_STD_BYTE 0  // https://developercommunity.visualstudio.com/t/error-c2872-byte-ambiguous-symbol/93889
#define NOMINMAX // https://stackoverflow.com/questions/1825904/error-c2589-on-stdnumeric-limitsdoublemin
#endif

#include <iostream>

#include "solve_intersections.h"
#include "io_functions.h"

#include <cinolib/meshes/meshes.h>
#include <cinolib/gl/glcanvas.h>
#include <cinolib/gl/surface_mesh_controls.h>
#include <iostream>
#include <chrono>
#include <ctime>

using namespace cinolib;


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
bool debug = false;
bool old_version = false;

int main(int argc, char **argv)
{
    std::string filename;

    std::vector<double> in_coords, out_coords;
    std::vector<uint> in_tris, out_tris;
    std::vector<genericPoint*> gen_points;
    point_arena arena;

    if(!debug){
        if(argc > 1) {
            filename = argv[1];
        }
        else
        {
            std::cout << "input file missing" << std::endl;
            return -1;
        }

        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        char buffer[80];

        // Convert time to string
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now_time));

        // Print message with date and time
        std::cout << buffer << " - I'm starting" << std::endl;
        load(filename, in_coords, in_tris);
        // Get current system time



        solveIntersections(in_coords, in_tris, arena, gen_points, out_tris);
        computeApproximateCoordinates(gen_points, out_coords);
        // Convert time to string
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now_time));

        // Print message with date and time
        std::cout << buffer << " -End" << std::endl;


        // Find the position of the last occurrence of '/'
        size_t pos_slash = filename.find_last_of("/");
        // Find the position of the last occurrence of '.'
        size_t pos_dot = filename.find_last_of(".");

        // Extract the filename
        std::string name_out = filename.substr(pos_slash + 1, pos_dot - pos_slash - 1);

        std::string output_name = "./results/output_" + name_out + ".obj";

        save(output_name, out_coords, out_tris);

        return 0;
    }

    /** FUNZIONANTE **/
    filename = "../data/test/ttt3.off";

    load(filename, in_coords, in_tris);

    DrawableTrimesh<> m;
    GLcanvas gui;

    std::queue<uint> vertices;
    std::vector<uint> debug_points;

    /*-------------------------------------------------------------------
     * There are 4 versions of the solveIntersections function. Please
     * refer to the solve_intersections.h file to see how to use them. */

    //solveIntersections(in_coords, in_tris, arena, gen_points, out_tris);

/********************************** solveIntersections function ***************/

    std::vector< std::bitset<NBIT>> tmp_in_labels(in_tris.size() / 3), out_labels;

    //meshArrangementPipeline(in_coords, in_tris, tmp_in_labels, arena, gen_points, out_tris, out_labels);

/******************************** meshArrangementPipeline function ************/
    initFPU();

    AuxiliaryStructure g;

    double multiplier = computeMultiplier(in_coords);

    std::vector<uint> tmp_tris;
    std::vector< std::bitset<NBIT> > tmp_labels;

    mergeDuplicatedVertices(in_coords, in_tris, arena, gen_points, tmp_tris, true);

    removeDegenerateAndDuplicatedTriangles(gen_points, tmp_in_labels, tmp_tris, tmp_labels);

    TriangleSoup ts(arena, gen_points, tmp_tris, tmp_labels, multiplier, true);

    //detectIntersections(ts, g.intersectionList());

    /********************************** detect intersections ********************************************/
    std::vector<cinolib::vec3d> verts(ts.numVerts());

    for(uint v_id = 0; v_id < ts.numVerts(); v_id++)
        verts[v_id] = cinolib::vec3d(ts.vertX(v_id), ts.vertY(v_id), ts.vertZ(v_id));

    g.intersectionList().reserve((int)sqrt(ts.numTris()));
    find_intersections(verts, ts.trisVector(), g.intersectionList());

    g.initFromTriangleSoup(ts);

    classifyIntersections(ts, arena, g);

    //triangulation(ts, arena, g, out_tris, out_labels);


/*********************************** Triangulation function *************/

    out_labels.clear();
    out_tris.clear();
    out_tris.reserve(2 * 3 * ts.numTris());
    out_labels.reserve(2 * ts.numTris());

    std::vector<uint> tris_to_split;
    tris_to_split.reserve(ts.numTris());

    for(uint t_id = 0; t_id < ts.numTris(); t_id++)
    {
        if(g.triangleHasIntersections(t_id) || g.triangleHasCoplanars(t_id))
            tris_to_split.push_back(t_id);
        else
        {
            // triangle without intersections directly goes to the output list
            out_tris.push_back(ts.triVertID(t_id, 0));
            out_tris.push_back(ts.triVertID(t_id, 1));
            out_tris.push_back(ts.triVertID(t_id, 2));
            out_labels.push_back(ts.triLabel(t_id));
        }
    }

    // processing the triangles to split
    tbb::spin_mutex mutex;
    //tbb::parallel_for((uint)0, (uint)tris_to_split.size(), [&](uint t) {
    for (uint t=0; t < (uint)tris_to_split.size(); t++) {
        uint t_id = tris_to_split[t];
        FastTrimesh subm(ts.triVert(t_id, 0),
                         ts.triVert(t_id, 1),
                         ts.triVert(t_id, 2),
                         ts.tri(t_id),
                         ts.triPlane(t_id));

        //triangulateSingleTriangle(ts, arena, subm, t_id, g, out_tris, out_labels, mutex);

        /*****************************TriangulateSingleTriangle*****************************/
        /*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
         *                                  POINTS AND SEGMENTS RECOVERY
         * :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

        const auto& t_points = g.trianglePointsList(t_id);

        int e0_id = ts.edgeID(subm.vertOrigID(0), subm.vertOrigID(1));      assert(e0_id != -1);
        int e1_id = ts.edgeID(subm.vertOrigID(1), subm.vertOrigID(2));      assert(e1_id != -1);
        int e2_id = ts.edgeID(subm.vertOrigID(2), subm.vertOrigID(0));      assert(e2_id != -1);


        auxvector<uint> e0_points_old, e1_points_old, e2_points_old;
        if(old_version) {
            sortedVertexListAlongSegment(ts, g.edgePointsList(static_cast<uint>(e0_id)), subm.vertOrigID(0),
                                         subm.vertOrigID(1), e0_points_old);
            sortedVertexListAlongSegment(ts, g.edgePointsList(static_cast<uint>(e1_id)), subm.vertOrigID(1),
                                         subm.vertOrigID(2), e1_points_old);
            sortedVertexListAlongSegment(ts, g.edgePointsList(static_cast<uint>(e2_id)), subm.vertOrigID(2),
                                         subm.vertOrigID(0), e2_points_old);
        }
        const auxvector<uint> &e0_points = g.edgePointsList(static_cast<uint>(e0_id));
        const auxvector<uint> &e1_points = g.edgePointsList(static_cast<uint>(e1_id));
        const auxvector<uint> &e2_points = g.edgePointsList(static_cast<uint>(e2_id));


        auxvector<UIPair> t_segments(g.triangleSegmentsList(t_id).begin(), g.triangleSegmentsList(t_id).end());

        uint estimated_vert_num = static_cast<uint>(3 + t_points.size() + e0_points.size() + e1_points.size() + e2_points.size());
        subm.preAllocateSpace(estimated_vert_num);

        /*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
         *                                  TRIANGLE SPLIT
         * :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

        //if(t_points.size() < 50)
        if(old_version)
            splitSingleTriangle(ts, subm, t_points);
        //else
        //splitSingleTriangleWithTree(ts, subm, t_points);
        else
        splitSingleTriangleWithStack(ts, subm, t_points, e0_points, e1_points, e2_points);

        /*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
         *                                  EDGE SPLIT
         * :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
        if(old_version) {
            splitSingleEdge(ts, subm, 0, 1, e0_points_old);
            splitSingleEdge(ts, subm, 1, 2, e1_points_old);
            splitSingleEdge(ts, subm, 2, 0, e2_points_old);
        }

        /*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
         *                           CONSTRAINT SEGMENT INSERTION
         * :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
        addConstraintSegmentsInSingleTriangle(ts, arena, subm, g, t_segments, mutex);

        /*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
         *                      POCKETS IN COPLANAR TRIANGLES SOLVING
         * :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/


        if(g.triangleHasCoplanars(t_id))
        {
            { // start critical section...
                std::lock_guard<tbb::spin_mutex> lock(mutex);
                solvePocketsInCoplanarTriangle(subm, g, out_tris, out_labels, ts.triLabel(t_id));
            } // end critical section
        }
        else
        {
            /*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
             *                     NEW TRIANGLE CREATION (for final mesh)
             * :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

            { // start critical section...
                std::lock_guard<tbb::spin_mutex> lock(mutex);
                for(uint ti = 0; ti < subm.numTris(); ti++)
                {
                    const uint *tri = subm.tri(ti);
                    out_tris.push_back(subm.vertOrigID(tri[0]));
                    out_tris.push_back(subm.vertOrigID(tri[1]));
                    out_tris.push_back(subm.vertOrigID(tri[2]));
                    out_labels.push_back(ts.triLabel(t_id));
                } // endl critical section
            }
        }

    }

/******************************************************************************/

    ts.appendJollyPoints();
    computeApproximateCoordinates(gen_points, out_coords);

    //visualization
    m = DrawableTrimesh(out_coords, out_tris);

    //Example marker vert
    for(uint i = 0; i < m.num_edges(); i++){
        m.edge_data(i).flags[MARKED] = false;
    }

    m.updateGL();
    std::string name = get_file_name(filename, false);
    m.mesh_data().filename = name;
    m.updateGL();

    gui.push(&m);
    gui.push(new SurfaceMeshControls<DrawableTrimesh<>>(&m, &gui));

    int ii = 0;
    int state = 1;
    gui.callback_app_controls = [&]()
    {
        if(ImGui::Button("Go ahead edge")) {
            for(uint i = 0; i < m.num_edges(); i++){
                m.edge_data(i).flags[MARKED] = false;
            }
            m.edge_data(ii).flags[MARKED] = true;
            if (ii == m.num_edges() - 1) ii = 0;
            else ii++;
            m.updateGL();

        }

        if(ImGui::Button("show me vert")) {
            for(uint i = 0; i < debug_points.size(); i++){

                    gui.push_marker(m.vert(debug_points[i]),to_string(debug_points[i]), Color::BLUE(), 2,4);

            }
        }// slider moved: do something

    };



    // Find the position of the last occurrence of '/'
    size_t pos_slash = filename.find_last_of("/");
    // Find the position of the last occurrence of '.'
    size_t pos_dot = filename.find_last_of(".");

    // Extract the filename
    std::string name_out = filename.substr(pos_slash + 1, pos_dot - pos_slash - 1);

    std::string output_name = "output_" + name_out + ".obj";

    save(output_name, out_coords, out_tris);

    return gui.launch();
}



