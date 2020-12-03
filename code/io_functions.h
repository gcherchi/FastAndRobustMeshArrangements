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
