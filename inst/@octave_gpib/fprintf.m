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
## @deftypefn {Function File} {} frintf (@var{obj}, @var{cmd})
## @deftypefnx {Function File} {} frintf (@var{obj}, @var{format}, @var{cmd})
## @deftypefnx {Function File} {} frintf (@var{obj}, @var{cmd}, @var{mode})
## @deftypefnx {Function File} {} frintf (@var{obj}, @var{format}, @var{cmd}, @var{mode})
## Writes string @var{cmd} to GPIB instrument
##
## @var{obj} is a GPIB object
##
## @var{cmd} String 
## @var{format} Format specifier
## @var{mode} sync
##
## @end deftypefn

## TODO: 
function fprintf (obj, format, cmd, mode)

defaultformat = '%s\n';
defaultmode = 'sync';

if ((nargin < 2) || (nargin > 4))
  print_usage ();
elseif (nargin < 3)
  format = defaultformat;
  mode = defaultmode;
elseif (nargin < 4)
  %% decide for syntax
  if (!isempty (find (format == '%')))
    %% detected: fprintf (obj, format, cmd))
    mode = defaultmode;
  else
    %% fprintf (obj, cmd, mode)
    mode = cmd;
    cmd = format;
    format = defaultformat;
  end
end

if (! ( ischar (format) && ischar (mode) ))
  print_usage ();
end

if (strcmp (mode, 'async'))
  error ("async mode not supported yet");
end

gpib_write (obj, sprintf (format, cmd))

end
