/*
 * Serial Toolbox v0.1
 *
 * Copyright (C) 2006 Sphairon Access Systems GmbH
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
 * Written by Stefan Mahr <dac922@gmx.de>, 2006
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
#include <termios.h>
#include <fcntl.h>

DEFUN_DLD(ser_open,args,,"open ser")
{
	octave_value_list retval;

	int serfd,sererr;
	struct termios options;
	
	
	std::string serif = args(0).string_value();
	int baudrate = args(1).int_value();
	
	
	serfd = open(serif.c_str(),O_RDWR | O_NOCTTY | O_NDELAY);

	if (serfd == -1) {
		error("unable to open port");
		return retval;
	}

	fcntl(serfd, F_SETFL,FNDELAY);    // non blocking
	
	tcgetattr(serfd, &options);
	
	switch (baudrate) {
		case 9600:
			cfsetispeed(&options, B9600);
			cfsetospeed(&options, B9600);
			break;
		case 19200:
			cfsetispeed(&options, B19200);
			cfsetospeed(&options, B19200);
			break;
		case 115200:
		default:
			cfsetispeed(&options, B115200);
			cfsetospeed(&options, B115200);
	}
	
	options.c_cflag |= (CLOCAL | CREAD);
	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;
	options.c_cflag &= ~CRTSCTS;
	
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	options.c_iflag &= ~(IXON | IXOFF | IXANY);
	options.c_oflag &= ~OPOST;
	
	tcsetattr(serfd, TCSANOW, &options);


	retval.append(octave_value(serfd));

	return retval;
}

DEFUN_DLD(ser_close,args,,"close ser")
{
	octave_value_list retval;

	int serfd;

	serfd = args(0).int_value();

	close(serfd);

	return retval;
}

DEFUN_DLD(ser_write,args,,"write ser")
{
	octave_value_list retval;
	charMatrix data;
	char *cdata;
	
	int serfd=args(0).int_value();

	data = args(1).char_matrix_value();
	cdata = (char*)malloc(sizeof(char)*data.cols());

	for (int i=0;i<data.cols();i++)
		cdata[i]=data.elem(0,i);

	int sererr = write(serfd,cdata,data.cols());
	
	retval.append(octave_value(sererr));

	free(cdata);
	
	return retval;
}


DEFUN_DLD(ser_read,args,,"read ser - ser_read(serfd,bytes,timeout)")
{
	octave_value_list retval;

	char *cdata;
	struct timeval tv;
	fd_set readfds;
	int readcount;


	int serfd=args(0).int_value();
	int readbytes=args(1).int_value();
	int timeout=args(2).int_value();

	cdata = (char *)malloc(sizeof(char)*readbytes);
	
	readcount = 0;
	
	while (readcount < readbytes) {
		tv.tv_sec = timeout / 1000;
		tv.tv_usec = (timeout % 1000) * 1000;

		FD_ZERO(&readfds);
		FD_SET(serfd,&readfds);

		select(serfd+1,&readfds,NULL,NULL,&tv);

		if (FD_ISSET(serfd,&readfds)) {
			readcount += read(serfd,cdata+(readcount),readbytes-readcount);
		} else
			break;
	}
	
	charMatrix values(1,readcount);
	
	for (int i=0;i<readcount;i++)
		values(0,i) = *(cdata+i);
	
	retval.append(values);

	free(cdata);
	
	return retval;
}
