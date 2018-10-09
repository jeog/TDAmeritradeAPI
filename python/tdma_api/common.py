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

""" tdma_api/common.py - functions/objects used across interfaces """

from ctypes import c_uint, c_double
from . import clib

def build_option_symbol(underlying, month, day, year, is_call, strike):
    """Returns standard option symbol string."""
    c = clib.c_char_p()
    n = clib.c_size_t()
    clib.call("BuildOptionSymbol_ABI", clib.PCHAR(underlying), 
              c_uint(month), c_uint(day), c_uint(year), 
              clib.c_int(1 if is_call else 0), c_double(strike), 
              clib.REF(c), clib.REF(n)) 
    s = c.value.decode() 
    clib.free_buffer(c)
    return s

def check_option_symbol(symbol):
    """Check if standard option symbol is of valid format.
    
    THROWS -> LibraryNotLoaded
           -> CLibException (if invalid, provides an explanation of error)
    
    (Note, this only checks the *format* not if the option actually exists.
    """
    clib.call("CheckOptionSymbol_ABI", clib.PCHAR(symbol))
