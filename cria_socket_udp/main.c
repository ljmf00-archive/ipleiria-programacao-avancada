#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <limits.h>

#include "args.h"

#define INFO_LOG(msg) printf("[INFO] "msg"\n")
#define INFO_LOG_F(msg, ...) printf("[INFO] "msg"\n", ##__VA_ARGS__)

#define WARN_LOG(msg) printf("[WARNING] "msg"\n")
#define WARN_LOG_F(msg, ...) printf("[WARNING] "msg"\n", ##__VA_ARGS__)

#define ERR_LOG(msg) fprintf(stderr, "[ERROR] "msg"\n")
#define ERR_LOG_F(msg, ...) fprintf(stderr, "[ERROR] " msg "\n", ##__VA_ARGS__);

#define FATAL_LOG(msg)      \
	{                       \
		ERR_LOG(msg);       \
		exit(EXIT_FAILURE); \
	}
#define FATAL_LOG_F(msg, ...)          \
	{                                  \
		ERR_LOG_F(msg, ##__VA_ARGS__); \
		exit(EXIT_FAILURE);            \
	}

#define ASSERT_MSG(cond, msg) \
	{                         \
		if (!(cond))          \
			FATAL_LOG(msg);   \
	}

#define ASSERT_F_MSG(cond, msg, ...)         \
	{                                        \
		if (!(cond))                         \
			FATAL_LOG_F(msg, ##__VA_ARGS__); \
	}

#define MAXLINE 255

int main(int argc, char** argv)
{
	struct gengetopt_args_info args_info;
	ASSERT_MSG(cmdline_parser(argc, argv, &args_info) != -1, "Error on cmdline_parser");

	int sfd;

	ASSERT_MSG((sfd = socket(AF_INET, SOCK_DGRAM, 0)) != -1,
		"Failed creating IPv4 socket");

	printf("IPv4 Socket descriptor: %d\n", sfd);

	struct sockaddr_in addr, cliaddr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	memset(&cliaddr, 0, sizeof(cliaddr)); 

	switch(inet_pton(AF_INET, args_info.address_arg, &addr.sin_addr.s_addr))
	{
		case -1:
			FATAL_LOG("Invalid address family");
			break;
		case 0:
			FATAL_LOG("Error converting IPv4 address");
			break;
		default:
			printf("IPv4 address: %s (%d)\n",
				args_info.address_arg,
				addr.sin_addr.s_addr);
	}

	ASSERT_F_MSG(args_info.port_arg > 0 && args_info.port_arg < SHRT_MAX,
				"Invalid port number %d",
				args_info.port_arg);
	printf("IP Port: %d\n", args_info.port_arg);
	

	addr.sin_family = AF_INET;
	addr.sin_port = htons(args_info.port_arg);

	if(args_info.server_given)
		ASSERT_MSG(bind(sfd, (struct sockaddr*) &addr, sizeof(addr)) != -1,
			"Failed on binding socket");

	while(true)
	{
		int len = 1, n;
		socklen_t recvlen;
		char ch;
		char *buf = malloc(len * sizeof(char));
		buf[0] = '\0';
		while ((ch = getchar()) != '\n')
		{
			buf[len-1] = ch;
			++len;
			buf = realloc(buf, len * sizeof(char));
		}
		buf[len] = '\0';
		printf("Send buffer: %s ...\n", buf);

		char recvbuffer[MAXLINE];
		if(args_info.server_given)
		{
			printf("Waiting for client...\n");
			n = recvfrom(sfd, (char *)recvbuffer, MAXLINE,
						 MSG_WAITALL, (struct sockaddr *)&cliaddr,
						 &recvlen);
			recvbuffer[n] = '\0';
			printf("Found a client!\n");
			printf("Client: %s\n", recvbuffer);
			sendto(sfd, (const char *)buf, len,
				MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
					recvlen);
			printf("Message sent to client\n");
		}
		else
		{
			printf("Send to server...\n");
			sendto(sfd, (const char *)buf, len,
				   MSG_CONFIRM, (const struct sockaddr *)&addr,
				   sizeof(addr));

			printf("Waiting for server...\n");
			n = recvfrom(sfd, (char *)recvbuffer, MAXLINE,
						MSG_WAITALL, (struct sockaddr *)&addr,
						&recvlen);
			recvbuffer[n] = '\0';
			printf("Server: %s\n", recvbuffer);
		}
	}

	ASSERT_MSG(close(sfd) != -1, "Failed closing IPv4 socket");

	cmdline_parser_free(&args_info);
	return 0;
}