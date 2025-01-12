#ifndef PRODUCER_CONSUMER_H
#define PRODUCER_CONSUMER_H

#define BUFFER_SIZE 6 //缓冲池大小
#define BUFFER_LENGTH 11 //单个缓冲区的大小
#define PRODUCERS 4 //生产者总数
#define CONSUMERS 4 //消费者总数
#define PRODUCER_OPERATIONS 8 //生产者总操作次数
#define CONSUMER_OPERATIONS 8 //消费者总操作次数

typedef struct buffer {
	char pool[BUFFER_SIZE][BUFFER_LENGTH];
	int head, tail;
} BUFFER;

#endif