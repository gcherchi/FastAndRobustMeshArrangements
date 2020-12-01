#ifndef PREPROCESSING_H
#define PREPROCESSING_H

#include "structures/triangle_soup.h"

#include <cinolib/octree.h>
#include <cinolib/predicates.h>

typedef unsigned int uint;

void mergeDuplicatedVertices(const std::vector<double> &in_coords, const std::vector<uint> &in_tris, const double &multiplier,
                             std::vector<double> &out_coords, std::vector<uint> &out_tris);

void removeDegenerateAndDuplicatedTriangles(const std::vector<double> &in_coords, std::vector<uint> &tris, std::vector<std::bitset<NBIT> > &labels);

void detectIntersectionsWithOctree(TriangleSoup &ts, const std::vector<uint> &in_tris, std::vector<std::vector<uint> > &intersection_list);


#endif // PREPROCESSING_H
