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

//���������
int Random(int floor, int ceil) {
	srand(time(0) + id); //�������������
	int random = floor + rand() % (ceil - floor);
	return random;
}
//��ȡ��ǰʱ��
char* GetTime() {
	time_t now;
	time(&now);
	return ctime(&now);
}
//������������תΪ�ַ���
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
//�ӻ������ڶ�ȡ����
void Consume() {
	Sleep(Random(1000, 4000)); //����ȴ�1��4��
	printf("����ȴ�����\n");
	char data[11] = { 0 };
	WaitForSingleObject(hFull, INFINITE); //P(full)
	WaitForSingleObject(hMutex, INFINITE); //P(mutex)
	printf("��ʼ��ȡ����\n");
	CopyMemory(&buffer, (PVOID)lpBuffer, sizeof(BUFFER)); 
	strcpy(data, buffer.pool[buffer.tail]);
	strcpy(buffer.pool[buffer.tail], "null"); //ȡ������
	buffer.tail = (buffer.tail + 1) % BUFFER_SIZE;
	CopyMemory((PVOID)lpBuffer, &buffer, sizeof(BUFFER));
	printf("�ɹ���ȡ����\n");
	FILE* fp = fopen("output.txt", "a");
	fprintf(fp, "%s", GetTime());
	fprintf(fp, "������%d�ӻ�������ȡ����%s\n", id, data);
	fprintf(fp, "��ʱ����������Ϊ%s\n\n", ShowBuffer(buffer.pool));
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