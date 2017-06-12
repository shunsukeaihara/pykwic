# -*- coding: utf-8 -*-
from setuptools import setup
from Cython.Build import cythonize
from setuptools.extension import Extension
import sys
sys.path.append('./src')

setup(
    name="pykwic",
    description="KWIC Library for unicode text using Suffix Array.",
    version="0.1.1",
    long_description=open('README.rst').read(),
    ext_modules=cythonize([
        Extension(
            'pykwic',
            ["src/pykwic.pyx", "lib/esary.cpp"],
            include_dirs=['lib']),
    ],
          language="c++"),
    classifiers=["Programming Language :: Python :: 3", "Programming Language :: Python :: 2"],
    author='Shunsuke Aihara',
    url='https://github.com/shunsukeaihara/pykwic',
    license="MIT License",
    include_package_data=True,
    test_suite='nose.collector',
    tests_require=['Nose', 'Cython', 'six', 'codepoints'],
)
