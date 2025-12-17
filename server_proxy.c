#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <sys/select.h>

#define SOCKET_NAME "/tmp/unix.sock"
#define TCP_PORT 9090

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int main() {
	int tcp_listen_sock, tcp_client_sock;
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t clilen;

	int unix_listen_sock, unix_client_sock;
	struct sockaddr_un unix_name;

	char buffer[1024];
	int n;
	int opt = 1;

	tcp_listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_listen_sock < 0) error("ERROR opening TCP socket");
	setsockopt(tcp_listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(TCP_PORT);

	if (bind(tcp_listen_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
		error("ERROR on TCP binding");
	listen(tcp_listen_sock, 1);
	printf("TCP Server listening on port %d\n", TCP_PORT);

	unix_listen_sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (unix_listen_sock < 0) error("ERROR opening UNIX socket");

	memset(&unix_name, 0, sizeof(unix_name));
	unix_name.sun_family = AF_UNIX;
	unlink(SOCKET_NAME);
	strncpy(unix_name.sun_path, SOCKET_NAME, sizeof(unix_name.sun_path) - 1);

	if (bind(unix_listen_sock, (struct sockaddr *) &unix_name, sizeof(unix_name)) < 0)
		error("ERROR on UNIX binding");
	listen(unix_listen_sock, 1);
	printf("UNIX Server listening on %s\n", SOCKET_NAME);

	printf("Waiting for TCP client...\n");
	clilen = sizeof(cli_addr);
	tcp_client_sock = accept(tcp_listen_sock, (struct sockaddr *) &cli_addr, &clilen);
	if (tcp_client_sock < 0) error("ERROR accept TCP");
	printf("TCP Client connected!\n");

	printf("Waiting for UNIX client...\n");
	unix_client_sock = accept(unix_listen_sock, NULL, NULL);
	if (unix_client_sock < 0) error("ERROR accept UNIX");
	printf("UNIX Client connected!\n");

	printf("\nBRIDGE ESTABLISHED: TCP <---> UNIX\n");

	fd_set readfds; // sockets we are listening
	int max_fd;

	while (1) {
		FD_ZERO(&readfds);
		FD_SET(tcp_client_sock, &readfds);
		FD_SET(unix_client_sock, &readfds);

		max_fd = (tcp_client_sock > unix_client_sock) ? tcp_client_sock : unix_client_sock;

		int activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);

		if ((activity < 0)) {
			printf("Select error\n");
			break;
		}

		if (FD_ISSET(tcp_client_sock, &readfds)) {
			memset(buffer, 0, sizeof(buffer));
			n = read(tcp_client_sock, buffer, sizeof(buffer));

			if (n <= 0) {
				printf("TCP Client disconnected.\n");
				break;
			}
			write(unix_client_sock, buffer, n);
		}

		if (FD_ISSET(unix_client_sock, &readfds)) {
			memset(buffer, 0, sizeof(buffer));
			n = read(unix_client_sock, buffer, sizeof(buffer));

			if (n <= 0) {
				printf("UNIX Client disconnected.\n");
				break;
			}
			 write(tcp_client_sock, buffer, n);
		}
	}

	close(tcp_client_sock);
	close(unix_client_sock);
	close(tcp_listen_sock);
	close(unix_listen_sock);
	unlink(SOCKET_NAME);

	return 0;
}