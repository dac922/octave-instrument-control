/*
 * VXI11 Toolbox v0.1
 *
 * Copyright (C) 2010 Sphairon Access Systems GmbH
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
 * Written by Stefan Mahr <dac922@gmx.de>, 2010
 *
 * v0.1		sm: first try
 *
 */


#include "vxi11/vxi11_user.h"
#include <octave/oct.h>
#include <octave/parse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VXI11 32
//#define DEBUG


struct VXI11_session {
	char	ip[17];
	char	devname[1];
	CLINK	*clink;
};

static int isinit=0;
static struct VXI11_session VXI11session[MAX_VXI11];


void vxi11_init(void) {

	int i;

	for (i=0;i<MAX_VXI11;i++) {
		VXI11session[i].ip[0] = 0;
		VXI11session[i].devname[0] = 0;
		VXI11session[i].clink = NULL;
	}

	//octave_value_list args;
	//args(0) = "vxi11_close";
	//feval("atexit",args);
	isinit=1;
}

DEFUN_DLD(vxi11_open,args,,"open vxi11")
{
	octave_value_list retval;
	std::string devname;
	int fid, ret;

	int nargin = args.length();
	std::string ip = args(0).string_value();
	
	if (nargin > 1) {
		devname = args(1).string_value();
	}
    
	// if (GPIBsession.fid[id] >= 0) {
	// 	// error: already open
	// 	// should never happen
	// 	return(-1);
	// }

	if (!isinit)
		vxi11_init();

	fid = 0;
	while ( ((VXI11session[fid].ip[0] != 0) || (VXI11session[fid].clink != NULL)) && (fid < MAX_VXI11))
	{
		fid++;
	}

	/* maximum reached */
	if (fid >= MAX_VXI11)
	{
		warning("vxi11_open: MAX_VXI11");
		retval.append(octave_value(-1));
		return retval;
	}

	VXI11session[fid].clink = new CLINK;
	printf("here! %d\n",fid);
	if ( devname.length() == 0 ) {
		printf("here too! %d\n",fid);
		ret=vxi11_open_device(ip.c_str(),VXI11session[fid].clink);
		printf("here not! %d\n",fid);
	} else {
		ret=vxi11_open_device(ip.c_str(),VXI11session[fid].clink,(char*)devname.c_str());   //args(1).string_value().c_str()
	}

	if (ret != 0) {
#ifdef DEBUG
		printf("vxi11_open: Error: could not open device %s, quitting\n",ip);
#endif
		warning("gpib_open: open device failed");
		delete VXI11session[fid].clink;
		VXI11session[fid].clink = NULL;
		retval.append(octave_value(-1));
		return retval;
	}

#ifdef DEBUG
	printf("vxi11_tb: opened device id: %s\n",ip);
#endif
	printf("1");
	strncpy(VXI11session[fid].ip,ip.c_str(),17);

	retval.append(octave_value(fid));
	return retval;
}

DEFUN_DLD(vxi11_close,args,,"close vxi11 session")
{
	octave_value_list retval;
	int i,ret=0;
	int fid=0,endfid=MAX_VXI11;

	if (args.length() == 1)
	{
		fid = args(0).int_value();
		endfid = fid + 1;
	} else
		isinit = 0; /* close all */

	for (i=fid;i<endfid;i++) {
		printf("try %d - %02X\n",i,VXI11session[i].ip[0]);
		if (VXI11session[i].ip[0]!=0) {
			ret=vxi11_close_device(VXI11session[i].ip,VXI11session[i].clink);
			printf("%d - closed\n",i);
			VXI11session[i].ip[0] = 0;
			delete VXI11session[i].clink;
			VXI11session[i].clink = NULL;
		}
	}

	retval.append(octave_value(ret));
	return retval;
}

DEFUN_DLD(vxi11_write,args,,"write to vxi11")
{
	octave_value_list retval;
	charMatrix data;
	char *cdata;
	
	int fid=args(0).int_value();

	data = args(1).char_matrix_value();
	cdata = (char*)malloc(sizeof(char)*data.cols());

	for (int i=0;i<data.cols();i++)
		cdata[i]=data.elem(0,i);

	int sockerr = vxi11_send(VXI11session[fid].clink, cdata, data.cols());
	
	retval.append(octave_value(sockerr));

	free(cdata);
	
	return retval;
}

DEFUN_DLD(vxi11_read,args,,"read vxi11 - vxi11_read(fid,bytes,timeout)")
{
	octave_value_list retval;

	char *cdata;
	int readcount;

	int fid=args(0).int_value();
	int readbytes=args(1).int_value();
	int timeout=args(2).int_value();

	cdata = (char *)malloc(sizeof(char)*readbytes);
	
	readcount = vxi11_receive(VXI11session[fid].clink,cdata,readbytes,timeout);

	if (readcount < 0) {
	  retval.append(octave_value(readcount));
	} else {
          charMatrix values(1,readcount);
	
	  for (int i=0;i<readcount;i++)
		values(0,i) = *(cdata+i);

	  retval.append(values);
        }

	free(cdata);
	return retval;
}

