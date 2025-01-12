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
//向缓冲区内添加数据
void Produce() {
	Sleep(Random(2000, 5000)); //随机等待2至5秒
	printf("随机等待结束\n");
	//写入一个int范围内的10位随机数
	char data[11] = { 0 };
	itoa(Random(1000000000, 2147483647), data, 10);
	WaitForSingleObject(hEmpty, INFINITE); //P(empty)
	WaitForSingleObject(hMutex, INFINITE); //P(mutex)
	printf("开始写入数据\n");
	CopyMemory(&buffer, (PVOID)lpBuffer, sizeof(BUFFER));
	strcpy(buffer.pool[buffer.head], data);
	buffer.head = (buffer.head + 1) % BUFFER_SIZE;
	CopyMemory((PVOID)lpBuffer, &buffer, sizeof(BUFFER));
	printf("成功写入数据\n");
	FILE* fp = fopen("output.txt", "a");
	fprintf(fp, "%s", GetTime());
	fprintf(fp, "生产者%d向缓冲区写入数据%s\n", id, data);
	fprintf(fp, "此时缓冲区内容为%s\n\n", ShowBuffer(buffer.pool));
	fclose(fp);
	ReleaseSemaphore(hFull, 1, NULL); //V(full)
	ReleaseSemaphore(hMutex, 1, NULL); //V(mutex)
}
int main(int argc, char* argv[]) {
	id = atoi(argv[1]);
	hMutex = CreateSemaphore(NULL, 1, 1, L"mutex");
	hEmpty = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, L"empty");
	hFull = CreateSemaphore(NULL, 0, BUFFER_SIZE, L"full");
	hBuffer = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(BUFFER), L"buffer");
	lpBuffer = (LPTSTR)MapViewOfFile(hBuffer, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(BUFFER));
	for (int i = 0; i < PRODUCER_OPERATIONS; i++) {
		Produce();
	}
	return 0;
}