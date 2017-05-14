Matrix.h
========

[![Build Status](https://api.travis-ci.org/SirEelBiscuits/matrix.svg?branch=master)](https://travis-ci.org/SirEelBiscuits/matrix)

The purpose of matrix is to provide a generic matrix, allowing for any data
type for individual elements, so long as the requisite operators are defined
on those types.

Notably, types which may be multiplied resulting in a different type work
with Matrix.

Author
------

J Thatcher - @SirEelBiscuits - jameson@bluescreenofdoom.com

Prerequisites
-------------

- A C++14 compatible compiler

Usage
-----

To use the matrix, `#include "matrix.h"`, and create matrices as needed:

```cpp
using SuperGeneric::Matrix;
auto m = Matrix<float, 2, 2> { 1, 3, 5, 2 };
```

individual elements may be read and written via `operator()` like so:

```cpp
auto x = 0u, y = 1u;
m(x, y) = 3.5f;
```

Addition, multiplication, and scalar operations are defined where valid,
invalid operations, including multiplying matrices of inapproporiate
dimensions, will produce compile errors.

There are functions to get the identity and zero matrices
defined statically on square matrix types:

```cpp
SuperGeneric::Matrix<float, 2, 2>::Identity();
SuperGeneric::Matrix<float, 3, 3>::Zero();
```

One can also retrieve the inverse of 1x1 and 2x2 matrices

```cpp
m.Inverse()
//or
Invert(m);
```

Building Tests
--------------

The included tests can be built with the included makefile. `make all test`
will build and run the tests.

Future Work
-----------

- Make the matrix type mutable
- Add inverse functions for order 3 and greater matrices
