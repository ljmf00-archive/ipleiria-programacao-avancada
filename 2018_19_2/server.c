#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <limits.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

#include "common.h"
#include "server_args.h"

void server_child(int );

int
main(int argc, char** argv)
{
	struct gengetopt_args_info args_info;
	ASSERT_MSG(cmdline_parser(argc, argv, &args_info) != -1, "Error on cmdline_parser");

	int sfd, cfd;
	socklen_t c_len;

	ASSERT_MSG((sfd = socket(AF_INET, SOCK_STREAM, 0)) != -1,
		"Failed creating IPv4 socket");

	struct sockaddr_in addr, client_addr;
	memset(&addr, 0, sizeof(addr));

	ASSERT_F_MSG(args_info.port_arg > 0 && args_info.port_arg < SHRT_MAX,
		"Invalid port number %d",
		args_info.port_arg);
	
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(args_info.port_arg);

	ASSERT_MSG(bind(sfd, (struct sockaddr *)&addr, sizeof(addr)) != -1, "Failed to bind!");
	ASSERT_MSG(listen(sfd, 5) != -1, "Failed to listen!");

	for (;;)
	{
		c_len = sizeof(client_addr);
		ASSERT_MSG((cfd = accept(sfd, (struct sockaddr *)&client_addr, &c_len)) >= 0,
			"Error on accept!");
		
		// avoid zombie processes
		signal(SIGCHLD, SIG_IGN);

		switch(fork())
		{
			case 0:
				close(sfd);
				server_child(cfd);
				break;
			case -1: FATAL_LOG("Fail on fork()!");
		}
	}

	cmdline_parser_free(&args_info);
	return 0;
}

void
server_child(int cfd)
{
	cipher_t client_answer, server_answer;

	ASSERT_MSG(recv(cfd, &client_answer, sizeof(cipher_t), 0) != -1,
		"Error on recv!");

	char c;
	if(client_answer.key == 0)
	{
		srand(time(NULL) ^ getpid());
		server_answer.key = 1 + (uint8_t)(rand() % 127 + 1);
		for (int i = 0; i < MAX_CIPHER_MESSAGE; i++)
		{
			if(client_answer.message[i] == '\0')
			{
				server_answer.message[i] = '\0';
				break;
			}
			server_answer.message[i] = ((c = client_answer.message[i] ^ server_answer.key) >= 32)
				? c
				: client_answer.message[i];
		}
	} else {
		server_answer.key = 0;
		for (int i = 0; i < MAX_CIPHER_MESSAGE; i++)
		{
			if(client_answer.message[i] == '\0')
			{
				server_answer.message[i] = '\0';
				break;
			}
			server_answer.message[i] = ((c = client_answer.message[i] ^ client_answer.key) < 32)
				? client_answer.message[i]
				: c;
		}
	}

	ASSERT_MSG(send(cfd, &server_answer, sizeof(cipher_t), 0) != -1,
		"Error on send!");

	close(cfd);
	exit(0);
}
