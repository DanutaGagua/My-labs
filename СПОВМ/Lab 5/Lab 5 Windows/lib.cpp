#include "stdafx.h"
#include <Windows.h>

struct FileInfo
{
	HANDLE file;
	DWORD numberOfBytes;
	CHAR  buffer[100];
	DWORD inOffset;
	DWORD  outOffset;
	OVERLAPPED Overlapped;
};

DWORD readFromFile(FileInfo* fileInfo)
{
	ReadFile(fileInfo->file, fileInfo->buffer, fileInfo->numberOfBytes, NULL, &fileInfo->Overlapped);

	WaitForSingleObject(fileInfo->Overlapped.hEvent, INFINITE);
}

DWORD writeToFile(FileInfo* fileInfo)
{
	WriteFile(fileInfo->file, fileInfo->buffer, fileInfo->Overlapped.InternalHigh, NULL, &fileInfo->Overlapped);

	WaitForSingleObject(fileInfo->Overlapped.hEvent, INFINITE);
}