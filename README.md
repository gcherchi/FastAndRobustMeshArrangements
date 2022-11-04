# Fast and Robust Mesh Arrangements using Floating-point Arithmetic

This is a **HEADER-ONLY** version of the code we used for the experiments in the paper ["**Fast and Robust Mesh Arrangements using Floating-point Arithmetic**"](https://dl.acm.org/doi/abs/10.1145/3414685.3417818) by [G. Cherchi](http://www.gianmarcocherchi.com), [M. Livesu](http://pers.ge.imati.cnr.it/livesu/), [R. Scateni](https://people.unica.it/riccardoscateni/) and [M. Attene](https://www.cnr.it/en/people/marco.attene) (ACM TOG, SIGGRAPH Asia 2020) and later improved in ["**Interactive and Robust Mesh Booleans**"](https://www.gianmarcocherchi.com/pdf/interactive_exact_booleans.pdf) by [G. Cherchi](http://www.gianmarcocherchi.com), [F. Pellacini](https://pellacini.di.uniroma1.it), [M. Attene](https://www.cnr.it/en/people/marco.attene) and [M. Livesu](http://pers.ge.imati.cnr.it/livesu/) (SIGGRAPH Asia 2022). 

<p align="center"><img src="teaser_img.png"></p>

|:warning: WARNING: This code is subject to continuous updates. |
| --- |

## Usage
Clone this repository:
```
git clone https://github.com/gcherchi/FastAndRobustMeshArrangements.git
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

We tested our code on MacOS (CLANG 14.0 64 bit), Linux (GCC 7.3.0 64 bit) and Windows (MSVC 2019 64 bit).

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

@article{CPAL22,
  title   = {Interactive and Robust Mesh Booleans},
  author  = {Cherchi, Gianmarco and Pellacini, Fabio and Attene, Marco and Livesu, Marco},
  journal = {ACM Transactions on Graphics (SIGGRAPH Asia 2022)},
  year    = {2022},
  volume  = {41},
  number  = {6},
  doi     = {10.1145/3550454.3555460}
}

@article{LCSA21,
  title   = {Deterministic Linear Time Constrained Triangulation using Simplified Earcut},
  author  = {Livesu, Marco and Cherchi, Gianmarco and Scateni, Riccardo and Attene, Marco},
  journal = {IEEE Transactions on Visualization and Computer Graphics (TVCG)},
  year    = {2021},
  doi     = {10.1109/TVCG.2021.3070046}}
```


