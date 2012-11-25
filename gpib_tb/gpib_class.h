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

#ifndef GPIB_CLASS_H
#define GPIB_CLASS_H

#include <octave/oct.h>
#include <octave/ov-int32.h>

#include <string>
#include <termios.h>

using std::string;

#define BITMASK_SET(x,y) ((x) |= (y))
#define BITMASK_CLEAR(x,y) ((x) &= (~(y)))
#define BITMASK_TOGGLE(x,y) ((x) ^= (y))
#define BITMASK_CHECK(x,y) ((x) & (y))

class octave_gpib : public octave_base_value 
{
public:
    octave_gpib();
    octave_gpib(int, int, int, int);
    ~octave_gpib();

    int write(string);
    int write(unsigned char*, int);

    int read(char *, unsigned int);

    int close();

    int set_timeout(int);
    int get_timeout();
   
    //int set_sad(int);
    //int set_send_eoi(int);
    //int set_eos_mode(int);

    int get_fd() { return this->fd; }

    // Overloaded base functions
    double gpib_value() const { return (double)this->fd; }

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
	int timeout;

	int minor;
	int sad;
	int send_eoi;
	int eos_mode;

    DECLARE_OCTAVE_ALLOCATOR
    DECLARE_OV_TYPEID_FUNCTIONS_AND_DATA
};

#endif
