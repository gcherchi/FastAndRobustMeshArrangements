#ifndef SOLVE_INTERSECTIONS_H
#define SOLVE_INTERSECTIONS_H

#include "processing.h"
#include "aux_structure.h"
#include "triangle_soup.h"
#include "intersection_classification.h"
#include "triangulation.h"

#include <bitset>

inline void meshArrangementPipeline(const std::vector<double> &in_coords, const std::vector<uint> &in_tris, const std::vector< std::bitset<NBIT> > &in_labels,
                                    std::vector<double> &out_coords, std::vector<uint> &out_tris, std::vector< std::bitset<NBIT> > &out_labels);


// Without labels info
inline void solveIntersections(const std::vector<double> &in_coords, const std::vector<uint> &in_tris, std::vector<double> &out_coords, std::vector<uint> &out_tris);


// With labels info
inline void solveIntersections(const std::vector<double> &in_coords, const std::vector<uint> &in_tris, const std::vector<uint> &in_labels,
                               std::vector<double> &out_coords, std::vector<uint> &out_tris, std::vector< std::bitset<NBIT> > &out_labels, uint &num_labels);


#include "solve_intersections.cpp"


#endif // SOLVE_INTERSECTIONS_H
