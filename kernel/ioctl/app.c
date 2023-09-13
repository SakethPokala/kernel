
/** HEADERS */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/ioctl.h>
 
/** IOCTL MACROS for read and write */

#define WR_VALUE _IOW('a','a',char*)
#define RD_VALUE _IOR('a','b',char*)

#define WRITE 1
#define READ 2
#define EXIT 3

int main()
{
        int fd, opt;
        char buff[1024];
 
        fd = open("/dev/mydevss", O_RDWR);
        if(fd < 0) {
                printf("Cannot open device file...\n");
                return 0;
        }
 
	printf("Choose the below options\n 1.Write\n 2.Read\n 3.exit\n");
	scanf("%d", &opt);

	switch (opt) {
		case WRITE:
			printf("Enter the data to send\n");
        		scanf("%s", buff);
        		
			printf("Writing Value to Driver\n");
        		ioctl(fd, WR_VALUE, buff); 
			
			break;

		case READ:
        		printf("Reading Value from Driver\n");
        		ioctl(fd, RD_VALUE, buff);
        
			printf("Data is %s\n", buff);
			break;

		case EXIT: 
			exit(0);

		default:
			printf("Entered invalid option\n");
	}
 
        printf("Closing Driver\n");
        close(fd);

	return 0;
}
