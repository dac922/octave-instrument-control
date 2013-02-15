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
#include <errno.h>
#include <fcntl.h>
#endif

using std::string;

#include "vxi11_class.h"


static bool type_loaded = false;

DEFUN_DLD (vxi11, args, nargout, 
        "-*- texinfo -*-\n\
@deftypefn {Loadable Function} {@var{vxi11} = } vxi11 (@var{path})\n \
\n\
Open vxi11 interface.\n \
\n\
@var{path} - the interface path of type String. If omitted defaults to '/dev/vxi110'. @*\
\n\
The vxi11() shall return instance of @var{octave_vxi11} class as the result @var{vxi11}.\n \
@end deftypefn")
{
#ifdef __WIN32__
    error("vxi11: Windows platform support is not yet implemented, go away...");
    return octave_value();
#endif

    if (!type_loaded)
    {
        octave_vxi11::register_type();
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
    string path("127.0.0.1");

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

    // Open the interface
    octave_vxi11* retval = new octave_vxi11;
    
    if (retval->open(path) < 0)
    {
        error("vxi11: Error opening the interface: %s\n", strerror(errno));
        return octave_value();
    }

    return octave_value(retval);
}
