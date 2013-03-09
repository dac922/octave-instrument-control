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

DEFUN_DLD (srl_bytesize, args, nargout, 
"-*- texinfo -*-\n\
@deftypefn {Loadable Function} {} srl_bytesize (@var{serial}, @var{bsize})\n \
@deftypefnx {Loadable Function} {@var{bs} = } srl_bytesize (@var{serial})\n \
\n\
Set new or get existing serial interface byte size parameter.\n \
\n\
@var{serial} - instance of @var{octave_serial} class.@*\
@var{bsize} - byte size of type Integer. Supported values: 5/6/7/8.\n \
\n\
If @var{bsize} parameter is omitted, the srl_bytesize() shall return current byte size value or in case of unsupported setting -1, as the result @var{bs}.\n \
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

    // Setting new byte size
    if (args.length() > 1)
    {
        if ( !(args(1).is_integer_type() || args(1).is_float_type()) )
        {
            print_usage();
            return octave_value(-1);
        }

        serial->set_bytesize(args(1).int_value());

        return octave_value();
    }

    // Returning current byte size 
    return octave_value(serial->get_bytesize());
#endif
}
