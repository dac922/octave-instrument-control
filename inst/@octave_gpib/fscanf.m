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
## @deftypefn {Function File} {@var{res} =} fscanf (@var{obj})
## @deftypefnx {Function File} {@var{res} =} fscanf (@var{obj}, @var{format})
## @deftypefnx {Function File} {@var{res} =} fscanf (@var{obj}, @var{format}, @var{size})
## @deftypefnx {Function File} {[@var{res},@var{count}] =} fscanf (@var{obj}, ...)
## @deftypefnx {Function File} {[@var{res},@var{count},@var{errmsg}] =} fscanf (@var{obj}, ...)
## Reads data @var{res} from GPIB instrument
##
## @var{obj} is a GPIB object
##
## @var{format} Format specifier
## @var{size} number of values
##
## @var{count} values read
## @var{errmsg} read operation error message
##
## @end deftypefn

## TODO: 
function [res, count, errmsg] = fscanf (obj, format, size)

if (nargin < 1)
  print_usage ();
end

if (nargin < 2)
  format = '%c';
end

% TODO: use a max buffer property?
buffersize = 1e6;

eoi=0; tmp = [];
while (!eoi)
  [tmp1,~,eoi] = gpib_read (obj, buffersize);
  %% if successful tmp is never negative (uint8)
  if ((!eoi) || (tmp < 0))
    break;
  end
  tmp = [tmp tmp1];
end

if (nargin < 3)
  [res,count,errmsg]=sscanf (tmp,format);
else
  [res,count,errmsg]=sscanf (tmp,format,size);
end

end
