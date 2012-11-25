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

DEFINE_OCTAVE_ALLOCATOR (octave_gpib);
DEFINE_OV_TYPEID_FUNCTIONS_AND_DATA (octave_gpib, "octave_gpib", "octave_gpib");

octave_gpib::octave_gpib()
{
    this->fd = -1;
}

octave_gpib::octave_gpib(int minor, int gpibid, int secid, int timeout)
{
	const int send_eoi = 1;
	const int eos_mode = 0;

	this->fd = ibdev(minor, gpibid, secid, timeout, send_eoi, eos_mode);
}

octave_gpib::~octave_gpib()
{
    this->close();
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
    int gperr;

    if (this->get_fd() < 0)
    {
        error("gpib_read: Interface must be opened first...");
        return 0;
    }

    int bytes_read = 0, read_retval = -1;

	gperr = ibrd(this->get_fd(),(void *)(buf + bytes_read),len);

    if ( !(gperr & CMPL) && !(gperr & TIMO) && !(gperr & END) ) {
        error("gpib_read: Error while reading: %d\n", gperr);
        break;
    }
        
	// ibcnt not threadsafe. same for other globals? i dont know?
	bytes_read = ibcnt;

    return bytes_read;

}

int octave_gpib::write(string str)
{
	return octave_gpib::write(str.c_str(),str.length());
}

int octave_gpib::write(unsigned char *buf, int len)
{
    if (this->get_fd() < 0)
    {
        error("gpib: Interface must be opened first...");
        return -1;
    }

    int gperr;
    
    if ( (gperr = ibwrt(this->get_fd(),buf,len)) & ERR) {
		// warning: can not write
		if (iberr != ENOL) {
			// ENOL is handled by library
			error("gpib: can not write gpib data to device");
		}
	}
	
	return gperr;
}

int octave_gpib::set_timeout(int timeout)
{
    if (this->get_fd() < 0)
    {
        error("gpib: Interface must be opened first...");
        return -1;
    }

    if (timeout < -1 )
    {
        error("gpib_timeout: timeout value must be -1 or positive");
        return -1;
    }

    this->timeout = timeout;

    return 1;
}

int octave_gpib::get_timeout()
{
    return this->timeout;
}


int octave_gpib::close()
{
    int retval = -1;
    
    if (this->get_fd() > 0)
    {
        retval = ibonl(this->get_fd(),0);
        this->fd = -1;
    }

    return retval;
}
