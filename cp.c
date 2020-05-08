#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "fs.h"

char*
getFileName(char *s) {
  char *filename = s;
  char *temp = s;
  int i;

  for (i = strlen(temp); i >= 0; i--) {
    if (temp[i] == '/') {
      filename = &temp[i+1];
      break;
    }
  }
  return filename;
}

int
isDir(char *s) {
  struct stat st;

  int fd = open(s, O_RDONLY), res;
  fstat(fd, &st);

  if (st.type == T_DIR) 
    res = 1;
  else
    res = 0;

  close(fd);
  return res;
}

char*
strcat(char *dest,char *source){
  char *rdest = dest;
  while(*dest) dest++;
  while((*dest++ = *source++));
  return rdest;
}

void
copy(char *source, char *dest){
  char buf[512], destName[512];
  int fs, fd, r, w;

  if(isDir(source)){
    printf(1, "cp: -r is not declared.\n");
    return;
  }

  if ((fs = open(source, O_RDONLY)) < 0) {
    printf(1, "cp: open source %s failed\n", source);
    return;
  }

  strcpy(destName, dest);

  if(isDir(dest)){
    strcat(destName, "/");
    strcat(destName, getFileName(source));
  }

  if ((fd = open(destName, O_CREATE|O_WRONLY)) < 0) {
    printf(1, "cp: open dest %s failed\n", dest);
    return;
  }

  while ((r = read(fs, buf, sizeof(buf))) > 0) {
    w = write(fd, buf, r);
    if (w != r || w < 0) 
      break;
  }

  if (r < 0 || w < 0)
    printf(2, "cp: error in copying\n");

  close(fs);
  close(fd);
}

void
r_copy(char *source, char *dest){
  int f;

  if ((f = open(source, O_RDONLY)) < 0) {
    printf(1, "cp: open folder source %s failed\n", source);
    return;
  }

  char *ptr;
  char sourceName[512], destName[512];
  struct dirent d;

  strcpy(destName, dest);
  if (dest[strlen(dest)-1] != '/')
    strcat(destName, "/");
  strcat(destName, getFileName(source));
  mkdir(destName);

  strcpy(sourceName, source);
  ptr = sourceName + strlen(sourceName);
  *ptr = '/';
  ptr++;

  while(read(f, &d, sizeof(d)) == sizeof(d)) {
    if(d.inum == 0 || !strcmp(d.name, ".") || !strcmp(d.name, ".."))
      continue;

    memmove(ptr, d.name, strlen(d.name));
    ptr[strlen(d.name)] = 0;

    if (isDir(sourceName))
      r_copy(sourceName, destName);
    else
      copy(sourceName, destName);
  }

  close(f);
}

void
a_copy(char *dest){
  char workDir[512], *ptr;
  int fd;
  struct dirent d;

  if((fd = open(".", 0)) < 0){
    printf(1, "cp: cannot open current working directory\n");
    return;
  }

  strcpy(workDir, ".");
  ptr = workDir+strlen(workDir);
  *ptr = '/';
  ptr++;

  while(read(fd, &d, sizeof(d)) == sizeof(d)) {
    if(d.inum == 0 || !strcmp(d.name, ".") || !strcmp(d.name, ".."))
      continue;

    memmove(ptr, d.name, DIRSIZ);
    ptr[DIRSIZ] = 0;

    copy(workDir, dest);
  }

  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc < 3){
    printf(1, "cp: Need at least 3 arguments\n");
    exit();
  }

  if(strcmp(argv[1], "-r") == 0){
    r_copy(argv[2], argv[3]);
  } else if(strcmp(argv[1], "*") == 0){
    a_copy(argv[2]);
  } else{
    copy(argv[1], argv[2]);
  }

  exit();
}