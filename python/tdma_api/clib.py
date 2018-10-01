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


from ctypes import CDLL, c_int, c_char_p, c_size_t, byref as REF, POINTER

_lib = None

ERRORS = {
    0 : 'NONE',
    1 : 'TDMA_API_ERROR',
    2 : 'TDMA_API_CRED_ERROR',
    3 : 'TDMA_API_VALUE_ERROR',
    4 : 'TDMA_API_TYPE_ERROR',
    5 : 'TDMA_API_MEMORY_ERROR',
    101 : 'TDMA_API_CONNECT_ERROR',
    102 : 'TDMA_API_AUTH_ERROR',
    103 : 'TDMA_API_REQUEST_ERROR',
    104 : 'TDMA_API_SERVER_ERROR',
    201 : 'TDMA_API_STREAM_ERROR',
    301 : 'TDMA_API_EXECUTE_ERROR',
    501 : 'TDMA_API_STD_EXCEPTION',
    1001 : 'TDMA_API_UNKNOWN_EXCEPTION'
    }

class CLibException(Exception):
    def __init__(self, error_code):
        self.error_code = error_code
        msg = get_last_error_msg()
        assert error_code == get_last_error_code()
        lineno = get_last_error_lineno()
        fname = get_last_error_filename()
        info = " [error code: " + str(error_code) + ", line: " \
             + str(lineno) + ", file: " + fname + ']'
        super().__init__(ERRORS.get(error_code) + ": " + msg + info)
        
class LibraryNotLoaded(Exception):
    pass

def PCHAR_BUFFER(strs):
    s = [c_char_p(s.encode()) for s in strs]   
    return (c_char_p * len(s))(*s) 

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

def free_fields_buffer(fields):
    global _lib 
    if _lib is None:
        raise LibraryNotLoaded()
    _lib.FreeFieldsBuffer_ABI(fields)       
        
def get_str(fname, obj=None):
    c = c_char_p()
    n = c_size_t()
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
    n = c_size_t()
    if obj:
        call(fname, REF(obj), REF(p), REF(n))
    else:
        call(fname, REF(p), REF(n))
    symbols = [p[i].decode() for i in range(n.value)]  
    free_buffers(p, n)   
    return symbols 


def set_strs(fname, strs, obj=None):
    if obj:
        call(fname, REF(obj), PCHAR_BUFFER(strs), len(strs))
    else:
        call(fname, PCHAR_BUFFER(strs), len(strs))
    
    
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
        
        
def get_vals(fname, ty, obj=None, free=None):    
    ptr = POINTER(ty)()
    n = c_size_t()
    if obj:
        call(fname, REF(obj), REF(ptr), REF(n))
    else:
        call(fname, REF(ptr), REF(n))
    vals = [ptr[i] for i in range(n.value)]
    if free:
        free(ptr)
    return vals
    
def to_str(fname, ty, v):
    c = c_char_p()
    n = c_size_t()
    call(fname, ty(v), REF(c), REF(n))
    s = c.value.decode() 
    free_buffer(c)
    return s
            
def get_last_error_msg():
    return get_str('LastErrorMsg_ABI')

def get_last_error_code():
    return get_val('LastErrorCode_ABI', c_int)

def get_last_error_lineno():
    return get_val('LastErrorLineNumber_ABI', c_int)

def get_last_error_filename():
    return get_str('LastErrorFilename_ABI')
