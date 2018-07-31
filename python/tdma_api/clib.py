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


from ctypes import CDLL, c_int, c_char_p, byref as REF, POINTER

_lib = None

ERRORS = {
    1 : 'TDMA_API_ERROR',
    2 : 'TDMA_API_CRED_ERROR',
    3 : 'TDMA_API_VALUE_ERROR',
    4 : 'TDMA_API_TYPE_ERROR',
    5 : 'TDMA_API_MEMORY_ERROR',
    101 : 'TDMA_API_EXEC_ERROR',
    102 : 'TDMA_API_AUTH_ERROR',
    103 : 'TDMA_API_REQUEST_ERROR',
    104 : 'TDMA_API_SERVER_ERROR',
    201 : 'TDMA_API_STREAM_ERROR'
    }

class CLibException(Exception):
    def __init__(self, error_code):
        self.error_code = error_code
        msg = get_last_error_msg()
        assert error_code == get_last_error_code()
        super().__init__(ERRORS.get(error_code) + ": " + msg)
        
class LibraryNotLoaded(Exception):
    pass

def PCHAR_BUFFER(strs):
    s = [c_char_p(s.encode()) for s in strs]
    s.append( c_char_p() )
    return (c_char_p * (len(s)+1))(*s) 

PCHAR = lambda s: c_char_p(s.encode())

def init(lib, reload=False):
    global _lib
    if _lib is None or reload:
        _lib = CDLL(lib)           
    return bool(_lib)

def call(name, *args):
    global _lib
    if _lib is None:
        raise LibraryNotLoaded()    
    func = getattr(_lib, name)
    err = func(*(args + (c_int(0),)))
    if err:
        raise CLibException(err)
    
    
def free_buffer(buf):
    global _lib
    if _lib is None:
        raise LibraryNotLoaded()
    _lib.FreeBuffer_ABI(buf, 0)
    
    
def free_buffers(bufs, n):
    global _lib 
    if _lib is None:
        raise LibraryNotLoaded()
    _lib.FreeBuffers_ABI(bufs, n, 0)
        
        
def get_str(fname, obj=None):
    c = c_char_p()
    n = c_int()
    if obj:
        call(fname, REF(obj), REF(c), REF(n))
    else:
        call(fname, REF(c), REF(n))
    s = c.value.decode() 
    free_buffer(c)
    return s


def set_str(fname, s, obj=None):
    if obj:
        call(fname, REF(obj), PCHAR(s))
    else:
        call(fname, PCHAR(s))
    
    
def get_strs(fname, obj=None):
    p = POINTER(c_char_p)()
    n = c_int()
    if obj:
        call(fname, REF(obj), REF(p), REF(n))
    else:
        call(fname, REF(p), REF(n))
    symbols = [p[i].decode() for i in range(n.value-1)]  
    free_buffers(p, n)   
    return symbols 


def set_strs(fname, strs, obj=None):
    if obj:
        call(fname, REF(obj), PCHAR_BUFFER(strs))
    else:
        call(fname, PCHAR_BUFFER(strs))
    
    
def get_val(fname, ty, obj=None):
    w = ty()
    if obj:
        call(fname, REF(obj), REF(w))        
    else:
        call(fname, REF(w))
    return w.value


def set_val(fname, ty, val, obj=None):
    if obj:
        call(fname, REF(obj), ty(val))
    else:
        call(fname, ty(val))
        
def get_last_error_msg():
    return get_str('LastErrorMsg_ABI')

def get_last_error_code():
    return get_val('LastErrorCode_ABI', c_int)
