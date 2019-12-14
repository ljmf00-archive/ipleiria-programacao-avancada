#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>
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
		if (cond)             \
			FATAL_LOG(msg);   \
	}

#define ASSERT_F_MSG(cond, msg, ...)         \
	{                                        \
		if (cond)                            \
			FATAL_LOG_F(msg, ##__VA_ARGS__); \
	}


int main(int argc, char** argv)
{
	struct gengetopt_args_info args_info;

	if( cmdline_parser(argc, argv, &args_info) ){
		ERR_LOG("Error on cmdline_parser");
		exit(EXIT_FAILURE);
	}

	int32_t dest;

	switch(inet_pton(AF_INET, args_info.address_arg, &dest))
	{
		case -1:
			fprintf(stderr, "Invalid address family\n");
			break;
		case 0:
			fprintf(stderr, "Error converting IPv4 address\n");
			break;
		default:
			printf("%hhu.%hhu.%hhu.%hhu\n",
				((unsigned char *)&dest)[0],
				((unsigned char *)&dest)[1],
				((unsigned char *)&dest)[2],
				((unsigned char *)&dest)[3]);
	}

	cmdline_parser_free(&args_info);

	return 0;
}