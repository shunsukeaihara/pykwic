# -*- coding: utf-8 -*-
from setuptools import setup, Extension
import sys

try:
    from Cython.Build import cythonize
    ext_modules = cythonize(Extension(
        'pykwic',
        sources=["src/pykwic.pyx", "lib/esary.cpp"],
        include_dirs=['lib'],
        extra_compile_args=["-O3"]),
    )
except ImportError:
    ext_modules = [Extension(
        'pykwic',
        sources=["src/pykwic.cpp", "lib/esary.cpp"],
        include_dirs=['lib'],
        extra_compile_args=["-O3"])
    ]

sys.path.append('./src')

setup(
    name="pykwic",
    description="KWIC Library for unicode text using Suffix Array.",
    version="0.1.5",
    long_description=open('README.rst').read(),
    ext_modules=ext_modules,
    classifiers=["Programming Language :: Python :: 3", "Programming Language :: Python :: 2"],
    author='Shunsuke Aihara',
    url='https://github.com/shunsukeaihara/pykwic',
    license="MIT License",
    include_package_data=True,
    install_requires=["codepoints"],
    test_suite='nose.collector',
    tests_require=['nose', 'Cython', 'six'],
)
