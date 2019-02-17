#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "util.h"

int main(int argc, char *argv[])
{
	Config cfg = readConfig();

	printf("IP: %s\n", cfg.ip);
	printf("Port: %d\n", cfg.port);
	printf("Timeout: %d\n", cfg.timeout);

	// Make a socket
	//struct sockaddr_in address; 
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
        printf("Invalid address/ Address not supported \n");
        exit(EXIT_FAILURE);
    }

	// Connect socket to server
    if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    { 
        printf("Connection Failed \n");
        exit(EXIT_FAILURE); 
    }


	// Communicate with server
    // char *hello = "Hello from client"; 
    // send(sock_fd , hello, strlen(hello), 0 );
    // printf("Hello message sent\n");

    // char buffer[1024] = {0}; 
    // int valread = read(sock_fd , buffer, 1024);
    // printf("%s\n",buffer );

	// Main loop
	char input_buffer[256];
	while (1)
	{
		// Ask the user what seat they want
		// Optionally, the user may input an exit command
		char send_buffer[256];
		puts("What seat would you like? Please enter as \"<x> <y>\": ");
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

	close(sock_fd);

	return EXIT_SUCCESS;
}