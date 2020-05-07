#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "fs.h"

int main(int argc, char *argv[])
{
    int fd;
	if(argc<2){
		printf(1,"Need at least 1 argument!\n");
		exit();
	}
    else if (argc >= 2)
    {
        for (int i = 1; i < argc; i++)
        {
            if( (fd = open(argv[i], O_CREATE|O_RDWR)) < 0)
            {
                printf(1, "touch: can't create %s\n", argv[i]);
            }
            printf(1, "created %s\n", argv[i]);
            close(fd);
        }
        
        
    }
    

	
	exit();
}
