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

#include "gpib_class.h"

static bool type_loaded = false;

DEFUN_DLD (gpib_timeout, args, nargout, 
        "-*- texinfo -*-\n\
@deftypefn {Loadable Function} {} gpib_timeout (@var{gpib}, @var{timeout})\n \
@deftypefnx {Loadable Function} {@var{t} = } gpib_timeout (@var{gpib})\n \
\n\
Set new or get existing gpib interface timeout parameter used for gpib_read() requests. The timeout value is specified in tenths of a second.\n \
\n\
@var{gpib} - instance of @var{octave_gpib} class.@*\
@var{timeout} - gpib_read() timeout value in tenths of a second. Value of -1 means a blocking call. Maximum value of 255 (i.e. 25.5 seconds).\n \
\n\
If @var{timeout} parameter is omitted, the gpib_timeout() shall return current timeout value as the result @var{t}.\n \
@end deftypefn")
{
    if (!type_loaded)
    {
        octave_gpib::register_type();
        type_loaded = true;
    }
    
    if (args.length() < 1 || args.length() > 2 || args(0).type_id() != octave_gpib::static_type_id())
    {
        print_usage();
        return octave_value(-1);
    }

    octave_gpib* gpib = NULL;

    const octave_base_value& rep = args(0).get_rep();
    gpib = &((octave_gpib &)rep);

    // Setting new timeout
    if (args.length() > 1)
    {
        if ( !(args(1).is_integer_type() || args(1).is_float_type()) )
        {
            print_usage();
            return octave_value(-1);
        }

        gpib->set_timeout(args(1).int_value());

        return octave_value(); // Should it return by default?
    }

    // Returning current timeout
    return octave_value(gpib->get_timeout());
}
