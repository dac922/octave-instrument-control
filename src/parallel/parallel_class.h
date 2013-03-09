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

#ifndef PARALLEL_CLASS_H
#define PARALLEL_CLASS_H

#include <octave/oct.h>
#include <octave/ov-int32.h>

#include <string>

using std::string;

class octave_parallel : public octave_base_value 
{
public:
    octave_parallel();
    ~octave_parallel();

    int open(string /* path */, int /* open flags */);
    int close();
    int get_fd();

    int get_datadir();
    int set_datadir(int /* direction */);

    int get_data();
    int set_data(uint8_t /* value */);

    int get_stat();
    //int set_stat(uint8_t);

    int get_ctrl();
    int set_ctrl(uint8_t /* value */);

    // Overloaded base functions
    double parallel_value() const
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

    // 1 - Input
    // 0 - Output
    int dir;

    DECLARE_OCTAVE_ALLOCATOR
    DECLARE_OV_TYPEID_FUNCTIONS_AND_DATA
};


#endif
