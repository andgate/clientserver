#ifndef _UTIL_H_
#define _UTIL_H_

#define CONFIG_PATH "config.txt"

typedef struct Config {
	char* ip;
	int port;
	int timeout;
} Config;

Config readConfig();

#endif