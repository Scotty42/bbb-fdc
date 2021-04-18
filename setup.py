# Build 
# python3 setup.py build

import os
import sys
from setuptools import setup, Extension

from bbbfdc.version import __version__

wd37c65_direct_ext = Extension('bbbfdc.wd37c65_direct_ext',
                             sources = ['bbbfdc/wd37c65_direct_ext.c', 'bbbfdc/micros.c'],
                             libraries = [])

realtime_ext = Extension('bbbfdc.realtime_ext',
                             sources = ['bbbfdc/realtime_ext.c'],
                             libraries = [])                             

setup_result = setup(name='bbbfdc',
      version=__version__,
      description="Markus Friedrich's BBB-FDC Library. Inspired by Scott M Baker.",
      packages=['bbbfdc'],
      zip_safe=False,
      ext_modules=[wd37c65_direct_ext, realtime_ext]
     )