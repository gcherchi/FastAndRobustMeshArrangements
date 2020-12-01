#include <QApplication>
#include <cinolib/gui/qt/qt_gui_tools.h>
#include <iostream>

#include "solve_intersections.h"
#include "io_functions.h"

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int main(int argc, char **argv)
{
    QApplication a(argc, argv);

    std::string filename;

    if(argc > 1)
        filename = argv[1];
    else
    {
        //std::cout << "input file missing" << std::endl;
        //return -1;

        filename = std::string(DATA_PATH) + "two_spheres.stl";
    }

    std::vector<double> in_coords, out_coords;
    std::vector<uint> in_tris, out_tris;

    load(filename, in_coords, in_tris);

    solveIntersections(in_coords, in_tris, out_coords, out_tris);

    cinolib::DrawableTrimesh<> m(out_coords, out_tris);

    cinolib::GLcanvas gui;
    gui.push_obj(&m);
    gui.show();

    // CMD+1 to show tri-mesh controls.
    cinolib::SurfaceMeshControlPanel<cinolib::DrawableTrimesh<>> panel(&m, &gui);

    QApplication::connect(new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_1), &gui), &QShortcut::activated, [&](){panel.show();});

    return a.exec();
}



