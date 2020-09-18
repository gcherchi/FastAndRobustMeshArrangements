cmake_minimum_required(VERSION 3.10)

# set the project name
project(mesh_arrangement)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_executable(${PROJECT_NAME} 
    main.cpp
    external/Cinolib/external/predicates/shewchuk.c
    intersections_detection/intersection_classification.cpp
    non_manifold/nmIntersections.cpp
    non_manifold/nonManifold.cpp
    preprocessing/pre_processing.cpp
    triangulation/triangulation.cpp
    external/IndirectPredicates/implicit_point.cpp
    external/IndirectPredicates/numerics.cpp
    external/IndirectPredicates/predicates/indirect_predicates.cpp
	external/TMesh_Kernel/src/coordinates.cpp
	external/TMesh_Kernel/src/mixedPredicates.cpp
	external/TMesh_Kernel/src/orientation.cpp
	external/TMesh_Kernel/src/point.cpp
	external/TMesh_Kernel/src/tmesh.cpp
)

# Compiler-specific options
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
# grant IEEE 754 compliance
	target_compile_options(${PROJECT_NAME} PUBLIC -frounding-math)
# use intrinsic functions (CHECK WHAT TO DO FOR GCC !!!!!!!!)
#	target_compile_options(${PROJECT_NAME} PUBLIC "/Oi")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
# grant IEEE 754 compliance
	target_compile_options(${PROJECT_NAME} PUBLIC "/fp:strict")
# use intrinsic functions
	target_compile_options(${PROJECT_NAME} PUBLIC "/Oi")
# turn off annoying warnings
	target_compile_options(${PROJECT_NAME} PUBLIC "/D _CRT_SECURE_NO_WARNINGS")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
# grant IEEE 754 compliance
	target_compile_options(${PROJECT_NAME} PUBLIC -O0)
endif()

# Public include directory
target_include_directories(${PROJECT_NAME} PUBLIC
	./
	external/Cinolib/external/eigen/
	external/Cinolib/include/
	external/IndirectPredicates/
	external/IndirectPredicates/predicates/
	external/TMesh_Kernel/include/
)

# preprocessor definitions
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	target_compile_definitions(${PROJECT_NAME} PUBLIC -DIS64BITPLATFORM)
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC
	CINOLIB_USES_EXACT_PREDICATES
	DATA_PATH="./data/"
)

