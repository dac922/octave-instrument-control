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
#include "parallel_class.h"

static bool type_loaded = false;
#endif

DEFUN_DLD (pp_datadir, args, nargout, 
"-*- texinfo -*-\n\
@deftypefn {Loadable Function} {} pp_datadir (@var{parallel}, @var{direction})\n \
@deftypefnx {Loadable Function} {@var{dir} = } pp_datadir (@var{parallel})\n \
\n\
Controls the Data line drivers. Normally the computer's parallel port will drive the data lines, \
but for byte-wide transfers from the peripheral to the host it is useful to turn off those drivers \
and let the peripheral drive the signals. (If the drivers on the computer's parallel port are left \
on when this happens, the port might be damaged.)\n \
\n\
@var{parallel} - instance of @var{octave_parallel} class.@*\
@var{direction} - direction parameter of type Integer. Supported values: 0 - the drivers are turned on \
(Output/Forward direction); 1 - the drivers are turned off (Input/Reverse direction).\n \
\n\
If @var{direction} parameter is omitted, the pp_datadir() shall return current Data direction as the result @var{dir}.\n \
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

    
    if (args.length() < 1 || args.length() > 2 || args(0).type_id() != octave_parallel::static_type_id())
    {
        print_usage();
        return octave_value(-1);
    }

    octave_parallel* parallel = NULL;

    const octave_base_value& rep = args(0).get_rep();
    parallel = &((octave_parallel &)rep);

    // Set new direction
    if (args.length() > 1)
    {
        if ( !(args(1).is_integer_type() || args(1).is_float_type()) )
        {
            print_usage();
            return octave_value(-1);
        }

        parallel->set_datadir(args(1).int_value());

        return octave_value();
    }

    // Return current direction
    return octave_value(parallel->get_datadir());
#endif
}
