#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>

/* FS_IOC_GETFLAGS, FS_IOC_SETFLAGS, FS_IMMUTABLE_FL */
#include <linux/fs.h>

/* getuid, getgid, crypt */
#include <unistd.h>

/* getspnam */
#include <shadow.h>

/* getpwuid */
#include <pwd.h>

/* for disabling echo to terminal on password prompt */
#include <termios.h>

int verify_user(char * username,char * password){
 struct spwd * entry = getspnam(username);

 char * tmp = entry->sp_pwdp;
 int saltsize = 0;
 int usd_symbol = 0;
 while( usd_symbol < 3 ){
  if( *tmp == '$' ){
   usd_symbol++;
  }

  saltsize++;
  tmp++;
 }

 char * salt = (char*) malloc(sizeof(char)*saltsize+1);
 strncpy(salt,entry->sp_pwdp,saltsize);
 *(salt+saltsize) = '\0';

 char * crypted = crypt(password,salt);

 int result = 0;
 if( strcmp(entry->sp_pwdp,crypted) != 0 ){
  result = 1;
 }

 free(salt);

 return result;
}

int lock_config(int fd){
 int flags = 0;

 if( ioctl(fd, FS_IOC_GETFLAGS, &flags) == -1){
  puts("error: cannot get file flags");
  return -1;
 }

 if( flags & (FS_IMMUTABLE_FL == FS_IMMUTABLE_FL )){
  return 0;
 }

 flags |= FS_IMMUTABLE_FL;
 if( ioctl(fd, FS_IOC_SETFLAGS, &flags) == -1){
  puts("error: cannot set file flags");
  return -1;
 }

 return 0;
}

int unlock_config(int fd){
 int flags = 0;

 if( ioctl(fd, FS_IOC_GETFLAGS, &flags) == -1){
  puts("error: cannot get file flags");
  return -1;
 }

 if( flags & (FS_IMMUTABLE_FL != FS_IMMUTABLE_FL)){
  return 0;
 }

 flags &= ~FS_IMMUTABLE_FL;
 if( ioctl(fd, FS_IOC_SETFLAGS, &flags) == -1){
  puts("error: cannot set file flags");
  return -1;
 }

 return 0;
}

uid_t uid = -1;
gid_t gid = -1;

int main(int argc,char * args[]){
 uid = getuid();
 gid = getgid();

 int handled = 0;
 if(argc == 3){
  int fd = open( args[2], O_RDONLY|O_NONBLOCK);
  if( fd == -1 ){
   puts("error: cannot open file, may be not exists?");
   return 1;
  }

  struct stat s;

  if( stat(args[2],&s) != 0 ){
   puts("error: cannot stat file. i do not know this");
   return 1;
  }

  if( s.st_gid != gid ){
   puts("error: you can't lock/unlock other's file!");
   return 1;
  }

  /* password prompt */
  struct termios old;
  struct termios current;
  int terminal_fd = fileno(stdout);
  if( tcgetattr(terminal_fd,&old) != 0 || tcgetattr(terminal_fd,&current) != 0){
   puts("error: cannot hide password prompt");
   return 1;
  }
  current.c_lflag &= ~ECHO;
  if ( tcsetattr(terminal_fd,TCSAFLUSH,&current) != 0 ){
   puts("error: cannot set tcattr ( for hide password input )");
   return 1;
  }

  char buffer[256];
  printf("password:");
  scanf("%255s",buffer);
  puts("");

  if( tcsetattr(terminal_fd,TCSAFLUSH,&old) != 0 ){
   puts("error: cannot restore attr after password prompt");
   return 1;
  }
  struct passwd * pwd = getpwuid(uid);
  if( verify_user(pwd->pw_name,buffer) != 0 ){
   puts("wrong password");
   return 1;
  }
  /* password prompt end */

  if(setuid(0) != 0){
   puts("error: i cannot change to root. did you fucked up my permissions?");
   return 1;
  }

  if( strcmp(args[1],"lock") == 0 ){
   handled = 1;

   unlock_config(fd);

   if( chown(args[2],0,gid) != 0 || chmod(args[2],0000750) != 0 ){
    puts("error: something gone wrong at setting owner(chmod) and permissions(chmod)");
   }

   if( lock_config(fd) != 0){
    puts("error: lock failed");
   }

  } else if ( strcmp(args[1],"unlock") == 0){
   handled = 1;

   if( unlock_config(fd) != 0){
    puts("error: unlock failed");
    return 1;
   }

   if( chown(args[2],uid,gid) != 0 || chmod(args[2],0000770) != 0){
    puts("error: something gone wrong at setting owner(chmod) and permissions(chmod)");
   }
  }
 }


 if(handled == 0){
  puts("\n kuesji koesnu | kuesji.koesnu.com | 14 may 2020");
  puts("\n usage:\n  config lock <filename>\n  config unlock <filename>\n");
 }

 return 0;
}

