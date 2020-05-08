#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"


char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

char* findLastName(char *path)
{
    char* buf = path;
    char* p;
    for (p = path; *p; p++)
    {
        if(*p == '/')
        {
          p++;
            buf = p;
        }
    }
    return buf;
}

int findSubstring(char *str, char *substr)
{
    int i = 0, j = 0;
    while ((*(str + j) != '\0')&&(*(substr + i) != '\0')) {
        if (*(substr + i) != *(str + j)) {
            j++;
            i = 0;
        }
        else {
            i++;
            j++;
        }
    }
    if (*(substr + i) == '\0')
        return 1;
    else
        return -1;
}

void findBiasa(char *path, char *fileToFind, int modeN) // modeN = 1 kalo pake -n
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    printf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    printf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    // printf(1, "files ");
    printf(1, "%s\n", path);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf(1, "ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';

    while(read(fd, &de, sizeof(de)) == sizeof(de))
    {
    // printf(1, "--%s\n", findLastName(buf));
      if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) continue;
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf(1, "ls: cannot stat %s\n", buf);
        continue;
      }
      // printf(1, "%s = %s\n", fileToFind, findLastName(buf));
      
      switch (st.type)
      {
        case T_FILE:
            if(modeN == 1)
            {
                if(strcmp(fileToFind, findLastName(buf)) == 0)
                {
                    // printf(1, "files ");
                    printf(1, "%s\n", buf);
                    break;
                }
            }
            else if(modeN == 0)
            {
                printf(1, "%s\n", buf);
            }
            else
                printf(1, "%s\n", buf);
            
            break;
        
        case T_DIR:
            if(modeN == 1)
            {
                if(strcmp(fileToFind, findLastName(buf)) == 0)
                {
                    printf(1, "%s\n", buf);
                }
                findBiasa(buf, fileToFind, modeN);
            }
            else if(modeN == 0)
            {
                printf(1, "%s\n", buf);
                findBiasa(buf, fileToFind, modeN);
            }
            else
            {
                printf(1, "%s\n", buf);
                findBiasa(buf, fileToFind, modeN);
            }
            // printf(1, "rekursi\n");
            break;

        case T_DEV:
            if(modeN == 1)
            {
                if(strcmp(fileToFind, findLastName(buf)) == 0)
                {
                    printf(1, "%s\n", buf);
                    break;
                }
            }
            else if( modeN == 0 )
            {
                printf(1, "%s\n", buf);
            }
            else
                printf(1, "%s\n", buf);
            break;
      }
      // printf(1, "%s\n", buf);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
    int modeN = 0, modeD = 0;
    int countFiles = 0, countPath = 0;
    char* pathsToBeSearch[100];
    char* filesToSearch[100];
    if(argc < 2) //cuman find
    {
        findBiasa(".", "", -1); // pass path "."
    }
    else if(argc == 2)
    {
        findBiasa(argv[1], "", 0); // pass path argv[1]
    }
    else if(argc >= 2)
    {
        for (int i = 1; i < argc; i++) // hitung -n sama -d
        {
            if(strcmp(argv[i], "-n") == 0)
            {
                modeN = 1;
                filesToSearch[countFiles] = argv[i+1];
                countFiles++;
            }
            else if(strcmp(argv[i], "-d") == 0)
            {
                modeD = 1;
                pathsToBeSearch[countPath] = argv[i+1];
                // strcpy(path, argv[i+1]);
                // path = argv[i+1];
                // printf(1, "%s\n", pathsToBeSearch[countPath]);
                countPath++;
            }
        }
        
        // for (int i = 0; i < countFiles; i++)
        // {
        //     printf(1, "file: %s ;", filesToSearch[i]);
        // }
        // printf(1, "\n");

        if(modeN == 1 && modeD == 1) // ada -n sama -d
        {
            for (int i = 0; i < countPath; i++)
            {
                for (int j = 0; j < countFiles; j++)
                {
                    findBiasa(pathsToBeSearch[i], filesToSearch[j],1);
                }
            }
            
            
        }
        else if(modeN == 1) //cuman ada -n
        {
            for (int i = 0; i < countFiles; i++)
            {
                findBiasa(".", filesToSearch[i],1);
            }
        }
        else if(modeD == 1)//cuman ada -d
        {
            for (int i = 0; i < countPath; i++)
            {
                findBiasa(pathsToBeSearch[i], "" , 0);
            }
        }
        else
        {
            for (int i = 1; i < argc; i++)
            {
                findBiasa(argv[i], "", 0);
            }
            
        }
    }
  
  exit();
}
