#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

int main(int argc, char *argv[])
{
	Config cfg = readConfig();

	printf("IP: %s\n", cfg.ip);
	printf("Port: %d\n", cfg.port);
	printf("Timeout: %d\n", cfg.timeout);

	return EXIT_SUCCESS;
}