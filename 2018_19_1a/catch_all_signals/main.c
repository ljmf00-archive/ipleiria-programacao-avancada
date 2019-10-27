#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include "args.h"

#define INFO_LOG_F(msg, ...) printf("[INFO] "msg"\n", ##__VA_ARGS__)
#define WARN_LOG_F(msg, ...) printf("[WARNING] "msg"\n", ##__VA_ARGS__)

#define ERR_LOG(msg) fprintf(stderr, "[ERROR] "msg"\n")
#define ERR_LOG_F(msg, ...) fprintf(stderr, "[ERROR] "msg"\n", ##__VA_ARGS__)

int term = 0;

void signal_handler(int signal, siginfo_t *info, void *ptr)
{
	(void)ptr;
	printf("[PID:%d] Got signal '%s' (%d) from process '%d'\n",
		   getpid(),
		   strsignal(signal),
		   signal,
		   info->si_pid);

	if( signal == SIGINT)
	{
		printf("Got SIGINT -- terminating\n");
		term = 1;
	}
}

int main(int argc, char** argv)
{
	struct gengetopt_args_info args_info;
	struct sigaction act;

	memset(&act, 0, sizeof(act));

	act.sa_sigaction = signal_handler;
	act.sa_flags = SA_SIGINFO;

	if( cmdline_parser(argc, argv, &args_info) ){
		ERR_LOG("Error on cmdline_parser");
		exit(EXIT_FAILURE);
	}

	int min = (args_info.min_given) ? args_info.min_arg : 1;
	int max = (args_info.max_given) ? args_info.max_arg : 64;

	if(min < 1)
	{
		ERR_LOG_F("min signal must be >= 1 (%d given)", min);
		exit(EXIT_FAILURE);
	}

	if(min > 64)
	{
		ERR_LOG("minimum value need to be between [1;64]");
		exit(EXIT_FAILURE);
	}

	if(max < 1 && max > 64)
	{
		ERR_LOG("maximum value need to be between [1;64]");
		exit(EXIT_FAILURE);
	}

	if( min > max)
	{
		ERR_LOG_F("min signal must be <= max (%d)", max);
	}

	INFO_LOG_F("Processing signal from [MIN:%d, MAX:%d]", min, max);

	int i;
	for (i = min; i <= max; i++)
	{
		if(sigaction(i, &act, NULL) == -1)
		{
			WARN_LOG_F("Can't install handler for signal '%s' (%d)", strsignal(i), i);
		}
	}

	int pid = getpid();
	INFO_LOG_F("Terminate: kill -s SIGKILL %d | kill -s SIGINT %d", pid, pid);
	printf("Waiting for a signal\n");

	while(!term)
		pause();
	
	cmdline_parser_free(&args_info);

	return 0;
}