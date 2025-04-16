#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <windows.h>
#include <string>
#include <vector>
#include <conio.h>
#include <time.h>
#include <sstream>
#include <fstream>
#include <typeinfo>

using namespace std;

int flag = 0, add = 0, read = 0;

bool create_thread(bool flag, char *path, vector<HANDLE> &threads, vector<HANDLE> &close,
	vector<HANDLE> &print, HANDLE &write, HANDLE &reset);
void delete_thread(vector<HANDLE> &threads, vector<HANDLE> &close, vector<HANDLE> &print);

void print_from_buff();
void create_buff();
void print_in_buff(char symbol);

void close_parent_handles(HANDLE &reset, HANDLE &write);
void close_man(PROCESS_INFORMATION  &pi, HANDLE &print, HANDLE &close, HANDLE &child);
void close_man_handles(HANDLE &print, HANDLE &close, HANDLE &child);
void child_wait_close(HANDLE &child);

DWORD WINAPI man(LPVOID args);
void parent(char *path);
void archive(int argc, char *argv[]);

void process_enter(bool flag1, char *path, vector<HANDLE> &threads, vector<HANDLE> &close,
	vector<HANDLE> &print, HANDLE &write, HANDLE &reset);

typedef struct MyData {
	string program;
	string id;
} MYDATA, *PMYDATA;

int main(int argc, char* argv[]) {
	switch (argc) {
		case 1: parent(argv[0]);  break;
		default: archive(argc, argv); break;
	}

	return 0;
}

void parent(char *path) {
	vector<HANDLE> threads, print, close;

	HANDLE reset = CreateMutex(NULL, TRUE, "Reset");
	HANDLE write = CreateMutex(NULL, TRUE, "Write");

	create_buff();

	srand(time(0));

	while (1) {
		add = 0;

		if (_kbhit()) {
			process_enter(FALSE, path, threads, close, print, write, reset);
			add = 0;
		}

		if (flag) {
			close_parent_handles(reset, write);
			return;
		}

		system("CLS");

		for (int i = 0; i < print.size(); i++) {
			ReleaseMutex(print[i]);

			cout << i << " ";
			Sleep(5);

			while (WaitForSingleObject(print[i], 1) == WAIT_TIMEOUT) {	
				if (WaitForSingleObject(write, 1) == WAIT_OBJECT_0 && read) {
					print_from_buff();
					read = 0;
				}

				if (_kbhit()) {
					process_enter(TRUE, path, threads, close, print, write, print[i]);

					if (flag) {
						close_parent_handles(reset, write);
						return;
					}

					if (add) {
						add = 0;
						break;
					}
				}

				if (i >= print.size()) {
					break;
				}
			};

			Sleep(5);

			if (i >= print.size()) {
				break;
			}
		}

		Sleep(1500);
	}

	return;
}

void close_parent_handles(HANDLE &reset, HANDLE &write) {
	CloseHandle(reset);
	CloseHandle(write);
}

DWORD WINAPI man(LPVOID args) {
	string id = ((PMYDATA)args)->id;

	HANDLE child = CreateSemaphore(NULL, 0, 1, (char*)("Child " + id).data());
	HANDLE print = OpenMutex(SYNCHRONIZE, FALSE, (char*)("Print " + id).data());
	HANDLE close = OpenMutex(SYNCHRONIZE, FALSE, (char*)("Close " + id).data());

	HANDLE write = OpenMutex(SYNCHRONIZE, FALSE, "Write");

	char buf[255];

	STARTUPINFO  si;
	PROCESS_INFORMATION  pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	while (WaitForSingleObject(print, 1) == WAIT_TIMEOUT);

	int number;

	while (1) {
		cout << "Enter number raws (min 2): ";

		cin >> number;
		if (number > 1) {
			break;
		}

		cin.clear();
		cin.ignore(1000, '\n');
	}

	char data[200];

	data[0] = '\0';

	cin.ignore(1);

	for (int i = 0; i < number; i++) {
		char str[200];

		cin.getline(str, 200);

		strcat_s(data, 200, str);
		strcat_s(data, 200, " ");
	}

	string str = ((PMYDATA)args)->program + " " + id + " " + data;

	if (!CreateProcess(NULL,					// No module name (use command line)
		(char*)str.data(),
		NULL,					// Process handle not inheritable
		NULL,					// Thread handle not inheritable
		FALSE,					// Set handle inheritance to FALSE
		CREATE_NEW_CONSOLE,		// lol
		NULL,					// Use parent's environment block
		NULL,					// Use parent's starting directory
		&si,					// Pointer to STARTUPINFO structure
		&pi)					// Pointer to PROCESS_INFORMATION structure
		) {
		printf_s("CreateProcess failed.\n");

		close_man_handles(print, close, child);
	}

	ReleaseMutex(print);

	sprintf_s(buf, "Dark Alive Zombie %s\n\0", (char*)id.data());

	while (1) {
		char str[2];

		if (WaitForSingleObject(close, 1) == WAIT_OBJECT_0) {
			close_man(pi, print, close, child);
			return 0;
		}

		if (WaitForSingleObject(print, 1) == WAIT_OBJECT_0) {
			for (int i = 0; i < strlen(buf); i++) {
				if (WaitForSingleObject(close, 1) == WAIT_OBJECT_0) {
					close_man(pi, print, close, child);
					return 0;
				}

				print_in_buff(buf[i]);	
				read = 1;
				ReleaseMutex(write);
				Sleep(50);
			
			}
			ReleaseMutex(print);
			Sleep(5);
		}
	}
}

void close_man(PROCESS_INFORMATION  &pi, HANDLE &print, HANDLE &close, HANDLE &child) {
	ReleaseSemaphore(child, 1, NULL);
	WaitForSingleObject(pi.hProcess, INFINITE);

	close_man_handles(print, close, child);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
}

void close_man_handles(HANDLE &print, HANDLE &close, HANDLE &child) {
	CloseHandle(print);
	CloseHandle(close);
	CloseHandle(child);
}

void archive(int argc, char *argv[]) {
	string str_id = "Child " + (string)argv[1];
	HANDLE child = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, (char*)str_id.data());

	if (argc < 4) {
		cout << endl << "Not enough arguments." << endl;
		child_wait_close(child);
		return;
	}

	fstream file;
	file.open("input.txt", ios::in);

	if (!file) {
		cout << "File doesn't exist.\n";
		child_wait_close(child);
		return;
	}

	vector <string> data;

	while (!file.eof()) {
		char str[1000];

		file.getline(str, 1000);
		data.push_back(string(str));
	}

	for (int j = 1; j < argc; j++) {
		int flag = 1;

		for (vector<string>::iterator i = data.begin(); i != data.end(); i++) {
			if ((*i).find(argv[j]) != string::npos) {
				cout << flag << " " << (*i) << endl;
			}

			flag++;
		}
	}

	file.close();
	child_wait_close(child);
}

void child_wait_close(HANDLE &child) {
	while (1) {
		if (WaitForSingleObject(child, 1) == WAIT_OBJECT_0) {
			break;
		}

		if (_kbhit()) {
			break;
		}
	}

	CloseHandle(child);
}

void print_in_buff(char symbol) {
	char sym[2] = { '\0', '\0' }, empty[2];

	memset(empty, '\0', 2);

	HANDLE conn = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "FileMap");
	LPVOID buff = MapViewOfFile(conn, FILE_MAP_ALL_ACCESS, 0, 0, 2);

	sym[0] = symbol;

	CopyMemory((PVOID)buff, empty, sizeof(empty));

	CopyMemory((PVOID)buff, sym, 2);
}

void print_from_buff() {
	HANDLE conn = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "FileMap");
	LPVOID buff = MapViewOfFile(conn, FILE_MAP_ALL_ACCESS, 0, 0, 2);

	char str[2];
	memset(str, '\0', 2);
	strncpy_s(str, (char*)buff, 2);
	printf_s("%c", str[0]);

	CloseHandle(conn);
}

void create_buff() {
	HANDLE conn = CreateFileMapping(INVALID_HANDLE_VALUE,
		NULL, PAGE_READWRITE,
		0, 2, "FileMap");

	LPVOID buff = MapViewOfFile(conn, FILE_MAP_ALL_ACCESS, 0, 0, 2);
}

void delete_thread(vector<HANDLE> &threads, vector<HANDLE> &close,	vector<HANDLE> &print)
{
	HANDLE thread = threads.back();

	HANDLE c = close.back();
	HANDLE p = print.back();

	ReleaseMutex(close.back());

	threads.pop_back();
	close.pop_back();
	print.pop_back();

	WaitForSingleObject(thread, INFINITE);

	CloseHandle(c);
	CloseHandle(p);

	CloseHandle(thread);

	return;
}

bool create_thread(bool flag, char *path, vector<HANDLE> &threads, vector<HANDLE> &close,
	vector<HANDLE> &print, HANDLE &write, HANDLE &reset)
{	
	HANDLE print_mutex, close_mutex;

	string str_id;
	stringstream buffer;

	buffer << rand();
	buffer >> str_id;

	close_mutex = CreateMutex(NULL, TRUE, (char*)("Close " + str_id).data());
	print_mutex = CreateMutex(NULL, TRUE, (char*)("Print " + str_id).data());

	PMYDATA args = (PMYDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MYDATA));
	args->program = (string)path;
	args->id = (string)str_id;

	HANDLE descr = CreateThread(
		NULL,					// default security attributes
		0,						 // use default stack size  
		man,					 // thread function name
		args,					 // argument to thread function 
		0,						  // use default creation flags 
		NULL);					// doesn't return the thread identifier 
	if (!descr) {
		printf("Create Handle failed (%d)\n", GetLastError());

		getchar();
		return false;
	}

	if (flag) {
		while (WaitForSingleObject(reset, 1) == WAIT_TIMEOUT) {
			if (WaitForSingleObject(write, 1) == WAIT_OBJECT_0 && read) {
				print_from_buff();
				read = 0;
			}
		};
	}

	ReleaseMutex(print_mutex);
	Sleep(5);

	while (WaitForSingleObject(print_mutex, 1) == WAIT_TIMEOUT);

	threads.push_back(descr);
	close.push_back(close_mutex);
	print.push_back(print_mutex);

	return true;
}

void process_enter(bool flag1, char *path, vector<HANDLE> &threads, vector<HANDLE> &close,
	vector<HANDLE> &print, HANDLE &write, HANDLE &reset)
{
	char symbol;
	symbol = _getch();

	switch (symbol) {
	case 'a':
		if (!create_thread(flag1, path, threads, close, print, write, reset)) {
			return;
		};
		add = 1;
		break;

	case '-':
		if (threads.empty()) {
			break;
		}
		delete_thread(threads, close, print);
		break;

	case 'q':
		while (!threads.empty()) {
			delete_thread(threads, close, print);
		}
		flag = 1;
		return;
	}
}