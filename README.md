# Fast and Robust Mesh Arrangements using Floating-point Arithmetic

This is the code we used for the experiments in the paper "**Fast and Robust Mesh Arrangements using Floating-point Arithmetic**" by G. Cherchi, M. Livesu, R. Scateni and M. Attene (ACM TOG, SIGGRAPH Asia 2020). 

<p align="center"><img src="teaser_img.png"></p>

|:warning: WARNING: We are rewriting portions of the code to be more efficient. The code is therefore subject to continuous updates. We are also preparing an easy to use header-only version of our algorithm. It will be released as soon as possible.  |
| --- |

## Usage
Clone this repository, including submodules, with:
```
git clone --recursive https://github.com/gcherchi/FastAndRobustMeshArrangements.git
```
Once done, you may build the executable as follows:
```
mkdir build
cd build
cmake ..
```
This will produce an appropriate building configuration for your system.
On Windows MSVC, this will produce a mesh_arrangement.sln file.
On Linux/MacOS, this will produce a Makefile. 
Use it as usual to compile mesh_arrangement.

We tested our code on MacOS (CLANG 10.0 64 bit), Linux (GCC 7.3.0 64 bit) and Windows (MSVC 2019 64 bit).

|:warning: WARNING: Apparently, CLANG does not support a fully IEEE compliant floating point environment. The only way we found to guarantee correctness on this compiler was to disable all optimizations. Please be aware of this fact should you notice a performance degradation in your experiments. |
| --- |

## Citing us
If you use our code in your academic projects, please cite our paper using the following BibTeX entry:
```
@article{CLSA20,
  title   = {Fast and Robust Mesh Arrangements using Floating-point Arithmetic},
  author  = {Cherchi, Gianmarco and Livesu, Marco and Scateni, Riccardo and Attene, Marco},
  journal = {ACM Transactions on Graphics (SIGGRAPH Asia 2020)},
  year    = {2020},
  volume  = {39},
  number  = {6},
  doi     = {10.1145/3414685.3417818}
}
```


