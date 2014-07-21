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
## @deftypefn {Loadable Function} {} srl_parity (@var{serial}, @var{parity})
## @deftypefnx {Loadable Function} {@var{p} = } srl_parity (@var{serial})
##
## Set new or get existing serial interface parity parameter. Even/Odd/None values are supported.
##
## @var{serial} - instance of @var{octave_serial} class.
## @var{parity} - parity value of type String. Supported values: Even/Odd/None (case insensitive, can be abbreviated to the first letter only)
##
## If @var{parity} parameter is omitted, the srl_parity() shall return current parity value as the result @var{p}.
##
## This function is obsolete. Use get and set method instead.
##
## @end deftypefn
function retval = srl_parity (serial, parity)

  try
    if (nargin>1)
      __srl_properties__ (serial, 'parity', parity);
    else
      retval = __srl_properties__ (serial, 'parity');
    end
  catch
    print_usage();
  end
end
