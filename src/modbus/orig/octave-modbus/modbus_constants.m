%%             
%% Copyright (C) 2008-2012 Julien Salort
%%
%% This file is part of OctMI.
%%
%% OctMI is free software; you can redistribute it and/or modify it
%% under the terms of the GNU General Public License as published by the
%% Free Software Foundation; either version 3 of the License, or (at your
%% option) any later version.
%%
%% OctMI is distributed in the hope that it will be useful, but WITHOUT
%% ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
%% FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
%% for more details.
%%                                                        
%% You should have received a copy of the GNU General Public License
%% along with Octave; see the file LICENSE.  If not, see
%% <http://www.gnu.org/licenses/>.
%%
## Copyright (C) 2012 pdiribarne
## 
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 3 of the License, or
## (at your option) any later version.
## 
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with Octave; see the file COPYING.  If not, see
## <http://www.gnu.org/licenses/>.

## -- Function: struc = modbus_constants
## Returns a structure containing the various constants associated
## to modbus.h

## Author: pdiribarne <pdiribarne@new-host.home>
## Created: 2012-05-01

function [ cst ] = modbus_constants ()
  if nargin > 0
    print_usage
  endif
  cst.MODBUS_MAX_READ_BITS = 2000;
  cst.MODBUS_MAX_WRITE_BITS = 1968;
  cst.MODBUS_MAX_READ_REGISTERS = 125;
  cst.MODBUS_MAX_WRITE_REGISTERS = 123;
  cst.MODBUS_MAX_RW_WRITE_REGISTERS = 121;
endfunction
