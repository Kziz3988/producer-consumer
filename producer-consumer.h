#ifndef PRODUCER_CONSUMER_H
#define PRODUCER_CONSUMER_H

#define BUFFER_SIZE 6 //����ش�С
#define BUFFER_LENGTH 11 //�����������Ĵ�С
#define PRODUCERS 4 //����������
#define CONSUMERS 4 //����������
#define PRODUCER_OPERATIONS 8 //�������ܲ�������
#define CONSUMER_OPERATIONS 8 //�������ܲ�������

typedef struct buffer {
	char pool[BUFFER_SIZE][BUFFER_LENGTH];
	int head, tail;
} BUFFER;

#endif