# -*- coding: utf-8 -*-
# distutils: language = c++
from __future__ import unicode_literals
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.pair cimport pair
cimport esary

cdef class Kwic:
     cdef esary.ESary* _esary

     def __init__(self):
         self._esary = new esary.ESary()

     def add_line(self, uline):
          cdef bytes line = uline.encode('utf8')
          self._esary.addLine(line)

     def build(self):
         return self._esary.build_sa()

     def save(self,fname):
         return self._esary.save(fname)

     def load(self,fname):
         return self._esary.load(fname,0)

     cdef _result_to_unicode(self,vector[pair[string,string]] &result):
         cdef int num = result.size()
         ret = []
         cdef int i
         for i in xrange(num):
             ret.append((result[i].first.c_str().decode('utf8'),result[i].second.c_str().decode('utf8')))
         return ret

     def search(self,unicode ustring):
         cdef bytes st = ustring.encode('utf8')
         cdef vector[unsigned int] indexes
         self._esary.search(st,indexes)
         cdef vector[pair[string,string]] result
         self._esary.getResult(indexes,result)
         return self._result_to_unicode(result)


cdef class EKwic(Kwic):
     def build(self):
         return self._esary.build_esa()

     def load(self,fname):
         return self._esary.load(fname,1)

     def maxmal_substring(self):
         cdef int nodeNum = self._esary.getNodeNum()
         cdef unsigned int pos = 0
         cdef pair[string,unsigned int] maxmalsubst
         while pos < nodeNum:
             maxmalsubst = self._esary.findMaximalSubstring(pos)
             if maxmalsubst.first.size()!=0:
                 yield maxmalsubst.first.c_str().decode('utf8'),maxmalsubst.second
