# distutils: language = c++
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.pair cimport pair

cdef extern from "../lib/esary.hpp" namespace "esary" nogil:
     cdef cppclass ESary:
          ESary()
          void addLine(char *line) except +
          int build() except +
          void search(char *query, vector[unsigned int]& indexes) except +
          void getResult(vector[unsigned int] &indexes, vector[pair[string,string]] &result) except +
          void getResultSuffix(vector[unsigned int] &indexes, vector[string] &result) except +
          int load(char *fileName) except +
          int save(char *fileName) except +
