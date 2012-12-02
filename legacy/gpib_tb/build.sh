#!/bin/sh
#
CURDIR=`pwd`
#
#
#
#
# gpib firmware
GPIBFW="http://linux-gpib.sourceforge.net/firmware/gpib_firmware-2006-11-12.tar.gz"
FIRMWAREPATH="$CURDIR/gpib_firmware-2006-11-12/ni_gpib_usb_b"
# gpib linux
GPIBSVN="https://linux-gpib.svn.sourceforge.net/svnroot/linux-gpib/trunk/linux-gpib"
GPIBPATH=$CURDIR/linux-gpib


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
  cd ${CURDIR}
  rm -Rf ${GPIBPATH}
  svn co ${GPIBSVN} ${GPIBPATH}
  cd ${GPIBPATH}
  ./bootstrap
  #make distclean
  ./configure
  make
}

#
#
# build octave oct file
buildoctfile() {
  make
}

#
#
# build udev rules
buildudev() {
  cd $CURDIR
  local HEXPATH=$( echo $FIRMWAREPATH | sed -e 's/\//\\\//g' )
  local GPIBAPATH=$( echo $GPIBPATH | sed -e 's/\//\\\//g' )
  echo $HEXLOADERPATH
  cp -f ./41-gpib-permissions.rules.template ./41-gpib-permissions.rules
  sed -i -e "s/__NIUSB_HEX__/$HEXPATH/g" ./41-gpib-permissions.rules
  sed -i -e "s/__NIUSB_GPIB__/$GPIBAPATH/g" ./41-gpib-permissions.rules
  #exit
  #sudo cp -f ./permissions.temp /etc/udev/rules.d/41-gpib-permissions.rules
  echo sudo cp -f ./41-gpib-permissions.rules /lib/udev/rules.d/
  echo sudo cp -f ./gpib.conf /etc/gpib.conf
  #sudo udevcontrol reload_rules
  echo sudo service udev restart
  echo sudo udevadm trigger
}

#getgpibfirmware
#buildgpib
#buildoctfile
buildudev
