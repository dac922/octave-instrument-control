#!/bin/sh
#
MKOCTFILE=mkoctfile
#
#
#
# cygwin
#VISAINCLUDE="/cygdrive/c/Programme/IVI\ Foundation/VISA/WinNT/include"
#VISALIBRARY="/cygdrive/c/Programme/IVI\ Foundation/VISA/WinNT/lib/msc"
#VISACFLAGS="-DWIN32 -lvisa32"
#
# linux openvisa
VISAINCLUDE="/data/devel/openvisa/"
VISALIBRARY="/data/devel/openvisa/"
VISACFLAGS="-lopenvisa"
#
#
$MKOCTFILE -I"$VISAINCLUDE" -L"$VISALIBRARY" $VISACFLAGS visa_tb.cc      #"$LIBRARY/visa32.lib"
ln -sf visa_tb.oct visa_close.oct
ln -sf visa_tb.oct visa_open.oct
ln -sf visa_tb.oct visa_read.oct
ln -sf visa_tb.oct visa_write.oct
#
#
#
