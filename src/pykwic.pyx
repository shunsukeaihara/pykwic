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
         return self._esary.build()

     def save(self,fname):
         return self._esary.save(fname)

     def load(self,fname):
         return self._esary.load(fname)

     def search(self,ustring):
         cdef bytes st = ustring.encode('utf8')
         cdef vector[unsigned int] indexes
         self._esary.search(st,indexes)
         cdef vector[pair[string,string]] result
         self._esary.getResult(indexes,result)
         cdef int num = result.size()
         ret = []
         for i in xrange(num):#遅いのでcdefに変える
             ret.append((result[i].first.c_str().decode('utf8'),result[i].second.c_str().decode('utf8')))
         return ret