#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include "args.h"


#define ERR_LOG(msg) fprintf(stderr, "[ERROR] "msg"\n")

#define FATAL_LOG(msg)      \
	{                       \
		ERR_LOG(msg);       \
		exit(EXIT_FAILURE); \
	}

int main(int argc, char** argv) {
	struct gengetopt_args_info args_info;

	if( cmdline_parser(argc, argv, &args_info) != 0){
		ERR_LOG("Error on cmdline_parser");
		exit(EXIT_FAILURE);
	}
	
	if(args_info.procs_arg > 16 || args_info.procs_arg < 4)
	{
		fprintf(stderr, "ERR: num_procs must be within [4,16]:%d given\n",
			args_info.procs_arg);
		exit(EXIT_FAILURE);
	}

	int i;

	for(i = 0; i < args_info.procs_arg; i++)
	{
		switch(fork())
		{
			case -1:
				FATAL_LOG("Error running fork()");
				break;
				
			case 0:
				printf("[PAI:%d] Filho %d tem PID=%d\n",
					getppid(),
					i,
					getpid()
					);
				exit(EXIT_SUCCESS);
				break;
		}

		wait(NULL);
	}
	
	cmdline_parser_free(&args_info);
	
	return 0;
}
