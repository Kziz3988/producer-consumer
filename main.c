#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include "producer-consumer.h"

//�洢�ӽ��̵ľ���Ա����
HANDLE process[PRODUCERS + CONSUMERS];

//����ת�ַ�����ƴ��
void IntCat(int num, char dest[]) {
	strcat(dest, " ");
	char number[10] = { 0 };
	itoa(num, number, 10);
	strcat(dest, number);
}

//��������
HANDLE NewProcess(const char* str) {
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = TRUE;
	int size = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	LPWSTR lpwstr = (LPWSTR)malloc(size * sizeof(WCHAR));
	MultiByteToWideChar(CP_ACP, 0, str, -1, lpwstr, size);
	return CreateProcess(NULL, lpwstr, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
}

int main() {
	//����������
	BUFFER buffer;
	HANDLE hBuffer = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(BUFFER), L"buffer");
	LPCTSTR lpBuffer = (LPTSTR)MapViewOfFile(hBuffer, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(BUFFER));
	for (int i = 0; i < BUFFER_SIZE; i++) {
		strcpy(buffer.pool[i], "null");
	}
	buffer.head = 0;
	buffer.tail = 0;
	CopyMemory((PVOID)lpBuffer, &buffer, sizeof(BUFFER));

	//���������־
	FILE* fp = fopen("output.txt", "w");
	fclose(fp);

	//�����ź���
	HANDLE hMutex = CreateSemaphore(NULL, 1, 1, L"mutex");
	HANDLE hEmpty = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, L"empty");
	HANDLE hFull = CreateSemaphore(NULL, 0, BUFFER_SIZE, L"full");

	//���������߽���
	for (int i = 0; i < PRODUCERS; i++) {
		char path[1000] = { 0 };
		strcat(path, "producer.exe");
		IntCat(i, path);
		process[i] = NewProcess(path);
	}
	//���������߽���
	for (int i = 0; i < CONSUMERS; i++) {
		char path[1000] = { 0 };
		strcat(path, "consumer.exe");
		IntCat(i, path);
		process[i + PRODUCERS] = NewProcess(path);
	}

	WaitForMultipleObjects(PRODUCERS + CONSUMERS, process, TRUE, INFINITE);
	for (int i = 0; i < PRODUCERS + CONSUMERS; i++) {
		CloseHandle(process[i]);
	}
	CloseHandle(hMutex);
	CloseHandle(hEmpty);
	CloseHandle(hFull);
	UnmapViewOfFile(hBuffer);
	CloseHandle(hBuffer);
	return 0;
}