#include <iostream>

#include "solve_intersections.h"
#include "io_functions.h"

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    std::string filename;

    if(argc > 1)
        filename = argv[1];
    else
    {
        std::cout << "input file missing" << std::endl;
        return -1;
    }

    std::vector<double> in_coords, out_coords;
    std::vector<uint> in_tris, out_tris;

    load(filename, in_coords, in_tris);

    solveIntersections(in_coords, in_tris, out_coords, out_tris);

    save("output.obj", out_coords, out_tris);

    return 0;
}



