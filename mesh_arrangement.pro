TARGET          = mesh_arrangement
CONFIG          += c++11
CONFIG          -= app_bundle
CXXFLAGS        ="-mmacosx-version-min=10.14"
CONFIG          += sdk_no_version_check
DEFINES         += CINOLIB_USES_EXACT_PREDICATES
DATA_PATH       = \\\"$$PWD/data/\\\"
DEFINES         += DATA_PATH=$$DATA_PATH

QMAKE_CXXFLAGS  += -frounding-math


SOURCES         +=  main.cpp \
                    external/Cinolib/external/predicates/shewchuk.c \
                    intersections_detection/intersection_classification.cpp \
                    non_manifold/nmIntersections.cpp \
                    non_manifold/nonManifold.cpp \
                    preprocessing/pre_processing.cpp \
                    triangulation/triangulation.cpp \
                    external/IndirectPredicates/implicit_point.cpp \
                    external/IndirectPredicates/numerics.cpp \
                    external/IndirectPredicates/predicates/indirect_predicates.cpp \

HEADERS         +=  \
                    intersections_detection/intersection_classification.h \
                    solve_intersections.h \
                    structures/aux_structure.h \
                    structures/aux_structure.tpp \
                    non_manifold/nonManifold.h \
                    preprocessing/pre_processing.h \
                    structures/common.h \
                    structures/fast_trimesh.h \
                    structures/fast_trimesh.tpp \
                    structures/triangle_soup.h \
                    structures/triangle_soup.tpp \
                    structures/tree.h \
                    triangulation/triangulation.h \
                    external/IndirectPredicates/implicit_point.h \
                    external/IndirectPredicates/numerics.h \
                    external/IndirectPredicates/predicates/indirect_predicates.h \

#EIGEN
INCLUDEPATH     += external/Cinolib/external/eigen

# CINOLIB
INCLUDEPATH     += external/Cinolib/include
DEPENDPATH      += external/Cinolib/include

#INDIRECT PREDICATES
INCLUDEPATH     += external/IndirectPredicates/
INCLUDEPATH     += external/IndirectPredicates/predicates/

#TMESH_KERNEL
LIBS            += -L[TMesh_Kernel path]/TMesh_Kernel/build/ -lTMesh_Kernel
INCLUDEPATH     += [TMesh_Kernel path]/TMesh_Kernel/include
PRE_TARGETDEPS  += [TMesh_Kernel path]/TMesh_Kernel/build/libTMesh_Kernel.a


unix:macx{

QMAKE_CXXFLAGS -= -O
QMAKE_CXXFLAGS -= -O1
QMAKE_CXXFLAGS -= -O2
QMAKE_CXXFLAGS -= -O3
QMAKE_CXXFLAGS += -O0
}


#parallel version (only linux)
#QMAKE_CXXFLAGS  += -fopenmp
#QMAKE_LFLAGS    += -fopenmp


DEFINES += IS64BITPLATFORM

#DEFINES += NDEBUG






