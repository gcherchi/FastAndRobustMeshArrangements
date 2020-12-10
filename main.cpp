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
//        std::cout << "input file missing" << std::endl;
//        return -1;

        //filename = "/Users/gianmarco/Desktop/2sphere_coarse.stl";
        //filename = "/Users/gianmarco/Desktop/coplanar5.stl";
        filename = "/Users/gianmarco/Desktop/three_cubes.stl";
        //filename = "/Users/gianmarco/Desktop/tri_test100.stl";
    }

    std::vector<double> in_coords, out_coords;
    std::vector<uint> in_tris, out_tris;
    std::vector<genericPoint*> impl_points;

    load(filename, in_coords, in_tris);

    //solveIntersections(in_coords, in_tris, out_coords, out_tris);
    solveIntersections(in_coords, in_tris, impl_points, out_tris);

    computeApproximateCoordinates(impl_points, out_coords);
    freePointsMemory(impl_points);

    //save("/Users/gianmarco/Desktop/output.obj", out_coords, out_tris);

    return 0;
}



