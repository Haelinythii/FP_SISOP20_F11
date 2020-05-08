#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "fs.h"

int main(int argc, char *argv[])
{
    int fd, fd1;
	if(argc<2){
		printf(1,"Need at least 1 argument!\n");
		exit();
	}
    else if (argc >= 2)
    {
        for (int i = 1; i < argc; i++)
        {
            char isiLama[1024];
            if((fd = open(argv[i], O_RDONLY)) > -1)
            {
                read(fd, isiLama, sizeof(isiLama));
                // printf(1, "text: %s\n", isiLama);
                unlink(argv[i]);
                close(fd);
            }

            if( (fd1 = open(argv[i], O_CREATE|O_RDWR)) < 0)
            {
                printf(1, "touch: can't create %s\n", argv[i]);
            }
            else printf(1, "created %s\n", argv[i]);

            write(fd1, isiLama, strlen(isiLama));
            close(fd1);
        }
    }
	exit();
}
