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

#include <errno.h>
#include <fcntl.h>

#include "gpib_class.h"

static bool type_loaded = false;

DEFUN_DLD (gpib, args, nargout,
"-*- texinfo -*-\n\
@deftypefn {Loadable Function} {@var{gpib} = } gpib ([@var{gpibid}], [@var{timeout}])\n \
\n\
Open gpib interface.\n \
\n\
@var{gpibid} - the interface number. @*\
@var{timeout} - the interface timeout value. If omitted defaults to blocking call.\n \
\n\
The gpib() shall return instance of @var{octave_gpib} class as the result @var{gpib}.\n \
@end deftypefn")
{

    if (!type_loaded)
    {
        octave_gpib::register_type();
        type_loaded = true;
    }

    // Do not open interface if return value is not assigned
    if (nargout != 1)
    {
        print_usage();
        return octave_value();
    }

    // Default values
    int gpibid;
    const int minor = 0;
    int timeout = -1;
    const int secid = 0;
    const int send_eoi = 1;
    const int eos_mode = 0;


    // Parse the function arguments
    if (args.length() > 0)
    {
        if (args(0).is_integer_type() || args(0).is_float_type())
        {
            gpibid = args(0).int_value();
        }
        else
        {
            print_usage();
            return octave_value();
        }
    }
    else
    {
        print_usage();
        return octave_value();
    }

    // is_float_type() is or'ed to allow expression like ("", 123), without user
    // having to use ("", int32(123)), as we still only take "int_value"
    if (args.length() > 1)
    {
        if (args(1).is_integer_type() || args(1).is_float_type())
        {
            timeout = args(1).int_value();
        }
        else
        {
            print_usage();
            return octave_value();
        }
    }

    // Open the interface
    octave_gpib* retval = new octave_gpib();

    retval->open(minor, gpibid, secid, timeout, send_eoi, eos_mode);

    //retval->set_timeout(timeout);
    //retval->set_sad(eot);
    //retval->set_send_eoi(eot);
    //retval->set_eos_mode(eot);

    return octave_value(retval);
}
