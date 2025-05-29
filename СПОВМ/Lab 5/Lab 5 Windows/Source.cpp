#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include <string>
#include <conio.h>
#include <sstream>

using namespace std;

HINSTANCE dll_lib;
DWORD in, out;

struct FileInfo
{
	HANDLE file;
	DWORD numberOfBytes;
	CHAR  buffer[100];
	DWORD  inOffset;
	DWORD  outOffset;
	OVERLAPPED Overlapped;
} fileInfo;

void close_event_handles(HANDLE &e1, HANDLE &e2, HANDLE &e3);

DWORD WINAPI read_(LPVOID args);
DWORD WINAPI write_(LPVOID args);

string info;

int main(int argc, char* argv[]) {
	info = "";

	HANDLE threads[2];

	HANDLE read = CreateEvent(NULL, FALSE, FALSE, "Read");
	HANDLE write = CreateEvent(NULL, FALSE, FALSE, "Write");
	HANDLE close = CreateEvent(NULL, FALSE, FALSE, "Close");
	HANDLE eventH = CreateEvent(NULL, FALSE, FALSE, "Overlapped");

	fileInfo.Overlapped.Offset = 0;
	fileInfo.Overlapped.OffsetHigh = 0;
	fileInfo.Overlapped.hEvent = eventH;
	fileInfo.numberOfBytes = sizeof(fileInfo.buffer);

	dll_lib = LoadLibrary("lib.dll");

	threads[0] = CreateThread(NULL, 0, read_, NULL, 0, NULL);
	if (!threads[0]) {
		printf("Create Handle failed (%d)\n", GetLastError());
		getchar();
		return 1;
	}

	threads[1] = CreateThread(NULL, 0, write_, NULL, 0, NULL);
	if (!threads[1]) {
		printf("Create Handle failed (%d)\n", GetLastError());
		getchar();
		return 1;
	}

	WaitForMultipleObjects(2, threads, TRUE, INFINITE);

	CloseHandle(threads[0]);
	CloseHandle(threads[1]);

	FreeLibrary(dll_lib);

	close_event_handles(read, write, close);
	CloseHandle(eventH);

	getchar();

	return 0;
}

DWORD WINAPI read_(LPVOID args) {
	HANDLE write = OpenEvent(EVENT_ALL_ACCESS, FALSE, "Write");
	HANDLE read = OpenEvent(EVENT_ALL_ACCESS, FALSE, "Read");
	HANDLE close = OpenEvent(EVENT_ALL_ACCESS, FALSE, "Close");

	DWORD(*readFunc)(FileInfo*) = (DWORD(*)(FileInfo*))GetProcAddress(dll_lib, "readFromFile");

	cout << "Start reading from files." << endl;

	for (int i = 1; i < 11; i++) {
		string str_id;
		stringstream buffer;

		buffer << i;
		buffer >> str_id;

		info = str_id + ".txt";
		cout << "File " << info << endl;

		HANDLE readFile = CreateFile((char*)info.data(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		fileInfo.inOffset = 0;

		while (true) {
			fileInfo.file = readFile;
			fileInfo.Overlapped.Offset = in;

			DWORD result = (readFunc)(&fileInfo);

			cout << fileInfo.buffer << endl;

			in = fileInfo.Overlapped.InternalHigh;

			SetEvent(write);
			WaitForSingleObject(read, INFINITE);

			if (fileInfo.Overlapped.InternalHigh < fileInfo.numberOfBytes)
				break;				
		}
		
		CloseHandle(readFile);
	}
	SetEvent(close);

	close_event_handles(read, write, close);

	return 0;
}

DWORD WINAPI write_(LPVOID args) {
	HANDLE close = OpenEvent(EVENT_ALL_ACCESS, FALSE, "Close");
	HANDLE write = OpenEvent(EVENT_ALL_ACCESS, FALSE, "Write");
	HANDLE read = OpenEvent(EVENT_ALL_ACCESS, FALSE, "Read");
	DWORD(*writeFunc)(FileInfo*) = (DWORD(*)(FileInfo*))GetProcAddress(dll_lib, "writeToFile");

	HANDLE writeFile = CreateFile((char*)"output.txt", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, NULL);

	while (1) {
		if (WaitForSingleObject(write, 1) == WAIT_OBJECT_0) {
			fileInfo.file = writeFile;
			fileInfo.Overlapped.Offset = out;

			DWORD transfered = (writeFunc)(&fileInfo);

			out = fileInfo.Overlapped.InternalHigh;

			SetEvent(read);
		}

		if (WaitForSingleObject(close, 1) == WAIT_OBJECT_0) {
			CloseHandle(writeFile);
			close_event_handles(read, write, close);
			return 0;
		}
	}
}

void close_event_handles(HANDLE &e1, HANDLE &e2, HANDLE &e3) {
	CloseHandle(e1);
	CloseHandle(e2);
	CloseHandle(e3);
}