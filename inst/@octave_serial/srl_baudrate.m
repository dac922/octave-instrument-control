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
## @deftypefn {Loadable Function} {} srl_baudrate (@var{serial}, @var{baudrate})\
## @deftypefnx {Loadable Function} {@var{br} = } srl_baudrate (@var{serial})
##
## Set new or get existing serial interface baudrate parameter. Only standard values are supported.
##
## @var{serial} - instance of @var{octave_serial} class.
## @var{baudrate} - the baudrate value used. Supported values: 0, 50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800, 2400, 4800, 9600 19200, 38400, 57600, 115200 and 230400.
##
## If @var{baudrate} parameter is omitted, the srl_baudrate() shall return current baudrate value as the result @var{br}.
##
## This function is obsolete. Use get and set method instead.
##
## @end deftypefn
function retval = srl_baudrate (serial, baudrate)

  try
    if (nargin>1)
      __srl_properties__ (serial, 'baudrate', baudrate);
    else
      retval = __srl_properties__ (serial, 'baudrate');
    end
  catch
    print_usage();
  end
end
