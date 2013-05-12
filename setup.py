# -*- coding: utf-8 -*-
from distutils.core import setup
from Cython.Build import cythonize

setup(ext_modules = cythonize(
           "esary.pyx",                 # Cython ソース
           sources=["esary.cpp"],  # その他のソースファイル
           language="c++",             # C++ コードを生成させる
      ))
