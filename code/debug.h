/*****************************************************************************************
 *              MIT License                                                              *
 *                                                                                       *
 * Copyright (c) 2020 Gianmarco Cherchi, Marco Livesu, Riccardo Scateni e Marco Attene   *
 *                                                                                       *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this  *
 * software and associated documentation files (the "Software"), to deal in the Software *
 * without restriction, including without limitation the rights to use, copy, modify,    *
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to    *
 * permit persons to whom the Software is furnished to do so, subject to the following   *
 * conditions:                                                                           *
 *                                                                                       *
 * The above copyright notice and this permission notice shall be included in all copies *
 * or substantial portions of the Software.                                              *
 *                                                                                       *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,   *
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A         *
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT    *
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION     *
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE        *
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                *
 *                                                                                       *
 * Authors:                                                                              *
 *      Gianmarco Cherchi (g.cherchi@unica.it)                                           *
 *      https://people.unica.it/gianmarcocherchi/                                        *
 *                                                                                       *
 *      Marco Livesu (marco.livesu@ge.imati.cnr.it)                                      *
 *      http://pers.ge.imati.cnr.it/livesu/                                              *
 *                                                                                       *
 *      Riccardo Scateni (riccardo@unica.it)                                             *
 *      https://people.unica.it/riccardoscateni/                                         *
 *                                                                                       *
 *      Marco Attene (marco.attene@ge.imati.cnr.it)                                      *
 *      https://www.cnr.it/en/people/marco.attene/                                       *
 *                                                                                       *
 * ***************************************************************************************/

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

inline void saveStatisticsOnFile(const std::string &file_in, const double &time, const double &time2)
{
    std::string filename = "log.csv";

    std::string header = "model; time; time mul\n";

    std::string data = file_in.substr(0, file_in.length() - 4); // model name without extension

    data = data + ";" + ts(time) + ";" + ts(time2) + "\n";

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
