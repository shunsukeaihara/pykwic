# -*- coding: utf-8 -*-
import distribute_setup
distribute_setup.use_setuptools()

from setuptools import setup
from Cython.Build import cythonize
from setuptools.extension import Extension
import multiprocessing
import sys
sys.path.append('./src')


setup(
    name="pykwic",
    description="KWIC Library for unicode text using Suffix Array.",
    version="0.1",
    long_description = open('README.rst').read(),
    ext_modules = cythonize([
        Extension(
            'pykwic',
            ["src/pykwic.pyx","lib/esary.cpp"],
            include_dirs=['lib','/opt/local/include','/usr/local/include'],
            library_dirs=['/opt/local/lib','/usr/local/lib'],
            libraries =['pthread','m','icui18n','icuuc','icudata'],
            extra_compile_args = ['-g','-O0']
            ),
        ],language="c++",
                            ),
    test_suite='nose.collector',
    tests_require=['Nose','cython'],
    )
