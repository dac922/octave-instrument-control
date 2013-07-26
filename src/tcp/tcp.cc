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

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#ifdef BUILD_TCP
#include "tcp_class.h"

static bool type_loaded = false;
#endif

DEFUN_DLD (tcp, args, nargout,
        "-*- texinfo -*-\n\
@deftypefn {Loadable Function} {@var{tcp} = } tcp ([@var{ipaddress}], [@var{port}], [@var{timeout}])\n \
\n\
Open tcp interface.\n \
\n\
@var{ipaddress} - the ip address of type String. If omitted defaults to '127.0.0.1'.@* \
@var{port} - the port number to connect. If omitted defaults to 23.@* \
@var{timeout} - the interface timeout value. If omitted defaults to blocking call.\n \
\n\
The tcp() shall return instance of @var{octave_tcp} class as the result @var{tcp}.\n \
@end deftypefn")
{
#ifndef BUILD_TCP
    error("tcp: Your system doesn't support the TCO interface");
    return octave_value();
#else
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
    string address("127.0.0.1");
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

    // Open the interface and connect
    octave_tcp* retval = new octave_tcp();

    if (retval->open(address, port) < 0)
    {
        return octave_value();
    }

    retval->set_timeout(timeout);

    //retval->flush(2);

    return octave_value(retval);
#endif
}
