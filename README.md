# RBase

This project combines the once separated RComm and RRegistry into one package, also containing
a support library for connecting to a RMaster instance and files for SWIG-usage.

Additionally, the support library RSupport for Regulation Kernels is implemented in this repository.

All other parts of the codebase include this repository as a submodule, which reduces the overhead of
having the registry and the communication standards in different repositories. The support library is also
needed for the master, which uses its headers to receive initial configuration for pipe management. Additionally,
tests can be performed on this repository alone, which facilitates modularized unit testing on the CI server.

## Getting Started

Look into the tests and other programs using this software for examples. This software serves as interconnecting
piece between SWIG-modules, C++ code and other parts of the stack.

## Deployment

Please link the generated binaries with your program and use the provided API to store, exchange and
provide data to other parts of the codebase.

## Built With

* [CMake](https://cmake.org/) - Build Generator

## Contributing

## Authors

* **Max Heisinger** - *Initial work* - [maximaximal.com](https://maximaximal.com)
