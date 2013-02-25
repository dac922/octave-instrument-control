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

#ifndef __WIN32__
#include <fcntl.h>
#endif

using std::string;

#include "usbtmc_class.h"

static bool type_loaded = false;

DEFUN_DLD (usbtmc, args, nargout,
        "-*- texinfo -*-\n\
@deftypefn {Loadable Function} {@var{usbtmc} = } usbtmc (@var{path})\n \
\n\
Open usbtmc interface.\n \
\n\
@var{path} - the interface path of type String. If omitted defaults to '/dev/usbtmc0'. @*\
\n\
The usbtmc() shall return instance of @var{octave_usbtmc} class as the result @var{usbtmc}.\n \
@end deftypefn")
{
#ifdef __WIN32__
    error("usbtmc: Windows platform support is not yet implemented, go away...");
    return octave_value();
#endif

    if (!type_loaded)
    {
        octave_usbtmc::register_type();
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
    string path("/dev/usbtmc0");

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

    octave_usbtmc* retval = new octave_usbtmc();

    // Open the interface
    if (retval->open(path, oflags) < 0)
        return octave_value();

    return octave_value(retval);
}
