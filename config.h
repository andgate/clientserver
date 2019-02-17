#ifndef _CONFIG_H_
#define _CONFIG_H_

#define CONFIG_PATH "config.ini"

typedef struct Config {
	char* ip;
	int port;
	int timeout;
} Config;

Config readConfig();

#endif // _CONFIG_H_