#include <sys/socket.h> // socket()
#include <unistd.h> 	// read(), write()
#include <fcntl.h> 		// open(), close()
#include <errno.h>		// errno
#include <stdlib.h>		// malloc(), free()
#include <netinet/in.h> // struct sockaddr_in
#include <stdio.h> 		// sprintf()
#include <arpa/inet.h>	// inet_addr()
#include <string.h> 	// memcpy(), strlen()
#include <time.h> 		// clock()

#define BUF_SIZE 2048
#define MAX_CLIENTS 20



void recv_forever(int connection_fd, int buf_size) {
	void* buf = malloc(buf_size);
	long total = 0;
	int recvd;
	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC, &start);
	while((recvd = recv(connection_fd, buf, buf_size, 0)) > 0) {
		//printf("received packet_id: %li\n", *(size_t*)buf);
		total += recvd;
	}
	clock_gettime(CLOCK_MONOTONIC, &end);
	double run_time = (end.tv_sec - start.tv_sec) + 1e-9*(end.tv_nsec - start.tv_nsec);

	printf("total received: %li\n", total);
	printf("run_time: %f\n", run_time);

	printf("average throughput(Mbps): %f\n", (total*8/(1000000))/run_time);
}



// starts the server to listen on port
// receives connections until user hits ctrl-c
void open_server(int port, int buf_size) {
	int server_fd, connection_fd;
	int recvd_file_size;
	struct sockaddr_in address;
	int addrlen = sizeof(address);


	// opens a socket
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	perror("socket");

	// initializes a sockaddr_in structure to listen on 
	address.sin_family = AF_INET; // IPv4
	address.sin_addr.s_addr = htonl(INADDR_ANY); // binds to all available interfaces
	address.sin_port = htons(port); // converts from host to network byte order

	// binds server_fd to a specific sockaddr_in
	int enable = 1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
	bind(server_fd, (struct sockaddr*)&address, sizeof(address));
	perror("bind");

	// listen for new connections on the created/bound socket
	// ** waits for TCP SYNs
	listen(server_fd, MAX_CLIENTS);
	perror("listen");

	// accepts 1 connection 
	printf("\n");
	// accept TCP connection
	connection_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
	perror("accept");
	recv_forever(connection_fd, buf_size); // receives receives on this socket forever(theoretically)
}



// ./tcp_server <port> 
int main(int argc, char* argv[]) {
	if (argc != 3) {
	 	fprintf(stderr, "usage: ./tcp_server <port> <buf_size>\n");
	 	exit(1);
	}


	int port = atoi(argv[1]);
	int buf_size = atoi(argv[2]);

	open_server(port, buf_size);
}