#include "pre_processing.h"

#include "cinolib/find_intersections.h"

void mergeDuplicatedVertices(const std::vector<double> &in_coords, const std::vector<uint> &in_tris, const double &multiplier,
                             std::vector<double> &out_coords, std::vector<uint> &out_tris)
{
    out_coords.reserve(in_coords.size());
    out_tris.reserve(in_tris.size());

    std::map<std::vector<double>, uint> v_map;

    for(const uint &v_id : in_tris)
    {
        std::vector<double> v = {in_coords[(3 * v_id)] * multiplier,
                                 in_coords[(3 * v_id) +1] * multiplier,
                                 in_coords[(3 * v_id) +2] * multiplier};

        auto ins = v_map.insert({v, v_map.size()});

        if(ins.second) // new vtx added
        {
            out_coords.push_back(v[0]);
            out_coords.push_back(v[1]);
            out_coords.push_back(v[2]);
        }

        out_tris.push_back(ins.first->second);
    }
}


//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void removeDegenerateAndDuplicatedTriangles(const std::vector<double> &in_coords, std::vector<uint> &tris, std::vector< std::bitset<NBIT> > &labels)
{
    uint num_orig_tris = static_cast<uint>(tris.size() / 3);
    uint t_off = 0;
    uint l_off = 0;

    std::map< std::vector<uint>, uint> tris_map;

    for(uint t_id = 0; t_id < num_orig_tris; t_id++)
    {
        uint v0_id = tris[(3 * t_id)];
        uint v1_id = tris[(3 * t_id) +1];
        uint v2_id = tris[(3 * t_id) +2];
        std::bitset<NBIT> l = labels[t_id];

        double v0[3] = {in_coords[3 * v0_id], in_coords[(3 * v0_id) +1], in_coords[(3 * v0_id) +2]};
        double v1[3] = {in_coords[3 * v1_id], in_coords[(3 * v1_id) +1], in_coords[(3 * v1_id) +2]};
        double v2[3] = {in_coords[3 * v2_id], in_coords[(3 * v2_id) +1], in_coords[(3 * v2_id) +2]};

        if(!cinolib::points_are_colinear_3d(v0, v1, v2)) // good triangle
        {
            std::vector<uint> tri = {v0_id, v1_id, v2_id};
            std::sort(tri.begin(), tri.end());

            auto tri_it = tris_map.find(tri);

            if(tri_it == tris_map.end()) // first time for tri v0, v1, v2
            {
                tris_map[tri] = l_off;
                labels[l_off] = l;
                l_off++;

                tris[t_off]    = v0_id;
                tris[t_off +1] = v1_id;
                tris[t_off +2] = v2_id;
                t_off += 3;
            }
            else
            {
                labels[tri_it->second] |= l; // label for duplicates
            }
        }
    }

    tris.resize(t_off);
    labels.resize(l_off);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void detectIntersectionsWithOctree(TriangleSoup &ts, const std::vector<uint> &in_tris, std::vector<std::vector<uint> > &intersection_list)
{
    std::vector<cinolib::vec3d> verts(ts.numVerts());

    for(uint v_id = 0; v_id < ts.numVerts(); v_id++)
        verts[v_id] = ts.vertCinolib(v_id);

    std::set< std::pair<uint, uint> > tmp_inters;
    cinolib::find_intersections(verts, in_tris, tmp_inters);

    intersection_list.resize(ts.numTris());

    for(auto &p : tmp_inters)
    {
        intersection_list[p.first].push_back(p.second);
        intersection_list[p.second].push_back(p.first);
    }
}


