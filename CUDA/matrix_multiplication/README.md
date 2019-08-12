Primary goal is to learn CUDA programming. A matrix multiplication is an example. Once we understand CUDA programming itself, the next goal is to get the same performance as in CUBLAS. Here are references.

- [Ref1] CUDA C Programming Guide; v10.1, May 2019
- [Ref2] [Matrix multiplication in CUDA](http://www.es.ele.tue.nl/~mwijtvliet/5KK73/?page=mmcuda); matrix multiplication comparision; at the very end "Example written by Zhenyu Ye (z.ye@tue.nl)"
- [Ref3] A series of CUDA programming by Mark Harris
    - [An Easy Introduction to CUDA C and C++](https://devblogs.nvidia.com/easy-introduction-cuda-c-and-c/)
        - [An Even Easier Introduction to CUDA](https://devblogs.nvidia.com/even-easier-introduction-cuda/)
    - [How to Implement Performance Metrics in CUDA C/C++](https://devblogs.nvidia.com/how-implement-performance-metrics-cuda-cc/)
    - [How to Query Device Properties and Handle Errors in CUDA C/C++](https://devblogs.nvidia.com/how-query-device-properties-and-handle-errors-cuda-cc/)
    - [How to Optimize Data Transfers in CUDA C/C++](https://devblogs.nvidia.com/how-optimize-data-transfers-cuda-cc/)
    - [How to Overlap Data Transfers in CUDA C/C++](https://devblogs.nvidia.com/how-overlap-data-transfers-cuda-cc/)
    - [How to Access Global Memory Efficiently in CUDA C/C++ Kernels](https://devblogs.nvidia.com/how-access-global-memory-efficiently-cuda-c-kernels/)
    - [Using Shared Memory in CUDA C/C++](https://devblogs.nvidia.com/using-shared-memory-cuda-cc/)
    - [An Efficient Matrix Transpose in CUDA C/C++](https://devblogs.nvidia.com/efficient-matrix-transpose-cuda-cc/)
    - [Finite Difference Methods in CUDA C/C++, Part 1](https://devblogs.nvidia.com/finite-difference-methods-cuda-cc-part-1/)
    - [Finite Difference Methods in CUDA C++, Part 2
](https://devblogs.nvidia.com/finite-difference-methods-cuda-c-part-2/)
- [Ref4] [matrixMulCUBLAS - Matrix Multiplication (CUBLAS)](https://docs.nvidia.com/cuda/cuda-samples/index.html#matrix-multiplication--cublas-); after installation of CUDA Toolkit, we can find it under ```samples/0_Simple/matrixMulCUBLAS/```
- [Ref5] [Benchmarking GPUs to Tune Dense Linear Algebra](https://dl.acm.org/citation.cfm?id=1413402) by Vasily Volkov and James W. Demmel
- [Ref6] [SGEMM](https://github.com/NervanaSystems/maxas/wiki/SGEMM); it would be helpful for optimization if read along with [Ref5]

In matrix multiplication example, we can skip
* [How to Overlap Data Transfers in CUDA C/C++](https://devblogs.nvidia.com/how-overlap-data-transfers-cuda-cc/) because it is related to multiple computes.
* [Finite Difference Methods in CUDA C/C++, Part 1](https://devblogs.nvidia.com/finite-difference-methods-cuda-cc-part-1/) and [Finite Difference Methods in CUDA C++, Part 2
](https://devblogs.nvidia.com/finite-difference-methods-cuda-c-part-2/) because it is another application, which should be studied later.

# Learn CUDA by matrix multiplication
## Comments on [Ref2]
It is not clear what are differences in "Tiling", "Memory coalescing" and "Avoiding memory bank conflicts". Comments are added in seperate files("Comments on Matrix Multiplication in CUDA.docx", "Comments on Matrix Multiplication in CUDA.pdf") with another called _Way 4_.

"Computation Optimization" should be read along with, for example, [Ref5].

## Performance on [Ref2]
All tests in [Ref2] are done on "GeForce GTX 960M" with compute capability 5.0, but the results are not consistent with ones in [Ref2].
### Steps
[cuda.zip] from [Ref2] are downloaded and modified as we proceed to the tests. They are copied under `cuda`. For details, use `git history`.
### Compared to CUBLAS
CUBLAS run faster than [Ref2], "Computation Optimization" by 3 times! Measure is done by `nvprof` only for Compute (kernels).
