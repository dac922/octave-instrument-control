/*
 * TCP Toolbox v0.1
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
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

//#define DEBUG

DEFUN_DLD(tcp_open,args,,"open tcp")
{
	octave_value_list retval;
	struct sockaddr_in sin;
	int sockfd,sockerr;
	
	std::string ipaddr = args(0).string_value();
	int tcpport = args(1).int_value();

	sin.sin_addr.s_addr = inet_addr(ipaddr.c_str());
	sin.sin_family = AF_INET;
	sin.sin_port = htons(tcpport);
	bzero(&(sin.sin_zero),8);
	
#ifdef DEBUG
	printf("%s \n",ipaddr.c_str());
	printf("len: %d lenc++: %d port: %d\n",strlen(ipaddr.c_str()),ipaddr.length(),tcpport);
#endif

	sockfd = socket(AF_INET, SOCK_STREAM,0);
	sockerr = connect(sockfd,(struct sockaddr*)&sin, sizeof(struct sockaddr));

#ifdef DEBUG
	printf("errno: %d\n",errno);
#endif

	retval.append(octave_value(sockfd));
	retval.append(octave_value(sockerr));
	return retval;
}

DEFUN_DLD(tcp_close,args,,"close tcp")
{
	octave_value_list retval;

	int sockfd;

	sockfd = args(0).int_value();

	close(sockfd);

	return retval;
}

DEFUN_DLD(tcp_write,args,,"write tcp")
{
	octave_value_list retval;
	charMatrix data;
	char *cdata;
	
	int sockfd=args(0).int_value();

	data = args(1).char_matrix_value();
	cdata = (char*)malloc(sizeof(char)*data.cols());

	for (int i=0;i<data.cols();i++)
		cdata[i]=data.elem(0,i);

	int sockerr = send(sockfd,cdata,data.cols(),0);
	
	retval.append(octave_value(sockerr));

	free(cdata);
	
	return retval;
}


DEFUN_DLD(tcp_read,args,,"read tcp - tcp_read(sockfd,bytes,timeout)")
{
	octave_value_list retval;

	char *cdata;
	struct timeval tv;
	fd_set readfds;
	int readcount;
	int tcperr,actread;


	int sockfd=args(0).int_value();
	int readbytes=args(1).int_value();
	int timeout=args(2).int_value();

	cdata = (char *)malloc(sizeof(char)*readbytes);
	
	readcount = 0;
	
	while (readcount < readbytes) {
		/*
		tv.tv_sec = timeout / 1000;
		tv.tv_usec = (timeout % 1000) * 1000;
		*/
		tv.tv_sec = 0;
		tv.tv_usec = timeout * 1000;
		
		FD_ZERO(&readfds);
		FD_SET(sockfd,&readfds);
                //fprintf(stderr,"here i am\n");
		tcperr = select(sockfd+1,&readfds,NULL,NULL,&tv);
		//fprintf(stderr,"here i am again - tcperr: %d\n",tcperr);
                if (tcperr == -1) break;

		if (FD_ISSET(sockfd,&readfds)) {
			actread = recv(sockfd,cdata+(readcount),readbytes-readcount,0);
			if (actread < 1) break;
			readcount = readcount + actread;
			//fprintf(stderr,"inside world - actread: %d\n",actread);
		} else
			break;
	}
	//fprintf(stderr,"outside world\n");
	if (tcperr == -1) {
	  retval.append(octave_value(tcperr));
	  retval.append(octave_value(errno));
	  retval.append(octave_value(actread));
	} else {
          charMatrix values(1,readcount);
	
	  for (int i=0;i<readcount;i++)
	  //	values.insert(*(cdata+i),1,i+1);
		values(0,i) = *(cdata+i);

	  //memcpy(values.data(),cdata,sizeof(char)*readcount);
	
	  retval.append(values);
        }
	
	free(cdata);
	
	return retval;
}
