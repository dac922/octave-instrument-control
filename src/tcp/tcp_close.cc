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

#include "tcp_class.h"

static bool type_loaded = false;

DEFUN_DLD (tcp_close, args, nargout,
"-*- texinfo -*-\n\
@deftypefn {Loadable Function} {} tcp_close (@var{tcp})\n \
\n\
Close the interface and release a file descriptor.\n \
\n\
@var{tcp} - instance of @var{octave_tcp} class.@*\
@end deftypefn")
{
    if (!type_loaded)
    {
        octave_tcp::register_type();
        type_loaded = true;
    }

    if (args.length() != 1 || args(0).type_id() != octave_tcp::static_type_id())
    {
        print_usage();
        return octave_value(-1);
    }

    octave_tcp* tcp = NULL;

    const octave_base_value& rep = args(0).get_rep();
    tcp = &((octave_tcp &)rep);

    tcp->close();

    return octave_value();
}
