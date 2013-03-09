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

#ifdef BUILD_SERIAL
#include "serial_class.h"

static bool type_loaded = false;
#endif

DEFUN_DLD (srl_baudrate, args, nargout, 
"-*- texinfo -*-\n\
@deftypefn {Loadable Function} {} srl_baudrate (@var{serial}, @var{baudrate})\n \
@deftypefnx {Loadable Function} {@var{br} = } srl_baudrate (@var{serial})\n \
\n\
Set new or get existing serial interface baudrate parameter. Only standard values are supported.\n \
\n\
@var{serial} - instance of @var{octave_serial} class.@*\
@var{baudrate} - the baudrate value used. Supported values: 0, 50, 75, 110, \
134, 150, 200, 300, 600, 1200, 1800, 2400, 4800, 9600 19200, 38400, 57600, \
115200 and 230400.\n \
\n\
If @var{baudrate} parameter is omitted, the srl_baudrate() shall return current baudrate value as the result @var{br}.\n \
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
    
    if (args.length() < 1 || args.length() > 2 ||
        args(0).type_id() != octave_serial::static_type_id())
    {
        print_usage();
        return octave_value(-1);
    }
    
    octave_serial* serial = NULL;

    const octave_base_value& rep = args(0).get_rep();
    serial = &((octave_serial &)rep);

    // Setting new baudrate
    if (args.length() > 1)
    {
        if ( !(args(1).is_integer_type() || args(1).is_float_type()) )
        {
            error("srl_baudrate: expecting second argument of type integer...");
            return octave_value(-1);
        }

        serial->set_baudrate(args(1).int_value());

        return octave_value();
    }

    // Returning current baud rate
    return octave_value(serial->get_baudrate());
#endif
}
