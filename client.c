#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_NAME "/tmp/unix_client.sock"

int main()
{
	int unix_socket;
	struct sockaddr_un server_addr;
	int n;

	char buffer[256];

	unix_socket = socket(AF_UNIX, SOCK_STREAM, 0);
	if (unix_socket < 0) {
		printf("Error opening socket");
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sun_family = AF_UNIX;
	strncpy(server_addr.sun_path, SOCKET_NAME, sizeof(server_addr.sun_path) - 1);

	int connect_status = connect(unix_socket, (const struct sockaddr *) &server_addr, sizeof(server_addr));
	if (connect_status == -1) {
		printf("Connect error");
	}

	printf("Welcome to client app!\n");

	while (1) {
		printf("Please enter the message: ");
		memset(&buffer, 0, sizeof(buffer));
		fgets(buffer, sizeof(buffer) - 1, stdin);
		n = write(unix_socket, buffer, strlen(buffer));
		if (n < 0) {
			printf("Error writing to socket");
		}

		memset(&buffer, 0, sizeof(buffer));
		n = read(unix_socket, buffer, sizeof(buffer) - 1);
		if (n < 0) {
			printf("Error reading from socket");
		}
		printf("%s\n", buffer);
	}

	close(unix_socket);
	return 0;
}