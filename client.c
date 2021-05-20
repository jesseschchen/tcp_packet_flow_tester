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
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	perror("socket");

	// no need to bind because client doesn't care what port it's on

	// initializes a sockaddr_in structure to connect to
	address.sin_family = AF_INET; // IPv4
	address.sin_addr.s_addr = inet_addr(server_ip); // converts ip from char* to binary
	address.sin_port = htons(port); // converts from host to network byte order

	// attempt to connect to <server_ip>:<port>
	connect(socket_fd, (struct sockaddr*)&address, sizeof(address));
	perror("connect");
	printf("successfully connected to %s:%i\n", server_ip, port);

	// returns socket file descriptor
	return socket_fd;
}


// infinite loops and sends packets until killed
int send_packets(int socket_fd, int buf_size, int delay_us) {
	void* buf = malloc(buf_size);

	size_t pack_id = 0;

	for (; ; pack_id++) {
		//printf("packet_id: %li\n", pack_id);
		memcpy(buf, &pack_id, sizeof(size_t));
		if (send(socket_fd, buf, buf_size, 0) != buf_size) {
			perror("send()");
		}

		if (delay_us != 0)
			usleep(delay_us);
	}
}

void send_udp_packets(char* server_ip, int port, int buf_size, int delay_us) {
	int socket_fd;
	struct sockaddr_in address;

	// opens a socket 
	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	perror("socket");

	// no need to bind because client doesn't care what port it's on

	// initializes a sockaddr_in structure to connect to
	address.sin_family = AF_INET; // IPv4
	address.sin_addr.s_addr = inet_addr(server_ip); // converts ip from char* to binary
	address.sin_port = htons(port); // converts from host to network byte order



	void* buf = malloc(buf_size);
	size_t pack_id = 0;

	for (; ; pack_id++) {
		//printf("packet_id: %li\n", pack_id);
		memcpy(buf, &pack_id, sizeof(size_t));
		if (sendto(socket_fd, buf, buf_size, 0, (const struct sockaddr*)&address, sizeof(address)) != buf_size) {
			perror("send()");
		}

		if (delay_us != 0)
			usleep(delay_us);
	}

	close(socket_fd);

}




// ./tcp_client <server_ip> <port> <filename>
int main(int argc, char* argv[]) {
	if (argc != 6){
		fprintf(stderr, "usage: %s <server_ip_address> <port> <buf_size> <delay_us> <tcp>\n", argv[0]);
		exit(1);
	}

	char* server_ip = argv[1]; // server ip
	int port = atoi(argv[2]);  // port number
	int buf_size = atoi(argv[3]);  // packet size
	int delay_us = atoi(argv[4]);  // interpacket gap
	int tcp = atoi(argv[5]);       // tcp or udp - 1 for tcp


	if (tcp) {
		printf("TCP\n");
		// opens connection to server_ip:port
		int socket_fd = open_tcp_connection(server_ip, port);
		// starts sending packets forever
		send_packets(socket_fd, buf_size, delay_us);
		close(socket_fd);
	}
	else {
		printf("UDP\n");
		send_udp_packets(server_ip, port, buf_size, delay_us);
	}





}