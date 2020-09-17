cmake_minimum_required(VERSION 3.10)

# Uncomment and adapt the following definition after having downloaded
# TMesh_Kernel and compiled it is FAST mode.
#set(TMESH_KERNEL_LOCATION "D:/TMesh_Kernel")

if(NOT DEFINED TMESH_KERNEL_LOCATION)
  message(FATAL_ERROR "TMESH_KERNEL_LOCATION is undefined. Please set it on line 5 in CMakeLists.txt")
endif(NOT DEFINED TMESH_KERNEL_LOCATION)

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
)

# Compiler-specific options
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
# grant IEEE 754 compliance
	target_compile_options(${PROJECT_NAME} PUBLIC -frounding-math)
# use intrinsic functions (CHECK WHAT TO DO FOR GCC !!!!!!!!)
#	target_compile_options(${PROJECT_NAME} PUBLIC "/Oi")
# link against TMesh_Kernel
	target_link_libraries(${PROJECT_NAME} ${TMESH_KERNEL_LOCATION}/build/kernel_Fast64.a)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
# grant IEEE 754 compliance
	target_compile_options(${PROJECT_NAME} PUBLIC "/fp:strict")
# use intrinsic functions
	target_compile_options(${PROJECT_NAME} PUBLIC "/Oi")
# turn off annoying warnings
	target_compile_options(${PROJECT_NAME} PUBLIC "/D _CRT_SECURE_NO_WARNINGS")
# link against TMesh_Kernel
	target_link_libraries(${PROJECT_NAME} ${TMESH_KERNEL_LOCATION}/build/Release/kernel_Fast64.lib)
endif()

# Public include directory
target_include_directories(${PROJECT_NAME} PUBLIC
	./
	external/Cinolib/external/eigen/
	external/Cinolib/include/
	external/IndirectPredicates/
	external/IndirectPredicates/predicates/
	${TMESH_KERNEL_LOCATION}/include/
)

# preprocessor definitions
target_compile_definitions(${PROJECT_NAME} PUBLIC
	CINOLIB_USES_EXACT_PREDICATES
	IS64BITPLATFORM
	DATA_PATH="./data/"
)


