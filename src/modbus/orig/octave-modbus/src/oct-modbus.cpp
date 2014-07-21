/*
Copyright (C) 2013 Pantxo Diribarne

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Octave; see the file COPYING.  If not, see
<http://www.gnu.org/licenses/>.

*/

#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <modbus/modbus.h>
#include <octave/oct.h>
#include <octave/parse.h>


static std::map<std::string, modbus_t*> ctxmap;

int mymin (uint16NDArray tabin)
{
  int current = 65535;
  for (int ii = 0; ii < tabin.nelem (); ii ++)
    if (current > (int) tabin(ii))
      current = (int) tabin(ii);
  return current;
};

int mymax (uint16NDArray tabin)
{
  int current = 0;
  for (int ii = 0; ii < tabin.nelem (); ii ++)
    if (current < (int) tabin(ii))
      current = (int) tabin(ii);
  return (current);
};



//New tcp_modbus context
DEFUN_DLD (modbus_new_tcp, args, nargout, "-*- texinfo -*-\n\
@deftypefn {Function File} {@var{CTX} = } modbus_new_tcp (@var{IPSTRING}, @var{PORT})\n \
Opens a modbus comunication context for address @var{IPSTRING} (string) through @var{PORT} (int),\n\
and returns a unique identifier @var{CTX} to be used for further modbus operations. \n\
The created context should be freed using modbus_free function.\n\
@seealso{modbus_free, modbus_connect}\n\
@end deftypefn")
{
  octave_value_list retval;
  
  if (args.length() != 2)
    {
      print_usage ();
      return retval;
    }

  std::string add = args(0).string_value();
  int port = args(1).int_value();
  
  if (error_state)
    {
      print_usage ();
      return retval;
    }

  //Create context
  if (ctxmap.find (add) == ctxmap.end ())
    {
      ctxmap[add] = modbus_new_tcp (add.c_str(), port);
      retval.append (octave_value (add));
    }
  else
    {
      octave_stdout << "modbus: modbus_new_tcp: a modbus context to " << 
        add << "already exists, close it first." << std::endl;  
    }
  return retval;
}

//Connect

DEFUN_DLD (modbus_connect, args, nargout, "-*- texinfo -*-\n\
@deftypefn {Function File} {@var{STATUS} = } modbus_connect (@var{CTX})\n\
Connects to the device associated to @var{CTX} and returns @var{STATUS} (0 if ok).\n\
@seealso{modbus_new_tcp, modbus_close, modbus_free}\n\
@end deftypefn")
{
  octave_value_list retval;
  
  if (args.length() != 1)
    {
      print_usage ();
      return retval;
    }
  
  std::string key = args(0).string_value();
  
  if (error_state)
    {
      print_usage ();
      return retval;
    }
  else if (ctxmap.find (key) == ctxmap.end ()) 
    {
      octave_stdout << "modbus_connect: no modbus context is associated to " 
                    << key << std::endl;  
      return retval;
    }

  modbus_t *ctx = ctxmap[key];
  
  //Connection
  int status = modbus_connect(ctx);
  if (status == -1) 
    {
      octave_stdout << "Connection failed: "
                    << std::string (modbus_strerror(errno)) << std::endl;
    }

  retval.append (octave_value(status));

  return retval;
}

//Close 

DEFUN_DLD (modbus_close, args, nargout, "-*- texinfo -*-\n\
@deftypefn {Function File} {} modbus_close (@var{CTX})\n\
Closes modbus connection to device associated with @var{CTX}.\n\
@seealso{modbus_new_tcp, modbus_free}\n\
@end deftypefn")
{
  octave_value_list retval;

  if (args.length() != 1)
    {
      print_usage ();
      return retval;
    }

  std::string key = args(0).string_value();
  
  if (error_state)
    {
      print_usage ();
      return retval;
    }
  else if (ctxmap.find (key) == ctxmap.end ()) 
    {
      octave_stdout << "modbus_close: no modbus context is associated to " 
                    << key << std::endl;  
      return retval;
    }

  modbus_t *ctx = ctxmap[key];
  modbus_close (ctx);

  return retval;
}

//Free

DEFUN_DLD (modbus_free, args, nargout, "-*- texinfo -*-\n\
@deftypefn {Function File} {} modbus_free (@var{CTX})\n\
Free the pointer to the context @var{CTX} created by modmub_new_tcp.\n\
@seealso{modbus_new_tcp, modbus_close}\n\
@end deftypefn")
{
  octave_value_list retval;
  
  if (args.length() != 1)
    {
      print_usage ();
      return retval;
    }

  std::string key = args(0).string_value();
  
  if (error_state)
    {
      print_usage ();
      return retval;
    }
  else if (ctxmap.find (key) == ctxmap.end ()) 
    {
      octave_stdout << "modbus_free: no modbus context is associated to " 
                    << key << std::endl;  
      return retval;
    }

  modbus_t *ctx = ctxmap[key];

  ctxmap.erase (ctxmap.find(key));

  modbus_free(ctx);

  return retval;
}

//Flush

DEFUN_DLD (modbus_flush, args, nargout, "-*- texinfo -*-\n\
@deftypefn {Function File} {@var{STATUS} = } modbus_flush (@var{CTX})\n\
Discard data received but not read to the socket or file descriptor\n\
associated to the context @var{CTX}. Returns @var{STATUS} = 0 if succesfull.\n\
@seealso{modbus_new_tcp, modbus_close}\n\
@end deftypefn")
{
  octave_value_list retval;
  
  if (args.length() != 1)
    {
      print_usage ();
      return retval;
    }

  std::string key = args(0).string_value();
  
  if (error_state)
    {
      print_usage ();
      return retval;
    }
  else if (ctxmap.find (key) == ctxmap.end ()) 
    {
      octave_stdout << "modbus_flush: no modbus context is associated to " 
                    << key << std::endl;  
      return retval;
    }

  modbus_t *ctx = ctxmap[key];

  int status = modbus_flush (ctx);

  retval.append (octave_value (status));

  return retval;
}

//Read bits

DEFUN_DLD (modbus_read_bits, args, nargout, "-*- texinfo -*-\n\
@deftypefn {Function File} {@var{BITOUT} = } modbus_read_bits (@var{CTX}, @var{BITNUM})\n\
@deftypefnx {Function File} {@var{BITOUT} = } modbus_read_bits (@var{CTX}, @var{BITNUM}, @var{INDIV})\n\
Read bit(s) @var{BITNUM} (vector of indices >= 0) on the device associated with @var{CTX}, \n\
and return the content of the bits in the form of a row vector @var{BITOUT} of type \"uint8\". \n\n\
By default, the bits are retrieved all at once from the server, \
including all the bits from the lowest index to the highest. \
Only the requested ones are returned in @var{BITOUT}. \
This can be slow if only few distant bits are resquested, and impossible \
if the max number of read bits is reached (see modbus_constants). \
To disable this behavior a third argument @var{INDIV} can be set to true.\n\
@seealso{modbus_new_tcp, modbus_write_bits}\n\
@end deftypefn")
{
  octave_value_list retval;
  int nargin = args.length();
  if (nargin < 2 || nargin > 3)
    {
      print_usage ();
      return retval;
    }

  std::string key = args(0).string_value();
  
  if (error_state)
    {
      print_usage ();
      return retval;
    }
  else if (ctxmap.find (key) == ctxmap.end ()) 
    {
      error ("modbus_read_bits: no modbus context is associated to %s", 
             key.c_str ());  
      return retval;
    }

  modbus_t *ctx = ctxmap[key];
  
  // Check indices
  uint16NDArray idx = args(1).uint16_array_value ();
  bool indiv = false;
  if (nargin == 3)
    indiv = args(2).bool_value ();
  
  if (error_state || ! idx.is_vector ())
    {
      print_usage ();
      return retval;
    }

  int lower = mymin (idx);
  int higher = mymax (idx);

  if (lower < 0)
    {
      error ("modbus_read_bits: expect positive or 0 indices"); 
      return retval;
    }

  int nb = idx.nelem ();
  int nb_read = higher - lower + 1;

  // Initialize octave output table
  dim_vector dv;
  dv(0) = 1;
  dv(1) = nb;
  uint8NDArray tabout (dv);
  
  
  int rc;
   
  if (indiv | nb == 1)
    {
      uint8_t val;
      for (int ii = 0; ii < nb; ii++)
        {
          rc = modbus_read_bits (ctx, idx(ii), 1, &val);
          tabout(ii) = val;
        }
    }
  else
    {
      if (nb_read > MODBUS_MAX_READ_BITS)
        {
          error ("modbus_read_bits: max number of read bits is %d",
                 MODBUS_MAX_READ_BITS);
          return retval;
        }
      uint8_t tabval[nb_read * sizeof (uint8_t)];
      rc = modbus_read_bits (ctx, lower, nb_read, tabval);
  
      //Populate octave output
      if (rc == nb_read)
        { 
          for (int ii = 0; ii < nb; ii++)
            {
              tabout(ii) = tabval[((int) idx (ii) - lower)];
            }
        }
    }
  
  retval.append(octave_value(tabout));

  return retval;
}

//Write bits

DEFUN_DLD (modbus_write_bits, args, nargout, "-*- texinfo -*-\n\
@deftypefn {Function File} {@var{NB} = } modbus_write_bits (@var{CTX}, @var{BITNUM}, @var{VALUES})\n\
@deftypefnx {Function File} {@var{NB} = } modbus_write_bits (@var{CTX}, @var{BITNUM}, @var{VALUES}, @var{INDIV})\n\
Write @var{VALUES} (0 or 1) in bit(s) @var{BITNUM} (numeric row vector >= 0) on the device \n\
associated with @var{CTX}, and returns the number of successfully written bits, @var{NB}. \n\
@var{VALUES} row vector must be \"uint8\" and be the same length as @var{BITNUM}.\n\n\
The implementation involves reading and writing ranges of bits and may \n\
sometimes fail or be slow (see modbus_read_bits help). Set the \n\
optional argument @var{INDIV} true, to write each register individually.\n\
@seealso{modbus_new_tcp, modbus_read_bits}\n\
@end deftypefn")
{
  octave_value_list retval;

  int nargin = args.length();

  if (nargin < 3 || nargin > 4)
    {
      print_usage ();
      return retval;
    }

  std::string key = args(0).string_value();
  
  if (error_state)
    {
      print_usage ();
      return retval;
    }
  else if (ctxmap.find (key) == ctxmap.end ()) 
    {
      error ("modbus_write_bits: no modbus context is associated to %s", 
             key.c_str ());  
      return retval;
    }

  modbus_t *ctx = ctxmap[key];
  
  // Check indices
  uint16NDArray idx = args(1).uint16_array_value ();
  uint8NDArray val = args(2).uint8_array_value ();

  bool indiv = false;
  if (nargin == 4)
    indiv = args(3).bool_value ();
  
  if (error_state || ! idx.is_vector () || idx.nelem () != val.nelem ())
    {
      print_usage ();
      return retval;
    }

  int lower = mymin (idx);
  int higher = mymax (idx);

  if (lower < 0)
    {
      error ("modbus_write_bits: expect positive or 0 indices"); 
      return retval;
    }

  int nb = idx.nelem ();
  int nb_read = higher - lower + 1;

  // Initialize octave output table
  dim_vector dv;
  dv(0) = 1;
  dv(1) = nb;
  uint8NDArray tabout (dv);
  
  
  int rc = 0;
   
  if (indiv | nb == 1)
    {
      for (int ii = 0; ii < nb; ii++)
        {
          uint8_t tmp[1];
          tmp[0] = (uint8_t) val(ii);
          rc += modbus_write_bits (ctx, idx(ii), 1, tmp);
        }
    }
  else
    {
      if (nb_read > MODBUS_MAX_READ_BITS)
        {
          error ("modbus_write_bits: max number of read bits is %d",
                 MODBUS_MAX_READ_BITS);
          return retval;
        }
      uint8_t tabval[nb_read * sizeof (uint8_t)];
      // Read current values
      rc = modbus_read_bits (ctx, lower, nb_read, tabval);
  
      if (rc == nb_read)
        { 
          rc = 0;
          // Replace with new
          for (int ii = 0; ii < nb; ii++)
            tabval[(int) idx(ii) - lower] = (uint8_t) val(ii);

          // Write
          rc += modbus_write_bits (ctx, lower, nb_read, tabval);
          if (rc != nb_read)
            octave_stdout << std::string (modbus_strerror(errno)) << std::endl;
        }
      else
        rc = 0;
    }
  
  retval.append(octave_value(rc));

  return retval;
}

//Read registers

DEFUN_DLD (modbus_read_registers, args, nargout, "-*- texinfo -*-\n\
@deftypefn {Function File} {@var{REGOUT} = } modbus_read_registers (@var{CTX}, @var{REGNUM})\n\
@deftypefnx {Function File} {@var{REGOUT} = } modbus_read_registers (@var{CTX}, @var{REGNUM}, @var{INDIV})\n\
Read bit(s) @var{REGNUM} (vector of indices >= 0) on the device associated with @var{CTX}, \n\
and return the content of the registers in the form of a row vector @var{REGOUT} of type \"uint16\". \n\n\
By default, the registers are retrieved all at once from the server, \
including all the registers from the lowest index to the highest. \
Only the requested ones are returned in @var{REGOUT}. \
This can be slow if only few distant registers are resquested, and impossible \
if the max number of read registers is reached (see modbus_constants). \
To disable this behavior a third argument @var{INDIV} can be set to true.\n\
@seealso{modbus_new_tcp, modbus_write_registers}\n\
@end deftypefn")
{
  octave_value_list retval;
  int nargin = args.length();
  if (nargin < 2 || nargin > 3)
    {
      print_usage ();
      return retval;
    }

  std::string key = args(0).string_value();
  
  if (error_state)
    {
      print_usage ();
      return retval;
    }
  else if (ctxmap.find (key) == ctxmap.end ()) 
    {
      error ("modbus_read_registers: no modbus context is associated to %s", 
             key.c_str ());  
      return retval;
    }

  modbus_t *ctx = ctxmap[key];
  
  // Check indices
  uint16NDArray idx = args(1).uint16_array_value ();
  bool indiv = false;
  if (nargin == 3)
    indiv = args(2).bool_value ();
  
  if (error_state || ! idx.is_vector ())
    {
      print_usage ();
      return retval;
    }

  int lower = mymin (idx);
  int higher = mymax (idx);

  if (lower < 0)
    {
      error ("modbus_read_registers: expect positive or 0 indices"); 
      return retval;
    }

  int nb = idx.nelem ();
  int nb_read = higher - lower + 1;

  // Initialize octave output table
  dim_vector dv;
  dv(0) = 1;
  dv(1) = nb;
  uint16NDArray tabout (dv);
  
  
  int rc;
   
  if (indiv | nb == 1)
    {
      uint16_t val;
      for (int ii = 0; ii < nb; ii++)
        {
          rc = modbus_read_registers (ctx, idx(ii), 1, &val);
          tabout(ii) = val;
        }
    }
  else
    {
      if (nb_read > MODBUS_MAX_READ_REGISTERS)
        {
          error ("modbus_read_registers: max number of read registers is %d",
                 MODBUS_MAX_READ_REGISTERS);
          return retval;
        }
      uint16_t tabval[nb_read * sizeof (uint16_t)];
      rc = modbus_read_registers (ctx, lower, nb_read, tabval);
  
      //Populate octave output
      if (rc == nb_read)
        { 
          for (int ii = 0; ii < nb; ii++)
            {
              tabout(ii) = tabval[((int) idx (ii) - lower)];
            }
        }
    }
  
  retval.append(octave_value(tabout));

  return retval;
}

//Write registers

DEFUN_DLD (modbus_write_registers, args, nargout, "-*- texinfo -*-\n\
@deftypefn {Function File} {@var{NB} = } modbus_write_registers (@var{CTX}, @var{REGNUM}, @var{VALUES})\n\
@deftypefnx {Function File} {@var{NB} = } modbus_write_registers (@var{CTX}, @var{REGNUM}, @var{VALUES},  @var{INDIV})\n\
Write @var{VALUES} in registers @var{REGNUM} (vector of indices >= 0) on the device \n\
associated with @var{CTX}, and returns the number of successfully written registers, @var{NB}. \n\
@var{VALUES} vector must be \"uint16\" and be the same length as @var{REGNUM}.\n\n\
The implementation involves reading and writing ranges of registers and may \n\
sometimes fail or be slow (see modbus_read_registers help). Set the \n\
optional argument @var{INDIV} true, to write each register individually.\n\
@seealso{modbus_new_tcp, modbus_read_registers}\n\
@end deftypefn")
{
  octave_value_list retval;

  int nargin = args.length();

  if (nargin < 3 || nargin > 4)
    {
      print_usage ();
      return retval;
    }

  std::string key = args(0).string_value();
  
  if (error_state)
    {
      print_usage ();
      return retval;
    }
  else if (ctxmap.find (key) == ctxmap.end ()) 
    {
      error ("modbus_write_registers: no modbus context is associated to %s", 
             key.c_str ());  
      return retval;
    }

  modbus_t *ctx = ctxmap[key];
  
  // Check inputs
  uint16NDArray idx = args(1).uint16_array_value ();
  uint16NDArray val = args(2).uint16_array_value ();

  bool indiv = false;
  if (nargin == 4)
    indiv = args(3).bool_value ();
  
  if (error_state || ! idx.is_vector () || idx.nelem () != val.nelem ())
    {
      print_usage ();
      return retval;
    }

  int lower = mymin (idx);
  int higher = mymax (idx);

  if (lower < 0)
    {
      error ("modbus_write_registers: expect positive or 0 indices"); 
      return retval;
    }

  int nb = idx.nelem ();
  int nb_read = higher - lower + 1;

  // Initialize octave output table
  dim_vector dv;
  dv(0) = 1;
  dv(1) = nb;
  uint16NDArray tabout (dv);
  
  
  int rc = 0;
   
  if (indiv | nb == 1)
    {
      for (int ii = 0; ii < nb; ii++)
        {
          uint16_t tmp[1];
          tmp[0] = (uint16_t) val(ii);
          rc += modbus_write_registers (ctx, idx(ii), 1, tmp);
        }
    }
  else
    {
      if (nb_read > MODBUS_MAX_READ_REGISTERS)
        {
          error ("modbus_write_registers: max number of read registers is %d",
                 MODBUS_MAX_READ_REGISTERS);
          return retval;
        }
      uint16_t tabval[nb_read * sizeof (uint16_t)];
      // Read current values
      rc = modbus_read_registers (ctx, lower, nb_read, tabval);
  
      if (rc == nb_read)
        { 
          rc = 0;
          // Replace with new
          for (int ii = 0; ii < nb; ii++)
            tabval[(int) idx(ii) - lower] = (uint16_t) val(ii);

          // Write
          rc += modbus_write_registers (ctx, lower, nb_read, tabval);
          if (rc != nb_read)
            octave_stdout << std::string (modbus_strerror(errno)) << std::endl;
        }
      else
        rc = 0;
    }
  
  retval.append(octave_value(rc));

  return retval;
}

//Read input bits

DEFUN_DLD (modbus_read_input_bits, args, nargout, "-*- texinfo -*-\n\
@deftypefn {Function File} {@var{BITOUT} = } modbus_read_input_bits (@var{CTX}, @var{BITNUM})\n\
@deftypefnx {Function File} {@var{BITOUT} = } modbus_read_input_bits (@var{CTX}, @var{BITNUM}, @var{INDIV})\n\
Read bit(s) @var{BITNUM} (vector of indices >= 0) on the device associated with @var{CTX}, \n\
and return the content of the input_bits in the form of a row vector @var{BITOUT} of type \"uint8\". \n\n\
By default, the bits are retrieved all at once from the server, \
including all the bits from the lowest index to the highest. \
Only the requested ones are returned in @var{BITOUT}. \
This can be slow if only few distant bits are resquested, and impossible \
if the max number of read bits is reached (see modbus_constants). \
To disable this behavior a third argument @var{INDIV} can be set to true.\n\
@seealso{modbus_new_tcp, modbus_write_bits}\n\
@end deftypefn")
{
  octave_value_list retval;
  int nargin = args.length();
  if (nargin < 2 || nargin > 3)
    {
      print_usage ();
      return retval;
    }

  std::string key = args(0).string_value();
  
  if (error_state)
    {
      print_usage ();
      return retval;
    }
  else if (ctxmap.find (key) == ctxmap.end ()) 
    {
      error ("modbus_read_input_bits: no modbus context is associated to %s", 
             key.c_str ());  
      return retval;
    }

  modbus_t *ctx = ctxmap[key];
  
  // Check indices
  uint16NDArray idx = args(1).uint16_array_value ();
  bool indiv = false;
  if (nargin == 3)
    indiv = args(2).bool_value ();
  
  if (error_state || ! idx.is_vector ())
    {
      print_usage ();
      return retval;
    }

  int lower = mymin (idx);
  int higher = mymax (idx);

  if (lower < 0)
    {
      error ("modbus_read_input_bits: expect positive or 0 indices"); 
      return retval;
    }

  int nb = idx.nelem ();
  int nb_read = higher - lower + 1;

  // Initialize octave output table
  dim_vector dv;
  dv(0) = 1;
  dv(1) = nb;
  uint8NDArray tabout (dv);
  
  
  int rc;
   
  if (indiv | nb == 1)
    {
      uint8_t val;
      for (int ii = 0; ii < nb; ii++)
        {
          rc = modbus_read_input_bits (ctx, idx(ii), 1, &val);
          tabout(ii) = val;
        }
    }
  else
    {
      if (nb_read > MODBUS_MAX_READ_BITS)
        {
          error ("modbus_read_input_bits: max number of read bits is %d",
                 MODBUS_MAX_READ_BITS);
          return retval;
        }
      uint8_t tabval[nb_read * sizeof (uint8_t)];
      rc = modbus_read_input_bits (ctx, lower, nb_read, tabval);
  
      //Populate octave output
      if (rc == nb_read)
        { 
          for (int ii = 0; ii < nb; ii++)
            {
              tabout(ii) = tabval[((int) idx (ii) - lower)];
            }
        }
    }
  
  retval.append(octave_value(tabout));

  return retval;
}

//Read input registers

DEFUN_DLD (modbus_read_input_registers, args, nargout, "-*- texinfo -*-\n\
@deftypefn {Function File} {@var{REGOUT} = } modbus_read_input_registers (@var{CTX}, @var{REGNUM})\n\
@deftypefnx {Function File} {@var{REGOUT} = } modbus_read_input_registers (@var{CTX}, @var{REGNUM}, @var{INDIV})\n\
Read registers @var{REGNUM} on the device associated with @var{CTX}, \n\
and return the content of the input registers in the form of a row vector @var{REGOUT} of type \"uint16\". \n\n\
By default, the registers are retrieved all at once from the server, \
including all the registers from the lowest index to the highest. \
Only the requested ones are returned in @var{REGOUT}. \
This can be slow if only few distant registers are resquested, and impossible \
if the max number of read registers is reached (see modbus_constants). \
To disable this behavior a third argument @var{INDIV} can be set to true.\n\
@seealso{modbus_new_tcp, modbus_write_registers}\n\
@end deftypefn")
{
  octave_value_list retval;
  int nargin = args.length();
  if (nargin < 2 || nargin > 3)
    {
      print_usage ();
      return retval;
    }

  std::string key = args(0).string_value();
  
  if (error_state)
    {
      print_usage ();
      return retval;
    }
  else if (ctxmap.find (key) == ctxmap.end ()) 
    {
      error ("modbus_read_input_registers: no modbus context is associated to %s", 
             key.c_str ());  
      return retval;
    }

  modbus_t *ctx = ctxmap[key];
  
  // Check indices
  uint16NDArray idx = args(1).uint16_array_value ();
  bool indiv = false;
  if (nargin == 3)
    indiv = args(2).bool_value ();
  
  if (error_state || ! idx.is_vector ())
    {
      print_usage ();
      return retval;
    }

  int lower = mymin (idx);
  int higher = mymax (idx);

  if (lower < 0)
    {
      error ("modbus_read_input_registers: expect positive or 0 indices"); 
      return retval;
    }

  int nb = idx.nelem ();
  int nb_read = higher - lower + 1;

  // Initialize octave output table
  dim_vector dv;
  dv(0) = 1;
  dv(1) = nb;
  uint16NDArray tabout (dv);
  
  
  int rc;
   
  if (indiv | nb == 1)
    {
      uint16_t val;
      for (int ii = 0; ii < nb; ii++)
        {
          rc = modbus_read_input_registers (ctx, idx(ii), 1, &val);
          tabout(ii) = val;
        }
    }
  else
    {
      if (nb_read > MODBUS_MAX_READ_REGISTERS)
        {
          error ("modbus_read_input_registers: max number of read input registers is %d",
                 MODBUS_MAX_READ_REGISTERS);
          return retval;
        }
      uint16_t tabval[nb_read * sizeof (uint16_t)];
      rc = modbus_read_input_registers (ctx, lower, nb_read, tabval);
  
      //Populate octave output
      if (rc == nb_read)
        { 
          for (int ii = 0; ii < nb; ii++)
            {
              tabout(ii) = tabval[((int) idx (ii) - lower)];
            }
        }
    }
  
  retval.append(octave_value(tabout));

  return retval;
}


