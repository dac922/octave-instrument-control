/*
 * VISA Toolbox v0.1
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
 * v0.1		sm: use visa32 for test ! this violates the GPL, so wait for openvisa
 *
 */


#include <visa.h>
#include <octave/oct.h>
#include <octave/parse.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_VISA 32


#define DEBUG

#ifdef DEBUG
#define dbgprintf(arg...) printf( arg )
#else
#define dbgprintf(...) (0)
#endif

static ViSession VISA_session;

struct VISA_device_session_s {
	ViSession vid;
	short init;
	std::string name;
};

// TCPIP0::192.168.100.1::23::SOCKET
// ASRL4::INSTR


static int isinit=0;
static struct VISA_device_session_s VISA_device_session[MAX_VISA];

void visa_init(void) {

	int i;
	ViStatus vierr;

	if (isinit == 0) {
		vierr = viOpenDefaultRM(&VISA_session);
		dbgprintf("%s: %d\n",__func__,vierr);
		
		for (i=0;i<MAX_VISA;i++) {
			VISA_device_session[i].vid = VI_NULL;
			VISA_device_session[i].init = 0;
		}

		octave_value_list args;
		args(0) = "visa_close";
		feval("atexit",args);
		isinit=1;
	}
}

DEFUN_DLD(visa_open,args,,"open visa device")
{
	octave_value_list retval;

	int visa_tb_id = -1;
	int i;
	int nargin;
	ViStatus vierr;
	ViSession vid;

	visa_init();
	
	nargin = args.length();
	
	if (nargin == 0) {
		printf("VID   VISA address\n");

		for (i=0;i<MAX_VISA;i++)
			if (VISA_device_session[i].init == 1) {
				printf("%3d    %s\n",i,VISA_device_session[i].name.c_str());
			}
				
		return retval;
	}
	
	std::string visa_addr = args(0).string_value();

	// find unused ID for new session
	for (i=0;i<MAX_VISA;i++)
		if (VISA_device_session[i].init == 0) {
		   visa_tb_id = i;
		   break;
		}
		
	vierr = viOpen(VISA_session,(ViChar*)visa_addr.c_str(),VI_NULL,VI_NULL,&vid);
	
	if (vierr == VI_SUCCESS) {
		VISA_device_session[visa_tb_id].init = 1;
		VISA_device_session[visa_tb_id].vid = vid;
		VISA_device_session[visa_tb_id].name = visa_addr;
		retval.append(octave_value(visa_tb_id));
	} else {
		retval.append(octave_value(-1));
	}
	

	retval.append(octave_value(vierr));
	
	dbgprintf("%s: %d\n",__func__,vierr);
	
	return retval;
}


DEFUN_DLD(visa_close,args,,"close visa sessions")
{
	octave_value_list retval;
	int i;
	int vid,nargin;
	ViStatus vierr;

	if (isinit) {
			
		nargin = args.length();
		
		if (nargin == 0) {
			for (i=0;i<MAX_VISA;i++) {
				if (VISA_device_session[i].init) {
					vierr = viClose(VISA_device_session[i].vid);
					dbgprintf("%s: device %d - %d\n",__func__,i,vierr);
					if (vierr == VI_SUCCESS) {
						VISA_device_session[i].vid = VI_NULL;
						VISA_device_session[i].init = 0;
						VISA_device_session[i].name.clear();
					}
				}
			}
			vierr = viClose(VISA_session);
			dbgprintf("%s: session %d\n",__func__,vierr);
			isinit = 0;
			
		} else {
		
			vid = args(0).int_value();
			
			if (VISA_device_session[vid].init)
					vierr = viClose(VISA_device_session[vid].vid);
					dbgprintf("%s: device %d - %d\n",__func__,vid,vierr);
					if (vierr == VI_SUCCESS) {
						VISA_device_session[vid].vid = VI_NULL;
						VISA_device_session[vid].init = 0;
						VISA_device_session[vid].name.clear();
					}
		}
		
		retval.append(octave_value(vierr));
	} else {
		dbgprintf("%s: visa not initialised\n",__func__);
		//warning("visa not initialised");
	}
	
	return retval;
}

DEFUN_DLD(visa_write,args,,"write visa")
{
	octave_value_list retval;
	charMatrix data;
	char *cdata;
	
	int vid;
	ViStatus vierr;
	ViUInt32 actwrite;
	
	vid=args(0).int_value();

	if (VISA_device_session[vid].init == 0) {
		warning("visa_tb: nothing initialised");
		return retval;
	}
	
	
	data = args(1).char_matrix_value();
	cdata = (char*)malloc(sizeof(char)*data.cols());

	for (int i=0;i<data.cols();i++)
		cdata[i]=data.elem(0,i);

	vierr = viWrite(VISA_device_session[vid].vid, (ViBuf) cdata, data.cols(), &actwrite);
	dbgprintf("%s: vid %d - err %d - actwrite: %d\n",__func__,vid,vierr,actwrite);
	
	if (vierr == VI_SUCCESS) {
		retval.append(octave_value(actwrite));
		retval.append(octave_value(vierr));
	}
	
	free(cdata);
	
	return retval;

}

DEFUN_DLD(visa_read,args,,"read visa")
{
	octave_value_list retval;

	char *cdata;

	int vid;
	ViStatus vierr;
	ViUInt32 readbytes,readcount,actread;

	vid=args(0).int_value();
	readbytes=args(1).int_value();

	cdata = (char *)malloc(sizeof(char)*readbytes);
	
	readcount = 0;

#if 0
	while (readcount < readbytes) {	
		vierr = viRead(VISA_device_session[vid].vid, (ViBuf) (cdata+readcount), readbytes-readcount, &actread);
		dbgprintf("%s: vid %d - err %d - actread: %d\n",__func__,vid,vierr,actread);
		if (vierr == VI_SUCCESS) {
			readcount = readcount + actread;
			//retval.append(octave_value(actwrite));
			//retval.append(octave_value(vierr));
			if (actread < 1) break;
		} else
			break;
	}
#else
	vierr = viRead(VISA_device_session[vid].vid, (ViBuf) (cdata), readbytes, &actread);
	dbgprintf("%s: vid %d - err %d - actread: %d\n",__func__,vid,vierr,actread);
	readcount = actread;
#endif

	
	if (readcount > 0) {

		charMatrix values(1,readcount);
	
		for (int i=0;i<readcount;i++)
			values(0,i) = *(cdata+i);
			
		retval.append(values);
		
	} else {
		charMatrix values(0,0);
		retval.append(values);
	}
	
	//memcpy(values.data(),cdata,sizeof(char)*readcount);
	
	retval.append(octave_value(vierr));
	retval.append(octave_value(actread));
	
	free(cdata);
	
	return retval;
}
