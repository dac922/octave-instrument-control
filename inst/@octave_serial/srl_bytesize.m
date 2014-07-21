## Copyright (C) 2014 Stefan Mahr <dac922@gmx.de>
##
## This program is free software; you can redistribute it and/or modify it under
## the terms of the GNU General Public License as published by the Free Software
## Foundation; either version 3 of the License, or (at your option) any later
## version.
##
## This program is distributed in the hope that it will be useful, but WITHOUT
## ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
## FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
## details.
##
## You should have received a copy of the GNU General Public License along with
## this program; if not, see <http://www.gnu.org/licenses/>.

## -*- texinfo -*-
## @deftypefn {Loadable Function} {} srl_bytesize (@var{serial}, @var{bsize})
## @deftypefnx {Loadable Function} {@var{bs} = } srl_bytesize (@var{serial})
##
## Set new or get existing serial interface byte size parameter.
##
## @var{serial} - instance of @var{octave_serial} class.
## @var{bsize} - byte size of type Integer. Supported values: 5/6/7/8.
##
## If @var{bsize} parameter is omitted, the srl_bytesize() shall return current byte size value or in case of unsupported setting -1, as the result @var{bs}.
##
## This function is obsolete. Use get and set method instead.
##
## @end deftypefn
function retval = srl_bytesize (serial, bytesize)

  try
    if (nargin>1)
      __srl_properties__ (serial, 'bytesize', bytesize);
    else
      retval = __srl_properties__ (serial, 'bytesize');
    end
  catch
    print_usage();
  end
end
