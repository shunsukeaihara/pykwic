# -*- coding: utf-8 -*-
from setuptools import setup
from Cython.Build import cythonize
from setuptools.extension import Extension
import multiprocessing
import sys
sys.path.append('./src')


setup(
    name="pykwic",
    description="KWIC Library for unicode text using Suffix Array.",
    version="0.03",
    long_description = open('README.rst').read(),
    ext_modules = cythonize([
        Extension(
            'pykwic',
            ["src/pykwic.pyx","lib/esary.cpp"],
            include_dirs=['lib'],
            ),
        ],language="c++",
                            ),
    author = 'Shunsuke Aihara',
    url = 'https://bitbucket.org/aihara/pykwic/',
    license = "MIT License",
    include_package_data=True,
    test_suite='nose.collector',
    tests_require=['Nose','cython'],
    )
