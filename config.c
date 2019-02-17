#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"

#define BUFFER_CAP 1024
#define SPACE_DELIM " \n\r\t"

Config readConfig()
{
	Config cfg;

	FILE *fp = fopen(CONFIG_PATH, "r");
	if(fp == NULL) {
		perror(NULL);
        fprintf(stderr, "Error opening %s\n", CONFIG_PATH);
		exit(EXIT_FAILURE);
	}

	char line[BUFFER_CAP];
	int line_num = 1;
	while (fgets(line, sizeof(line), fp))
	{
		char* var_tok = NULL;
		char* val_tok = NULL;

		int line_length = strlen(line);
		int index = 0;
		
		// find the start of the first token
		for(; index < line_length; ++index)
		{
			if(line[index] != ' ')
			{
				var_tok = &line[index++];
				break;
			}
		}

		// Find the equals sign or another space
		for(; index < line_length; ++index)
		{
			if(line[index] == ' ' || line[index] == '=')
			{
				line[index++] = '\0';
				break;
			}
		}
		

		// Find the start of the value token
		for(; index < line_length; ++index)
		{
			if(line[index] != ' ' && line[index] != '=')
			{
				val_tok = &line[index++];
				break;
			}
		}

		// Find and mark the end of the value token
		for(; index < line_length; ++index)
		{
			if(line[index] == ' ' || line[index] == '\n')
			{
				line[index] = '\0';
				break;
			}
		}

		if (val_tok == NULL || var_tok == NULL) {
			perror(NULL);
			fprintf(stderr, "Error parsing %s on line %d:\n%s\n", CONFIG_PATH, line_num, line);
			exit(EXIT_FAILURE);
		}

		if (strcmp(var_tok, "IP") == 0)
		{
			int ip_len = strlen(val_tok);
			cfg.ip = (char*)malloc(ip_len*sizeof(char));
			strcpy(cfg.ip, val_tok);
		}
		else if (strcmp(var_tok, "Port") == 0)
		{
			cfg.port = atoi(val_tok);
		}
		else if (strcmp(var_tok, "Timeout") == 0)
		{
			cfg.timeout = atoi(val_tok);
		}
		else
		{
			fprintf(stderr, "Unrecognized variable encountered on line %d: %s\n", line_num, CONFIG_PATH);
			exit(EXIT_FAILURE);
		}

		++line_num;
	}

    if(line == NULL) {
        perror(NULL);
		fprintf(stderr, "Error reading contents of %s\n", CONFIG_PATH);
		exit(EXIT_FAILURE);
	}

    if(fclose(fp) != 0){
        perror(NULL);
        fprintf(stderr, "Error closing %s\n", CONFIG_PATH);
		exit(EXIT_FAILURE);
    }

	return cfg;
}