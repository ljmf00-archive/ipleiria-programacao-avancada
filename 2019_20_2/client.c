#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>

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

	ASSERT_F_MSG(args_info.port_arg > 0 && args_info.port_arg < 65535,
		"invalid port '%d' ([1,65535])",
		args_info.port_arg);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(args_info.port_arg);

	int sfd, n;
	socklen_t c_len = sizeof(addr);

	ASSERT_MSG((sfd = socket(AF_INET, SOCK_DGRAM, 0)) != -1,
		"Failed creating IPv4 socket");

	if(args_info.timeout_given)
	{
		ASSERT_F_MSG(args_info.timeout_arg >= 2 && args_info.timeout_arg <= 32,
			"invalid value '%d' for timeout [2,32]",
			args_info.timeout_arg);

		struct timeval timeout;
		timeout.tv_sec = args_info.timeout_arg;
		ASSERT_MSG(setsockopt(sfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) != -1,
			"Cannot setsockopt 'SO_RCVTIMEO'");
	}

	printf("[CLIENT] Sending data to server\n");
	ASSERT_MSG((n = sendto(sfd, args_info.letter_arg, sizeof(char),
						   0, (const struct sockaddr *)&addr,
						   sizeof(addr))) != -1,
			   "Error on sendto");
	printf("[CLIENT] Data sent (%d byte(s) sent)\n",
		   n);

	printf("[CLIENT] Waiting for server's answer\n");
	int16_t answ;
	n = recvfrom(sfd, &answ, sizeof(answ),
				 0, (struct sockaddr *)&addr,
				 &c_len);
	
	if(n <= 0)
	{
		if (args_info.timeout_given && errno == EAGAIN)
		{
			FATAL_LOG_F("Timeout (%d secs) expired for recvfrom: quiting",
				args_info.timeout_arg);
		}
		else {
			FATAL_LOG("Error on recvfrom");
		}
	}

	answ = ntohs(answ);

	printf("[CLIENT] Top speed for letter '%c'=%d kph\n",
		*(args_info.letter_arg),
		answ);

	ASSERT_MSG(close(sfd) != -1, "Failed closing IPv4 socket");
	cmdline_parser_free(&args_info);

	return 0;
}