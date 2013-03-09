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

#ifndef I2C_CLASS_H
#define I2C_CLASS_H

#include <octave/oct.h>

#include <string>

using std::string;

class octave_i2c : public octave_base_value 
{
public:
    octave_i2c();
    ~octave_i2c();

    int open(string /* path */, int /* open flags */);
    int close();    
    int get_fd();

    int set_addr(int /* slave ddress */);
    int get_addr();

    // Simple i2c commands
    int write(uint8_t* /* buffer */, unsigned int /* buffer size */);
    int read(uint8_t* /* buffer */, unsigned int /* buffer size */);


    // Overloaded base functions
    double i2c_value() const
    {
        return (double)this->fd;
    }

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
    int addr;

    DECLARE_OCTAVE_ALLOCATOR
    DECLARE_OV_TYPEID_FUNCTIONS_AND_DATA
};


#endif
