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

#ifdef BUILD_SERIAL
#include <iostream>
#include <string>
#include <algorithm>

#ifndef __WIN32__
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#endif

using std::string;

#include "serial_class.h"

volatile bool read_interrupt = false;

DEFINE_OCTAVE_ALLOCATOR (octave_serial);
DEFINE_OV_TYPEID_FUNCTIONS_AND_DATA (octave_serial, "octave_serial", "octave_serial");

octave_serial::octave_serial()
{
    this->fd = -1;
}

int octave_serial::open(string path, int flags)
{
    this->fd = ::open(path.c_str(), flags);

    if (this->get_fd() > 0)
    {   
        // Check whether fd is an open file descriptor referring to a terminal 
        if(!isatty(fd)) 
        { 
            error("serial: Interface does not refer to a terminal: %s\n", strerror(errno));
            this->close();
            return -1;
        }

        if (tcgetattr(this->fd, &this->config) < 0)
        {
            error("serial: Failed to get terminal attributes: %s\n", strerror(errno));
            this->close();
            return -1;
        }

        // Clear all settings
        this->config.c_iflag = 0; // Input modes
        this->config.c_oflag = 0; // Output modes
        this->config.c_cflag = CS8 | CREAD | CLOCAL; // Control modes, 8n1
        this->config.c_lflag = 0; // Local modes
        this->config.c_cc[VMIN] = 1;

        if (tcsetattr(this->get_fd(), TCSANOW, &this->config) < 0)
        {
            error("serial: Failed to set default terminal attributes: %s\n", strerror(errno));
            this->close();
            return -1; 
        }

        // Disable NDELAY
        if (fcntl(this->get_fd(), F_SETFL, 0) < 0)
        {
            error("serial: Failed to disable NDELAY flag: %s\n", strerror(errno));
            this->close();
            return -1; 
        }

        this->blocking_read = true;
    } 
    else
    {
        error("serial: Error opening the interface: %s\n", strerror(errno));
        return -1;
    }

    return this->get_fd();
}

octave_serial::~octave_serial()
{
    this->close();
}

void octave_serial::print (std::ostream& os, bool pr_as_read_syntax ) const
{
    print_raw(os, pr_as_read_syntax);
    newline(os);
}

void octave_serial::print_raw (std::ostream& os, bool pr_as_read_syntax) const
{
    os << this->fd;
}

int octave_serial::read(uint8_t *buf, unsigned int len)
{
    if (this->get_fd() < 0)
    {
        error("srl_read: Interface must be opened first...");
        return 0;
    }

    int bytes_read = 0, read_retval = -1;

    // While not interrupted in blocking mode
    while (!read_interrupt) 
    {
        read_retval = ::read(this->get_fd(), (void *)(buf + bytes_read), len - bytes_read);
        //printf("read_retval: %d\n\r", read_retval);

        if (read_retval < 0)
        {
            error("srl_read: Error while reading: %s\n", strerror(errno));
            break;
        }

        bytes_read += read_retval;

        // Required number of bytes read
        if (bytes_read >= len)
            break;

        // Timeout while in non-blocking mode
        if (read_retval == 0 && !this->blocking_read)
            break;
    }

    return bytes_read;
}

int octave_serial::write(string str)
{
    if (this->get_fd() < 0)
    {
        error("serial: Interface must be opened first...");
        return -1;
    }

    return ::write(get_fd(), str.c_str(), str.length());
}

int octave_serial::write(uint8_t *buf, unsigned int len)
{
    if (this->get_fd() < 0)
    {
        error("serial: Interface must be opened first...");
        return -1;
    }

    return ::write(get_fd(), buf, len);
}

int octave_serial::set_timeout(short timeout)
{
    if (this->get_fd() < 0)
    {
        error("serial: Interface must be opened first...");
        return -1;
    }

    if (timeout < -1 || timeout > 255)
    {
        error("srl_timeout: timeout value must be between [-1..255]...");
        return -1;
    }

    // Disable custom timeout, enable blocking read
    if (timeout < 0)
    {
        this->blocking_read = true;
        timeout = 5;
    } 
    // Enable custom timeout, disable blocking read
    else 
    {
        this->blocking_read = false;
    }

    BITMASK_CLEAR(this->config.c_lflag, ICANON); // Set non-canonical mode
    this->config.c_cc[VMIN] = 0;
    this->config.c_cc[VTIME] = (unsigned) timeout; // Set timeout of 'timeout * 10' seconds

    if (tcsetattr(this->get_fd(), TCSANOW, &this->config) < 0) {
        error("srl_timeout: error setting timeout...");
        return -1;
    }

    return 1;
}

int octave_serial::get_timeout()
{
    if (blocking_read)
        return -1;
    else
        return this->config.c_cc[VTIME];
}

int octave_serial::set_stopbits(unsigned short stopbits)
{
    if (this->get_fd() < 0)
    {
        error("serial: Interface must be opened first...");
        return -1;
    }

    /*
     * CSTOPB Send two stop bits, else one.
     */

    if (stopbits == 1)
    {
        // Set to one stop bit
        BITMASK_CLEAR(this->config.c_cflag, CSTOPB);
    }
    else if (stopbits == 2)
    {
        // Set to two stop bits
        BITMASK_SET(this->config.c_cflag, CSTOPB);
    }
    else
    {
        error("srl_stopbits: Only 1 or 2 stop bits are supported...");
        return false;
    }

    if (tcsetattr(this->get_fd(), TCSANOW, &this->config) < 0) {
        error("srl_stopbits: error setting stop bits: %s\n", strerror(errno));
        return false;
    }

    return true;
}

int octave_serial::get_stopbits()
{
    if (this->get_fd() < 0)
    {
        error("serial: Interface must be opened first...");
        return -1;
    }

    if (BITMASK_CHECK(this->config.c_cflag, CSTOPB))
        return 2;
    else
        return 1;
}

int octave_serial::set_bytesize(unsigned short bytesize)
{
    if (this->get_fd() < 0)
    {
        error("serial: Interface must be opened first...");
        return -1;
    }

    tcflag_t c_bytesize = 0;

    switch (bytesize) 
    {
    case 5: c_bytesize = CS5; break;
    case 6: c_bytesize = CS6; break;
    case 7: c_bytesize = CS7; break;
    case 8: c_bytesize = CS8; break;

    default:
        error("srl_bytesize: expecting value between [5..8]...");
        return false;
    }

    // Clear bitmask CSIZE
    BITMASK_CLEAR(this->config.c_cflag, CSIZE);

    // Apply new
    BITMASK_SET(this->config.c_cflag, c_bytesize);

    if (tcsetattr(this->get_fd(), TCSANOW, &this->config) < 0) {
        error("srl_bytesize: error setting byte size: %s\n", strerror(errno));
        return false;
    }

    return true;
}

int octave_serial::get_bytesize()
{
    if (this->get_fd() < 0)
    {
        error("serial: Interface must be opened first...");
        return -1;
    }

    int retval = -1;

    if (BITMASK_CHECK(this->config.c_cflag, CS5))
        retval = 5;
    else if (BITMASK_CHECK(this->config.c_cflag, CS6))
        retval = 6;
    else if (BITMASK_CHECK(this->config.c_cflag, CS7))
        retval = 7;
    else if (BITMASK_CHECK(this->config.c_cflag, CS8))
        retval = 8;

    return retval;
}

int octave_serial::set_baudrate(unsigned int baud) 
{
    if (this->get_fd() < 0)
    {
        error("serial: Interface must be opened first...");
        return -1;
    }

    speed_t baud_rate = 0;

    switch (baud) 
    {
    case 0: 
        baud_rate = B0; break;
    case 50:
        baud_rate = B50; break;
    case 75:
        baud_rate = B75; break;
    case 110:
        baud_rate = B110; break;
    case 134:
        baud_rate = B134; break;
    case 150:
        baud_rate = B150; break;
    case 200:
        baud_rate = B200; break;
    case 300:
        baud_rate = B300; break;
    case 600:
        baud_rate = B600; break;
    case 1200:
        baud_rate = B1200; break;
    case 1800:
        baud_rate = B1800; break;
    case 2400:
        baud_rate = B2400; break;
    case 4800:
        baud_rate = B4800; break;
    case 9600:
        baud_rate = B9600; break;
    case 19200:
        baud_rate = B19200; break;
    case 38400:
        baud_rate = B38400; break;
    case 57600:
        baud_rate = B57600; break;
    case 115200:
        baud_rate = B115200; break;
    case 230400:
        baud_rate = B230400; break;
    default:
        error("srl_baudrate: currently only 0, 50, 75, 110, \
                134, 150, 200, 300, 600, 1200, 1800, 2400, 4800, \
                9600 19200, 38400, 57600, 115200 and 230400 baud rates are supported...");
        return false;
    }

    cfsetispeed(&this->config, baud_rate);
    cfsetospeed(&this->config, baud_rate);

    if (tcsetattr(this->get_fd(), TCSANOW, &this->config) < 0) {
        error("srl_baudrate: error setting baud rate: %s\n", strerror(errno));
        return false;
    }

    return true;
}

int octave_serial::get_baudrate()
{
    if (this->get_fd() < 0)
    {
        error("serial: Interface must be opened first...");
        return -1;
    }

    int retval = -1;

    speed_t baudrate = cfgetispeed(&this->config);

    if (baudrate == B0)
        retval = 0;
    else if (baudrate == B50)
        retval = 50;
    else if (baudrate == B75)
        retval = 75;
    else if (baudrate == B110)
        retval = 110;
    else if (baudrate == B134)
        retval = 134;
    else if (baudrate == B150)
        retval = 150;
    else if (baudrate == B200)
        retval = 200;
    else if (baudrate == B300)
        retval = 300;
    else if (baudrate == B600)
        retval = 600;
    else if (baudrate == B1200)
        retval = 1200;
    else if (baudrate == B1800)
        retval = 1800;
    else if (baudrate == B2400)
        retval = 2400;
    else if (baudrate == B4800)
        retval = 4800;
    else if (baudrate == B9600)
        retval = 9600;
    else if (baudrate == B19200)
        retval = 19200;
    else if (baudrate == B38400)
        retval = 38400;
    else if (baudrate == B57600)
        retval = 57600;
    else if (baudrate == B115200)
        retval = 115200;
    else if (baudrate == B230400)
        retval = 230400;

    return retval;

}

int octave_serial::flush(unsigned short queue_selector)
{
    if (this->get_fd() < 0)
    {
        error("serial: Interface must be opened first...");
        return -1;
    }

    /*
     * TCIOFLUSH Flush both pending input and untransmitted output.
     * TCOFLUSH Flush untransmitted output.
     * TCIFLUSH Flush pending input.
     */

    int flag;

    switch (queue_selector)
    {
    case 0: flag = TCOFLUSH; break;
    case 1: flag = TCIFLUSH; break;
    case 2: flag = TCIOFLUSH; break;
    default:
        error("srl_flush: only [0..2] values are accepted...");
        return false;
    }

    return ::tcflush(this->get_fd(), flag);
}


int octave_serial::set_parity(string parity)
{
    if (this->get_fd() < 0)
    {
        error("serial: Interface must be opened first...");
        return -1;
    }

    // Convert string to lowercase
    std::transform(parity.begin(), parity.end(), parity.begin(), ::tolower);

    /*
     * PARENB Enable parity generation on output and parity checking for input.
     * PARODD If set, then parity for input and output is odd; otherwise even parity is used.
     */

    if (parity == "n" || parity == "none")
    {
        // Disable parity generation/checking
        BITMASK_CLEAR(this->config.c_cflag, PARENB);
    }
    else if (parity == "e" || parity == "even")
    {
        // Enable parity generation/checking
        BITMASK_SET(this->config.c_cflag, PARENB);

        // Set to Even
        BITMASK_CLEAR(this->config.c_cflag, PARODD);

    }
    else if (parity == "o" || parity == "odd")
    {
        // Enable parity generation/checking
        BITMASK_SET(this->config.c_cflag, PARENB);

        // Set to Odd
        BITMASK_SET(this->config.c_cflag, PARODD);

    }
    else
    {
        error("srl_parity: Only [N]one, [E]ven or [O]dd parities are supported...");
        return false;
    }

    if (tcsetattr(this->get_fd(), TCSANOW, &this->config) < 0) {
        error("srl_parity: error setting parity: %s\n", strerror(errno));
        return false;
    }

    return true;
}

string octave_serial::get_parity()
{
    if (!BITMASK_CHECK(this->config.c_cflag, PARENB))
        return "None";
    else if (BITMASK_CHECK(this->config.c_cflag, PARODD))
        return "Odd";
    else
        return "Even";
}

int octave_serial::get_fd()
{
    return this->fd;
}

int octave_serial::close()
{
    int retval = -1;

    if (this->get_fd() > 0)
    {
        retval = ::close(this->get_fd());
        this->fd = -1;
    }

    return retval;
}
#endif
