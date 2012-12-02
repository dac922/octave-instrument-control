#!/bin/sh
#
#
#
VXI11VER="1.10"
VXI11URL="http://optics.eee.nottingham.ac.uk/vxi11/source/vxi11_$VXI11VER.tar.gz"
VXI11DIR="vxi11_$VXI11VER"
#
MKOCTFILE=mkoctfile
#
#
#
if [ "x$1" = "xclean" ]; then
  rm -Rf vxi11
  rm -Rf $VXI11DIR
  rm -Rf *.tar.gz *.o *.oct
  exit 0
fi

#
if [ ! -e vxi11_$VXI11VER.tar.gz ]; then
  wget $VXI11URL &&
  tar -xvzf vxi11_$VXI11VER.tar.gz &&
  mv $VXI11DIR vxi11 &&
  cd vxi11 &&
  rpcgen -M vxi11.x &&
  cd ..
fi

#
#
#
if uname -s | grep -q CYGWIN ; then
  CYGWINFLAGS="/usr/lib/libtirpc.a -I/usr/include/tirpc"
fi

#
#
#$MKOCTFILE vxi11_tb.cc vxi11_user.o vxi11_clnt.o vxi11_xdr.o
$MKOCTFILE vxi11_tb.cc vxi11/vxi11_clnt.c vxi11/vxi11_user.cc vxi11/vxi11_xdr.c $CYGWINFLAGS
ln -sf vxi11_tb.oct vxi11_close.oct
ln -sf vxi11_tb.oct vxi11_open.oct
ln -sf vxi11_tb.oct vxi11_read.oct
ln -sf vxi11_tb.oct vxi11_write.oct
