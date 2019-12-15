#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#include "common.h"
#include "server_args.h"

int
main(int argc, char** argv)
{
	struct gengetopt_args_info args_info;
	ASSERT_MSG(cmdline_parser(argc, argv, &args_info) != -1, "Error on cmdline_parser");

	int sfd;

	ASSERT_MSG((sfd = socket(AF_INET, SOCK_DGRAM, 0)) != -1,
		"Failed creating IPv4 socket");

	struct sockaddr_in addr, client_addr;
	socklen_t c_len = sizeof(client_addr);
	memset(&addr, 0, sizeof(addr));

	ASSERT_F_MSG(args_info.port_arg > 0 && args_info.port_arg < 65535,
		"invalid port '%d' ([1,65535])",
		args_info.port_arg);
	
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(args_info.port_arg);

	ASSERT_MSG(bind(sfd, (struct sockaddr *)&addr, sizeof(addr)) != -1, "Failed to bind!");

	printf("[SERVER] Waiting for clients UDP/%d\n", args_info.port_arg);
	for (;;)
	{
		int n;
		char c;

		ASSERT_MSG((n = recvfrom(sfd, &c, sizeof(char),
								 0, (struct sockaddr *)&client_addr,
								 &c_len)) != -1,
				   "Error on recvfrom");
		printf("[SERVER] %d byte(s) received ('%c')\n",
			   n, c);

		int16_t velocity;
		switch (c)
		{
			case 'L': velocity = 120; break;
			case 'M': velocity = 130; break;
			case 'N': velocity = 140; break;
			case 'Q': velocity = 160; break;
			case 'R': velocity = 170; break;
			case 'S': velocity = 180; break;
			case 'T': velocity = 190; break;
			case 'U': velocity = 200; break;
			case 'H': velocity = 210; break;
			case 'V': velocity = 240; break;
			case 'W': velocity = 270; break;
			case 'Y': velocity = 300; break;
			default: velocity = -1;
		}

		/* ignore when invalid */
		if(velocity != -1)
		{
			velocity = htons(velocity);

			printf("[SERVER] Sending answer to client\n");
			ASSERT_MSG((n = sendto(sfd, &velocity, sizeof(velocity),
								0, (const struct sockaddr *)&client_addr,
								c_len)) != -1,
					"Error on sendto");
			printf("[SERVER] Answer sent to client (%d byte(s))\n",
				n);
		}
	}
	
	cmdline_parser_free(&args_info);
	return 0;
}
