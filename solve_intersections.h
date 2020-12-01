#ifndef SOLVE_INTERSECTIONS_H
#define SOLVE_INTERSECTIONS_H

#include "preprocessing/pre_processing.h"
#include "structures/aux_structure.h"
#include "structures/triangle_soup.h"
#include "intersections_detection/intersection_classification.h"
#include "triangulation/triangulation.h"
#include "postprocessing/post_processing.h"

#include <bitset>

void meshArrangementPipeline(const std::vector<double> &in_coords, const std::vector<uint> &in_tris, const std::vector< std::bitset<NBIT> > &in_labels,
                             std::vector<double> &out_coords, std::vector<uint> &out_tris, std::vector< std::bitset<NBIT> > &out_labels);


// Without labels info
void solveIntersections(const std::vector<double> &in_coords, const std::vector<uint> &in_tris, std::vector<double> &out_coords, std::vector<uint> &out_tris);


// With labels info
void solveIntersections(const std::vector<double> &in_coords, const std::vector<uint> &in_tris, const std::vector<uint> &in_labels,
                        std::vector<double> &out_coords, std::vector<uint> &out_tris, std::vector< std::bitset<NBIT> > &out_labels, uint &num_labels);





#endif // SOLVE_INTERSECTIONS_H
