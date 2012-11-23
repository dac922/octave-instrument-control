/*
 * USBTMC Toolbox v0.1
 *
 * Copyright (C) 2012 Sphairon Technologies GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * Written by Stefan Mahr <dac922@gmx.de>, 2012
 *
 * v0.1		sm: initial release
 *
 */

#include <octave/oct.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/ioctl.h>
//#include <termios.h>
#include <fcntl.h>

DEFUN_DLD(usbtmc_open,args,,"open usbtmc")
{
	octave_value_list retval;

	int usbtmcfd,usbtmcerr;
	
	std::string usbtmcif = args(0).string_value();
	
	
	usbtmcfd = open(usbtmcif.c_str(),O_RDWR);

	if (usbtmcfd == -1) {
		error("unable to open port");
		return retval;
	}

	retval.append(octave_value(usbtmcfd));

	return retval;
}

DEFUN_DLD(usbtmc_close,args,,"close usbtmc")
{
	octave_value_list retval;

	int usbtmcfd;

	usbtmcfd = args(0).int_value();

	close(usbtmcfd);

	return retval;
}

DEFUN_DLD(usbtmc_write,args,,"write usbtmc")
{
	octave_value_list retval;
	charMatrix data;
	char *cdata;
	
	int usbtmcfd=args(0).int_value();

	data = args(1).char_matrix_value();
	cdata = (char*)malloc(sizeof(char)*data.cols());

	for (int i=0;i<data.cols();i++)
		cdata[i]=data.elem(0,i);

	int usbtmcerr = write(usbtmcfd,cdata,data.cols());
	
	retval.append(octave_value(usbtmcerr));

	free(cdata);
	
	return retval;
}


DEFUN_DLD(usbtmc_read,args,,"read usbtmc - usbtmc_read(usbtmcfd,bytes)")
{
	octave_value_list retval;

	char *cdata;
	int readcount;
	int readerr;

	int usbtmcfd=args(0).int_value();
	int readbytes=args(1).int_value();

	cdata = (char *)malloc(sizeof(char)*readbytes);
	
	readcount = read(usbtmcfd,cdata,readbytes);
	if (readcount > 0)
	{
	
		charMatrix values(1,readcount);
		
		for (int i=0;i<readcount;i++)
			values(0,i) = *(cdata+i);
		
		retval.append(values);
	} else {
		retval.append(readcount);
	}
	free(cdata);
	
	return retval;
}
