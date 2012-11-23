#!/bin/sh
#
MKOCTFILE=mkoctfile
#
$MKOCTFILE tcp_tb.cc
ln -sf tcp_tb.oct tcp_close.oct
ln -sf tcp_tb.oct tcp_open.oct
ln -sf tcp_tb.oct tcp_read.oct
ln -sf tcp_tb.oct tcp_test.oct
ln -sf tcp_tb.oct tcp_write.oct
