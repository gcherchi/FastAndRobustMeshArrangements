#include "processing.h"



inline double computeMultiplier(const std::vector<double> &coords)
{
    double max = *std::max_element(coords.begin(), coords.end());
    double min = *std::max_element(coords.begin(), coords.end());

    double abs_max = std::max(std::abs(max), std::abs(min));

    //...
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline void mergeDuplicatedVertices(const std::vector<double> &in_coords, const std::vector<uint> &in_tris,
                                    std::vector<genericPoint*> &verts, std::vector<uint> &tris)
{
    verts.reserve(in_coords.size() / 3);
    tris.reserve(in_tris.size());

    std::map<std::vector<double>, uint> v_map;

    for(const uint &v_id : in_tris)
    {
        std::vector<double> v = {in_coords[(3 * v_id)], in_coords[(3 * v_id) +1], in_coords[(3 * v_id) +2]};

        auto ins = v_map.insert({v, v_map.size()});
        if(ins.second)
            verts.push_back(new explicitPoint3D(v[0], v[1], v[2])); // new vtx added

        tris.push_back(ins.first->second);
    }
}


//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline void removeDegenerateAndDuplicatedTriangles(const std::vector<genericPoint*> &verts, const std::vector<std::bitset<NBIT> > &in_labels,
                                                   std::vector<uint> &tris, std::vector< std::bitset<NBIT> > &labels)
{
    labels = in_labels;

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

        if(!cinolib::points_are_colinear_3d(verts[v0_id]->toExplicit3D().ptr(),
                                            verts[v1_id]->toExplicit3D().ptr(),
                                            verts[v2_id]->toExplicit3D().ptr())) // good triangle
        {
            std::vector<uint> tri = {v0_id, v1_id, v2_id};
            std::sort(tri.begin(), tri.end());

            auto ins = tris_map.insert({tri, l_off});

            if(ins.second) // first time for tri v0, v1, v2
            {
                labels[l_off] = l;
                l_off++;

                tris[t_off]    = v0_id;
                tris[t_off +1] = v1_id;
                tris[t_off +2] = v2_id;
                t_off += 3;
            }
            else
            {
                uint pos = ins.first->second;
                labels[pos] |= l; // label for duplicates
            }
        }
    }

    tris.resize(t_off);
    labels.resize(l_off);
}


//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void freePointsMemory(std::vector<genericPoint *> &points)
{
    for(uint p = 0; p < points.size(); p++)
        delete points[p];
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void computeApproximateCoordinates(const std::vector<genericPoint *> &vertices, std::vector<double> &coords, const double &multiplier)
{
    coords.reserve(3 * vertices.size());

    for(auto &v : vertices)
    {
        if(v->isExplicit3D())
        {
            coords.push_back(v->toExplicit3D().X() / multiplier);
            coords.push_back(v->toExplicit3D().Y() / multiplier);
            coords.push_back(v->toExplicit3D().Z() / multiplier);
        }
        else
        {
            double x, y, z;
            v->getApproxXYZCoordinates(x, y, z);
            coords.push_back(x / multiplier);
            coords.push_back(y / multiplier);
            coords.push_back(z / multiplier);
        }
    }
}


