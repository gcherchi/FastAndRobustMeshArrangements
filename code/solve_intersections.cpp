#include "solve_intersections.h"

inline void meshArrangementPipeline(const std::vector<double> &in_coords, const std::vector<uint> &in_tris, const std::vector< std::bitset<NBIT> > &in_labels,
                        std::vector<double> &out_coords, std::vector<uint> &out_tris, std::vector< std::bitset<NBIT> > &out_labels)
{
    TriangleSoup ts;
    AuxiliaryStructure g;
    const double multiplier = 67108864.0;

    mergeDuplicatedVertices(in_coords, in_tris, multiplier, out_coords, out_tris);

    out_labels = in_labels;
    removeDegenerateAndDuplicatedTriangles(out_coords, out_tris, out_labels);

    ts = TriangleSoup(out_coords, out_tris, out_labels);

    detectIntersectionsWithOctree(ts, out_tris, g.intersectionList());

    g.initFromTriangleSoup(ts);

    classifyIntersections(ts, g);

    triangulation(ts, g, out_tris, out_labels);

    ts.createDoubleVectorOfCoords(out_coords, multiplier);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline void solveIntersections(const std::vector<double> &in_coords, const std::vector<uint> &in_tris, std::vector<double> &out_coords, std::vector<uint> &out_tris)
{
    uint num_tris = static_cast<uint>(in_tris.size() / 3);
    std::vector< std::bitset<NBIT>> tmp_in_labels(num_tris), out_labels;

    meshArrangementPipeline(in_coords, in_tris, tmp_in_labels, out_coords, out_tris, out_labels);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline void solveIntersections(const std::vector<double> &in_coords, const std::vector<uint> &in_tris, const std::vector<uint> &in_labels,
                                  std::vector<double> &out_coords, std::vector<uint> &out_tris, std::vector< std::bitset<NBIT> > &out_labels, uint &num_labels)
{
    std::vector< std::bitset<NBIT>> tmp_in_labels(in_labels.size());

    std::set<uint> lset;

    for(uint i = 0; i < in_labels.size(); i++)
    {
        tmp_in_labels[i][in_labels[i]] = 1;
        lset.insert(in_labels[i]);
    }

    meshArrangementPipeline(in_coords, in_tris, tmp_in_labels, out_coords, out_tris, out_labels);

    num_labels = static_cast<uint>(lset.size());
}



