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

#ifdef BUILD_TCP
#include <iostream>
#include <string>
#include <algorithm>

#ifndef __WIN32__
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#else
#include <winsock2.h>
#endif

#ifndef __WIN32__
#define SOCKETERR errno
#define STRSOCKETERR strerror(errno)
#else
#define SOCKETERR WSAGetLastError()
#define STRSOCKETERR ""
#endif

using std::string;

#include "tcp_class.h"

volatile bool read_interrupt = false;

DEFINE_OCTAVE_ALLOCATOR (octave_tcp);
DEFINE_OV_TYPEID_FUNCTIONS_AND_DATA (octave_tcp, "octave_tcp", "octave_tcp");

octave_tcp::octave_tcp()
{
    this->fd = -1;
}

int octave_tcp::open(string address, int port)
{
    struct sockaddr_in sin;
    int sockerr;

#ifdef __WIN32__
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD( 2, 2 );
    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 )
    {
      error( "could not initialize winsock library" );
      return -1;
    }
#endif

    sin.sin_addr.s_addr = inet_addr(address.c_str());
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    memset(&(sin.sin_zero),0,8*sizeof(char));

    this->fd = socket(AF_INET, SOCK_STREAM,0);
    if (this->fd < 0)
    {
        error("tcp: error opening socket : %d - %s\n", SOCKETERR, STRSOCKETERR);
        octave_tcp::close();
        return -1;
    }

    sockerr = connect(this->fd,(struct sockaddr*)&sin, sizeof(struct sockaddr));
    if (sockerr < 0)
    {
        error("tcp: error on connect : %d - %s\n", SOCKETERR, STRSOCKETERR);
        octave_tcp::close();
        return -1;
    }
    return this->get_fd();
}

octave_tcp::~octave_tcp()
{
    this->close();
}

void octave_tcp::print (std::ostream& os, bool pr_as_read_syntax ) const
{
    print_raw(os, pr_as_read_syntax);
    newline(os);
}

void octave_tcp::print_raw (std::ostream& os, bool pr_as_read_syntax) const
{
    os << this->fd;
}

int octave_tcp::read(uint8_t *buf, unsigned int len, int timeout)
{

    struct timeval tv;
    struct timeval *ptv;

    fd_set readfds;

    if (this->get_fd() < 0)
    {
        error("tcp_read: Interface must be opened first...");
        return 0;
    }

    int bytes_read = 0, read_retval = -1;

    // While not interrupted in blocking mode
    while (!read_interrupt)
    {
        /* tv.tv_sec = timeout / 1000;
         * tv.tv_usec = (timeout % 1000) * 1000;
         */

        ptv = &tv;
        tv.tv_sec = 0;
        tv.tv_usec = timeout * 1000;

        // blocking read
        if (timeout < 0)
        {
            ptv = NULL;
        }

        FD_ZERO(&readfds);
        FD_SET(this->get_fd(),&readfds);
        if (::select(this->get_fd()+1,&readfds,NULL,NULL,ptv) < 0)
        {
            error("tcp_read: Error while reading/select: %d - %s\n", SOCKETERR, STRSOCKETERR);
            break;
        }

        if (FD_ISSET(this->get_fd(),&readfds))
        {
            read_retval = ::recv(get_fd(),reinterpret_cast<char *>((buf + bytes_read)),len - bytes_read,0);
            if (read_retval < 0)
            {
                error("tcp_read: Error while reading: %d - %s\n", SOCKETERR, STRSOCKETERR);
                break;
            }
        } else {
            // Timeout
            break;
        }

        bytes_read += read_retval;

        // Required number of bytes read
        if (bytes_read >= len)
            break;

    }

    return bytes_read;

}

int octave_tcp::write(string str)
{
    if (this->get_fd() < 0)
    {
        error("tcp: Interface must be opened first...");
        return -1;
    }

    return ::send(get_fd(),str.c_str(),str.length(),0);
}

int octave_tcp::write(uint8_t *buf, unsigned int len)
{
    if (this->get_fd() < 0)
    {
        error("tcp: Interface must be opened first...");
        return -1;
    }

    return ::send(get_fd(), reinterpret_cast<const char *>(buf), len, 0);
}

int octave_tcp::set_timeout(int timeout)
{
    if (this->get_fd() < 0)
    {
        error("tcp: Interface must be opened first...");
        return -1;
    }

    if (timeout < -1 )
    {
        error("tcp_timeout: timeout value must be -1 or positive");
        return -1;
    }

    this->timeout = timeout;

    return 1;
}

int octave_tcp::get_timeout()
{
    return this->timeout;
}

int octave_tcp::get_fd()
{
    return this->fd;
}

int octave_tcp::close()
{
    int retval = -1;

    if (this->get_fd() > 0)
    {
#ifndef __WIN32__
        retval = ::close(this->get_fd());
#else
        retval = ::closesocket(this->get_fd());
#endif
        this->fd = -1;
    }

    return retval;
}
#endif
