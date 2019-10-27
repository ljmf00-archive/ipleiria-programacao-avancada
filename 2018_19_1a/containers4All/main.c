#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "args.h"

#define INFO_LOG_F(msg, ...) printf("[INFO] "msg"\n", ##__VA_ARGS__)
#define WARN_LOG_F(msg, ...) printf("[WARNING] "msg"\n", ##__VA_ARGS__)

#define ERR_LOG(msg) fprintf(stderr, "[ERROR] "msg"\n")
#define ERR_LOG_F(msg, ...) fprintf(stderr, "[ERROR] "msg"\n", ##__VA_ARGS__)

typedef struct
{
	int *containers_left;
	pthread_mutex_t *mutex;

	int num_containers;

	int id;
	int c_counter;
	int d_counter;
} truck_t;

void *truck_task(void *);


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


int
main(int argc, char **argv)
{
	struct gengetopt_args_info args_info;

	if( cmdline_parser(argc, argv, &args_info) ){
		ERR_LOG("Error on cmdline_parser");
		exit(EXIT_FAILURE);
	}

	int trucks = args_info.trucks_arg;
	int containers = args_info.containers_arg;

	if(trucks <= 0)
	{
		ERR_LOG_F("Invalid number of trucks (%d)\n", trucks);
		exit(EXIT_FAILURE);
	}

	if(containers <= 0)
	{
		ERR_LOG_F("Invalid number of containers (%d)\n", containers);
		exit(EXIT_FAILURE);
	}

	pthread_t* truck_tid = malloc(sizeof(pthread_t) * trucks);
	truck_t *params = malloc(sizeof(truck_t) * trucks);

	int containers_left = containers;
	pthread_mutex_t mutex;

	if(pthread_mutex_init(&mutex, NULL) != 0)
	{
		ERR_LOG("Error initializing a mutex");
		exit(EXIT_FAILURE);
	}

	int i;

	srand(time(NULL));

	for (i = 0; i < trucks; i++)
	{
		params[i].containers_left = &containers_left;
		params[i].mutex = &mutex;
		params[i].c_counter = 0;
		params[i].d_counter = 0;
		params[i].id = i+1;
		params[i].num_containers = containers;

		if (pthread_create(&(truck_tid[i]), NULL, truck_task, &(params[i])) != 0)
		{
			ERR_LOG("Error creating a thread!");
			exit(EXIT_FAILURE);
		}
	}

	for (i = 0; i < trucks; i++)
	{
		if(pthread_join(truck_tid[i], NULL) != 0)
		{
			ERR_LOG("Error joining a thread!");
			exit(EXIT_FAILURE);
		}
	}

	int total_kms = 0;
	for (i = 0; i < trucks; i++)
	{
		printf("-------------------------------------------\n\
Truck %d: %d container(s); %d kms\n",
			i+1,
			params[i].c_counter,
			params[i].d_counter);

		total_kms += params[i].d_counter;
	}

	printf("-------------------------------------------\n\
Total kms: %d\n\
Avg kms per truck: %.1f\n",
		total_kms,
		(float)total_kms/trucks);

	if (pthread_mutex_destroy(&mutex) != 0)
	{
		ERR_LOG("Error destroing mutex");
		exit(EXIT_FAILURE);
	}

	cmdline_parser_free(&args_info);

	return 0;
}

int container_lock(truck_t *);

void*
truck_task(void* args)
{
	truck_t *truck = (truck_t *)args;

	INFO_LOG_F("TruckID '%d' starting service", truck->id);

	while(container_lock(truck))
	{
		int container_tout = random_number(1, 10);
		int dist = container_tout * 100;

		printf("[ID:%d] Got container %03d ( %d left): %dkms\n",
			   truck->id,
			   truck->num_containers-(*(truck->containers_left)),
			   *(truck->containers_left),
			   dist);

		truck->d_counter += dist;

		sleep(container_tout);
	}

	return NULL;
}

int
container_lock(truck_t* truck)
{
	int ret;

	if (pthread_mutex_lock(truck->mutex) != 0)
	{
		ERR_LOG("Can't lock mutex");
		exit(EXIT_FAILURE);
	}

	if(*(truck->containers_left) <= 0)
		ret = 0;
	else {
		(*(truck->containers_left))--;
		truck->c_counter++;
		ret = 1;
	}

	if(pthread_mutex_unlock(truck->mutex) != 0)
	{
		ERR_LOG("Can't lock mutex");
		exit(EXIT_FAILURE);
	}

	return ret;
}