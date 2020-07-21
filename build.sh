#!/bin/bash
set -e

if [ -z $CC]; then
 CC=gcc
fi

show(){
 echo  "$*"
 $*
}

build(){
 show $CC config.c -o config -lcrypt -O3 -s 
}

clean(){
 if [ -f config ]; then
  show rm config
 fi
}

install(){
 if [ "$(id -u)" != "0" ]; then
  echo "root needed for installation"
  return
 fi

 show mv config $DESTDIR/usr/bin/config
 show chown root:root $DESTDIR/usr/bin/config
 show chmod 4755 $DESTDIR/usr/bin/config
}

uninstall(){
 if [ "$(id -u)" != 0 ]; then
  echo "root needed for uninstallation"
  return
 fi

 show rm $DESTDIR/usr/bin/config
}


if [ "$1" == 'build' ]; then
 build
elif [ "$1" == 'clean' ]; then
 clean
elif [ "$1" == 'install' ]; then
 install
elif [ "$1" == 'uninstall' ]; then
 uninstall
else
 echo "use with one of following commands: build,clean,install,uninstall"
 exit 1
fi
exit 0
