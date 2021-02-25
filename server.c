#include <sys/socket.h> // socket()
#include <unistd.h> 	// read(), write()
#include <fcntl.h> 		// open(), close()
#include <errno.h>		// errno
#include <stdlib.h>		// malloc(), free()
#include <netinet/in.h> // struct sockaddr_in
#include <stdio.h> 		// sprintf()
#include <arpa/inet.h>	// inet_addr()
#include <string.h> 	// memcpy(), strlen()

#define BUF_SIZE 2048
#define MAX_CLIENTS 20

typedef struct file_info {
	char* filename;
	int size;
} file_info;

void recv_forever(int connection_fd, int buf_size) {
	void* buf = malloc(buf_size);
	while(recv(connection_fd, buf, buf_size, 0) > 0) {
		printf("received packet_id: %ln\n", (size_t*)buf);
	}
}



// starts the server to listen on port
// receives connections until user hits ctrl-c
void open_server(int port, int buf_size) {
	int server_fd, connection_fd;
	int recvd_file_size;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	file_info received_file;


	// opens a socket
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("failed to create socket");
		exit(1);
	}
	printf("created socket!\n");

	// initializes a sockaddr_in structure to listen on 
	address.sin_family = AF_INET; // IPv4
	address.sin_addr.s_addr = htonl(INADDR_ANY); // binds to all available interfaces
	address.sin_port = htons(port); // converts from host to network byte order

	// binds server_fd to a specific sockaddr_in
	int enable = 1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
	if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
		fprintf(stderr, "unable to bind to port %i\n", port);
		exit(1);
	}
	printf("bound socket to port %i\n", port);

	// listen for new connections on the created/bound socket
	// ** waits for TCP SYNs
	if (listen(server_fd, MAX_CLIENTS) < 0) {
		perror("listen failed");
		exit(1);
	}
	printf("listening...\n");

	// accepts 1 connection 
	printf("\n");
	// accept TCP connection
	if ((connection_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
		fprintf(stderr, "failed to accept connection on port %i\n", port);
		exit(1);
	}
	printf("accepted connection!\n");\
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