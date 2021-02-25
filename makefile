CC = gcc
OPTIMIZE = -O2
#CFLAGS = $(OPTIMIZE) -pthread -g -lm
CFLAGS = -g 
# -pthread - pthread support
# -g - generate debug info
# -O2 - optimization level
# -lm - necessary for math.h


default: server.c client.c
	@make client
	@make server

client: client.c
	@echo "building client..."
	@$(CC) -o tcp_client client.c 
	@chmod +x tcp_client
	@echo "finished building client"

server: server.c
	@echo "buildling server.."
	@$(CC) -o tcp_server server.c
	@chmod +x tcp_server
	@echo "finished building server"