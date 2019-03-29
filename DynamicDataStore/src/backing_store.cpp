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


#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <cstdio>

#include "common.h"
#include "backing_store.h"

using FFLAG = std::ios_base;
using std::string;

bool
BackingStore::directory_exists( const string& dir_path )
{
    struct stat info;

    int err = stat(dir_path.c_str(), &info);
    if( info.st_mode & S_IFDIR ){
        return true;
    }else if ( err ){
        std::cerr<< "invalid directory path: " << dir_path << std::endl;
    }else{
        std::cerr<< "'" << dir_path << "' is not a directory" << std::endl;
    }

    return false;
}


BackingStore::BackingStore( const string& directory_path )
    :
        _directory_path( directory_path ),
        _index_path( directory_path + "main.dsindex" ),
        _indx( _index_path )
    {
        if( !_indx ){
            log_info("FILE", "no index file, try to create new", _index_path);
            _indx.open( _index_path, FFLAG::in | FFLAG::out | FFLAG::app );
            if( !_indx ){
                 log_error("FILE", "failed to open file for append");
                 return;
             }
        }
        log_info( "FILE", "successfully opened", _index_path);

        for( auto& s : _read_index() ){
            if( !_add_store(s) ) {
                log_error("BACKING-STORE", "failed to add symbol store", s);
                _indx.close();
                return;
            }
        }
    }


bool
BackingStore::is_valid() const
{
    return !(!_indx); // GCC no operator bool()
}


std::set<string>
BackingStore::get_symbols() const
{
    using P = std::map<std::string, SymbolStore>::value_type;

    std::set<string> s;
    std::transform( _stores.cbegin(), _stores.cend(), std::inserter(s, s.end()),
                    [](const P& p){ return p.first; } );
    return s;
}


bool
BackingStore::add_symbol_store( const string& symbol )
{
    if( _stores.count(symbol) )
        return true;

    return _add_store(symbol) && _write_index(symbol);
}


bool
BackingStore::remove_symbol_store( const string& symbol )
{
    /*
     * NOTE - not deleting .store files (may want to add it back later)
     */
    _indx.close();
    _indx.open( _index_path, FFLAG::out | FFLAG::trunc );
    if( !_indx ){
        log_error("FILE", "failed to erase index file during remove");
        return false;
    }
    _indx.close();
    _indx.open( _index_path, FFLAG::in | FFLAG::out | FFLAG::app );
    if( !_indx ){
         log_error("FILE", "failed to re-open index file during remove");
         return false;
     }

    _stores.erase(symbol);
    for( auto& p : _stores ){
        if( !_write_index(p.first) ){
            log_error("BACKING-STORE", "failed to write symbol during remove");
            return false;
        }
    }

    return true;
}


bool
BackingStore::delete_symbol_store( const string& symbol )
{
    auto f = _stores.find(symbol);
    if( f != _stores.end() ){
        log_error("BACKING-STORE", "can't delete, store still exists", symbol);
        return false;
    }

    string front_path = _directory_path + symbol + ".front.store";
    int result1 = remove(front_path.c_str());
    if( result1 ){
        string err = std::to_string(errno);
        log_error("FILE", "failed to delete " + front_path + ", errno", err);

    }

    string back_path = _directory_path + symbol + ".back.store";
    int result2 = remove(back_path.c_str());
    if( result2 ){
        string err = std::to_string(errno);
        log_error("FILE", "failed to delete " + back_path + ", errno", err);
    }

    return (result1 == 0 and result2 == 0);
}


bool
BackingStore::read_from_symbol_store( const string& symbol,
                                      fileio_func_ty read_func_front,
                                      fileio_func_ty read_func_back )
{
    auto f = _stores.find(symbol);
    if( f == _stores.end() ){
        log_error("BACKING-STORE", "symbol store doesn't exist", symbol);
        return false;
    }

    int result = _read_store( f->second.back, read_func_back );
    if( result ){
        string err = "failed to read (back) data for " + symbol + ", line";
        log_error( "BACKING-STORE", err, std::to_string(result) );
        return false;
    }

    if( f->second.back.file->eof() )
        f->second.back.file->clear();

    result = _read_store( f->second.front, read_func_front );
    if( result ){
        string err = "failed to read (front) data for " + symbol + ", line";
        log_error( "BACKING-STORE", err, std::to_string(result) );
        return false;
    }

    if( f->second.front.file->eof() )
        f->second.front.file->clear();

    return true;
}


bool
BackingStore::write_to_symbol_store( const string& symbol,
                                     fileio_func_ty write_func_front,
                                     fileio_func_ty write_func_back )
{
    auto f = _stores.find(symbol);
    if( f == _stores.end() ){
        log_error("BACKING-STORE", "symbol store doesn't exist", symbol);
        return false;
    }

    int result1 = _write_store( f->second.back, write_func_back );
    if( result1 ){
        string err = "failed to write (back) data for " + symbol + ", line";
        log_error( "BACKING-STORE", err, std::to_string(result1) );
    }

    int result2 = _write_store( f->second.front, write_func_front );
    if( result2 ){
        string err = "failed to write (front) data for " + symbol + ", line";
                log_error( "BACKING-STORE", err, std::to_string(result2) );
    }

    return (result1 == 0 and result2 == 0);
}


std::set<string>
BackingStore::_read_index( )
{
    std::set<string> symbols;
    string line;
    if( _indx ){
        _indx.seekg( 0, std::ios_base::beg );
        while( std::getline(_indx, line) ){
            if( !line.empty() )
                symbols.insert( line );
        }
    }

    if( _indx.bad() )
        log_error("BACKING-STORE", "bad bit set in index file");

    if( _indx.eof() )
        _indx.clear();

    return symbols;
}


bool
BackingStore::_add_store( const string& symbol )
{
    static const auto FLAGS = FFLAG::in | FFLAG::out | FFLAG::app;

    if( _stores.count(symbol) )
        return true;

    SymbolStore ss;

    ss.back.path = _directory_path + symbol + ".back.store";
    ss.back.file.reset( new std::fstream(ss.back.path, FLAGS) );
    if( !(*ss.back.file) ){
        log_error("BACKING-STORE",
                  "failed to open (back) symbol file", ss.back.path);
        return false;
    }

    ss.front.path= _directory_path + symbol + ".front.store";
    ss.front.file.reset( new std::fstream(ss.front.path, FLAGS) );
    if( !(*ss.front.file) ){
        log_error("BACKING-STORE",
                  "failed to open (front) symbol file", ss.front.path);
        return false;
    }

    _stores[symbol] = std::move(ss);
    return true;
}


bool
BackingStore::_write_index( const string& symbol )
{
    _indx << symbol << std::endl;
    if( !_indx ){
        log_error("FILE", "write symbol to index file failed", symbol);
        return false;
    }
    return true;
}

int
BackingStore::_read_store( SymbolStore::Side& side, fileio_func_ty read_func )
{
    side.file->seekp(0, FFLAG::end);
    if( side.file->fail() ){
        log_error("FILE", "seekp to end of file failed", side.path);
        return -1;
    }

    if( side.file->tellp() == std::fstream::pos_type(0) )
        return 0;

    side.file->seekg(0, FFLAG::beg);
    if( side.file->fail() ){
        log_error("FILE", "seek to beginning of file failed", side.path);
        return -1;
    }

    int i = read_func( *(side.file) );

    if( side.file->bad() ){
        log_error("FILE", "I/O error reading from .store file", side.path);
        return i;
    }else if( !side.file->eof() ){
        log_error("FILE", "failed to reach EOF reading from .store file",
                  side.path);
        return i;
    }

    return 0;
}

int
BackingStore::_write_store( SymbolStore::Side& side, fileio_func_ty write_func )
{
    int i = write_func( *(side.file) );
    if( !(*side.file) ){
        log_error("FILE", "symbol store write failed", side.path);
        return i;
    }

    log_info("FILE", "symbol store write succeeded", side.path);
    return 0;
}


