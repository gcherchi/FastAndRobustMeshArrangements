#ifndef DEBUG_H
#define DEBUG_H

#include <string>
#include <fstream>

inline std::string ts(const double &n)
{
    std::string s = std::to_string(n);
    std::replace(s.begin(), s.end(), '.', ',');
    return s;
}

inline void saveStatisticsOnFile(const std::string &file_in, const double &time)
{
    std::string filename = "log.csv";

    std::string header = "model; time\n";

    std::string data = file_in.substr(0, file_in.length() - 4); // model name without extension

    data = data + ";" + ts(time) + "\n";

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




#endif // DEBUG_H
