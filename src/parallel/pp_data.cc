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

DEFUN_DLD (pp_data, args, nargout, 
"-*- texinfo -*-\n\
@deftypefn {Loadable Function} {} pp_data (@var{parallel}, @var{data})\n \
@deftypefnx {Loadable Function} {@var{d} = } pp_data (@var{parallel})\n \
\n\
Sets or Read the Data lines.\
\n\
@var{parallel} - instance of @var{octave_parallel} class.@*\
@var{data} - data parameter to be set of type Byte.\n \
\n\
If @var{data} parameter is omitted, the pp_data() shall return current Data lines state as the result @var{d}.\n \
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

    // Set new Data register value
    if (args.length() > 1)
    {
        if ( !(args(1).is_integer_type() || args(1).is_float_type()) )
        {
            print_usage();
            return octave_value(-1);
        }

        parallel->set_data(args(1).int_value());

        return octave_value();
    }

    // Return current Data register value on port
    return octave_value(parallel->get_data());
#endif
}
