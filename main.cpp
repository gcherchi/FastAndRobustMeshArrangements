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

#include "solve_intersections.h"
#include "cinolib/io/read_STL.h"
#include "cinolib/io/write_OBJ.h"
#include <iostream>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char *argv[])
{
    std::string file_in;
    if (argc == 1)
        file_in = std::string(DATA_PATH) + "two_spheres.stl";
    else
        file_in = argv[1];

    //::::::: Loading file data ::::::::::::::::::::::::::::::::::::
    std::vector<cinolib::vec3d> cinolib_verts;
    std::vector<double> in_coords;
    std::vector<uint> in_tris;

    cinolib::read_STL(file_in.c_str(), cinolib_verts, in_tris, false);

    for(auto &v : cinolib_verts)
    {
        in_coords.push_back(v.x());
        in_coords.push_back(v.y());
        in_coords.push_back(v.z());
    }

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    std::vector<double> out_coords;
    std::vector<uint> out_tris;

    solveIntersections(in_coords, in_tris, out_coords, out_tris);

    //::::::: Save final result :::::::::::::::::::::::::::::::::::
    std::string file_out = file_in.substr(0, file_in.length()-4) + "_OUT.obj";
    cinolib::write_OBJ(file_out.c_str(), out_coords, out_tris, {});

    return 0;
}

