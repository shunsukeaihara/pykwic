# distutils: language = c++
from __future__ import unicode_literals
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.pair cimport pair
import codepoints
cimport esary

ctypedef vector[int]* viptr

cdef class Kwic:
     cdef esary.ESary* _esary

     def __init__(self):
         self._esary = new esary.ESary()

     def add_line(self, uline):
         cdef vector[int] codes = codepoints.from_unicode(uline)
         self._esary.addLine(codes)

     def build(self):
         return self._esary.build_sa()

     def save(self,fname):
         return self._esary.save(fname)

     def load(self,fname):
         return self._esary.load(fname,0)

     cdef _result_to_unicode(self,vector[pair[viptr, viptr]] &result):
         cdef int num = result.size()
         ret = []
         cdef int i
         for i in xrange(num):
             ret.append((
                 codepoints.to_unicode(
                     <vector[int]>(result[i].first[0])),
                 codepoints.to_unicode(
                     <vector[int]>(result[i].second[0]))))
         return ret

     def search(self,unicode ustring):
         cdef vector[int] codes = codepoints.from_unicode(ustring)
         cdef vector[unsigned int] indexes
         self._esary.search(codes, indexes)
         cdef vector[pair[viptr, viptr]] results
         self._esary.getResults(indexes, results)
         ret = self._result_to_unicode(results)
         self._esary.freeResults(results)
         return ret

cdef class EKwic(Kwic):
     def build(self):
         return self._esary.build_esa()

     def load(self,fname):
         return self._esary.load(fname,1)

     def maxmal_substring(self):
         cdef int nodeNum = self._esary.getNodeNum()
         cdef unsigned int pos = 0
         cdef pair[viptr, unsigned int] maxmalsubst
         while pos < nodeNum:
             maxmalsubst = self._esary.findMaximalSubstring(pos)
             if maxmalsubst.first.size()!=0:
                 s = codepoints.to_unicode(maxmalsubst.first[0])
                 yield s, maxmalsubst.second
             self._esary.freeIntVec(maxmalsubst.first)
