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
#include <octave/uint8NDArray.h>

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#ifdef BUILD_I2C
#include <errno.h>

#include "i2c_class.h"

static bool type_loaded = false;
#endif

DEFUN_DLD (i2c_read, args, nargout, 
"-*- texinfo -*-\n\
@deftypefn {Loadable Function} {[@var{data}, @var{count}] = } i2c_read (@var{i2c}, @var{n})\n \
\n\
Read from i2c slave device.\n \
\n\
@var{i2c} - instance of @var{octave_i2c} class.@*\
@var{n} - number of bytes to attempt to read of type Integer.\n \
\n\
The i2c_read() shall return number of bytes successfully read in @var{count} as Integer and the bytes themselves in @var{data} as uint8 array.\n \
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

    uint8_t *buffer = NULL;
    unsigned int buffer_len = 1;

    if (args.length() > 1)
    {
        if ( !(args(1).is_integer_type() || args(1).is_float_type()) )
        {
            print_usage();
            return octave_value(-1);
        }

        buffer_len = args(1).int_value();
    }

    buffer = new uint8_t [buffer_len + 1];

    if (buffer == NULL)
    {
        error("i2c_read: cannot allocate requested memory: %s\n", strerror(errno));
        return octave_value(-1);  
    }

    octave_i2c* i2c = NULL;

    const octave_base_value& rep = args(0).get_rep();
    i2c = &((octave_i2c &)rep);

    int retval;
    
    retval = i2c->read(buffer, buffer_len);
    
    octave_value_list return_list;
    uint8NDArray data( dim_vector(1, retval) );
    
    for (int i = 0; i < retval; i++)
        data(i) = buffer[i];

    return_list(0) = data;
    return_list(1) = retval; 
        
    delete[] buffer;

    return return_list;
#endif
}
