#include <stdio.h>
#include <unistd.h>

int
main(int argc, char **argv)
{

	unsigned k = 15000;

	for (; k > 0; --k) {
		if (chdir(".") == -1) {
			perror("chdir!");
		}
	}

	return 0;
}
