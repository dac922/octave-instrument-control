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

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#ifdef BUILD_SERIAL
#include <errno.h>
#include <fcntl.h>

#include "serial_class.h"

static bool type_loaded = false;
#endif

DEFUN_DLD (serial, args, nargout, 
        "-*- texinfo -*-\n\
@deftypefn {Loadable Function} {@var{serial} = } serial ([@var{path}], [@var{baudrate}], [@var{timeout}])\n \
\n\
Open serial interface.\n \
\n\
@var{path} - the interface path of type String. If omitted defaults to '/dev/ttyUSB0'. @*\
@var{baudrate} - the baudrate of interface. If omitted defaults to 115200. @*\
@var{timeout} - the interface timeout value. If omitted defaults to blocking call.\n \
\n\
The serial() shall return instance of @var{octave_serial} class as the result @var{serial}.\n \
@end deftypefn")
{
#ifndef BUILD_SERIAL
    error("serial: Your system doesn't support the SERIAL interface");
    return octave_value();
#else
    if (!type_loaded)
    {
        octave_serial::register_type();
        type_loaded = true;
    }

    // Do not open interface if return value is not assigned
    if (nargout != 1)
    {
        print_usage();
        return octave_value();
    }

    // Default values
    string path("/dev/ttyUSB0");
    unsigned int baud_rate = 115200;
    short timeout = -1;

    unsigned short bytesize = 8;
    string parity("N");
    unsigned short stopbits = 1;
    int oflags = O_RDWR | O_NOCTTY | O_SYNC | O_NDELAY; 
    // O_SYNC - All writes immediately effective, no buffering
    // O_NOCTTY - Do not make serial terminal the controlling terminal for the process
    // O_NDELAY - Do not care what state the DCD signal line is in. Used for open only, later disabled.

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
            baud_rate = args(1).int_value();
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


    octave_serial* retval = new octave_serial();

    // Open the interface
    if (retval->open(path, oflags) < 0)
        return octave_value();

    retval->set_baudrate(baud_rate);
    retval->set_timeout(timeout);
    retval->set_parity(parity);
    retval->set_bytesize(bytesize);
    retval->set_stopbits(stopbits);

    //retval->flush(2);

    return octave_value(retval);
#endif
}
