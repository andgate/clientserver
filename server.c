#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#include "table.h"
#include "util.h"

#define MAX_THREADS 1000


void* client_handler(void* client_fd_ptr);
void client_cleanup(void* client_fd_ptr);


typedef struct client_session {
	int socket_fd;
	pthread_t thread_id;
	pthread_mutex_t mutex;
	table_t* seat_table;
} client_session;


int main(int argc, char* argv[])
{
	if (argc < 3) {
		fprintf(stderr, "Not enough arguments.\n");
		exit(EXIT_FAILURE);
	}

	int rows = atoi(argv[1]);
	int cols = atoi(argv[2]);
	table_t table = new_table(rows, cols);

	// dump_table(table);
	// display_table(table);

	Config cfg = readConfig();

	int server_fd;
    int opt = 1;
       
    // Create socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
       
    // Forcefully attaching socket to the port 8080 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    }

	// Prepare to bind the socket
    struct sockaddr_in address;
    int addrlen = sizeof(address); 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons(cfg.port); 
       
    // Forcefully attaching socket to the port 8080
	int bind_result = bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    if (bind_result < 0)
    { 
        perror("bind failed");
        exit(EXIT_FAILURE); 
    }

	puts("Server socket bind was successful.");

    if (listen(server_fd, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    }

	puts("Now accepting connections...");

	// Accept incoming connections
	int client_fd;
	pthread_mutex_t table_mutex;
	while( (client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) )
    {
		puts ("Connection accepted\n");

		// Create client session
		client_session* session = (client_session*)malloc(sizeof(client_session));
		session->socket_fd = client_fd;
		session->mutex = table_mutex;
		session->seat_table = &table;

		// Create a thread
		if (pthread_create( &(session->thread_id), NULL, client_handler, (void*) session) < 0)
		{
			perror("Could not create thread.");
			exit(EXIT_FAILURE);
		}

		printf ("Client handler assigned to thread %ld\n", session->thread_id);
	}

	if (client_fd < 0)
	{
		fputs("Accept failed", stderr);
		exit(EXIT_FAILURE);
	}

	close(server_fd);
	delete_table(table);
}


void* client_handler(void* session_vptr)
{
	client_session* session = (client_session*) session_vptr;

	pthread_cleanup_push(client_cleanup, session_vptr);

	while (1) 
	{
		char input_buffer[1024] = {0}; 
		int valread = read( session->socket_fd , input_buffer, 1024);

		// If client disconnect, exit gracefully.	
		if(valread == 0) break;

		//printf("%s\n", buffer);
		// Either this is a request for the seat size,
		// or this is a request for a specifc seat

		if (strcmp(input_buffer, "size\n") == 0)
		{
			char out_buffer[256];
			sprintf(out_buffer, "%dx%d", session->seat_table->dim.rows, session->seat_table->dim.cols);
			send(session->socket_fd, out_buffer, strlen(out_buffer), 0 );
			puts("Sent client the table size");
		} else {
			// try to reserve the seat
		}
	}
	pthread_cleanup_pop(1);

	printf("Thread %ld exited.\n", session->thread_id);
	pthread_exit(EXIT_SUCCESS);
}

void client_cleanup(void* session_vptr)
{
	client_session* session = (client_session*)session_vptr;
	close( session->socket_fd );
	free(session);
	puts( "Client session has ended" );
}