#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_NAME "/tmp/unix.sock"

int main()
{
	int unix_server_sock;
	struct sockaddr_un unix_name;
	int n;

	char buffer[256];

	unix_server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (unix_server_sock < 0) {
		printf("Error opening socket");
	}

	memset(&unix_name, 0, sizeof(unix_name));
	unix_name.sun_family = AF_UNIX;
	strncpy(unix_name.sun_path, SOCKET_NAME, sizeof(unix_name.sun_path) - 1);

	int connect_status = connect(unix_server_sock, (const struct sockaddr *) &unix_name, sizeof(unix_name));
	if (connect_status == -1) {
		printf("Connect error");
	}

	printf("Welcome to server app!\n");

	while (1) {
		memset(buffer, 0, sizeof(buffer));
		n = read(unix_server_sock, buffer, sizeof(buffer)-1);
		if (n < 0) {
			printf("Error reading from socket");
		}
		printf("Here is the message: %s\n", buffer);
		printf("Write your answer: ");
		memset(&buffer, 0, sizeof(buffer));
		fgets(buffer, sizeof(buffer) - 1, stdin);
		n = write(unix_server_sock, buffer, strlen(buffer));
		if (n < 0) {
			printf("Error writing to socket");
		}
	}

	close(unix_server_sock);
	return 0;
}