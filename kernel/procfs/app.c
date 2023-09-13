
/** Headers */

#include<fcntl.h>
#include<stdio.h>
#include<stdio_ext.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<unistd.h>

void main()
{
	int fd, ret;
	char buf[1000];

	printf("Enter data\n");
	scanf("%[^\n]s", buf);
	__fpurge(stdin);

	fd = open("/proc/saketh/proc", O_RDWR);
	if (fd < 0) {
		printf("Failed to open\n");
		exit(1);
	}

	ret = write(fd, buf , strlen(buf));
	if (ret < 0) {
		printf("Failed to write\n");
		exit(1);
	}

	memset(&buf, 0 ,sizeof(buf));

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
}
