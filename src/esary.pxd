# distutils: language = c++
from libcpp.string cimport string
from libcpp.vector cimport vector

cdef extern from "../lib/esary.hpp" namespace "esary" nogil:
     cdef cppclass ESary:
          ESary()
          void addLine(char *line) except +
          int build() except +
          void search(char *query, vector[int]& indexes) except +
          void getResult(vector[int] &indexes, vector[string] &result) except +
          void getResultSuffix(vector[int] &indexes, vector[string] &result) except +
          int load(char *fileName) except +
          int save(char *fileName) except +
