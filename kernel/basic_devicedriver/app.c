
/** Headers */

#include<fcntl.h>
#include<stdio.h>
#include<stdio_ext.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<unistd.h>

#define READ 2
#define WRITE 1
#define EXIT 3

void main()
{
	int fd, ret, opt;
	char buf[1000];

	fd = open("/dev/mydevss", O_RDWR);
	if (fd < 0) {
		printf("Failed to open\n");
		exit(1);
	}
	
	while (1) {

		printf("Choose the option from below\n 1.write\n 2.Read\n 3.exit\n");
		scanf("%d", &opt);
		__fpurge(stdin);

		switch (opt) {
			case WRITE :
				printf("Enter data\n");
				scanf("%[^\n]s", buf);
				__fpurge(stdin);

				ret = write(fd, buf , strlen(buf));
				if (ret < 0) {
					printf("Failed to write\n");
					exit(1);
				}

				memset(&buf, 0 ,sizeof(buf));

				break;
		
			case READ:
				ret = read(fd, buf, sizeof(buf));
				if (ret < 0) {
					printf("Failed to read\n");
					exit(1);
				}

				ret = write(1, buf , strlen(buf));
				if (ret < 0) {
					printf("Failed to write\n");
					exit(1);
				}

				memset(&buf, 0 ,sizeof(buf));

				break;

			case EXIT:
				exit(0);

			default:
				printf("Enter valid option\n");
		}

	}
}
