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

#ifndef IO_FUNCTIONS_H
#define IO_FUNCTIONS_H

#include <cinolib/vector_serialization.h>

#include <cinolib/io/read_OFF.h>
#include <cinolib/io/read_OBJ.h>
#include <cinolib/io/read_STL.h>

#include <cinolib/io/write_OFF.h>
#include <cinolib/io/write_OBJ.h>
#include <cinolib/io/write_STL.h>

inline void load(const std::string &filename, std::vector<double> &coords, std::vector<uint> &tris)
{
    std::vector<cinolib::vec3d> tmp_verts;

    std::string filetype = filename.substr(filename.size() - 4, 4);

    if (filetype.compare(".off") == 0 || filetype.compare(".OFF") == 0)
    {
        std::vector< std::vector<uint> > tmp_tris;
        cinolib::read_OFF(filename.c_str(), tmp_verts, tmp_tris);
        tris = cinolib::serialized_vids_from_polys(tmp_tris);
    }
    else if (filetype.compare(".obj") == 0 || filetype.compare(".OBJ") == 0)
    {
        std::vector< std::vector<uint> > tmp_tris;
        cinolib::read_OBJ(filename.c_str(), tmp_verts, tmp_tris);
        tris = cinolib::serialized_vids_from_polys(tmp_tris);
    }
    else if (filetype.compare(".stl") == 0 || filetype.compare(".STL") == 0)
    {
        cinolib::read_STL(filename.c_str(), tmp_verts, tris, false);
    }
    else
    {
        std::cerr << "ERROR: file format not supported yet " << std::endl;
    }

    coords = cinolib::serialized_xyz_from_vec3d(tmp_verts);
}

inline void save(const std::string &filename, std::vector<double> &coords, std::vector<uint> &tris)
{
    std::string filetype = filename.substr(filename.size() - 4, 4);

    std::vector< std::vector<uint> > tmp_tris = cinolib::polys_from_serialized_vids(tris, 3);

    if (filetype.compare(".off") == 0 || filetype.compare(".OFF") == 0)
    {
        cinolib::write_OFF(filename.c_str(), coords, tmp_tris);
    }
    else if (filetype.compare(".obj") == 0 || filetype.compare(".OBJ") == 0)
    {
        cinolib::write_OBJ(filename.c_str(), coords, tmp_tris);
    }
    else
    {
        std::cerr << "ERROR: file format not supported yet " << std::endl;
    }
}

#endif // IO_FUNCTIONS_H
