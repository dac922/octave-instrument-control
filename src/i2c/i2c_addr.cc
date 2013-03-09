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

#include <octave/oct.h>

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#ifdef BUILD_I2C
#include "i2c_class.h"

static bool type_loaded = false;
#endif

DEFUN_DLD (i2c_addr, args, nargout, 
"-*- texinfo -*-\n\
@deftypefn {Loadable Function} {} i2c_addr (@var{i2c}, @var{address})\n \
@deftypefnx {Loadable Function} {@var{addr} = } i2c_addr (@var{i2c})\n \
\n\
Set new or get existing i2c slave device address.\n \
\n\
@var{i2c} - instance of @var{octave_i2c} class.@*\
@var{address} - i2c slave device address of type Integer. \
The address is passed in the 7 or 10 lower bits of the argument.\n \
\n\
If @var{address} parameter is omitted, the i2c_addr() shall return \
current i2c slave device address as the result @var{addr}.\n \
@end deftypefn")
{
#ifndef BUILD_I2C
    error("i2c: Your system doesn't support the I2C interface");
    return octave_value();
#else
    if (!type_loaded)
    {
        octave_i2c::register_type();
        type_loaded = true;
    }
    
    if (args.length() < 1 || args.length() > 2 || args(0).type_id() != octave_i2c::static_type_id()) 
    {
        print_usage();
        return octave_value(-1);
    }
    
    octave_i2c* i2c = NULL;

    const octave_base_value& rep = args(0).get_rep();
    i2c = &((octave_i2c &)rep);


    // Setting new slave address
    if (args.length() > 1)
    {
        if ( !(args(1).is_integer_type() || args(1).is_float_type()) )
        {
            print_usage();
            return octave_value(-1);
        }

        i2c->set_addr(args(1).int_value());

        return octave_value();
    }

    // Returning current slave address
    return octave_value(i2c->get_addr());
#endif
}
