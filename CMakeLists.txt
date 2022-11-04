cmake_minimum_required(VERSION 3.10)

# set the project name
project(mesh_arrangement)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_executable(${PROJECT_NAME} 
    main.cpp
    external/Cinolib/external/shewchuk_predicates/shewchuk.c
    external/IndirectPredicates/implicit_point.cpp
    external/IndirectPredicates/numerics.cpp
    external/IndirectPredicates/predicates/indirect_predicates.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
		./
		code/
		external/Cinolib/external/eigen/
		external/Cinolib/include/
		external/IndirectPredicates/
		external/IndirectPredicates/predicates/
		)

set(TBB_TEST OFF CACHE BOOL " " FORCE)
set(TBB_EXAMPLES OFF CACHE BOOL " " FORCE)
add_subdirectory(external/oneTBB)

target_compile_definitions(${PROJECT_NAME} PUBLIC TBB_PARALLEL=1)

# Compiler-specific options
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
# grant IEEE 754 compliance
	target_compile_options(${PROJECT_NAME} PUBLIC -frounding-math)
	find_package(Threads)
	target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
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
	#target_compile_options(${PROJECT_NAME} PUBLIC -O0)
endif()

target_link_libraries(${PROJECT_NAME} tbb)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/external/abseil-cpp/)

set(cinolib_DIR ${PROJECT_SOURCE_DIR}/external/Cinolib)
set(CINOLIB_USES_OPENGL_GLFW_IMGUI ON)
set(CINOLIB_USES_SHEWCHUK_PREDICATES ON)

find_package(cinolib REQUIRED)
target_link_libraries(${PROJECT_NAME} cinolib)

# preprocessor definitions
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	target_compile_definitions(${PROJECT_NAME} PUBLIC -DIS64BITPLATFORM)
endif()

