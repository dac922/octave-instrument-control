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

#ifndef __WIN32__
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#include <string>

using std::string;

#include "usbtmc_class.h"

DEFINE_OCTAVE_ALLOCATOR (octave_usbtmc);
DEFINE_OV_TYPEID_FUNCTIONS_AND_DATA (octave_usbtmc, "octave_usbtmc", "octave_usbtmc");

octave_usbtmc::octave_usbtmc()
{
    this->fd = -1;
}

octave_usbtmc::~octave_usbtmc()
{
    this->close();
}

int octave_usbtmc::get_fd()
{
    return this->fd;
}

void octave_usbtmc::print (std::ostream& os, bool pr_as_read_syntax ) const
{
    print_raw(os, pr_as_read_syntax);
    newline(os);
}

void octave_usbtmc::print_raw (std::ostream& os, bool pr_as_read_syntax) const
{
    os << this->fd;
}

int octave_usbtmc::open(string path, int flags)
{
    this->fd = ::open(path.c_str(), flags, 0);

    if (this->get_fd() < 0)
    {
        error("usbtmc: Error opening the interface: %s\n", strerror(errno));
        return -1;
    }

    return this->get_fd();
}

int octave_usbtmc::read(uint8_t *buf, unsigned int len)
{
    if (this->get_fd() < 0)
    {
        error("usbtmc: Interface must be open first...");
        return -1;
    }

    int retval = ::read(get_fd(), buf, len);

    if (retval < 0)
        error("usbtmc: Failed to read from the usbtmc bus: %s\n", strerror(errno));

    return retval;
}

int octave_usbtmc::write(uint8_t *buf, unsigned int len)
{
    if (this->get_fd() < 0)
    {
        error("usbtmc: Interface must be open first...");
        return -1;
    }

    int retval = ::write(get_fd(), buf, len);

    if (retval < 0)
        error("usbtmc: Failed to write to the usbtmc bus: %s\n", strerror(errno));

    return retval;
}

int octave_usbtmc::close()
{
    int retval = -1;

    if (this->get_fd() > 0)
    {
        retval = ::close(this->get_fd());
        this->fd = -1;
    }

    return retval;
}
