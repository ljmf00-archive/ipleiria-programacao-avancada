#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <limits.h>
#include <unistd.h>

#include "common.h"
#include "client_args.h"

int
main(int argc, char** argv)
{
	struct gengetopt_args_info args_info;
	ASSERT_MSG(cmdline_parser(argc, argv, &args_info) != -1,
		"Error on cmdline_parser");

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));

	switch(inet_pton(AF_INET, args_info.ip_arg, &addr.sin_addr.s_addr))
	{
		case -1: FATAL_LOG("Invalid address family"); break;
		case 0: FATAL_LOG("Error converting IPv4 address"); break;
	}

	ASSERT_MSG(strlen(args_info.message_arg) <= 1024,
		"Message too long");

	ASSERT_F_MSG(args_info.port_arg > 0 && args_info.port_arg < SHRT_MAX,
		"Invalid port number %d",
		args_info.port_arg);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(args_info.port_arg);

	int sfd;

	ASSERT_MSG((sfd = socket(AF_INET, SOCK_STREAM, 0)) != -1,
		"Failed creating IPv4 socket");

	ASSERT_MSG(connect( sfd, (struct sockaddr *)&addr, sizeof(addr)) != -1,
		"Failed to connect");

	cipher_t client_answer, server_answer;

	if(args_info.key_given)
	{
		ASSERT_MSG(args_info.key_arg >= 1 && args_info.key_arg <= 127,
			"Invalid key");

		client_answer.key = args_info.key_arg;
	} else {
		client_answer.key = 0;
	}

	strcpy(client_answer.message, args_info.message_arg);

	ASSERT_MSG(send(sfd, &client_answer, sizeof(cipher_t), 0) != -1,
		"Error on send!");

	ASSERT_MSG(recv(sfd, &server_answer, sizeof(cipher_t), 0) != -1,
		"Error on recv!");

	if(!args_info.key_given)
		printf(">> KEY: %d\n", server_answer.key);

	printf(">> MESSAGE: %s\n", server_answer.message);

	close(sfd);
	cmdline_parser_free(&args_info);

	return 0;
}