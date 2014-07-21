## Copyright (C) 2013 Pantxo Diribarne
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

## bandwidth_client:
## This is a rough translation of the original c file (see libmodbus sources)
## write_read operations are not implemented

page_out = page_screen_output ();
n_loop = 1000;

## load modbus_constants
cst = modbus_constants ();

## Allocate and initialize the memory to store the status 
tab_bit = uint8 (zeros (1, cst.MODBUS_MAX_READ_BITS));

## Allocate and initialize the memory to store the registers 
tab_reg = uint16 (zeros (1, cst.MODBUS_MAX_READ_REGISTERS));

## load modbus functions
if (! exist ("modbus_new_tcp"))
  load_modbus ();
endif

## launch test server in the background, it will stop when the
## connection is closed.
here = fileparts (mfilename ("fullpath"));
system ([fullfile(here, "bandwidth-server-one") "&"]);
pause (1);



## Tests based on PI-MBUS-300 documentation 
tic ();

## get a context to the requested server
ctx = modbus_new_tcp ("127.0.0.1", 1502);

unwind_protect
  ## connect
  status = modbus_connect (ctx);

  if (status != 0)
    modbus_free (ctx);
    error ("couldn't connect\n")
  endif

  

  more off
  printf("READ BITS\n\n");
  nb_points = cst.MODBUS_MAX_READ_BITS;

  ## start reading bits
  start = toc ();

  idx = 0:1:(nb_points - 1);
  for i=1:n_loop
    tab_bit = modbus_read_bits (ctx, idx);
  endfor
  theend = toc();

  elapsed = theend - start;

  rate = (n_loop * nb_points) / elapsed;
  printf("Transfert rate in points/seconds:\n");
  printf("* %d points/s\n", rate);
  printf("\n");

  bytes = n_loop * floor (nb_points / 8) + mod (nb_points, 8) ;
  rate = bytes / 1024 / elapsed;
  printf("Values:\n");
  printf("* %d x %d values\n", n_loop, nb_points);
  printf("* %.3f ms for %d bytes\n", elapsed*1000, bytes);
  printf("* %d KiB/s\n", rate);
  printf("\n");

  ## TCP: Query and reponse header and values 
  bytes = 12 + 9 + floor (nb_points / 8) + mod (nb_points, 8);
  printf("Values and TCP Modbus overhead:\n");
  printf("* %d x %d bytes\n", n_loop, bytes);
  bytes = n_loop * bytes;
  rate = bytes / 1024 / elapsed;
  printf("* %.3f ms for %d bytes\n", elapsed*1000, bytes);
  printf("* %d KiB/s\n", rate);
  printf("\n\n");


  printf("READ REGISTERS\n\n");

  nb_points = cst.MODBUS_MAX_READ_REGISTERS;

  ## start reading registers
  start = toc ();
  idx = 0:1:(nb_points - 1);
  for i= 1:n_loop
    tab_reg = modbus_read_registers (ctx, idx);
  endfor
  theend = toc();
  elapsed = theend - start;

  rate = (n_loop * nb_points) / elapsed;
  printf("Transfert rate in points/seconds:\n");
  printf("* %d points/s\n", rate);
  printf("\n");

  bytes = n_loop * nb_points * sizeof (tab_reg(1));
  rate = bytes / 1024 / elapsed;
  printf("Values:\n");
  printf("* %d x %d values\n", n_loop, nb_points);
  printf("* %.3f ms for %d bytes\n", elapsed*1000, bytes);
  printf("* %d KiB/s\n", rate);
  printf("\n");
  ## TCP:Query and reponse header and values 
  bytes = 12 + 9 + (nb_points * sizeof (tab_reg (1)));
  printf("Values and TCP Modbus overhead:\n");
  printf("* %d x %d bytes\n", n_loop, bytes);
  bytes = n_loop * bytes;
  rate = bytes / 1024 / elapsed;
  printf("* %.3f ms for %d bytes\n", elapsed*1000, bytes);
  printf("* %d KiB/s\n", rate);
  printf("\n\n");
  modbus_close(ctx);
  
unwind_protect_cleanup
  if (page_out)
    more on
  endif
  modbus_free(ctx);
end_unwind_protect
