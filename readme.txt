
 config - secures your config from userspace rogue apps

 usage:
  for locking file:
   config lock filename
  for unlocking file:
   config unlock filename

  it uses immutable file attribute and not all filesystems support this.
  it wont work your system if you have an unsupported filesystem.
  my filesystem is ext4 and it is work perfectly.
  
 compiling:
  ./build.sh build

 installing:
  ./build.sh build
  su root -c "./build.sh install"

 uninstalling:
  ./build.sh uninstall

 * dont forget chmod +x build.sh


 kuesji koesnu | kuesji.com | kuesji@koesnu.com | 14 may 2020

