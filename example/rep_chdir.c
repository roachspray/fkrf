#include <stdio.h>
#include <unistd.h>

int
main(int argc, char **argv)
{

	unsigned k = 5000000;

	printf("entering loop\n");
	for (; k != 0; --k) {
		if (chdir(".") == -1) {
			perror("chdir!");
		}else {
			printf("chidr: ok\n");
		}
		if (chown(".", 0, 0) == -1) {
			perror("chown!");
		}else {
			printf("chown: ok\n");
		}
	}	
	printf("we gone\n");
	return 0;
}
