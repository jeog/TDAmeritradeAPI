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

#ifndef INCLUDE_BACKING_STORE_H_
#define INCLUDE_BACKING_STORE_H_

#include <set>
#include <string>
#include <fstream>
#include <memory>
#include <map>
#include <functional>


class BackingStore {
public:
    typedef std::function<std::pair<long long, long long>(std::fstream&)>
        fileio_func_ty;


    BackingStore( const std::string& directory_path );

    bool
    is_valid() const;

    std::set<std::string>
    get_symbols() const;

    bool
    add_symbol_store( const std::string& symbol );

    // {success, front elems pushed, back elems pushed}
    std::tuple<bool, unsigned long long, unsigned long long>
    read_from_symbol_store( const std::string& symbol,
                            fileio_func_ty read_func_front,
                            fileio_func_ty read_func_back );

    // {success, front elems pulled, back elems pulled}
    std::tuple<bool, unsigned long long, unsigned long long>
    write_to_symbol_store( const std::string& symbol,
                           fileio_func_ty write_func_front,
                           fileio_func_ty write_func_back );

    bool
    remove_symbol_store( const std::string& symbol );

    bool
    delete_symbol_store( const std::string& symbol );

    static bool
    directory_exists( const std::string& dir_path );

private:
    struct SymbolStore {
        struct Side{
            std::unique_ptr<std::fstream> file;
            std::string path;
        };

        Side front; // append newer
        Side back; // append older
    };

    std::string _directory_path;
    std::string _index_path;
    std::fstream _indx;
    std::map<std::string, SymbolStore>  _stores;

    std::set<std::string>
    _read_index();

    bool
    _write_index( const std::string& symbol );

    bool
    _add_store( const std::string& symbol );

    std::tuple<bool, long long, long long>
    _read_store( SymbolStore::Side& side, fileio_func_ty read_func );

    std::tuple<bool, long long, long long>
    _write_store( SymbolStore::Side& side, fileio_func_ty write_func );
};

#endif /* INCLUDE_BACKING_STORE_H_ */
