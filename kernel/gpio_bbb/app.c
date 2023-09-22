#include <stdio.h>
#include<stdio_ext.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

int main() {
	int fd, opt, ret;
	
	fd = open("/dev/mygpiodev", O_RDWR);
	if (fd < 0) {
		perror("Failed to open GPIO value file");
		return 1;
	}

	while (1) {
		printf("Enter the option\n 0.Turn off\n 1.Turn on\n");
		scanf("%d", &opt);
		__fpurge(stdin);
		
		if ((opt < 0) || (opt > 1)) {
			printf("Enter a valid option\n");
			continue;
		}
		ret = write(fd, &opt, 4);
		if(ret < 0) {
			printf("Failed to write\n");
			exit(1);
		}

	}

	close(fd);
	return 0;
}
