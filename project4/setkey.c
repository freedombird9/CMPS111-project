#include <stdio.h>
#include <unistd.h>

int set_key(unsigned k0, unsigned k1);

int main(int argc, char **argv){
	unsigned int k0;
	unsigned int k1;
	k0 = strtol (argv[1], NULL, 0);
	k1 = strtol (argv[2], NULL, 0);

	set_key(k0, k1);
	return 0;
 }
