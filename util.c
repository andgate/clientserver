#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "util.h"

#define BUFFER_CAP 1024

#define SPACE_DELIM " \n\r\t"

Config readConfig()
{
	FILE *fp = fopen(CONFIG_PATH, "r");
	if(fp == NULL) {
		perror(NULL);
        fprintf(stderr, "Error opening %s\n", CONFIG_PATH);
		exit(EXIT_FAILURE);
	}

	char fileBuffer[BUFFER_CAP];
	char* contents = fgets(fileBuffer, BUFFER_CAP, fp);
    if(contents == NULL) {
        perror(NULL);
		fprintf(stderr, "Error reading contents of %s\n", CONFIG_PATH);
		exit(EXIT_FAILURE);
	}

    if(fclose(fp) != 0){
        perror(NULL);
        fprintf(stderr, "Error closing %s\n", CONFIG_PATH);
		exit(EXIT_FAILURE);
    }

    printf("Contents: %s\n", contents);

    Config cfg;
    cfg.ip = strtok(fileBuffer, SPACE_DELIM);
    cfg.port = strtok(NULL, SPACE_DELIM);
    cfg.timeout = strtok(NULL, SPACE_DELIM);

	return cfg;
}