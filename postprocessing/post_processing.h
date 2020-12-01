#ifndef POST_PROCESSING_H
#define POST_PROCESSING_H

#include "structures/aux_structure.h"
#include "structures/triangle_soup.h"

#include <cinolib/memory_usage.h>

void saveStatisticsOnFile(const std::string &file_in, const double &timeBPS, const double &timeOctree, const bool &check);

std::string ts(const double &n);

void checkMemory(float &tot_mem);


#endif // POST_PROCESSING_H
