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

## load_modbus

## Author: pdiribarne <pdiribarne@macbook-pro-de-pantxo-diribarne.local>
## Created: 2012-11-05

function load_modbus ()
  funclist = {'modbus_free', 'modbus_close', ...
              'modbus_connect', 'modbus_new_tcp', ...
              'modbus_flush', 'modbus_read_bits', ...
              'modbus_write_bits', 'modbus_read_registers', ...
              'modbus_write_registers', ...
              'modbus_read_input_registers', ...
              'modbus_read_input_bits'};

  here = fileparts (mfilename ("fullpath"));
  
  for ii = 1:numel (funclist)
    autoload (funclist{ii}, fullfile (here, 'oct-modbus.oct'))
    ## autoload (funclist{ii}, fullfile (here, 'oct-modbus.oct'))
  endfor
endfunction
