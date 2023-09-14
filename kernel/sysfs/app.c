/** HEADERS */

#include<stdio.h>
#include<stdio_ext.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/ioctl.h>

#define READ 2
#define WRITE 1
#define EXIT 3

int main()
{
        int fd, opt, ret;
        char *buff;
 
        fd = open("/sys/kernel/sysfs_etx/sysfs_value", O_RDWR);
        if(fd < 0) {
                printf("Cannot open device file...\n");
                return 0;
        }

	printf("Choose the below options\n 1.Write\n 2.Read\n 3.exit\n");
	scanf("%d", &opt);

	switch (opt) {
		case READ:
			ret = read(fd, buff, sizeof(buff));
			if (ret < 0) {
				printf("Failed to read\n");
				exit(1);
			}

			printf("%s\n", buff);
			
			break;

		case WRITE:
			printf("Enter data\n");
			scanf("%s", buff);
			__fpurge(stdin);

			ret = write(fd, buff, strlen(buff));
			if (ret < 0) {
				printf("Failed to write\n");
				return -1;
			}

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
