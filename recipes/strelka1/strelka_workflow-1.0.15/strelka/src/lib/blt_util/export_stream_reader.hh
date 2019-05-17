// -*- mode: c++; indent-tabs-mode: nil; -*-
//
// Copyright (c) 2009-2016 Illumina, Inc.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

/// \file

/// \author Chris Saunders
///
#ifndef __EXPORT_STREAM_READER_HH
#define __EXPORT_STREAM_READER_HH

#include "blt_util/export_line_parser.hh"

#include <boost/utility.hpp>

#include <cassert>
#include <cstdlib>

#include <iosfwd>
#include <string>

/// general version for any stream - specialized file-specific class below
///
struct export_stream_reader : private boost::noncopyable {

    // null ctor will behave exactly like an empty file:
    export_stream_reader()
        : _is_line_set(false), _line_no(0), _isp(0) {}

    export_stream_reader(std::istream& is,
                         const char* stream_name)
        : _is_line_set(false), _line_no(0), _isp(&is), _stream_name(stream_name) {}

    virtual ~export_stream_reader() {}

    /// \brief Return false @ EOF:
    bool next();

    /// \brief Return NULL @ invalid (first,EOF);
    const export_line_parser* exline() const {
        if (_is_line_set) return &_elp;
        else             return 0;
    }

    const char* name() const { return _stream_name.c_str(); }

    unsigned line_no() const { return _line_no; }

    void report_state(std::ostream& os) const;

protected:
    // Allow export_file_reader to delay stream
    // initialization. Regular client code cannot change isp after the ctor call:
    void updateStream(std::istream& is,
                      const char* stream_name) {
        assert(! _isp);
        _isp=&is;
        _stream_name=stream_name;
    }

    bool _is_line_set;
    unsigned _line_no;

private:
    std::istream* _isp;

    enum { _line_buf_size = 2000 };
    char _line_buf[_line_buf_size];

    export_line_parser _elp;
    std::string _stream_name;
};



/// Provides a simplified way to use export_stream_reader for files:
///
struct export_file_reader : public export_stream_reader {

    explicit export_file_reader(const char* filename);

    virtual ~export_file_reader();

    /// \brief rewinds to the beginning of the file, the next line read
    /// will be the first in list
    ///
    void rewind();

private:
    std::ifstream* _fisp;
};


#endif
