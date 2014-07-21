## Copyright (C) 2013 Pantxo Diribarne

## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 3 of the License, or
## (at your option) any later version.

## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.

## You should have received a copy of the GNU General Public License
## along with Octave; see the file COPYING.  If not, see
## <http://www.gnu.org/licenses/>.


## This is roughly translated from random-test-client.c. See original
## libmodbus test files.

more off

LOOP = 20;
ADDRESS_START = 0;
ADDRESS_END = 99;

## load modbus functions
if (! exist ("modbus_new_tcp"))
  load_modbus ();
endif

## launch test server in the background, it will stop when the
## connection is closed.
here = fileparts (mfilename ("fullpath"));
system ([fullfile(here, "random-test-server") "&"]);
pause (1);                      # avoid race condition

ctx = modbus_new_tcp ("127.0.0.1", 1502);

if (modbus_connect (ctx) != 0)
  modbus_free (ctx);
  error ("Connection failed.");
endif
    
nb = ADDRESS_END - ADDRESS_START;
tab_rq_bits = uint8 (zeros (1, nb));
    
tab_rp_bits = tab_rq_bits;

tab_rq_registers = uint16 (zeros (1, nb));

tab_rp_registers = tab_rq_registers;

tab_rw_rq_registers = tab_rq_registers;

nb_loop = nb_fail = 0;

while (nb_loop++ < LOOP)
  %disp (nb_loop)
  for addr = ADDRESS_START:(ADDRESS_END-1)
    ##/* Random numbers (short) */
    tab_rq_registers = cast (rand (1, nb), "uint16");
    tab_rw_rq_registers = tab_rq_registers;
    tab_rq_bits = round (tab_rq_registers);
    regnum =  linspace (addr, addr+nb-1, nb);

    ##/* MULTIPLE BITS */
    rc = modbus_write_bits (ctx, regnum, tab_rq_bits);
    if (rc != nb) 
      printf("ERROR modbus_write_bits (%d)\n", rc);
      printf("Address = %d, nb = %d\n", addr, nb);
      nb_fail++;
    else 
      tab_rp_bits = modbus_read_bits(ctx, regnum);
      if (! all (tab_rp_bits == tab_rq_bits))
        printf("ERROR modbus_read_bits\n");
        nb_fail++;
      endif
    endif
   
    ##/* MULTIPLE REGISTERS */
    bitnum = regnum;
    rc = modbus_write_registers (ctx, bitnum, tab_rq_registers);
    if (rc != nb)
      printf("ERROR modbus_write_registers (%d)\n", rc);
      printf("Address = %d, nb = %d\n", addr, nb);
      nb_fail++;
    else 
      tab_rp_registers = modbus_read_registers(ctx, bitnum);
      if (! all (tab_rp_registers == tab_rq_registers))
        printf("ERROR modbus_read_registers\n");
        nb_fail++;
      endif
    endif
    if (nb_fail)
      printf("Test %d FAILS\n", nb_fail);
    endif
    nb = ADDRESS_END - addr;
  endfor
endwhile

printf ("RANDOM TEST: %d PASSED AMONG %d\n", ...
        LOOP - nb_fail, LOOP)

## /* Close the connection */
modbus_close (ctx);
modbus_free (ctx);
