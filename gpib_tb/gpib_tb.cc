/*
 * GPIB Toolbox v0.2.1
 *
 * Copyright (C) 2006-2007 Sphairon Access Systems GmbH
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
 * Written by Stefan Mahr <dac922@gmx.de>, 2006-2007
 *
 * v0.2.1	sm: minor fixes
 * v0.2		sm: use free linux-gpib
 * v0.1		sm: use visa32
 *
 */


#include "gpib/ib.h"
#include <octave/oct.h>
#include <octave/parse.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_GPIB 32
#define TIMEOUT_GPIB T1s

//#define DEBUG


struct GPIB_session {
	int	fid;
	int	timeout;
};


static int isinit=0;
static struct GPIB_session GPIBsession[MAX_GPIB];


void gpip_init(void) {

	int i;

	for (i=0;i<MAX_GPIB;i++) {
		GPIBsession[i].fid = -1;
		GPIBsession[i].timeout = TIMEOUT_GPIB;
	}

	octave_value_list args;
	args(0) = "gpib_close";
	feval("atexit",args);
	isinit=1;
}

int gpib_open(int id) {

	int fid;
	const int minor = 0;		// need to be more flexible
	const int sad = 0;
	const int send_eoi = 1;
	const int eos_mode = 0;

	// if (GPIBsession.fid[id] >= 0) {
	// 	// error: already open
	// 	// should never happen
	// 	return(-1);
	// }

	fid = ibdev(minor, id, sad, GPIBsession[id].timeout, send_eoi, eos_mode);

	if (fid < 0) {
		// warning: something is wrong
#ifdef DEBUG
		printf("gpib_tb: open device failed\n");
#endif
		warning("gpib_open: open device failed");
		GPIBsession[id].fid = -1;
		return(-1);
	}

#ifdef DEBUG
	printf("gpib_tb: opened device id: %d\n",fid);
#endif

	GPIBsession[id].fid=fid;
	return(fid);
}

DEFUN_DLD(gpib_settimeout,args,,"set gpib read timeout")
{
	octave_value_list retval;

	int id = args(0).int_value();
	int timeout = args(1).int_value();
	int gperr;

	if (!isinit)
		gpip_init();

	if (GPIBsession[id].fid < 0) {
		gpib_open(id);
	}

	if (timeout < 0 || timeout > 17) {
		// warning: invalid timeout value
		// set to default
		warning("gpib_settimeout: timeout must be between 0 and 17, set to default");
		timeout = TIMEOUT_GPIB;
	}

	GPIBsession[id].timeout=timeout;
	if ( (gperr = ibtmo(GPIBsession[id].fid,timeout)) & ERR) {
		warning("gpib_settimeout: set gpib timeout failed");
	}

#ifdef DEBUG
	printf("gpib_tb: settimeout - sta: %d\n",gperr);
#endif

	return retval;
}


DEFUN_DLD(gpib_close,args,,"close all gpib sessions")
{
	octave_value_list retval;
	int i;
	int gperr;

	if (!isinit)
		warning("gpib_close: gpib not initialized");

	for (i=0;i<MAX_GPIB;i++) {
		if (GPIBsession[i].fid > 0) {
			gperr = ibonl(GPIBsession[i].fid,0);
#ifdef DEBUG
			printf("gpib_close: device id close for %d: %d\n",i,gperr);
#endif

		}
	}

	isinit = 0;
	return retval;
}

DEFUN_DLD(gpib_write,args,,"write to gpib")
{
	octave_value_list retval;
	charMatrix data;
	char *cdata;
	int gperr;

	int id=args(0).int_value();

	data = args(1).char_matrix_value();

	int datlen=data.cols();

	cdata = (char*)malloc(sizeof(char)*datlen);

	for (int i=0;i<datlen;i++)
		cdata[i]=data.elem(0,i);

	if (!isinit)
		gpip_init();

	if (GPIBsession[id].fid < 0) {
		gpib_open(id);
	}

	if ( (gperr = ibwrt(GPIBsession[id].fid,cdata,datlen)) & ERR) {
		// warning: can not write
		if (iberr != ENOL) {
			// ENOL is handled by library
			warning("gpib_write: can not write gpib data to device");
		}
	}

#ifdef DEBUG
	printf("gpib_tb: gpib_write id: %d gperr: %d iberr: %d\n",id,gperr,iberr);
#endif

	retval.append(octave_value(0));
	free(cdata);

	return retval;
}


DEFUN_DLD(gpib_read,args,,"read gpib - gpib_read(id,bytes)")
{
	octave_value_list retval;

	char *cdata;
	int readcount=0;
	int gperr;
	
	int id=args(0).int_value();

	int readbytes=args(1).int_value();

	cdata = (char *)malloc(sizeof(char)*readbytes);

	if (!isinit)
		gpip_init();

	if (GPIBsession[id].fid < 0) {
		gpib_open(id);
	}

	gperr = ibrd(GPIBsession[id].fid,cdata,readbytes);

	if ( (gperr & CMPL) || (gperr & TIMO) || (gperr & END) ) {
		// ibcnt not threadsafe. same for other globals? i dont know?
		readcount = ibcnt;
		charMatrix values(1,readcount);
		for (int i=0;i<readcount;i++)
			values(0,i) = *(cdata+i);
		retval.append(values);
	} else {
		// warning: can not read
		warning("gpib_read: can not read data");
		charMatrix values(0,0);
		retval.append(values);
	}


#ifdef DEBUG
	printf("gpib_tb: gpib_read id: %d gperr: %d readcount: %d\n",id,gperr,readcount);
#endif

	free(cdata);

	//retval.append(octave_value((int) readcount));
	return retval;
}
