# distutils: language = c++
from __future__ import unicode_literals
from libcpp.string cimport string
from libcpp.vector cimport vector
cimport esary

cdef class ESary:
     cdef esary.ESary* _esary

     def __init__(self):
         self._esary = new esary.ESary()
         print 'aa'

     def add_line(self, uline):
          cdef bytes line = uline.encode('utf8')
          self._esary.addLine(line)

     def build(self):
         return self._esary.build()

     def save(self,fname):
         return self._esary.save(fname)

     def load(self,fname):
         return self._esary.load(fname)