#include <QApplication>
#include <cinolib/gui/qt/qt_gui_tools.h>
#include <iostream>

#include "solve_intersections.h"
#include "io_functions.h"

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    #ifdef DBG_MODE
    QApplication a(argc, argv);
    #endif

    std::string filename;

    #ifndef DBG_MODE
    if(argc > 1) filename = argv[1];
    else
    {
        std::cout << "input file missing" << std::endl;
        return -1;
    }
    #else
    filename = std::string(DATA_PATH) + "two_spheres.stl";
    #endif

    //::::::: Loading file data ::::::::::::::::::::::::::::::::::::
    std::vector<cinolib::vec3d> cinolib_verts;
    std::vector<double> in_coords;
    std::vector<uint> in_tris;

    cinolib::read_STL(filename.c_str(), cinolib_verts, in_tris, false);

    for(auto &v : cinolib_verts)
    {
        in_coords.push_back(v.x());
        in_coords.push_back(v.y());
        in_coords.push_back(v.z());
    }

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    std::vector<double> out_coords;
    std::vector<uint> out_tris;

    solveIntersections(in_coords, in_tris, out_coords, out_tris);

    cinolib::DrawableTrimesh<> m(out_coords, out_tris);

    for(uint e = 0; e < m.num_edges(); e++) m.edge_data(e).flags = 0;
    m.updateGL();

    /* ----------------------------------------
     * Visualization stuff
     * -------------------------------------- */
    #ifdef DBG_MODE

    cinolib::GLcanvas gui;
    gui.push_obj(&m);
    gui.show();

    // CMD+1 to show tri-mesh controls.
    cinolib::SurfaceMeshControlPanel<cinolib::DrawableTrimesh<>> panel(&m, &gui);

    QApplication::connect(new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_1), &gui), &QShortcut::activated, [&](){panel.show();});

    return a.exec();
    #else
    return 0;
    #endif
}



