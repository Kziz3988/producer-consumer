#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include "producer-consumer.h"

int id;
HANDLE hMutex;
HANDLE hEmpty;
HANDLE hFull;
HANDLE hBuffer;
LPCTSTR lpBuffer;
BUFFER buffer;

//生成随机数
int Random(int floor, int ceil) {
	srand(time(0) + id); //设置随机数种子
	int random = floor + rand() % (ceil - floor);
	return random;
}
//获取当前时间
char* GetTime() {
	time_t now;
	time(&now);
	return ctime(&now);
}
//将缓冲区内容转为字符串
char* ShowBuffer(char buffer[BUFFER_SIZE][BUFFER_LENGTH]) {
	char str[10000] = { 0 };
	for (int i = 0; i < BUFFER_SIZE; i++) {
		if (buffer[i][0] == '\0') {
			strcat(str, "null");
		}
		else {
			strcat(str, buffer[i]);
		}
		strcat(str, " ");
	}
	return str;
}
//从缓冲区内读取数据
void Consume() {
	Sleep(Random(1000, 4000)); //随机等待1至4秒
	printf("随机等待结束\n");
	char data[11] = { 0 };
	WaitForSingleObject(hFull, INFINITE); //P(full)
	WaitForSingleObject(hMutex, INFINITE); //P(mutex)
	printf("开始读取数据\n");
	CopyMemory(&buffer, (PVOID)lpBuffer, sizeof(BUFFER)); 
	strcpy(data, buffer.pool[buffer.tail]);
	strcpy(buffer.pool[buffer.tail], "null"); //取出数据
	buffer.tail = (buffer.tail + 1) % BUFFER_SIZE;
	CopyMemory((PVOID)lpBuffer, &buffer, sizeof(BUFFER));
	printf("成功读取数据\n");
	FILE* fp = fopen("output.txt", "a");
	fprintf(fp, "%s", GetTime());
	fprintf(fp, "消费者%d从缓冲区读取数据%s\n", id, data);
	fprintf(fp, "此时缓冲区内容为%s\n\n", ShowBuffer(buffer.pool));
	fclose(fp);
	ReleaseSemaphore(hEmpty, 1, NULL); //V(empty)
	ReleaseSemaphore(hMutex, 1, NULL); //V(mutex)
}
int main(int argc, char* argv[]) {
	id = atoi(argv[1]);
	hMutex = CreateSemaphore(NULL, 1, 1, L"mutex");
	hEmpty = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, L"empty");
	hFull = CreateSemaphore(NULL, 0, BUFFER_SIZE, L"full");
	hBuffer = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(BUFFER), L"buffer");
	lpBuffer = (LPTSTR)MapViewOfFile(hBuffer, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(BUFFER));
	for (int i = 0; i < CONSUMER_OPERATIONS; i++) {
		Consume();
	}
	return 0;
}