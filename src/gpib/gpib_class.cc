// Copyright (C) 2013   Stefan Mahr     <dac922@gmx.de>
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

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#ifdef BUILD_GPIB
#include <iostream>
#include <string>
#include <algorithm>

#include <stdio.h>
#include <unistd.h>
#include "gpib/ib.h"

#define GPIB_USEBLOCKREAD

using std::string;

#include "gpib_class.h"

volatile bool read_interrupt = false;

DEFINE_OCTAVE_ALLOCATOR (octave_gpib);
DEFINE_OV_TYPEID_FUNCTIONS_AND_DATA (octave_gpib, "octave_gpib", "octave_gpib");

octave_gpib::octave_gpib()
{
    this->minor = -1;
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
    os << this->gpibid;
}

int octave_gpib::open(int minor, int gpibid, int sad, int timeout, int send_eoi, int eos_mode)
{
    this->minor = minor;
    this->gpibid = gpibid;
    this->sad = sad;
    this->timeout = timeout;
    this->send_eoi = send_eoi;
    this->eos_mode = eos_mode;

    return 1;
}

int octave_gpib::read(uint8_t *buf, unsigned int len)
{
    int gperr,fd;
    int bytes_read = 0, read_retval = -1;

    if (this->minor < 0)
    {
        error("gpib_read: Interface must be opened first...");
        return -1;
    }

    fd = ibdev(this->minor, this->gpibid, this->sad, this->timeout, this->send_eoi, this->eos_mode);
    if (fd < 0)
    {
        error("gpib_read: error opening gpib device...");
        return -1;
    }

#if defined(GPIB_USEBLOCKREAD)
    // blocking read - not interruptable
    gperr = ibrd(fd,(void *)buf,len);
    if (gperr & ERR)
    {
        if (gperr & TIMO)
        {
            warning("gpib_read: read timeout");
        }
        else
        {
            int localiberr = ThreadIberr();
            error("gpib_read: Error while reading: %d - %d\n", gperr, localiberr);
            if (localiberr == 0)
            {
                localiberr = ThreadIbcnt();
                warning("gpib_read: failed system call: %d - %s\n", localiberr, strerror(localiberr));
            }
            ibonl(fd,0);
            return -1;
        }
    }

#else
    // async read - not interruptable as well
    gperr = ibrda(fd,(void *)buf,len);

    if (gperr & ERR)
    {
        error("gpib_read: Error while reading: %d\n", ThreadIberr());
        ibonl(fd,0);
        return -1;
    }

    while (!read_interrupt)
    {
        gperr = ibwait(fd,CMPL);
        warning("gpib_read: read timeout %d - %d - %d",gperr, ThreadIberr(),ThreadIbcnt());
        if (gperr & ERR)
        {
            if (gperr & TIMO)
            {
                warning("gpib_read: read timeout");
            }
            else
            {
                int localiberr = ThreadIberr();
                error("gpib_read: Error while reading: %d - %d\n", gperr, localiberr);
                if (localiberr == 0)
                {
                    localiberr = ThreadIbcnt();
                    warning("gpib_read: failed system call: %d - %s\n", localiberr, strerror(localiberr));
                }
                ibonl(fd,0);
                return -1;
            }
        }
    }
#endif

    bytes_read = ThreadIbcnt();

    ibonl(fd,0);

    return bytes_read;

}

int octave_gpib::write(string str)
{
    int gperr,fd;

    if (this->minor < 0)
    {
        error("gpib_write: Interface must be opened first...");
        return -1;
    }

    fd = ibdev(this->minor, this->gpibid, this->sad, this->timeout, this->send_eoi, this->eos_mode);
    if (fd < 0)
    {
        error("gpib_read: error opening gpib device...");
        return -1;
    }

    gperr = ibwrt(fd,str.c_str(),str.length());
    if (gperr & ERR) {
        // warning: can not write
        if (ThreadIberr() != ENOL) {
            // ENOL is handled by library
            error("gpib: can not write gpib data to device");
        }
    }

    ibonl(fd,0);

    return gperr;
}

int octave_gpib::write(uint8_t *buf, unsigned int len)
{
    int gperr,fd;

    if (this->minor < 0)
    {
        error("gpib_write: Interface must be opened first...");
        return -1;
    }

    fd = ibdev(this->minor, this->gpibid, this->sad, this->timeout, this->send_eoi, this->eos_mode);
    if (fd < 0)
    {
        error("gpib_read: error opening gpib device...");
        return -1;
    }

    gperr = ibwrt(fd,buf,len);
    if (gperr & ERR) {
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
    if (this->minor < 0)
    {
        error("gpib_timeout: Interface must be opened first...");
        return -1;
    }

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

int octave_gpib::close()
{
    int fd,gperr;

    if (this->minor > -1)
    {
        fd = ibdev(this->minor, this->gpibid, this->sad, this->timeout, this->send_eoi, this->eos_mode);
        if (fd < 0)
        {
            error("gpib_close: error opening gpib device...");
            return -1;
        }

        gperr = ibclr(fd);
        if (gperr & ERR) {
            error("gpib_close: can not clear device");
        }

        gperr = ibloc(fd);
        if (gperr & ERR) {
            error("gpib_close: can not set device to local");
        }

        ibonl(fd,0);
    }

    this->minor = -1;
    return -1;
}
#endif
