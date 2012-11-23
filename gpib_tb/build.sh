#!/bin/sh
#
CURDIR=`pwd`
#
MKOCTFILE=mkoctfile

#
#
#
# gpiv firmware
GPIBFW="http://linux-gpib.sourceforge.net/firmware/gpib_firmware-2006-11-12.tar.gz"
FIRMWAREPATH="$CURDIR/gpib_firmware-2006-11-12/ni_gpib_usb_b"
# gpib linux
GPIBVERSION=linux-gpib-3.2.16
GPIBPATH=$CURDIR/$GPIBVERSION


#
#
# get gpib firmware
getgpibfirmware() {
  wget ${GPIBFW}
  tar xvzf gpib_firmware-2006-11-12.tar.gz
}

#
#
# build gpib
buildgpib() {
  cd $CURDIR
  rm -Rf $GPIBPATH
  tar xvzf $GPIBVERSION.tar.gz
  cd $GPIBPATH
  #make distclean
  ./configure
  make
}

#
#
# build octave oct file
buildoctfile() {
  cd $CURDIR/../gpib_tb
  $MKOCTFILE gpib_tb.cc $GPIBPATH/lib/.libs/*.o -I$GPIBPATH/include
  #mkoctfile gpib_tb.cc -lgpib -I$GPIBPATH/include -L$GPIBPATH/lib/.libs
  #cp $GPIBPATH/lib/.libs/libgpib.so.0 $CURDIR/../gpib_tb
  #cp $GPIBPATH/lib/.libs/libgpib.so.0.1.3 $CURDIR/../gpib_tb
  #
  ln -sf gpib_tb.oct gpib_close.oct
  ln -sf gpib_tb.oct gpib_init.oct
  ln -sf gpib_tb.oct gpib_read.oct
  ln -sf gpib_tb.oct gpib_settimeout.oct
  ln -sf gpib_tb.oct gpib_write.oct
}

#
#
# build udev rules
buildudev() {
  cd $CURDIR
  local HEXPATH=$( echo $FIRMWAREPATH | sed -e 's/\//\\\//g' )
  local GPIBAPATH=$( echo $GPIBPATH | sed -e 's/\//\\\//g' )
  echo $HEXLOADERPATH
  cp -f ./41-gpib-permissions.rules.template ./permissions.temp
  sed -i -e "s/__NIUSB_HEX__/$HEXPATH/g" ./permissions.temp
  sed -i -e "s/__NIUSB_GPIB__/$GPIBAPATH/g" ./permissions.temp
  #exit
  #sudo cp -f ./permissions.temp /etc/udev/rules.d/41-gpib-permissions.rules
  sudo cp -f ./permissions.temp /lib/udev/rules.d/41-gpib-permissions.rules
  rm ./permissions.temp

  sudo cp -f ./gpib.conf /etc/gpib.conf
  #sudo udevcontrol reload_rules
  sudo service udev restart
  sudo udevadm trigger
}

getgpibfirmware
buildgpib
buildoctfile
#buildudev
