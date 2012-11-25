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

// TODO: Implement Flow Control
// TODO: Implement H/W handshaking

#include <octave/oct.h>

#ifndef __WIN32__
#include <errno.h>
#include <fcntl.h>
#endif

#include "tcp_class.h"

static bool type_loaded = false;

DEFUN_DLD (tcp, args, nargout, 
"-*- texinfo -*-\n\
@deftypefn {Loadable Function} {@var{tcp} = } tcp ([@var{path}], [@var{port}], [@var{timeout}])\n \
\n\
Open tcp interface.\n \
\n\
@var{address} - the interface path of type String. If omitted defaults to '/dev/ttyUSB0'. @*\
@var{baudrate} - the baudrate of interface. If omitted defaults to 115200. @*\
@var{timeout} - the interface timeout value. If omitted defaults to blocking call.\n \
\n\
The tcp() shall return instance of @var{octave_tcp} class as the result @var{tcp}.\n \
@end deftypefn")
{
#ifdef __WIN32__
    error("tcp: Windows platform support is not yet implemented, go away...");
    return octave_value();
#endif
    
    if (!type_loaded)
    {
        octave_tcp::register_type();
        type_loaded = true;
    }

    // Do not open interface if return value is not assigned
    if (nargout != 1)
    {
        print_usage();
        return octave_value();
    }
    
    // Default values
    string address("localhost");
    int port = 23;
    int timeout = -1;
    
    // Parse the function arguments
    if (args.length() > 0)
    {
        if (args(0).is_string())
        {
            address = args(0).string_value();
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
            port = args(1).int_value();
        }
        else
        {
            print_usage();
            return octave_value();
        }
    }

    if (args.length() > 2)
    {
        if (args(2).is_integer_type() || args(2).is_float_type())
        {
            timeout = args(2).int_value();
        }
        else
        {
            print_usage();
            return octave_value();
        }
    }

    // Open the interface
    octave_tcp* retval = new octave_tcp(address, port);

    if (retval->get_fd() < 0)
    {
        error("tcp: Error opening the interface: %s\n", strerror(errno));
        return octave_value();
    }
    
    retval->set_timeout(timeout);
    
    //retval->flush(2);
    
    return octave_value(retval);
}
