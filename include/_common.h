/*
Copyright (C) 2018 Jonathon Ogden <jeog.dev@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses.
*/

#ifndef COMMON_H_
#define COMMON_H_

#ifdef _WIN32
#ifdef THIS_EXPORTS_INTERFACE
#define DLL_SPEC_ __declspec(dllexport)
#else 
#define DLL_SPEC_ __declspec(dllimport)
#endif /* THIS_EXPORTS_INTERFACE */

#ifndef _WIN64
#define ZLIB_WINAPI
#endif /* _WIN64 */

#endif  /* _WIN32 */

#include <string>

extern std::string certificate_bundle_path;

#endif /* COMMON_H_ */