#!/bin/sh
#
MKOCTFILE=mkoctfile
#
#
$MKOCTFILE ser_tb.cc
ln -sf ser_tb.oct ser_close.oct
ln -sf ser_tb.oct ser_open.oct
ln -sf ser_tb.oct ser_read.oct
ln -sf ser_tb.oct ser_write.oct
#
#
#
