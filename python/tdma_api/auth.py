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

from ctypes import Structure as _Structure, c_char_p, c_longlong, \
                    byref as _REF

from . import clib
from .clib import PCHAR

class Credentials(_Structure):
    _fields_ = [
        ("access_token", c_char_p),
        ("refresh_token", c_char_p),
        ("epoch_sec_token_expiration", c_longlong),
        ("client_id", c_char_p)
        ]

    
def load_credentials(path, password):
    creds = Credentials()
    clib.call('LoadCredentials_ABI', PCHAR(path), PCHAR(password), 
              _REF(creds))
    return creds
    

def store_credentials(path, password, creds):
    clib.call('StoreCredentials_ABI', PCHAR(path), PCHAR(password),  
              _REF(creds))


def request_access_token(code, client_id, redirect_uri="https://127.0.0.1"):
    creds = Credentials()
    clib.call('RequestAccessToken_ABI', PCHAR(code), PCHAR(client_id), 
              PCHAR(redirect_uri), _REF(creds))
    return creds


def refresh_access_toke(creds):
    clib.call('RefreshAccessToken_ABI', _REF(creds))


def set_certificate_bundle_path(path):
    clib.set_str('SetCertificateBundlePath_ABI', path)


def get_certificate_bundle_path():
    return clib.get_str('GetCertificateBundlePath_ABI')


class CredentialsManager:    
    def __init__(self, path, password, verbose=False):
        self._verbose = verbose
        self.path = path
        self.password = password
        self.credentials = None
        
    def __enter__(self):
        if self._verbose:
            print("+ Load credentials from ", self.path)
        try:           
            self.credentials = load_credentials(self.path, self.password)
            if self._verbose:
                print("+ Successfully loaded credentials")
            return self
        except:
            if self._verbose:
                print("- Failed to load credentials")             
            raise                
      
    def __exit__(self, _1, _2, _3):        
        if self._verbose:
            print("+ Store credentials to ", self.path)
        if not self.credentials:
            if self._verbose:
                print("- No credentials to store")
            return
        try:           
            store_credentials(self.path, self.password, self.credentials)
            if self._verbose:
                print("+ Successfully stored credentials")
        except BaseException as e:
            if self._verbose:
                print("- Failed to store credentials")
                print("-", e.__class__.__name__ + ':', str(e))