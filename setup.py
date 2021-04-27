# Build 
# python3 setup.py build

# import os
# import sys
# from setuptools import setup, Extension
from distutils.core import setup, Extension

from bbbfdc.version import __version__

wd37c65_direct_ext = Extension('bbbfdc.wd37c65_direct_ext',
                             sources = ['bbbfdc/wd37c65_direct_ext.cpp', 'bbbfdc/wiringPi.cpp'],
                             library_dirs = ['/usr/local/lib'],
                             libraries = ['iobb'])

#realtime_ext = Extension('bbbfdc.realtime_ext',
#                             sources = ['bbbfdc/realtime_ext.cpp'],
#                             libraries = [])                             

setup_result = setup(
      name='bbbfdc',
      version=__version__,
      description="Markus Friedrich's BBB-FDC Library. Inspired by Scott M Baker.",
      author="Dr. MArkus Friedrich",
      packages=['bbbfdc'],
      ext_modules=[wd37c65_direct_ext]
     )
