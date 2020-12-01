#include "post_processing.h"

#include <iostream>
#include <fstream>


void saveStatisticsOnFile(const string &file_in, const double &timeBPS, const double &timeOctree, const bool &check)
{
    std::string filename = "time_log.csv";

    std::string header = "model; time BSP; time Octree; check\n";

    std::string data = file_in.substr(0, file_in.length() - 4); // model name without extension

    data = data + ";" + ts(timeBPS);

    data = data + ";" + ts(timeOctree);

    data = data + ";" + std::to_string(check) + "\n";

    std::ifstream ifs;
    bool exists;

    // we check if the file exist to insert the header
    ifs.open(filename);
    if(ifs)
    {
        exists = true;
        ifs.close();
    }
    else
        exists = false;

    // we inser the data
    std::ofstream ofs;
    ofs.open (filename, std::ofstream::out | std::ofstream::app);

    if(exists)
        ofs << data;
    else
    {
        ofs << header;
        ofs << data;
    }

    ofs.close();
}


string ts(const double &n)
{
    std::string s = std::to_string(n);
    std::replace(s.begin(), s.end(), '.', ',');
    return s;
}


void checkMemory(float &tot_mem)
{
    float memMB = cinolib::memory_usage_in_mega_bytes();
    float memGB = memMB / 1024;

    if(memGB > 90)
    {
        std::cerr << "Code terminated due to MEMORY PROBLEM" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    if(memMB > tot_mem)
        tot_mem = memMB;
}
