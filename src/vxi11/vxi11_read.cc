// Copyright (C) 2013   Stefan Mahr     <dac922@gmx.de>
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

#include "vxi11_class.h"

static bool type_loaded = false;

DEFUN_DLD (vxi11_read, args, nargout,
"-*- texinfo -*-\n\
@deftypefn {Loadable Function} {[@var{data}, @var{count}] = } vxi11_read (@var{vxi11}, @var{n})\n \
\n\
Read from vxi11 slave device.\n \
\n\
@var{vxi11} - instance of @var{octave_vxi11} class.@*\
@var{n} - number of bytes to attempt to read of type Integer.\n \
\n\
The vxi11_read() shall return number of bytes successfully read in @var{count} as Integer and the bytes themselves in @var{data} as uint8 array.\n \
@end deftypefn")
{
    if (!type_loaded)
    {
        octave_vxi11::register_type();
        type_loaded = true;
    }


    if (args.length() < 1 || args.length() > 2 || args(0).type_id() != octave_vxi11::static_type_id())
    {
        print_usage();
        return octave_value(-1);
    }

    char *buffer = NULL;
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

    buffer = new char [buffer_len + 1];

    if (buffer == NULL)
    {
        error("vxi11_read: cannot allocate requested memory...");
        return octave_value(-1);
    }

    octave_vxi11* vxi11 = NULL;

    const octave_base_value& rep = args(0).get_rep();
    vxi11 = &((octave_vxi11 &)rep);

    int retval;

    retval = vxi11->read(buffer, buffer_len);

    octave_value_list return_list;

    uint8NDArray data( dim_vector(1, (retval > 0) ? retval : 0) );

    for (int i = 0; i < retval; i++)
        data(i) = static_cast<uint8_t>(buffer[i]);

    return_list(0) = data;
    return_list(1) = retval;


    delete[] buffer;

    return return_list;
}
