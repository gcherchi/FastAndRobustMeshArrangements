# Fast and Robust Mesh Arrangements using Floating-point Arithmetic

This is a **HEADER-ONLY** version of the code we used for the experiments in the paper ["**Fast and Robust Mesh Arrangements using Floating-point Arithmetic**"](https://dl.acm.org/doi/abs/10.1145/3414685.3417818) by [G. Cherchi](http://www.gianmarcocherchi.com), [M. Livesu](http://pers.ge.imati.cnr.it/livesu/), [R. Scateni](https://people.unica.it/riccardoscateni/) and [M. Attene](https://www.cnr.it/en/people/marco.attene) (ACM TOG, SIGGRAPH Asia 2020). 

<p align="center"><img src="teaser_img.png"></p>

|:warning: WARNING: We are rewriting portions of this code to be more efficient. The code is therefore subject to continuous updates. |
| --- |

## Usage
Clone this repository, including submodules, with:
```
git clone --recursive https://github.com/gcherchi/FastAndRobustMeshArrangements.git
```
Once done, you may build the executable as follows (put ``Release`` or ``Debug`` instead of ``<build type>``):
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=<build type> ..
```
This will produce an appropriate building configuration for your system.
On Windows MSVC, this will produce a mesh_arrangement.sln file.
On Linux/MacOS, this will produce a Makefile. 
Use it as usual to compile mesh_arrangement.

There are 4 versions of the ``solveIntersections(...)`` function, with different outputs. Please, check the file ``solve_intersections.h`` for more details.

We tested our code on MacOS (CLANG 10.0 64 bit), Linux (GCC 7.3.0 64 bit) and Windows (MSVC 2019 64 bit).

|:warning: WARNING: Apparently, CLANG does not support a fully IEEE compliant floating point environment. The only way we found to guarantee correctness on this compiler was to disable all optimizations. Please be aware of this fact should you notice a performance degradation in your experiments. |
| --- |

## Citing us
This project is based on algorithms described in the following scientific articles. If you use our code in your academic projects, please cite our papers using the following BibTeX entries:
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

@article{LCSA21,
  title   = {Deterministic Linear Time Constrained Triangulation using Simplified Earcut},
  author  = {Livesu, Marco and Cherchi, Gianmarco and Scateni, Riccardo and Attene, Marco},
  journal = {IEEE Transactions on Visualization and Computer Graphics (TVCG)},
  year    = {2021},
  doi     = {10.1109/TVCG.2021.3070046}}
```


