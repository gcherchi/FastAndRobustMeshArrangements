# Fast and Robust Mesh Arrangements using Floating-point Arithmetic

<p align="center"><img src="teaser_img.png"></p>

This is a **HEADER-ONLY** version of the mesh arrangement algorithm described in

[**Fast and Robust Mesh Arrangements using Floating-point Arithmetic**](https://dl.acm.org/doi/abs/10.1145/3414685.3417818)<br />
[G. Cherchi](http://www.gianmarcocherchi.com),
[M. Livesu](http://pers.ge.imati.cnr.it/livesu/),
[R. Scateni](https://people.unica.it/riccardoscateni/),
[M. Attene](https://www.cnr.it/en/people/marco.attene)<br />
ACM Transactions on Graphics, 2020 (proceedings of SIGGRAPH Asia)<br />

and later improved in

[**Interactive and Robust Mesh Booleans**](https://www.gianmarcocherchi.com/pdf/interactive_exact_booleans.pdf)<br />
[G. Cherchi](http://www.gianmarcocherchi.com),
[F. Pellacini](https://pellacini.di.uniroma1.it),
[M. Attene](https://www.cnr.it/en/people/marco.attene),
[M. Livesu](http://pers.ge.imati.cnr.it/livesu/)<br />
ACM Transactions on Graphics, 2022 (proceedings of SIGGRAPH Asia)<br />

and in

[**Deterministic Linear Time Constrained Triangulation Using Simplified Earcut**](https://ieeexplore.ieee.org/abstract/document/9392369)<br />
[M. Livesu](http://pers.ge.imati.cnr.it/livesu/),
[G. Cherchi](http://www.gianmarcocherchi.com),
[R. Scateni](https://people.unica.it/riccardoscateni/),
[M. Attene](https://www.cnr.it/en/people/marco.attene)<br />
IEEE Transactions on Visualization and Computer Graphics, 2022

## Usage

|:warning: WARNING: This code is subject to continuous updates. |
| --- |

Clone this repository:
```
git clone https://github.com/gcherchi/FastAndRobustMeshArrangements.git
```
Once done, you may build the executable as follows (put ``Release`` or ``Debug`` instead of ``<build_type>``):
```
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=<build_type>
make
```

There are 4 versions of the ``solveIntersections(...)`` function, with different outputs. Please, check the file ``solve_intersections.h`` for more details.

We tested our code on MacOS (CLANG 14.0 64 bit), Linux (GCC 7.3.0 64 bit) and Windows (MSVC 2019 64 bit).

## Citing us
This project is based on algorithms described in the following scientific articles. If you use our code in your academic projects, please consider citing our articles using the following BibTeX entries:
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
  year    = {2022},
  volume  = {28},
  number  = {12},
  doi     = {10.1109/TVCG.2021.3070046}
}
```


