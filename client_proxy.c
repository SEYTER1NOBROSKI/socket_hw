#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/select.h>
#include <sys/un.h>

#define SOCKET_NAME "/tmp/unix_client.sock"
#define HOST_NAME "localhost"
#define TCP_PORT 9090

void error(const char *msg)
{
	perror(msg);
	exit(0);
}

int main()
{
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	int unix_listen_sock, unix_client_sock;
	struct sockaddr_un unix_name;

	char buffer[256];

	//portno
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");
	server = gethostbyname(HOST_NAME);
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	memcpy(&serv_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);
	serv_addr.sin_port = htons(TCP_PORT);
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
		error("ERROR connecting");

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

	printf("Waiting for UNIX client...\n");
	unix_client_sock = accept(unix_listen_sock, NULL, NULL);
	if (unix_client_sock < 0) error("ERROR accept UNIX");
	printf("UNIX Client connected!\n");

	fd_set readfds;
	int max_fd;

	printf("This is client proxy terminal!\n");
	
	while (1) {
		FD_ZERO(&readfds);
		FD_SET(sockfd, &readfds);
		FD_SET(unix_client_sock, &readfds);

		max_fd = (sockfd > unix_client_sock) ? sockfd : unix_client_sock;

		int activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);

		if ((activity < 0)) {
			printf("Select error\n");
			break;
		}

		if (FD_ISSET(sockfd, &readfds)) {
			//memset(buffer, 0, sizeof(buffer));
			//printf("Please enter the message: ");
			//bzero(buffer,256);
			//fgets(buffer,255,stdin);
			//n = write(sockfd,buffer,strlen(buffer));
			//if (n < 0) 
				//error("ERROR writing to socket");
			bzero(buffer,256);
			n = read(sockfd,buffer,255);
			printf("%s\n", buffer);
			write(unix_client_sock, buffer, strlen(buffer));
		}

		if (FD_ISSET(unix_client_sock, &readfds)) {
			memset(buffer, 0, sizeof(buffer));
			n = read(unix_client_sock,buffer,255);
			if (n < 0) 
				error("ERROR reading from socket");
			printf("%s\n",buffer);
			write(sockfd, buffer, n);
		}
	}

	close(sockfd);
	close(unix_client_sock);
	close(unix_listen_sock);
	unlink(SOCKET_NAME);
	return 0;
}