#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

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
		if (!(cond))          \
			FATAL_LOG(msg);   \
	}

#define ASSERT_F_MSG(cond, msg, ...)         \
	{                                        \
		if (!(cond))                         \
			FATAL_LOG_F(msg, ##__VA_ARGS__); \
	}


#define MAX_CIPHER_MESSAGE 1024

typedef struct {
	uint8_t key;
	char message[MAX_CIPHER_MESSAGE];
} cipher_t;

#endif /* COMMON_H */