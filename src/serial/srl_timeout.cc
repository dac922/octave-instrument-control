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

DEFUN_DLD (srl_timeout, args, nargout, 
        "-*- texinfo -*-\n\
@deftypefn {Loadable Function} {} srl_timeout (@var{serial}, @var{timeout})\n \
@deftypefnx {Loadable Function} {@var{t} = } srl_timeout (@var{serial})\n \
\n\
Set new or get existing serial interface timeout parameter used for srl_read() requests. The timeout value is specified in tenths of a second.\n \
\n\
@var{serial} - instance of @var{octave_serial} class.@*\
@var{timeout} - srl_read() timeout value in tenths of a second. Value of -1 means a blocking call. Maximum value of 255 (i.e. 25.5 seconds).\n \
\n\
If @var{timeout} parameter is omitted, the srl_timeout() shall return current timeout value as the result @var{t}.\n \
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
    
    if (args.length() < 1 || args.length() > 2 || args(0).type_id() != octave_serial::static_type_id())
    {
        print_usage();
        return octave_value(-1);
    }

    octave_serial* serial = NULL;

    const octave_base_value& rep = args(0).get_rep();
    serial = &((octave_serial &)rep);

    // Setting new timeout
    if (args.length() > 1)
    {
        if ( !(args(1).is_integer_type() || args(1).is_float_type()) )
        {
            print_usage();
            return octave_value(-1);
        }

        serial->set_timeout(args(1).int_value());

        return octave_value(); // Should it return by default?
    }

    // Returning current timeout
    return octave_value(serial->get_timeout());
#endif
}
