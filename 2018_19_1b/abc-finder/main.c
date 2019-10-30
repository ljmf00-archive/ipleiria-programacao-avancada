#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/wait.h>
#include <unistd.h>
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

	ASSERT_MSG(cmdline_parser(argc, argv, &args_info) != 0,
		"Error on cmdline_parser");

	printf("File: %s\n\n", args_info.file_arg);

	int i;
	for (i = 'a'; i <= 'z'; i++)
	{
		switch(fork())
		{
			case -1:
				FATAL_LOG("Error on cmdline_parser");
				break;

			case 0:

			{
				FILE* f = fopen(args_info.file_arg, "rb");
				int c;
				int counter = 0;

				while((c = fgetc(f)) != EOF)
					if(tolower(c) == i)
						++counter;
				
				char upper = toupper(i);
				printf("Process [%c]: found %d '%c' or '%c' occurrences!\n",
					upper,
					counter,
					i,
					upper);

				exit(0);
			}
		}

		wait(NULL);
	}

	cmdline_parser_free(&args_info);

	return 0;
}