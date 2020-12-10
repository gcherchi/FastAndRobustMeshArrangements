#ifndef PREPROCESSING_H
#define PREPROCESSING_H

#include "triangle_soup.h"

#include <cinolib/octree.h>
#include <cinolib/predicates.h>


inline double computeMultiplier(const std::vector<double> &coords);

inline void mergeDuplicatedVertices(const std::vector<double> &in_coords, const std::vector<uint> &in_tris, const double &multiplier,
                                    std::vector<explicitPoint3D> &verts, std::vector<uint> &tris);

inline void removeDegenerateAndDuplicatedTriangles(const std::vector<genericPoint *> &verts, const std::vector<std::bitset<NBIT> > &in_labels,
                                                   std::vector<uint> &tris, std::vector<std::bitset<NBIT> > &labels);

inline void freePointsMemory(std::vector<genericPoint*> &points);

inline void computeApproximateCoordinates(const std::vector<genericPoint *> &vertices, std::vector<double> &coords);

#include "processing.cpp"

#endif // PREPROCESSING_H
