#ifndef DEBUG_H
#define DEBUG_H

#include <string>
#include <fstream>
#include <chrono>

inline std::string ts(const double &n)
{
    std::string s = std::to_string(n);
    std::replace(s.begin(), s.end(), '.', ',');
    return s;
}

inline void saveStatisticsOnFile(const std::string &file_in, const double &time, double max)
{
    std::string filename = "log.csv";

    std::string header = "model; time; max_coord\n";

    std::string data = file_in.substr(0, file_in.length() - 4); // model name without extension

    data = data + ";" + ts(time) + ";" + ts(max) + "\n";

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

inline static std::chrono::time_point<std::chrono::system_clock> startChrono()
{
    return std::chrono::system_clock::now();
}

inline double stopChrono(std::chrono::time_point<std::chrono::system_clock> &start)
{
    auto time = std::chrono::system_clock::now() - start;
    return std::chrono::duration <double, std::milli> (time).count() / 1000;
}




#endif // DEBUG_H
