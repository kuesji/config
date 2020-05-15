#!/usr/bin/bash
set -e

c_run(){
echo $*
$*
}

c_build(){
 c_run gcc config.c -o config -lcrypt -O3 -s 
}

c_clean(){
 if [ -f config ]; then
  c_run rm config
 fi
}

c_install(){
 if [ "$(id -u)" != "0" ]; then
  echo "root needed for installation"
  return
 fi

 c_run mv config $DESTDIR/usr/bin/config
 c_run chown root:root $DESTDIR/usr/bin/config
 c_run chmod 4755 $DESTDIR/usr/bin/config
}

c_uninstall(){
 if [ "$(id -u)" != 0 ]; then
  echo "root needed for uninstallation"
  return
 fi

 c_run rm $DESTDIR/usr/bin/config
}


for i in $* ; do
 [ "$i" == 'build' ] && c_build
 [ "$i" == 'clean' ] && c_clean
 [ "$i" == 'install' ] && c_install
 [ "$i" == 'uninstall' ] && c_uninstall
 [ "$i" == 'help' ] && echo "use with one of following commands: build,clean,install,uninstall"
done
exit 0
