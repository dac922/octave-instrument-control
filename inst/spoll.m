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
## @deftypefn {Function File} {@var{out} =} spoll (@var{obj})
## @deftypefnx {Function File} {[@var{out},@var{statusByte}] =} spoll (@var{obj})
## Serial polls GPIB instruments.
##
## @var{obj} is a GPIB object or a cell array of GPIB objects
##
## @var{out} GPIB objects ready for service
## @var{statusByte} status Byte
##
## @end deftypefn

## TODO: 

function [out,statusByte] = spoll (obj)
#

if (nargin < 1)
  print_usage ();
end

if iscell (obj) && numel (obj) > 0
  if ~all (cellfun (@(x) isa (x,'octave_gpib'),obj))
    error ("obj contains wrong elements");
  end
  
  out = {};
  statusByte = [];
  for i = 1:numel (obj)
    tmp_status = uint8 (__gpib_spoll__ (obj{i}));
    if (bitget (tmp_status,7) == 0)
      out{end+1} = obj{i};
      statusByte(end+1) = tmp_status;
    end
  end
  
  return
  
elseif (!isa (obj,'octave_gpib'))
  error ('spoll: need octave_gpib object');
end

out = [];
statusByte = [];

tmp_status = uint8 (__gpib_spoll__ (obj));
if (bitget (tmp_status,7) == 0)
  out = obj;
  statusByte = tmp_status;
end
