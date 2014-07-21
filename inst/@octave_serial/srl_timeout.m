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
## @deftypefn {Loadable Function} {} srl_timeout (@var{serial}, @var{timeout})
## @deftypefnx {Loadable Function} {@var{t} = } srl_timeout (@var{serial})
##
## Set new or get existing serial interface timeout parameter used for srl_read() requests. The timeout value is specified in tenths of a second.
##
## @var{serial} - instance of @var{octave_serial} class.
## @var{timeout} - srl_read() timeout value in tenths of a second. Value of -1 means a blocking call. Maximum value of 255 (i.e. 25.5 seconds).
##
## If @var{timeout} parameter is omitted, the srl_timeout() shall return current timeout value as the result @var{t}.
##
## This function is obsolete. Use get and set method instead.
##
## @end deftypefn
function retval = srl_timeout (serial, timeout)

  try
    if (nargin>1)
      __srl_properties__ (serial, 'timeout', timeout);
    else
      retval = __srl_properties__ (serial, 'timeout');
    end
  catch
    print_usage();
  end
end
