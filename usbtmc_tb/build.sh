#!/bin/sh
#
MKOCTFILE=mkoctfile
#
#
$MKOCTFILE usbtmc_tb.cc
ln -sf usbtmc_tb.oct usbtmc_close.oct
ln -sf usbtmc_tb.oct usbtmc_open.oct
ln -sf usbtmc_tb.oct usbtmc_read.oct
ln -sf usbtmc_tb.oct usbtmc_write.oct
#
#
#
