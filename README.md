**CODE IS COMING...**

This is the code we used for the experiments in the paper "**Fast and Robust Mesh Arrangements using Floating-point Arithmetic**" by G. Cherchi, M. Livesu, R. Scateni and M. Attene (SIGGRAPH ASIA 2020). 

WARNING: We are rewriting portions of the code to be more efficient. We are also preparing an easy to use header-only version of our algorithm. It will be released as soon as possible.

At the moment, to run our code, you need to download and install the **TMesh_Kernel library** (https://github.com/MarcoAttene/TMesh_Kernel). Follow the instructions in the readme. The FAST MODE is sufficient.

The header-only **Cinolib library** is included as a submodule. To run the pipeline correctly, you must define the symbol `CINOLIB_USES_EXACT_PREDICATES` at compilation time, and also add the file *<CINOLIB_HOME>/external/predicates/shewchuk.c* in your project.

Also the **Indirect Predicates** used in the code are included as a submodule.

In this repository, a Qt project is present: please, check flags and includes in the .pro file if you want to use a different tool.
You need to replace the `[TMesh_Kernel path]` with the path in your computer.
In the main.cpp file a single function `solveIntersections(...)` is called. It contains the entire pipeline.

We tested this code on MacOS and Linux (we are still testing on Windows).



