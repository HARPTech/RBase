%include "../shared.i"
%include "numpy.i"

%numpy_typemaps(unsigned char, NPY_UNSIGNED_CHAR, int)
%numpy_typemaps(signed char, NPY_SIGNED_CHAR, int)
%numpy_typemaps(unsigned short, NPY_UNSIGNED_SHORT, int)
%numpy_typemaps(short, NPY_SHORT, int)
%numpy_typemaps(int, NPY_INT, int)
%numpy_typemaps(unsigned int, NPY_UNSIGNED_INT, int)
%numpy_typemaps(long, NPY_LONG, int)
%numpy_typemaps(unsigned long, NPY_UNSIGNED_LONG, int)
%numpy_typemaps(long long, NPY_LONG_LONG, int)
%numpy_typemaps(unsigned long long, NPY_UNSIGNED_LONG_LONG, int)
%numpy_typemaps(float, NPY_FLOAT, int)
%numpy_typemaps(double, NPY_DOUBLE, int)
%numpy_typemaps(bool, NPY_UINT, int)
