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

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#ifdef BUILD_PARALLEL
#include <octave/oct.h>
#include <octave/ov-int32.h>

#include <iostream>
#include <string>
#include <algorithm>

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#if defined (__linux__)
#include <linux/parport.h>
#include <linux/ppdev.h>
#endif

// Platform specific header files
#if defined (__FreeBSD__)
#include <dev/ppbus/ppi.h>
#include <dev/ppbus/ppbconf.h>

// And constants
#define PPWCONTROL PPISCTRL
#define PPRCONTROL PPIGCTRL
#define PPWSTATUS PPISSTATUS
#define PPRSTATUS PPIGSTATUS
#define PPWDATA PPISDATA
#define PPRDATA PPIGDATA
#endif

using std::string;

#include "parallel_class.h"

DEFINE_OCTAVE_ALLOCATOR (octave_parallel);
DEFINE_OV_TYPEID_FUNCTIONS_AND_DATA (octave_parallel, "octave_parallel", "octave_parallel");

octave_parallel::octave_parallel()
{
    this->fd = -1;
}

octave_parallel::~octave_parallel()
{
    this->close();
}

int octave_parallel::open(string path, int flags)
{
    this->fd = ::open(path.c_str(), flags, 0);

    if (this->get_fd() < 0)
    {
        error("parallel: Error opening the interface: %s\n", strerror(errno));
        return -1;
    }

    // Claim control of parallel port 
    // Not used with FreeBSD
#if !defined(__FreeBSD__)

    if (ioctl(this->get_fd(), PPCLAIM) < 0)
    {
        error("parallel: Error when claiming the interface: %s\n", strerror(errno));

        ::close(this->get_fd());
        this->fd = -1;

        return -1;
    }

#endif

    return this->get_fd();
}

int octave_parallel::get_fd()
{
    return this->fd;
}

void octave_parallel::print(std::ostream& os, bool pr_as_read_syntax ) const
{
    print_raw(os, pr_as_read_syntax);
    newline(os);
}

void octave_parallel::print_raw(std::ostream& os, bool pr_as_read_syntax) const
{
    os << this->fd;
}

int octave_parallel::set_datadir(int dir)
{
    if (this->get_fd() < 0)
    {
        error("parallel: Open the interface first...");
        return -1;
    }

    if (dir < 0 || 1 < dir)
    {
        error("parallel: Unsupported data direction...");
        return -1;
    }

    // The ioctl parameter is a pointer to an int. 
    // If the int is zero, the drivers are turned on (forward/output direction); 
    // if non-zero, the drivers are turned off (reverse/input direction).
    // Not used with FreeBSD
#if !defined(__FreeBSD__)

    if (ioctl(this->get_fd(), PPDATADIR, &dir) < 0) 
    {
        error("pp_datadir: error setting data direction: %s\n", strerror(errno));
        return false;
    }

#endif

    this->dir = dir;

    return 1;
}

int octave_parallel::get_datadir()
{
    if (this->get_fd() < 0)
    {
        error("parallel: Open the interface first...");
        return false;
    }

    return this->dir;
}

int octave_parallel::get_stat()
{
    if (this->get_fd() < 0)
    {
        error("parallel: Open the interface first...");
        return -1;
    }

    uint8_t status;

    if (ioctl(this->get_fd(), PPRSTATUS, &status) < 0)
    {
        error("parallel: Error while reading from Status register: %s\n", strerror(errno));
        return -1;
    }

    return status;
}

int octave_parallel::set_data(uint8_t data)
{
    if (this->get_fd() < 0)
    {
        error("parallel: Open the interface first...");
        return -1;
    }

    /*
    if (this->get_dir() == 1)
    {
        error("parallel: Trying to output data while in Input mode, this can result in hardware damage! \
                   Use override if you know what you are doing...");
        return false;
    }  
     */

    if (ioctl(this->get_fd(), PPWDATA, &data) < 0) 
    {
        error("parallel: Error while writing to Data register: %s\n", strerror(errno));
        return -1;
    }

    return 1;
}

int octave_parallel::get_data()
{
    if (this->get_fd() < 0)
    {
        error("parallel: Open the interface first...");
        return -1;
    }

    uint8_t data;

    if (ioctl(this->get_fd(), PPRDATA, &data) < 0)    
    {
        error("parallel: Error while reading from Data register: %s\n", strerror(errno));
        return -1;
    }

    return data;
}

int octave_parallel::set_ctrl(uint8_t ctrl)
{
    if (this->get_fd() < 0)
    {
        error("parallel: Open the interface first...");
        return -1;
    }

    if (ioctl(this->get_fd(), PPWCONTROL, &ctrl) < 0)
    {
        error("parallel: Error while writing to Control register: %s\n", strerror(errno));
        return -1;
    }

    return 1;
}

int octave_parallel::get_ctrl()
{
    if (this->get_fd() < 0)
    {
        error("parallel: Open the interface first...");
        return -1;
    }

    uint8_t ctrl;

    if (ioctl(this->get_fd(), PPRCONTROL, &ctrl) < 0)
    {
        error("parallel: Error while reading from Control register: %s\n", strerror(errno));
        return -1;
    }

    return ctrl;
}

int octave_parallel::close()
{
    if (this->get_fd() > 0)
    {
        // Release parallel port
        // Not used with FreeBSD
#if !defined(__FreeBSD__)

        if (ioctl(this->get_fd(), PPRELEASE) < 0)
            error("parallel: error releasing parallel port: %s\n", strerror(errno));

#endif

        int retval = ::close(this->get_fd());
        this->fd = -1;

        return retval;
    }

    return -1;
}
#endif
