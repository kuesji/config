#!/usr/bin/bash


build(){
 gcc config.c -o config -lcrypt
}

clean(){
 if [ -f config ]; then
  rm config
 fi
}

install(){
 if [ "$(id -u)" != "0" ]; then
  echo "root needed for installation"
  return
 fi

 mv config /usr/bin/config
 chown root:root /usr/bin/config
 chmod 4755 /usr/bin/config
}

uninstall(){
 if [ "$(id -u)" != 0 ]; then
  echo "root needed for uninstallation"
  return
 fi

 rm /usr/bin/config
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
