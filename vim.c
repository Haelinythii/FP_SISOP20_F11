#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "fs.h"

int
main(int argc, char *argv[]){
    int fs, fd, n;
    char buffer[512];

    if(argc < 3){
        printf(1, "%s: Too few arguments\n", argv[0]);
        exit();
    }

    if((fs = open(argv[1], O_RDONLY)) > -1){
        if((fd = open(argv[1], O_CREATE|O_RDWR)) < 0){
            printf(1, "vim: cannot open %s\n", argv[1]);
            exit();
        }

        while((n = read(fs, buffer, sizeof(buffer))) > 0){
            write(fd, buffer, n);
        }

        write(fd, " ", 1);
        write(fd, argv[2], strlen(argv[2]));

        close(fs);
    } else{

        if((fd = open(argv[1], O_CREATE|O_RDWR)) < 0){
            printf(1, "vim: cannot open %s\n", argv[1]);
            exit();
        }
        write(fd, argv[2], strlen(argv[2]));
    }

    close(fd);

    exit();
}