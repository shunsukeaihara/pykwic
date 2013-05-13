# -*- coding: utf-8 -*-
from distutils.core import setup
from Cython.Build import cythonize
from distutils.extension import Extension
import sys
sys.path.append('./src')

setup(
    name="pyesary",
    description="Suffix Array Library for unicode text",
    version="0.1",
    long_description = open('README.rst').read(),
    ext_modules = cythonize([
        Extension(
            'pyesary',
            ["src/pyesary.pyx","src/esary.cpp","lib/esary.cpp"],
            include_dirs=['lib','/opt/local/include','/usr/local/include'],
            library_dirs=['/opt/local/lib','/usr/local/lib'],
            libraries =['pthread','m','icui18n','icuuc','icudata'],
            ),
        ],language="c++",
        )
    )
