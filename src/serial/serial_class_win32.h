// Copyright (C) 2012   Andrius Sutas   <andrius.sutas@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.

#ifndef SERIAL_CLASS_WIN32_H
#define SERIAL_CLASS_WIN32_H

#include <octave/oct.h>
#include <octave/ov-int32.h>

#include <windows.h>

#include <string>

using std::string;

class octave_serial : public octave_base_value
{
public:
    octave_serial();
    ~octave_serial();

    int write(string /* buffer */);
    int write(uint8_t* /* buffer */, unsigned int /* buffer size */);

    int read(uint8_t* /* buffer */, unsigned int /* buffer size */);

    int open(string /* path */);
    int close();
    int get_fd() const;

    int flush(unsigned short /* stream select */);

    int set_timeout(short /* timeout */);
    int get_timeout();

    int set_baudrate(unsigned int /* baudrate */);
    int get_baudrate();

    int set_bytesize(unsigned short /* bytesize */);
    int get_bytesize();

    int set_parity(string /* parity */);
    string get_parity();

    int set_stopbits(unsigned short /* stop bits */);
    int get_stopbits();

    bool get_control_line(string);
    void set_control_line(string, bool);

    // Overloaded base functions
    double serial_value() const { return (double)get_fd(); }

    virtual double scalar_value (bool frc_str_conv = false) const
    {
        return (double)get_fd();
    }

    void print (std::ostream& os, bool pr_as_read_syntax = false);
    void print (std::ostream& os, bool pr_as_read_syntax = false) const;
    void print_raw (std::ostream& os, bool pr_as_read_syntax) const;

    // Properties
    bool is_constant (void) const { return true;}
    bool is_defined (void) const { return true;}
    bool print_as_scalar (void) const { return true;}


private:
    HANDLE fd;
    DWORD status;
    DCB config;
    COMMTIMEOUTS timeouts;

    volatile bool blocking_read;

    void get_control_line_status(void);

    DECLARE_OCTAVE_ALLOCATOR
    DECLARE_OV_TYPEID_FUNCTIONS_AND_DATA
};

#endif
