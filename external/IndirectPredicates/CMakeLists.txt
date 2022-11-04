cmake_minimum_required(VERSION 3.10)

project(indirectPredicates)

# build static library
add_library(${PROJECT_NAME}
            implicit_point.cpp
            numerics.cpp
            predicates/indirect_predicates.cpp
            predicates/hand_optimized_predicates.cpp)

# build test program
add_executable(${PROJECT_NAME}_test
               implicit_point.cpp
               numerics.cpp
               predicates/indirect_predicates.cpp
               predicates/hand_optimized_predicates.cpp
               test.cpp)

# Compiler-specific options to grant IEEE 754 compliance
set(ALL_TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_test)
foreach(TARGET ${ALL_TARGETS})
	if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")            
            target_compile_options(${TARGET} PUBLIC "/fp:strict")
            target_compile_options(${TARGET} PUBLIC "/Oi")
            target_compile_options(${TARGET} PUBLIC "/D _CRT_SECURE_NO_WARNINGS")
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            target_compile_options(${TARGET} PUBLIC -frounding-math)
            find_package(Threads)
            target_link_libraries(${TARGET} ${CMAKE_THREAD_LIBS_INIT})
        elseif (CMAKE_CXX_COMPILER_ID MATCHES  "Clang" OR
                CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
	    # this does not seem to be needed anymore....
	    # (uncomment it if you observe unexpected behaviour)
            #target_compile_options(${TARGET} PUBLIC -O0)
        endif()
        target_include_directories(${TARGET} PUBLIC .)
        target_include_directories(${TARGET} PUBLIC predicates)
        target_compile_features(${TARGET} PUBLIC cxx_std_11)
endforeach(TARGET)
