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

from distutils.core import setup
import sys, time, platform, json

NAME = 'tdma_api'
VERSION = '0.1'
DESCRIPTION = 'Python interface to TDAmeritradeAPI'
AUTHOR = 'Jonathon Ogden'
AUTHOR_EMAIL = 'jeog.dev@gmail.com'
PACKAGES = ['tdma_api']

BUILD_INFO_FILE = 'tdma_api_build.info'

if sys.version_info.major < 3:
    sys.stderr.write("fatal: tdma_api requires python3\n")
    exit(1)

if __name__ == '__main__':
    setup(name=NAME, version=VERSION, description=DESCRIPTION, author=AUTHOR,
          author_email=AUTHOR_EMAIL, packages=PACKAGES)
    try:
        print('+ Write build info to:', BUILD_INFO_FILE)
        with open(BUILD_INFO_FILE, 'w') as f:
            d = { 'NAME': NAME, 
                  'VERSION_PACKAGE': VERSION,                  
                  'VERSION_PY_MAJOR': sys.version_info.major,
                  'VERSION_PY_MINOR': sys.version_info.minor,
                  'VERSION_PY_MICRO': sys.version_info.micro,
                  'SYSTEM': platform.system(),
                  'ARCHITECTURE': platform.architecture(),
                  'DATETIME ': time.strftime("%m/%d/%Y %H:%M:%S") }            
            json.dump(d, f)
    except BaseException as e:
        print('- Failed to write build info file:', str(e))
        
