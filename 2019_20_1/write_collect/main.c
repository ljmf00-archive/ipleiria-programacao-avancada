#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include "args.h"


#define ERR_LOG(msg) fprintf(stderr, "[ERROR] "msg"\n")

int random_number(int min, int max)
{
	int ret, low, hi;

	if (min < max)
	{
		low = min;
		hi = max + 1;
	} else {
		low = max + 1;
		hi = min;
	}

	ret = (rand() % (hi - low)) + low;
	return ret;
}

typedef struct {
	int* arr;
	int* size_arr;
	int max_size;
	int id;
	int rounds_left;
	pthread_mutex_t* mutex;
} sensor_params_t;


void* task_sensor(void* );

int main(int argc, char** argv) {
	struct gengetopt_args_info args_info;

	if( cmdline_parser(argc, argv, &args_info) != 0){
		ERR_LOG("Error on cmdline_parser");
		exit(EXIT_FAILURE);
	}
	
	if(args_info.threads_arg > 20 || args_info.threads_arg < 3)
	{
		fprintf(stderr, "threads_arg must be within [3,20] (%d given)\n",
			args_info.threads_arg);
		exit(EXIT_FAILURE);
	}
	
	if(args_info.rounds_arg > 10 || args_info.rounds_arg < 3)
	{
		fprintf(stderr, "round_arg must be within [3,10] (%d given)\n",
			args_info.threads_arg);
		exit(EXIT_FAILURE);
	}
	
	if(args_info.size_arg < 5)
	{
		fprintf(stderr, "size_arg must be at least 5 (%d given)\n",
			args_info.threads_arg);
		exit(EXIT_FAILURE);
	}

	int* arr = malloc(sizeof(int) * args_info.size_arg);
	int size_arr = 0;
	pthread_mutex_t arr_mutex;
	pthread_t* tids = malloc(sizeof(pthread_t) * args_info.threads_arg);
	sensor_params_t* params = malloc(sizeof(sensor_params_t) * args_info.threads_arg);
	int rounds_left = args_info.rounds_arg;
	
	if(pthread_mutex_init(&arr_mutex, NULL) != 0)
	{
		ERR_LOG("Error initializing a mutex");
		exit(EXIT_FAILURE);
	}
	
	srand(time(NULL));
	
	int i, j;
	for(j = 0; j < args_info.rounds_arg; j++)
	{
		for(i = 0; i < args_info.threads_arg; i++)
		{
			params[i].arr = arr;
			params[i].size_arr = &size_arr;
			params[i].mutex = &arr_mutex;
			params[i].max_size = args_info.size_arg;
			params[i].id = i;
			params[i].rounds_left = rounds_left;

			if (pthread_create(&(tids[i]), NULL, task_sensor, &(params[i])) != 0)
			{
				ERR_LOG("Error creating a thread!");
				exit(EXIT_FAILURE);
			}
		}
		
		for(i = 0; i < args_info.threads_arg; i++)
		{
			if(pthread_join(tids[i], NULL) != 0)
			{
				ERR_LOG("Error joining a thread!");
				exit(EXIT_FAILURE);
			}
		}
		
		printf("[CLEANER]: about to start cleaning...\n");
		for(i = 0; i < size_arr; i++)
		{
			printf("%d|", arr[i]);
		}
		printf("[CLEANER]: cleaning done\n");
		size_arr = 0;
		rounds_left--;
	}
	
	
	if (pthread_mutex_destroy(&arr_mutex) != 0)
	{
		ERR_LOG("Error destroing mutex");
		exit(EXIT_FAILURE);
	}
	
	cmdline_parser_free(&args_info);
	
	return 0;
}

int sensor_lock(sensor_params_t* );
void* task_sensor(void* args)
{
	unsigned int seedp = pthread_self();
	sensor_params_t* params = (sensor_params_t*) args;
	
	while(sensor_lock(params))
	{
		int pos = *(params->size_arr);
		int sensor_value = (rand_r(&seedp) % 100) + 1;
		
		params->arr[pos] = sensor_value;
		printf("[Worker:%d] writing '%d' at position %d\n",
			params->id,
			sensor_value,
			pos);

		(*(params->size_arr))++;
		
		if(pthread_mutex_unlock(params->mutex) != 0)
		{
			ERR_LOG("Can't unlock mutex");
			exit(EXIT_FAILURE);
		}
		
		struct timespec _time;
		memset(&_time, 0, sizeof(struct timespec));
		_time.tv_nsec = 10000;
		
		nanosleep(&_time, NULL);
	}
	
	if(params->rounds_left == 1)
	{
		printf("[Worker:%d] All rounds done\n",
			params->id);
	}
	
	return NULL;
}

int
sensor_lock(sensor_params_t* params)
{
	if (pthread_mutex_lock(params->mutex) != 0)
	{
		ERR_LOG("Can't lock mutex");
		exit(EXIT_FAILURE);
	}

	if(*(params->size_arr) == params->max_size)
	{
		if(pthread_mutex_unlock(params->mutex) != 0)
		{
			ERR_LOG("Can't unlock mutex");
			exit(EXIT_FAILURE);
		}
		return 0;
	}
	else {
		return 1;
	}
}
