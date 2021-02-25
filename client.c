#include <sys/socket.h> // socket()
#include <unistd.h> 	// read(), write()
#include <fcntl.h> 		// open(), close()
#include <errno.h>		// errno
#include <stdlib.h>		// malloc(), free()
#include <netinet/in.h> // struct sockaddr_in
#include <stdio.h> 		// sprintf()
#include <arpa/inet.h>	// inet_addr()
#include <string.h>		// strlen()



// opens a connection to client-side connection to server_ip on port and returns a socket fd
int open_tcp_connection(char* server_ip, int port) {
	int socket_fd;
	struct sockaddr_in address;

	// opens a socket 
	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("failed to create socket");
		exit(1);
	}
	printf("created socket!\n");

	// no need to bind because client doesn't care what port it's on

	// initializes a sockaddr_in structure to connect to
	address.sin_family = AF_INET; // IPv4
	address.sin_addr.s_addr = inet_addr(server_ip); // converts ip from char* to binary
	address.sin_port = htons(port); // converts from host to network byte order

	// attempt to connect to <server_ip>:<port>
	if (connect(socket_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
		fprintf(stderr, "failed to connect to %s:%i\n", server_ip, port);
		exit(1);
	}
	printf("successfully connected to %s:%i\n", server_ip, port);

	// returns socket file descriptor
	return socket_fd;
}


// infinite loops and sends packets until killed
int send_packets(int socket_fd, int buf_size) {
	void* buf = malloc(buf_size);

	size_t pack_id = 0;

	for (; ; pack_id++) {
		printf("packet_id: %li\n", pack_id);
		memcpy(buf, &pack_id, sizeof(size_t));
		if (send(socket_fd, buf, buf_size, 0) != buf_size) {
			perror("send()");
		}
	}
}

// ./tcp_client <server_ip> <port> <filename>
int main(int argc, char* argv[]) {
	if (argc != 4){
		fprintf(stderr, "usage: %s <server_ip_address> <port> <buf_size> \n", argv[0]);
		exit(1);
	}

	char* server_ip = argv[1]; // server ip
	int port = atoi(argv[2]);  // port number
	int buf_size = atoi(argv[3]);  // filename to transfer

	// opens connection to server_ip:port
	int socket_fd = open_tcp_connection(server_ip, port);

	// starts sending packets forever
	send_packets(socket_fd, buf_size);



	close(socket_fd);

}