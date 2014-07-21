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
## @deftypefn {Loadable Function} {} srl_stopbits (@var{serial}, @var{stopb})
## @deftypefnx {Loadable Function} {@var{sb} = } srl_stopbits (@var{serial})
##
## Set new or get existing serial interface stop bits parameter. Only 1 or 2 stop bits are supported.
##
## @var{serial} - instance of @var{octave_serial} class.
## @var{stopb} - number of stop bits used. Supported values: 1, 2.
##
## If @var{stopb} parameter is omitted, the srl_stopbits() shall return current stop bits value as the result @var{sb}.
##
## This function is obsolete. Use get and set method instead.
##
## @end deftypefn
function retval = srl_stopbits (serial, stopbits)

  try
    if (nargin>1)
      __srl_properties__ (serial, 'stopbits', stopbits);
    else
      retval = __srl_properties__ (serial, 'stopbits');
    end
  catch
    print_usage();
  end
end
