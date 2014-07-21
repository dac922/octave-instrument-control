## Copyright (C) 2013 Stefan Mahr <dac922@gmx.de>
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
## @deftypefn {Function File} {} fwrite (@var{obj}, @var{data})
## @deftypefnx {Function File} {} fwrite (@var{obj}, @var{data}, @var{precision})
## @deftypefnx {Function File} {} fwrite (@var{obj}, @var{data}, @var{mode})
## @deftypefnx {Function File} {} fwrite (@var{obj}, @var{data}, @var{precision}, @var{mode})
## Writes @var{data} to GPIB instrument
##
## @var{obj} is a GPIB object
##
## @var{data} data to write
## @var{precision} precision of data
## @var{mode} sync
##
## @end deftypefn

## TODO: 
function fwrite(obj, data, precision, mode)

defaultmode = "sync";

if (nargin < 2)
  print_usage ();
elseif (nargin < 3)
  precision = [];
  mode = defaultmode;
elseif (nargin < 4)
  %% is 3rd argument precision or mode
  if (strcmp (precision,'sync') || strcmp (precision,'async'))
    mode = precision;
    precision = [];
  else
    mode = "sync";
  end
end

if (strcmp (mode,'async'))
  error ("async mode not supported yet");
end

switch (precision)
  case {"char" "schar" "int8"}
    data = int8 (data);
  case {"uchar" "uint8"}
    data = uint8 (data);
  case {"int16" "short"}
    data = int16 (data);
  case {"uint16" "ushort"}
    data = uint16 (data);
  case {"int32" "int"}
    data = int32 (data);
  case {"uint32" "uint"}
    data = uint32 (data);
  case {"long" "int64"}
    data = int64 (data);
  case {"ulong" "uint64"}
    data = uint64 (data);
  case {"single" "float" "float32"}
    data = single (data);
  case {"double" "float64"}
    data = double (data);
  case []
    %% use data as it is
  otherwise
    error ("precision not supported");
end

%% should we handle endianess ?
gpib_write (obj, typecast(data,'uint8'));

end
