#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "fs.h"

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

void treeBiasa(char *path, int level, char* ar, int levelThreshold)
{
    if(strcmp(ar, "-L") == 0 && level == levelThreshold) return;
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    printf(2, "tree: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    printf(2, "tree: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    printf(1, "file-------------------------------------\n");
    if(level == 0)
    {
        printf(1, "|---%s\n", fmtname(path));
    }
    else if(level > 0)
    {
        for (int i = 0; i < level; i++)
        {
            printf(1, "   ");
        }
        printf(1, "|---%s\n", fmtname(path));
    }
    break;

  case T_DIR:
    // printf(1, "dir--------------------------------------\n");

    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf(1, "tree: path too long\n");
      break;
    }

    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';

    if(strcmp(path, ".") == 0) printf(1, ".\n");
    else if(level == 0 && strcmp(path, ".") != 0) printf(1, "%s\n", fmtname(path));
    


    while(read(fd, &de, sizeof(de)) == sizeof(de))
    {
        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) continue;
        if(de.inum == 0)
            continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if(stat(buf, &st) < 0){
            printf(1, "tree: cannot stat %s\n", buf);
            continue;
        }

        if(strcmp(ar, "-d") == 0)
            if(st.type == T_FILE || st.type == T_DEV) continue;

        // printf(1, "%d\n", level);
        // if(level > 0) printf(1, "|");
        // if(level > 1) printf(1, " ");
        printf(1, "|");
        for (int i = 0; i < level; i++)
        {
            printf(1, "   |");
        }

        switch (st.type)
        {
        case T_FILE:
            printf(1, "---%s\n", fmtname(buf));
            break;
        
        case T_DIR:
            printf(1, "---%s\n", fmtname(buf));
            // printf(1, "rekursi\n");
            treeBiasa(buf, level+1, ar, levelThreshold);
            break;

        case T_DEV:
            printf(1, "---%s\n", fmtname(buf));
            break;
        }

        

        // printf(1, "%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
    }
    break;
  }
  close(fd);
}



int main(int argc, char *argv[])
{
	if(argc < 2){ //cuman tree
        printf(1, "pilih tree\n");
        treeBiasa(".", 0, "", -1);
		exit();
	}
    else if(argc >= 2)
    {
        if(strcmp(argv[1], "-d") == 0) // tree -d
        {
            if(argc > 2)// tree -d xx xx xx
            {
                for (int i = 2; i < argc; i++) //buat argument dibelakang -d
                {
                    printf(1, "pilih -d\n");
                    treeBiasa(argv[i], 0, "-d", -1);
                }
            }
            else
            {
                treeBiasa(".", 0, "-d", -1);
            }
            
        }
        else if(strcmp(argv[1], "-L") == 0) // tree -L
        {
            // int kedalaman = argv[2];
            printf(1, "pilih -L\n");
            if(argc > 3)// tree -L 2 xx xx xx
            {
                for (int i = 3; i < argc; i++) //buat arg dibelakang -L
                {
                    treeBiasa(argv[i], 0, "-L", atoi(argv[2]));
                }
            }
            else
            {
                treeBiasa(".", 0, "-L", atoi(argv[2]));
            }
            
        }
        else
        {
            for (int i = 1; i < argc; i++)
            {
                treeBiasa(argv[i], 0, "", -1);
            }
        }
        
    }

	
	exit();
}
