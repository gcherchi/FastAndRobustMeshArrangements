# Fast and Robust Mesh Arrangements using Floating-point Arithmetic

This is the code we used for the experiments in the paper "**Fast and Robust Mesh Arrangements using Floating-point Arithmetic**" by G. Cherchi, M. Livesu, R. Scateni and M. Attene (ACM TOG, SIGGRAPH Asia 2020). 

<p align="center"><img src="teaser_img.png"></p>

|:warning: WARNING: We are rewriting portions of the code to be more efficient. The code is therefore subject to continuous updates. We are also preparing an easy to use header-only version of our algorithm. It will be released as soon as possible.  |
| --- |

## Usage

To use the code you need to clone this repository, including submodules, with: 
```
git clone --recursive https://github.com/gcherchi/FastAndRobustMeshArrangements.git
```

At the moment, you also need to download and install the **TMesh_Kernel library** (https://github.com/MarcoAttene/TMesh_Kernel). Follow the instructions in the readme. The FAST MODE is sufficient.

The header-only **Cinolib library** is included as a submodule in the folder *external/Cinolib*. To run the pipeline correctly, you must define the symbol `CINOLIB_USES_EXACT_PREDICATES` at compilation time, and also add the file `Cinolib/external/predicates/shewchuk.c` in your project.

Also the **Indirect Predicates** used in the code are included as a submodule in the folder *external/IndirectPredicates*. You need to add the files `implicit_point.h/.cpp`, `numerics.h/.cpp` and `/predicates/indirect_predicates.h/.cpp` in your project.

In this repository, a Qt project is present: please, check flags and includes in the .pro file if you want to use a different tool.
You need to replace the `[TMesh_Kernel path]` with the path in your computer.
In the main.cpp file a single function `solveIntersections(...)` is called. It contains the entire pipeline.

We tested this code on MacOS and Linux (we are still testing on Windows).

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


