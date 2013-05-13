# distutils: language = c++
from __future__ import unicode_literals
cimport esary

cdef class PyESary:

     cdef esary.ESary* _esary

     def __init__(self):
         self._esary = new esary.ESary()