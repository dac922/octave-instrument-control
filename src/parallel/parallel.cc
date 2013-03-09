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

#ifdef BUILD_PARALLEL
#include <octave/ov-int32.h>

#include <iostream>
#include <string>
#include <algorithm>

#ifndef __WIN32__
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif

using std::string;

#include "parallel_class.h"

static bool type_loaded = false;
#endif

DEFUN_DLD (parallel, args, nargout, 
"-*- texinfo -*-\n\
@deftypefn {Loadable Function} {@var{parallel} = } parallel ([@var{path}], [@var{direction}])\n \
\n\
Open Parallel interface.\n \
\n\
@var{path} - the interface path of type String. If omitted defaults to '/dev/parport0'.@*\
@var{direction} - the direction of interface drivers of type Integer, see: PP_DATADIR for more info. \
If omitted defaults to 1 (Input).\n \
\n\
The parallel() shall return instance of @var{octave_parallel} class as the result @var{parallel}.\n \
@end deftypefn")
{
#ifndef BUILD_PARALLEL
    error("parallel: Your system doesn't support the GPIB interface");
    return octave_value();
#else
    if (!type_loaded)
    {
        octave_parallel::register_type();
        type_loaded = true;
    }

    // Do not open interface if return value is not assigned
    if (nargout != 1)
    {
        print_usage();
        return octave_value();
    }

    // Default values
    int oflags = O_RDWR;
    int dir = 1; // Input
    string path("/dev/parport0");

    // Parse the function arguments
    if (args.length() > 0)
    {
        if (args(0).is_string())
        {
            path = args(0).string_value();
        }
        else
        {
            print_usage();
            return octave_value();
        }
    }

    // is_float_type() is or'ed to allow expression like ("", 123), without user
    // having to use ("", int32(123)), as we still only take "int_value"
    if (args.length() > 1)
    {
        if (args(1).is_integer_type() || args(1).is_float_type())
        {
            dir = args(1).int_value();
        }
        else
        {
            print_usage();
            return octave_value();
        }
    }

    octave_parallel* retval = new octave_parallel();

    // Open the interface
    if (retval->open(path, oflags) < 0)
        return octave_value();

    // Set direction
    retval->set_datadir(dir);

    return octave_value(retval);
#endif
}
