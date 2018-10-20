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

/* HEADER COMMON TO ALL OTHER HEADERS/SOURCE */

#ifdef _WIN32
#ifdef THIS_EXPORTS_INTERFACE
#define DLL_SPEC_ __declspec(dllexport)
#else 
#define DLL_SPEC_ __declspec(dllimport)
#endif /* THIS_EXPORTS_INTERFACE */
#ifndef _WIN64
#define ZLIB_WINAPI
#endif /* _WIN64 */
#else
#define DLL_SPEC_
#define USE_SIGNAL_BLOCKER_
#endif /* _WIN32 */

#ifdef DEBUG
#undef NDEBUG
#endif /* DEBUG */

#ifndef NDEBUG
#define DEBUG_VERBOSE_1_
#endif /* NDEBUG */

#ifdef ALLOW_EXCEPTIONS_ACROSS_ABI
#define ALLOW_EXCEPTIONS 1
#else
#define ALLOW_EXCEPTIONS 0
#endif /* ALLOW_EXCEPTIONS_ACROSS_ABI */

#include <assert.h>

#ifdef __cplusplus
#include <string>

// TODO move from global
extern std::string certificate_bundle_path;

#define EXTERN_C_SPEC_ extern "C"
#else
#include <stddef.h>
#include <stdbool.h>
#define EXTERN_C_SPEC_
#endif /* __cplusplus */

#endif /* COMMON_H_ */
