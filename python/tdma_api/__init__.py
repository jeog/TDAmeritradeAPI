# 
# Copyright (C) 2018 Jonathon Ogden <jeog.dev@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see http://www.gnu.org/licenses.
# 

from tdma_api.clib import init, LibraryNotLoaded
from platform import system

TDMA_API_LIB_BASE = 'TDAmeritradeAPI'
TDMA_API_LIB_EXT = '.so'
TDMA_API_LIB_PRE = 'lib'
if system() == 'Windows':
    TDMA_API_LIB_EXT = '.dll'
    TDMA_API_LIB_PRE = ''
    
TDMA_API_LIB_NAME = TDMA_API_LIB_PRE + TDMA_API_LIB_BASE + TDMA_API_LIB_EXT

try:
    if init(TDMA_API_LIB_NAME):
        print("+ Successfully loaded", TDMA_API_LIB_NAME)
    else:
        raise LibraryNotLoaded()
except BaseException as e:
        print("- Failed to load", TDMA_API_LIB_NAME)
        print("-", e.__class__.__name__ + ':', str(e))
        print("- Use tdma_api.clib.init(path) to load manually")    
        