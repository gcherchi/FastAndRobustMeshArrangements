TARGET          = mesh_arrangements
CONFIG          += c++11
CONFIG          -= app_bundle
CXXFLAGS        ="-mmacosx-version-min=10.14"
CONFIG          += sdk_no_version_check

DEFINES         += CINOLIB_USES_EXACT_PREDICATES
DEFINES         += IS64BITPLATFORM


SOURCES         +=  main.cpp \
                    external/Cinolib/external/predicates/shewchuk.c \
                    indirect_predicates/implicit_point.cpp \
                    indirect_predicates/numerics.cpp \
                    indirect_predicates/predicates/indirect_predicates.cpp \
                    intersections_detection/intersection_classification.cpp \


HEADERS         +=  \
                    intersections_detection/intersection_classification.h \
                    indirect_predicates/implicit_point.h \
                    indirect_predicates/numerics.h \
                    indirect_predicates/predicates/indirect_predicates.h \ \

#INDIRECT PREDICATES
INCLUDEPATH     += external/IndirectPredicates/predicates
INCLUDEPATH     += external/IndirectPredicates/

#EIGEN
INCLUDEPATH     += external/Cinolib/external/eigen

# CINOLIB
INCLUDEPATH     += external/Cinolib/include


unix:macx{

QMAKE_CXXFLAGS  -= -O
QMAKE_CXXFLAGS  -= -O1
QMAKE_CXXFLAGS  -= -O2
QMAKE_CXXFLAGS  -= -O3
QMAKE_CXXFLAGS  += -O0
}


# just for Linux
unix:!macx{

QMAKE_CXXFLAGS  += -frounding-math

#parallel version
#QMAKE_CXXFLAGS  += -fopenmp
#QMAKE_LFLAGS    += -fopenmp

}


DEFINES += NDEBUG






