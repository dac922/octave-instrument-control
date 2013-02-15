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

//
// VXI11 functions are copied from Steve D. Sharples VXI11 library
//   see http://optics.eee.nottingham.ac.uk/vxi11/
//

#include <octave/oct.h>

#include <stdio.h>
#include <stdlib.h>
#include <string>

#ifndef __WIN32__
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#endif

// open or close vxi11 session only on call of vxi11 or vxi11_close
// not tested with libtirpc yet
#define OPENONCE

using std::string;

#include "vxi11_class.h"

DEFINE_OCTAVE_ALLOCATOR (octave_vxi11);
DEFINE_OV_TYPEID_FUNCTIONS_AND_DATA (octave_vxi11, "octave_vxi11", "octave_vxi11");


#define	VXI11_DEFAULT_TIMEOUT	10000	/* in ms */
#define	VXI11_READ_TIMEOUT	2000	/* in ms */
#define	VXI11_MAX_CLIENTS	256	/* maximum no of unique IP addresses/clients */
#define	VXI11_NULL_READ_RESP	50	/* vxi11_receive() return value if a query
					 * times out ON THE INSTRUMENT (and so we have
					 * to resend the query again) */
#define	VXI11_NULL_WRITE_RESP	51	/* vxi11_send() return value if a sent command
					 * times out ON THE INSTURMENT. */



octave_vxi11::octave_vxi11()
{
    this->ip = "";
}

octave_vxi11::~octave_vxi11()
{
    this->close();
}

void octave_vxi11::print (std::ostream& os, bool pr_as_read_syntax ) const
{
    print_raw(os, pr_as_read_syntax);
    newline(os);
}

void octave_vxi11::print_raw (std::ostream& os, bool pr_as_read_syntax) const
{
    os << this->ip;
}

int octave_vxi11::open(string ip)
{
    std::string inst="inst0";
    
    this->ip=ip;

#ifdef OPENONCE
    if (this->openvxi(this->ip.c_str(),&this->client,&this->link,inst.c_str()))
    {
        error("vxi11: Cannot open VXI11...");
        return -1;
    }
#endif
    return 0;
}

int octave_vxi11::read(char *buf, unsigned int len)
{
    CLIENT *client;
    Create_LinkResp *link;

    unsigned long timeout = VXI11_READ_TIMEOUT;

    if (this->ip.empty())
    {
        error("vxi11: setup ip first");
        return -1;
    }

#ifdef OPENONCE
    client = this->client;
    link = this->link;
#else
    std::string inst="inst0";
    if (this->openvxi(this->ip.c_str(),&client,&link,inst.c_str()))
    {
        error("vxi11: Cannot open VXI11...");
        return -1;
    }
#endif

    #define RCV_END_BIT	0x04	// An end indicator has been read
    #define RCV_CHR_BIT	0x02	// A termchr is set in flags and a character which matches termChar is transferred
    #define RCV_REQCNT_BIT	0x01	// requestSize bytes have been transferred.  This includes a request size of zero.

    //long	vxi11_receive(CLIENT *client, VXI11_LINK *link, char *buffer, unsigned long len, unsigned long timeout) {
    Device_ReadParms read_parms;
    Device_ReadResp  read_resp;
    unsigned int	curr_pos = 0;

    read_parms.lid			= link->lid;
    read_parms.requestSize		= len;
    read_parms.io_timeout		= timeout;	/* in ms */
    read_parms.lock_timeout		= timeout;	/* in ms */
    read_parms.flags		= 0;
    read_parms.termChar		= 0;

    do {
        memset(&read_resp, 0, sizeof(read_resp));

        read_resp.data.data_val = buf + curr_pos;
        read_parms.requestSize = len    - curr_pos;	// Never request more total data than originally specified in len

        if(device_read_1(&read_parms, &read_resp, client) != RPC_SUCCESS) {
            error("vxi11: cannot read");
            return -1; /* there is nothing to read. Usually occurs after sending a query
                                            which times out on the instrument. If we don't check this first,
                                            then the following line causes a seg fault */
        }
        if (read_resp.error != 0) {
            /* Read failed for reason specified in error code.
            *  (From published VXI-11 protocol, section B.5.2)
            *  0	no error
			*  1	syntax error
			*  3	device not accessible
			*  4	invalid link identifier
			*  5	parameter error
			*  6	channel not established
			*  8	operation not supported
			*  9	out of resources
			*  11	device locked by another link
			*  12	no lock held by this link
			*  15	I/O timeout
			*  17	I/O error
			*  21	invalid address
			*  23	abort
			*  29	channel already established
			*/
            error("vxi11: cannot read: %d",(int)read_resp.error);
            return -1;
        }

        if((curr_pos + read_resp.data.data_len) <= len) {
            curr_pos += read_resp.data.data_len;
        }
        if( (read_resp.reason & RCV_END_BIT) || (read_resp.reason & RCV_CHR_BIT) ) {
            break;
        }
        else if( curr_pos == len )
        {
            error("xvi11: read error: buffer too small. Read %d bytes without hitting terminator.", (int)curr_pos );
            return -1;
        }
    } while(1);

#ifndef OPENONCE
    // close VXI11 session
    if (this->closevxi(this->ip.c_str(),client,link))
    {
        error("vxi11:Cannot close VXI11...");
        return -1;
    }
#endif
    return curr_pos;
}

int octave_vxi11::write(const char *buf, int len)
{
    
    CLIENT *client;
    Create_LinkResp *link;
    
    if (this->ip.empty())
    {
        error("vxi11: setup ip first");
        return -1;
    }
    
#ifdef OPENONCE
    client = this->client;
    link = this->link;
#else
    std::string inst="inst0";
    if (this->openvxi(this->ip.c_str(),&client,&link,inst.c_str()))
    {
        error("vxi11: Cannot open VXI11...");
        return -1;
    }
#endif

    //int	vxi11_send(CLIENT *client, VXI11_LINK *link, const char *cmd, unsigned long len) {
    Device_WriteParms write_parms;
    unsigned int	bytes_left = len;
    char	*send_cmd;

    send_cmd = new char[len];
    memcpy(send_cmd, buf, len);

    write_parms.lid			= link->lid;
    write_parms.io_timeout		= VXI11_DEFAULT_TIMEOUT;
    write_parms.lock_timeout	= VXI11_DEFAULT_TIMEOUT;

    /* We can only write (link->maxRecvSize) bytes at a time, so we sit in a loop,
    * writing a chunk at a time, until we're done. */

    do {
        Device_WriteResp write_resp;
        memset(&write_resp, 0, sizeof(write_resp));

        if (bytes_left <= link->maxRecvSize) {
            write_parms.flags		= 8;
            write_parms.data.data_len	= bytes_left;
        }
        else
        {
            write_parms.flags		= 0;
            /* We need to check that maxRecvSize is a sane value (ie >0). Believe it
             * or not, on some versions of Agilent Infiniium scope firmware the scope
             * returned "0", which breaks Rule B.6.3 of the VXI-11 protocol. Nevertheless
             * we need to catch this, otherwise the program just hangs. */
            if (link->maxRecvSize > 0) {
                write_parms.data.data_len	= link->maxRecvSize;
            }
            else
            {
                write_parms.data.data_len	= 4096; /* pretty much anything should be able to cope with 4kB */
            }
        }
        write_parms.data.data_val	= send_cmd + (len - bytes_left);
		
        if(device_write_1(&write_parms, &write_resp, client) != RPC_SUCCESS) {
            delete[] send_cmd;
            error("vxi11: cannot write");
            return -VXI11_NULL_WRITE_RESP; /* The instrument did not acknowledge the write, just completely
							  dropped it. There was no vxi11 comms error as such, the 
							  instrument is just being rude. Usually occurs when the instrument
							  is busy. If we don't check this first, then the following 
							  line causes a seg fault */
        }
        if (write_resp.error != 0) {
            error("vxi11_user: write error: %d", (int)write_resp.error);
            delete[] send_cmd;
            return -(write_resp.error);
        }
        bytes_left -= write_resp.size;
    } while (bytes_left > 0);

    delete[] send_cmd;

#ifndef OPENONCE
    // close VXI11 session
    if (this->closevxi(this->ip.c_str(),client,link))
    {
        error("vxi11:Cannot close VXI11...");
        return -1;
    }
#endif

    return 0;
    
}

int octave_vxi11::close()
{
    int retval = 0;

#ifdef OPENONCE
    // close VXI11 session
    if (!this->ip.empty())
        if (this->closevxi(this->ip.c_str(),this->client,this->link))
        {
            error("vxi11:Cannot close VXI11...");
            return -1;
        }
#endif
    this->ip = "";
    
    return retval;
}

int octave_vxi11::openvxi(const char *ip, CLIENT **client, Create_LinkResp **link, const char *device)
{
    *client = clnt_create(ip, DEVICE_CORE, DEVICE_CORE_VERSION, "tcp");
    if (*client == NULL)
    {
		clnt_pcreateerror(ip);
        error("vxi11: Error creating client...");
        return -1;
    }
    
    Create_LinkParms link_parms;

    /* Set link parameters */
    link_parms.clientId	= (long) *client;
    link_parms.lockDevice	= 0;
    link_parms.lock_timeout	= VXI11_DEFAULT_TIMEOUT;
    link_parms.device	= (char *) device;

    *link = (Create_LinkResp *) calloc(1, sizeof(Create_LinkResp));

    if (create_link_1(&link_parms, *link, *client) != RPC_SUCCESS)
    {
        clnt_perror(*client, ip);
        error("vxi11: Error creating client...");
        return -2;
    }
    return 0;
}

int octave_vxi11::closevxi(const char *ip, CLIENT *client, Create_LinkResp *link)
{
    Device_Error dev_error;
	memset(&dev_error, 0, sizeof(dev_error)); 

	if (destroy_link_1(&link->lid, &dev_error, client) != RPC_SUCCESS)
    {
		clnt_perror(client,ip);
		return -1;
    }

    clnt_destroy(client);
    return 0;
}
