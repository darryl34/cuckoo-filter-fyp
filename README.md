# Efficient D-ary Cuckoo Filter using Higher Order Involution Functions

## Overview

This repository contains the code created and used for my MEng Computer Science Final Year Individual Project (COMP0138) at UCL.

This project implements the efficient d-ary cuckoo filter, and extends the works of Fan et al.(2014) and Xie et al. (2017).

## Repository Structure

- `src/`: Contains the C++ header files for the filter implementation
- `tests/`: Contains tests for various scenarios, `results/` contain results of the tests
- `plots/`: Python scripts for generating plots of results
- `unit-tests/`: Unit tests for the project
- `python-prototype/`: Initial prototype developed in Python (not meant to represent the final implementation)

## Build

To build the example `example/test.cpp`:
```
$ g++ test.cpp -o test.exe
$ test
```
The unit tests are built with Google Test. 

To build the tests:
```
$ cd unit-tests
$ cmake -S . -B build
$ cmake --build build
$ cd build && test
```


## References
- [Fan, Bin, Dave G. Andersen, Michael Kaminsky, and Michael D. Mitzenmacher. "Cuckoo filter: Practically better than bloom." In Proceedings of the 10th ACM International on Conference on emerging Networking Experiments and Technologies, pp. 75-88. 2014.](https://dl.acm.org/doi/pdf/10.1145/2674005.2674994)
- [Xie, Zhuohan, Wencheng Ding, Hongya Wang, Yingyuan Xiao, and Zhenyu Liu. "D-Ary Cuckoo Filter: A space efficient data structure for set membership lookup." In 2017 IEEE 23rd International Conference on Parallel and Distributed Systems (ICPADS), pp. 190-197. IEEE, 2017.](https://ieeexplore.ieee.org/abstract/document/8368364/?casa_token=kKW9LWWsVKIAAAAA:a3-OtOjQ_VfEsa7oLbDbRlSkxeIuPt5KqcAoEJDMQXJnnz-MYbdSeUujjdrIpI9payLtGnS7QQ)