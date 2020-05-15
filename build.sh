#!/bin/env bash
set -e

c_run(){
echo  "$*"
$*
}

build(){
 [ ! -n $CC ] && CC=gcc
 c_run $CC config.c -o config -lcrypt -O3 -s 
}

clean(){
 if [ -f config ]; then
  c_run rm config
 fi
}

install(){
 if [ "$(id -u)" != "0" ]; then
  echo "root needed for installation"
  return
 fi

 c_run mv config $DESTDIR/usr/bin/config
 c_run chown root:root $DESTDIR/usr/bin/config
 c_run chmod 4755 $DESTDIR/usr/bin/config
}

uninstall(){
 if [ "$(id -u)" != 0 ]; then
  echo "root needed for uninstallation"
  return
 fi

 c_run rm $DESTDIR/usr/bin/config
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
fi
exit 0
