#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

typedef struct {
	size_t id;
	size_t *inc;
	pthread_mutex_t* mutex;
} params_t;

#define MAX_THREADS 2

void *task(void *);

int
main(void)
{
	int i;
	pthread_t tid[MAX_THREADS];
	params_t param[MAX_THREADS];
	size_t inc = 0;
	pthread_mutex_t mutex;

	pthread_mutex_init(&mutex, NULL);

	for (i = 0; i < MAX_THREADS; i++)
	{
		param[i].id = i;
		param[i].inc = &inc;
		param[i].mutex = &mutex;
		if (pthread_create(&tid[i], NULL, task, &param[i]))
		{
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}
	}

	for (i = 0; i < MAX_THREADS; i++)
		if(pthread_join(tid[i], NULL))
		{
			fprintf(stderr, "Error joining thread\n");
			return 1;
		}

	pthread_mutex_destroy(&mutex);

	return 0;
}

void increment(size_t *inc, pthread_mutex_t *mutex);

void* task(void* args)
{
	params_t *param = (params_t *)args;

	while(true)
	{
		if (param->id % 2 == 0 && *(param->inc) % 2 == 0)
		{
			printf("ping\n");
		} else if (param->id % 2 != 0 && *(param->inc) % 2 != 0)
		{
			printf("pong\n");
		} else {
			continue;
		}

		if(pthread_mutex_lock(param->mutex))
		{
			fprintf(stderr, "Error locking mutex\n");
			return NULL;
		}

		(*(param->inc))++;

		if(pthread_mutex_unlock(param->mutex))
		{
			fprintf(stderr, "Error unlocking mutex\n");
			return NULL;
		}
	}

	return NULL;
}