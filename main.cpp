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
#undef NDEBUG
#include "solve_intersections.h"
#include "io_functions.h"

#include <cinolib/meshes/meshes.h>
#include <cinolib/gl/glcanvas.h>
#include <cinolib/gl/surface_mesh_controls.h>

#include <iostream>
#include <chrono>
#include <cinolib/profiler.h>

using namespace cinolib;


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
bool debug = false;
bool parallel = true;

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

        load(filename, in_coords, in_tris);
        solveIntersections(in_coords, in_tris, arena, gen_points, out_tris, parallel);
        computeApproximateCoordinates(gen_points, out_coords);

        size_t pos_slash = filename.find_last_of("/");
        size_t pos_dot = filename.find_last_of(".");

        std::string name_out = filename.substr(pos_slash + 1, pos_dot - pos_slash - 1);

        std::string output_name = "./results/output_" + name_out + ".obj";

        save(output_name, out_coords, out_tris);
        return 0;
    }

    /** FUNZIONANTE **/
    filename = "../data/test/test_point_e3.obj";
    //filename = "../Thingi10K/z_996816.off";



    genericPoint *A = new explicitPoint3D(0.0,0.0,0.0);
    genericPoint *B = new explicitPoint3D(1.0,0.0,0.0);
    genericPoint *C = new explicitPoint3D(0.5,1.0,0.0);
    genericPoint *POS = new explicitPoint3D(0.5,0.0,0.0);
    genericPoint *H = new explicitPoint3D(-1.0,2.0,7.0);

    genericPoint *QT0 = new explicitPoint3D(0.25,0.5,0.0);


    genericPoint *QT1 = new explicitPoint3D(0.65,0.5,0.0);
    genericPoint *QT2 = new explicitPoint3D(0.35,0.5,0.0);

    int T0_e2 = genericPoint::orient3D(*POS, *C, *A, *H);
    std::cout<<"T0_e2 = "<< T0_e2 << std::endl;

    int T0_e0 = genericPoint::orient3D(*QT1, *POS, *C, *H );
    int T0_e1 = genericPoint::orient3D(*QT2, *POS, *C, *H );

    int T1_e0 = genericPoint::orient3D(*QT1, *POS, *B, *H);
    int T1_e1 = genericPoint::orient3D(*QT2, *POS, *C, *H);


    std::cout<<"T0_e0 = "<< T0_e0 << " T0_e1: "<< T0_e1 << std::endl;
    std::cout<<"T1_e0 = "<< T0_e0*-1 << " T1_e1: "<< T1_e1 << std::endl;

    //take the time of the for loop

    Profiler p;

    p.push("Test");
    for(int i = 0; i<90000000; i++){
        int a = genericPoint::orient3D(*A, *B, *C, *H);
    }
    p.pop();
    //auto a = out_coords[-1000];
    //std:cout<<a<<std::endl;

    p.push("Test2");
    for(int i = 0; i<30000000; i++){
        int b = genericPoint::orient3D(*A, *B, *C, *H);
    }
    auto time = p.pop();

    load(filename, in_coords, in_tris);

    DrawableTrimesh<> m;
    GLcanvas gui;

    std::queue<uint> vertices;
    std::vector<uint> debug_points;

    /*-------------------------------------------------------------------
     * There are 4 versions of the solveIntersections function. Please
     * refer to the solve_intersections.h file to see how to use them. */

    solveIntersections(in_coords, in_tris, arena, gen_points, out_tris, parallel);
    computeApproximateCoordinates(gen_points, out_coords);



    /**----------------Visualization part-----------------**/
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



