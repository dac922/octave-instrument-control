// Copyright (C) 2012   Andrius Sutas   <andrius.sutas@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.

#include <octave/oct.h>

#include <iostream>
#include <string>
#include <algorithm>

#if 1
#include "gpib/ib.h"
#endif

#ifndef __WIN32__
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#endif

using std::string;

#include "gpib_class.h"

volatile bool read_interrupt = false;

//static int testfd;

DEFINE_OCTAVE_ALLOCATOR (octave_gpib);
DEFINE_OV_TYPEID_FUNCTIONS_AND_DATA (octave_gpib, "octave_gpib", "octave_gpib");

octave_gpib::octave_gpib()
{
    this->fd = -1;
}

octave_gpib::octave_gpib(int minor, int gpibid, int secid, int timeout, int send_eoi, int eos_mode)
{
    this->minor = minor;
    this->gpibid = gpibid;
    this->secid = secid;
    this->timeout = timeout;
    this->send_eoi = sendeoi;
    this->eos_mode = eos_mode;
}

void octave_gpib::print (std::ostream& os, bool pr_as_read_syntax ) const
{
    print_raw(os, pr_as_read_syntax);
    newline(os);
}

void octave_gpib::print_raw (std::ostream& os, bool pr_as_read_syntax) const
{
    os << this->fd;
}

int octave_gpib::read(char *buf, unsigned int len)
{
    int gperr,fd;
    int bytes_read = 0, read_retval = -1;
    
    fd = ibdev(this->minor, this->gpibid, this->secid, this->timeout, this->send_eoi, this->eos_mode);
    if (fd < 0)
    {
        error("gpib_read: error opening gpib device...");
        return -1;
    }

    gperr = ibrd(fd,(void *)(buf + bytes_read),len);

    if ( !(gperr & CMPL) && !(gperr & TIMO) && !(gperr & END) )
    {
		ibonl(fd,0);
        error("gpib_read: Error while reading: %d\n", gperr);
        return -1;
    } 

    bytes_read = ThreadIbcnt();

    ibonl(fd,0);

    return bytes_read;

}

int octave_gpib::write(string str)
{
    int gperr,fd;
    
    fd = ibdev(this->minor, this->gpibid, this->secid, this->timeout, this->send_eoi, this->eos_mode);
    if (fd < 0)
    {
        error("gpib_read: error opening gpib device...");
        return -1;
    }

    if ( (gperr = ibwrt(fd,str.c_str(),str.length())) & ERR) {
        // warning: can not write
        if (ThreadIberr() != ENOL) {
            // ENOL is handled by library
            error("gpib: can not write gpib data to device");
        }
    }
    
    ibonl(fd,0);
    
    return gperr;
}

int octave_gpib::write(unsigned char *buf, int len)
{
    int gperr,fd;
    
    fd = ibdev(this->minor, this->gpibid, this->secid, this->timeout, this->send_eoi, this->eos_mode);
    if (fd < 0)
    {
        error("gpib_read: error opening gpib device...");
        return -1;
    }
      
    if ( (gperr = ibwrt(this->get_fd(),buf,len)) & ERR) {
        // warning: can not write
        if (ThreadIberr() != ENOL) {
            // ENOL is handled by library
            error("gpib: can not write gpib data to device");
        }
    }
    
    ibonl(fd,0);
    
    return gperr;
}

int octave_gpib::set_timeout(int timeout)
{
    if (timeout < 0 || timeout > 17)
    {
        error("gpib_timeout: timeout must be between 0 and 17");
        return -1;
    }

    this->timeout = timeout;

    return 1;
}

int octave_gpib::get_timeout()
{
    return this->timeout;
}
