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

#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)
 
int main()
{
        int fd, opt;
        int32_t value, number;
 
        fd = open("/sys/kernel/sysfs_etx/sysfs_value", O_RDWR);
        if(fd < 0) {
                printf("Cannot open device file...\n");
                return 0;
        }

	printf("Choose the below options\n 1.Write\n 2.Read\n 3.exit\n");
	scanf("%d", &opt);

	switch (opt) {
		case 1:
			printf("Enter the Value to send\n");
        		scanf("%d",&number);
        		
			printf("Writing Value to Driver\n");
        		ioctl(fd, WR_VALUE, (int32_t*) &number); 
			
			break;

		case 2:
        		printf("Reading Value from Driver\n");
        		ioctl(fd, RD_VALUE, (int32_t*) &value);
        
			printf("Value is %d\n", value);
			break;

		case 3: 
			exit(0);

		default:
			printf("Entered invalid option\n");
	}
 
        printf("Closing Driver\n");
        close(fd);

	return 0;
}
