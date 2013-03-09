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

#ifndef SERIAL_CLASS_H
#define SERIAL_CLASS_H

#include <octave/oct.h>
#include <octave/ov-int32.h>

#include <string>
#include <termios.h>

using std::string;

#define BITMASK_SET(x,y) ((x) |= (y))
#define BITMASK_CLEAR(x,y) ((x) &= (~(y)))
#define BITMASK_TOGGLE(x,y) ((x) ^= (y))
#define BITMASK_CHECK(x,y) ((x) & (y))

class octave_serial : public octave_base_value 
{
public:
    octave_serial();
    ~octave_serial();

    int write(string /* buffer */);
    int write(uint8_t* /* buffer */, unsigned int /* buffer size */);

    int read(uint8_t* /* buffer */, unsigned int /* buffer size */);

    int open(string /* path */, int /* open flags */);
    int close();
    int get_fd();

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


    // Overloaded base functions
    double serial_value() const { return (double)this->fd; }

    virtual double scalar_value (bool frc_str_conv = false) const 
    {
        return (double)this->fd;
    }

    void print (std::ostream& os, bool pr_as_read_syntax = false) const;
    void print_raw (std::ostream& os, bool pr_as_read_syntax) const;

    // Properties
    bool is_constant (void) const { return true;}
    bool is_defined (void) const { return true;}
    bool print_as_scalar (void) const { return true;}


private:
    int fd;
    struct termios config;

    volatile bool blocking_read;

    DECLARE_OCTAVE_ALLOCATOR
    DECLARE_OV_TYPEID_FUNCTIONS_AND_DATA
};

#endif
