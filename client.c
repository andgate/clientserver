#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include "config.h"

#define MANUAL 0
#define AUTOMATIC 1
#define AUTOMATIC_WAIT_SEC 1

int main(int argc, char *argv[])
{
	Config cfg = readConfig();
	int mode = MANUAL;

	if (argc >= 2)
	{
		if(strcmp(argv[1], "automatic") == 0) {
			mode = AUTOMATIC;
		} else if (strcmp(argv[1], "manual") == 0) {
			mode = MANUAL;
		} else {
			printf("Unrecognized mode \"%s\".\n", argv[1]);
			printf("Supported modes are \"manual\"(default) and \"automatic\".\n");
		}
	}

	// Make a socket
    int sock_fd = 0;
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("Socket creation error \n"); 
        exit(EXIT_FAILURE); 
    } 

	
    struct sockaddr_in serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(cfg.port); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, cfg.ip, &serv_addr.sin_addr)<=0)
    {
        exit(EXIT_FAILURE);
    }

	// Set non-blocking
	/*if( fcntl(sock_fd, F_SETFL, fcntl(sock_fd, F_GETFL, NULL) | O_NONBLOCK) < 0) { 
		fprintf(stderr, "Could not set client to non-blocking mode.\n"); 
		exit(EXIT_FAILURE); 
	} */

	// Connect socket to server
	int connect_status = 1;
	int connect_attempts = 0;
	while (connect_status != 0 && connect_attempts < cfg.timeout)
	{
		connect_status = connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        ++connect_attempts;
		if (connect_status != 0) {
			printf("Could not connect. Retrying in 2 seconds. (%d/%d attempts)\n", connect_attempts, cfg.timeout);
			sleep(2);
		}
    }

	if (connect_status != 0) {
		fprintf(stderr, "Connection timeout.\n");
        exit(EXIT_FAILURE); 
	}
	else
	{
		puts("Connection successful.");
	}

	// Set to blocking mode again... 
	/*if (fcntl(sock_fd, F_SETFL, fcntl(sock_fd, F_GETFL, NULL) & (~O_NONBLOCK)) < 0)  { 
		fprintf(stderr, "Could not set client to blocking mode.\n"); 
		exit(EXIT_FAILURE); 
	} */


	// Communicate with server
    // char *hello = "Hello from client"; 
    // send(sock_fd , hello, strlen(hello), 0 );
    // printf("Hello message sent\n");

	// Server should tell us the size of the table
    char init_buff[1024] = {0}; 
    int init_valread = read(sock_fd, init_buff, 1024);
    printf("Table Size: %s\n", init_buff);

	// Main loop
	char input_buffer[256];
	while (1)
	{
		// Ask the user what seat they want
		// Optionally, the user may input an exit command

		if (mode == AUTOMATIC) {
			printf("Asking for a random seat.\n");
			// Ask for the random seat
			char send_buffer[] = "random\n";
			send(sock_fd , send_buffer, strlen(send_buffer), 0 );

			// Get response from the server and print it
			char read_buffer[1024] = {0};
			int valread = read(sock_fd, read_buffer, 1024);
			printf("%s\n", read_buffer);

			// 
			if(strcmp(read_buffer, "No more seats are available.\n") == 0)
				break;
			else
			{
				printf("Waiting for %d seconds before reserving another seat...\n", AUTOMATIC_WAIT_SEC);
				sleep(AUTOMATIC_WAIT_SEC);
			}
		}
		else if (mode == MANUAL) {
			char send_buffer[256];
			puts("What seat would you like? Please enter as \"<row> <column>\": ");
			fgets(send_buffer, sizeof(send_buffer), stdin);

			if (strcmp(send_buffer, "exit\n") == 0) break;

			// Send the message to the server
			send(sock_fd , send_buffer, strlen(send_buffer), 0 );

			// Get response from the server
			// The response is either "seat taken, try another number"
			// or "You have successfully reserved seat (x,y)"
			char read_buffer[1024] = {0};
			int valread = read(sock_fd, read_buffer, 1024);
			printf("%s\n", read_buffer);
		}
	}

	close(sock_fd);
	printf("Socket closed\n");

	return EXIT_SUCCESS;
}