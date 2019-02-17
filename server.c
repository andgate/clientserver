#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#include "table.h"
#include "config.h"

#define DEFAULT_ROWS 10
#define DEFAULT_COLUMNS 10
#define DEFAULT_PORT 8080


void* client_handler(void* client_fd_ptr);
void client_cleanup(void* client_fd_ptr);

void seat_table_full_msg(int fd);
void seat_available_msg(int fd, int row, int col);
void seat_reserved_msg(int fd, int row, int col);
void seat_does_not_exist_msg(int fd, int row, int col);

int random_num(int min, int max) {
	return (rand() % (max - min + 1)) + min;
}

typedef struct client_session {
	int socket_fd;
	pthread_t thread_id;
	pthread_mutex_t* table_mutex;
	table_t* seat_table;
} client_session;


int main(int argc, char* argv[])
{
	srand(time(0));
	int port = DEFAULT_PORT;
	int rows = DEFAULT_ROWS;
	int cols = DEFAULT_COLUMNS;
	
	if (argc >= 3) {
		rows = atoi(argv[1]);
		cols = atoi(argv[2]);
	}

	table_t table = new_table(rows, cols);

	if (argc >= 4) {
		port = atoi(argv[3]);
	}

	// dump_table(table);
	display_table(table);

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
    address.sin_port = htons(port); 
       
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
	pthread_mutex_t table_mutex = PTHREAD_MUTEX_INITIALIZER;
	while( (client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) )
    {
		puts ("Connection accepted\n");

		// Create client session
		client_session* session = (client_session*)malloc(sizeof(client_session));
		session->socket_fd = client_fd;
		session->table_mutex = &table_mutex;
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

	char out_buffer[256];
	sprintf(out_buffer, "%dx%d", session->seat_table->dim.rows, session->seat_table->dim.cols);
	send(session->socket_fd, out_buffer, strlen(out_buffer), 0);
	puts("Sent client the table size");

	while (1) 
	{
		char input_buffer[1024] = {0}; 
		int valread = read( session->socket_fd , input_buffer, 1024);

		// If client disconnect, exit gracefully.	
		if(valread == 0) break;

		int x;
		int y;
		if (strcmp(input_buffer, "random\n") == 0)
		{
			x = random_num(1, session->seat_table->dim.cols);
			y = random_num(1, session->seat_table->dim.rows);
		}
		else
		{
			// Parse the given input as coordinates
			coords_parse_result res = table_read_coords(input_buffer);
			if (res.err == COORDS_PARSE_FAILURE)
			{
				char msg[] = "Unable to parse coordinates.\nCoordinates should be in the following format: <column>x<row>\n";
				send(session->socket_fd, msg, strlen(msg), 0);
				puts("Client entered invalid coords and has been notified.");
				continue;
			}

			x = res.x;
			y = res.y;
		}

		
		printf("Client requested seat %dx%d\n", x, y);


		// Lock mutex
		printf("Thread %ld is waiting on the table mutex\n", session->thread_id);
		pthread_mutex_lock(session->table_mutex);
		printf("Thread %ld has locked the table mutex\n", session->thread_id);


		if (isFull(*session->seat_table)) {
			seat_table_full_msg(session->socket_fd);
		} else {

			switch(check_seat(*session->seat_table, x, y))
			{
				case SEAT_AVAILABLE:
					reserve_seat(*session->seat_table, x, y);
					seat_available_msg(session->socket_fd, x, y);
					display_table(*session->seat_table);
					break;
				case SEAT_RESERVED:
					seat_reserved_msg(session->socket_fd, x, y);
					break;
				case SEAT_DOES_NOT_EXIST:
				default:
					seat_does_not_exist_msg(session->socket_fd, x, y);
					break;
			}
		}

		pthread_mutex_unlock(session->table_mutex);
		printf("Thread %ld has unlocked the table mutex\n", session->thread_id);
	}

	pthread_cleanup_pop(1);

	printf("Thread %ld exited.\n", session->thread_id);
	pthread_exit(EXIT_SUCCESS);
}

void seat_table_full_msg(int socket_fd)
{
	char msg[] = "No more seats are available.\n";
	send(socket_fd, msg, strlen(msg), 0);
	puts("No seats available and client was notified.");
}

void seat_available_msg(int socket_fd, int row, int col)
{
	char msg[1024];
	sprintf(msg, "Seat (%d, %d) was available and has been reserved for you.\n", row, col);
	send(socket_fd, msg, strlen(msg), 0);
	puts("Seat was available and has been reserved for the client.");
}

void seat_reserved_msg(int socket_fd, int row, int col)
{
	char msg[1024];
	sprintf(msg, "Seat (%d, %d) was reserved. Please try a different seat.\n", row, col);
	send(socket_fd, msg, strlen(msg), 0);
	puts("Seat was reserved and client was notified.");
}

void seat_does_not_exist_msg(int socket_fd, int row, int col)
{
	char msg[1025];
	sprintf(msg, "Seat (%d, %d) does not exist. Please try a seat within bounds.\n", row, col);
	send(socket_fd, msg, strlen(msg), 0);
	puts("Seat did not exist and client was notified.");
}

void client_cleanup(void* session_vptr)
{
	client_session* session = (client_session*)session_vptr;
	close( session->socket_fd );
	free(session);
	printf("Client session has ended for thread %ld\n", session->thread_id);
}