#include "solve_intersections.h"

#include "debug.h"

extern std::string F;

inline void meshArrangementPipeline(const std::vector<double> &in_coords, const std::vector<uint> &in_tris, const std::vector< std::bitset<NBIT> > &in_labels,
                                    std::vector<genericPoint*> &vertices, std::vector<uint> &out_tris, std::vector< std::bitset<NBIT> > &out_labels)
{
    initFPU();

    AuxiliaryStructure g;
    //double multiplier = 67108864.0;
    double multiplier = computeMultiplier(in_coords);

    std::vector<uint> tmp_tris;

    auto s0 = startChrono();

    mergeDuplicatedVertices(in_coords, in_tris, vertices, tmp_tris);

    removeDegenerateAndDuplicatedTriangles(vertices, in_labels, tmp_tris, out_labels);

    TriangleSoup ts(vertices, tmp_tris, out_labels, multiplier);

    detectIntersections(ts, g.intersectionList());

    g.initFromTriangleSoup(ts);

    classifyIntersections(ts, g);

    triangulation(ts, g, out_tris, out_labels);

    ts.appendJollyPoints();

    double t = stopChrono(s0);
    saveStatisticsOnFile(F, t, 0);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline void solveIntersections(const std::vector<double> &in_coords, const std::vector<uint> &in_tris, std::vector<double> &out_coords, std::vector<uint> &out_tris)
{
    std::vector<genericPoint*> vertices;
    std::vector< std::bitset<NBIT>> tmp_in_labels(in_tris.size() / 3), out_labels;

    meshArrangementPipeline(in_coords, in_tris, tmp_in_labels, vertices, out_tris, out_labels);

    computeApproximateCoordinates(vertices, out_coords);
    freePointsMemory(vertices);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline void solveIntersections(const std::vector<double> &in_coords, const std::vector<uint> &in_tris,
                               std::vector<genericPoint *> &out_vertices, std::vector<uint> &out_tris)
{
    std::vector< std::bitset<NBIT>> tmp_in_labels(in_tris.size() / 3), out_labels;

    meshArrangementPipeline(in_coords, in_tris, tmp_in_labels, out_vertices, out_tris, out_labels);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline void solveIntersections(const std::vector<double> &in_coords, const std::vector<uint> &in_tris, const std::vector<uint> &in_labels,
                                  std::vector<double> &out_coords, std::vector<uint> &out_tris, std::vector< std::bitset<NBIT> > &out_labels)
{
    std::vector<genericPoint*> vertices;
    std::vector< std::bitset<NBIT>> tmp_in_labels(in_labels.size());

    for(uint i = 0; i < in_labels.size(); i++)
        tmp_in_labels[i][in_labels[i]] = 1;

    meshArrangementPipeline(in_coords, in_tris, tmp_in_labels, vertices, out_tris, out_labels);

    computeApproximateCoordinates(vertices, out_coords);
    freePointsMemory(vertices);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

inline void solveIntersections(const std::vector<double> &in_coords, const std::vector<uint> &in_tris, const std::vector<uint> &in_labels,
                               std::vector<genericPoint *> &vertices, std::vector<uint> &out_tris, std::vector<std::bitset<NBIT> > &out_labels)
{
    std::vector< std::bitset<NBIT>> tmp_in_labels(in_labels.size());

    for(uint i = 0; i < in_labels.size(); i++)
        tmp_in_labels[i][in_labels[i]] = 1;

    meshArrangementPipeline(in_coords, in_tris, tmp_in_labels, vertices, out_tris, out_labels);
}
