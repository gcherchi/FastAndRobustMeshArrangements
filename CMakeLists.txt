cmake_minimum_required(VERSION 3.21)


# Set the target architecture.
# All modern x86/x64 processors support AVX2.
# Older x86/x64 processors may support SSE2 but not AVX2.
# Very old x86/x64 processors, or non x86/x64
# processors, do not support any of the two.
set(ENABLE_SSE2 True)
set(ENABLE_AVX2 True)

# set the project name
project(mesh_arrangement)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(TBB_TEST OFF CACHE BOOL " " FORCE)
set(TBB_EXAMPLES OFF CACHE BOOL " " FORCE)

add_subdirectory(external/oneTBB)

# add the executable
add_executable(${PROJECT_NAME} 
	main.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
	./
	code/
)

target_compile_definitions(${PROJECT_NAME} PUBLIC TBB_PARALLEL=1)


include(FetchContent)
message("Fetch Indirect Predicates")
FetchContent_Declare(indirect_predicates
                    GIT_REPOSITORY "https://github.com/MarcoAttene/Indirect_Predicates.git"
                    GIT_TAG 142e3fbd922e45ade8b02c836a6b0386209a7ab2) # Jan 30, 2024 "Bigfux in lessThan functions"
FetchContent_MakeAvailable(indirect_predicates)
target_include_directories(${PROJECT_NAME} PUBLIC ${indirect_predicates_SOURCE_DIR}/include/)

message("Fetch Cinolib")
FetchContent_Declare(cinolib
                    GIT_REPOSITORY "https://github.com/mlivesu/cinolib.git"
                    GIT_TAG e2bc38c3ad8fc2ea2b487b2a3802f6bc2cdfeede) # Jan 18, 2024 "Update color.h"
FetchContent_MakeAvailable(cinolib)
target_include_directories(${PROJECT_NAME} PUBLIC ${cinolib_SOURCE_DIR}/external/eigen)
target_include_directories(${PROJECT_NAME} PUBLIC ${cinolib_SOURCE_DIR}/include/)
add_subdirectory(${cinolib_SOURCE_DIR}/external/shewchuk_predicates shewchuk_predicates)
target_compile_definitions(${PROJECT_NAME} PUBLIC CINOLIB_USES_SHEWCHUK_PREDICATES)

target_link_libraries(${PROJECT_NAME} PUBLIC shewchuk_predicates tbb)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/external/abseil-cpp/)

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	# grant IEEE 754 compliance
	target_compile_options(${PROJECT_NAME} PUBLIC "/fp:strict")
	# use intrinsic functions
	target_compile_options(${PROJECT_NAME} PUBLIC "/Oi")
	# set target architecture
	if(ENABLE_AVX2)
		target_compile_options(${PROJECT_NAME} PUBLIC "/arch:AVX2")
  	elseif(ENABLE_SSE2)
		target_compile_options(${PROJECT_NAME} PUBLIC "/arch:SSE2")
	endif()
	# reserve enough stack size
	target_link_options(${PROJECT_NAME} PUBLIC "/STACK:8421376")
	# turn off annoying warnings
	target_compile_options(${PROJECT_NAME} PUBLIC "/D _CRT_SECURE_NO_WARNINGS")
else()
	# set standard optimization level
	target_compile_options(${PROJECT_NAME} PUBLIC -O2)
	# reserve enough stack size
	target_compile_options(${PROJECT_NAME} PUBLIC -Wl,-z,stacksize=8421376)
	# grant IEEE 754 compliance
	target_compile_options(${PROJECT_NAME} PUBLIC -frounding-math)
	# set target architecture
	if(ENABLE_AVX2)
		target_compile_options(${PROJECT_NAME} PUBLIC "-mavx2")
		target_compile_options(${PROJECT_NAME} PUBLIC "-mfma")
	elseif(ENABLE_SSE2)
		target_compile_options(${PROJECT_NAME} PUBLIC "-msse2")
	endif()
endif()


