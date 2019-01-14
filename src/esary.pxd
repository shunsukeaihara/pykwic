# distutils: language = c++
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.pair cimport pair

ctypedef vector[int]* viptr

cdef extern from "../lib/esary.hpp" namespace "esary" nogil:
     cdef cppclass ESary:
          ESary()
          void addLine(vector[int] & line) except +
          int build_sa() except +
          int build_esa() except +
          void search(vector[int] & query, vector[unsigned int]& indexes) except +
          void getResults(vector[unsigned int] &indexes, vector[pair[viptr, viptr]] &result) except +
          void freeResults(vector[pair[viptr, viptr]] &result) except +
          void freeIntVec(vector[int]* vec) except +
          int load(char *fileName,int flag) except +
          int save(char *fileName) except +
          int getNodeNum()
          pair[viptr, unsigned int] findMaximalSubstring(unsigned int &pos)
