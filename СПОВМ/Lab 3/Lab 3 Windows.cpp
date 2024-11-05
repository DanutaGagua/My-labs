//#ifdef _WIN32 | _WIN64
#include "pch.h"
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

using namespace std;

int flag = 0, add = 0;

void delete_process(vector<STARTUPINFO> &si, vector<PROCESS_INFORMATION> &pi, vector<HANDLE> &close, vector<HANDLE> &print);
bool create_process(bool flag, char *path, vector<STARTUPINFO> &si, vector<PROCESS_INFORMATION> &pi, vector<HANDLE> &close,
	vector<HANDLE> &print, HANDLE &write);
void print_from_buff();
void create_buff();
void print_in_buff(char symbol);

void close_parent_handles(HANDLE &reset, HANDLE &write);
void close_man(PROCESS_INFORMATION  &pi, HANDLE &print, HANDLE &close, HANDLE &child, HANDLE &reset);
void close_man_handles(HANDLE &print, HANDLE &close, HANDLE &child, HANDLE &reset);
void child_wait_close(HANDLE &child);

void man(char *program, char *id);
void parent(char *path);
void archive(int argc, char *argv[]);

void process_enter(bool flag1, char *path, vector<STARTUPINFO> &si, vector<PROCESS_INFORMATION> &pi, vector<HANDLE> &close,
	vector<HANDLE> &print, HANDLE &write);

int main(int argc, char* argv[]){
	switch (argc) {
		case 1: parent(argv[0]);  break;
		case 2: man(argv[0], argv[1]); break;	
		default: archive(argc, argv); break;
	}

	return 0;
}

void parent(char *path){
	vector<STARTUPINFO> si;
	vector<PROCESS_INFORMATION> pi;
	vector<HANDLE> print, close;

	HANDLE reset = CreateSemaphore(NULL, 0, 1, "Reset");
	HANDLE write = CreateSemaphore(NULL, 0, 1, "Write");

	create_buff();

	srand(time(0));

	while (1)	{
		add = 0;

		if (_kbhit())	{
			process_enter(FALSE, path, si, pi, close, print, write);
			add = 0;
		}

		if (flag) {
			close_parent_handles(reset, write);
			return;
		}

		system("CLS");

		for (int i = 0; i < print.size(); i++)		{
			ReleaseSemaphore(print[i], 1, NULL);			
			
			while (WaitForSingleObject(reset, 1) == WAIT_TIMEOUT)			{
				if (WaitForSingleObject(write, 1) == WAIT_OBJECT_0)	{
					print_from_buff();
				}
				
				if (_kbhit())	{
					process_enter(TRUE, path, si, pi, close, print, write);

					if (flag)	{
						close_parent_handles(reset, write);
						return;
					}

					if (add) {
						add = 0;
						break;
					}
				}				

				if (i >= print.size())	{
					break;
				}				
			};

			if (i >= print.size())	{
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

void man(char *program, char *id) {
	HANDLE child = CreateSemaphore(NULL, 0, 1, (char*)("Child " + (string)id).data());
	HANDLE print = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, (char*)("Print " + (string)id).data());
	HANDLE close = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, (char*)("Close " + (string)id).data());

	HANDLE reset = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "Reset");
	HANDLE write = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "Write");

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
	}	

	char data[200];

	data[0] = '\0';

	cin.ignore(1);

	for (int i = 0; i < number; i++)	{
		char str[200];

		cin.getline(str, 200);
		//cin.ignore(1);

		strcat_s(data, 200, str);
		strcat_s(data, 200, " ");
	}

	string str = (string)program + " " + (string)id + " " + data;

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
		)	{
		printf_s("CreateProcess failed.\n");

		close_man_handles(print, close, child, reset);
	}

	ReleaseSemaphore(reset, 1, NULL);

	sprintf_s(buf, "Dark Alive Zombie %s\n\0", id);

	while (1)	{
		char str[2];

		if (WaitForSingleObject(close, 1) == WAIT_OBJECT_0)		{
			close_man(pi, print, close, child, reset);
			return;
		}

		if (WaitForSingleObject(print, 1) == WAIT_OBJECT_0) {
			for (int i = 0; i < strlen(buf); i++)	{
				if (WaitForSingleObject(close, 1) == WAIT_OBJECT_0)		{			
					close_man(pi, print, close, child, reset);
					return;
				}				

				print_in_buff(buf[i]);
				ReleaseSemaphore(write, 1, NULL);
				Sleep(50);
			}
			ReleaseSemaphore(reset, 1, NULL);
		}
	}
}

void close_man(PROCESS_INFORMATION  &pi, HANDLE &print, HANDLE &close, HANDLE &child, HANDLE &reset) {
	ReleaseSemaphore(child, 1, NULL);
	WaitForSingleObject(pi.hProcess, INFINITE);

	close_man_handles(print, close, child, reset);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
}

void close_man_handles(HANDLE &print, HANDLE &close, HANDLE &child, HANDLE &reset) {
	CloseHandle(print);
	CloseHandle(close);
	CloseHandle(child);
	CloseHandle(reset);
}

void archive(int argc, char *argv[]) {
	string str_id = "Child " + (string)argv[1];
	HANDLE child = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, (char*)str_id.data());

	if (argc < 4){
		cout << endl << "Not enough arguments." << endl;
		child_wait_close(child);
		return;
	}

	fstream file;
	file.open("input.txt", ios::in);

	if (!file)	{
		cout << "File doesn't exist.\n";
		child_wait_close(child);
		return;
	}

	vector <string> data;

	while (!file.eof())	{
		char str[1000];

		file.getline(str, 1000);
		data.push_back(string(str));
	}

	for (int j = 1; j < argc; j++)	{
		int flag = 1;

		for (vector<string>::iterator i = data.begin(); i != data.end(); i++)	{
			if ((*i).find(argv[j]) != string::npos)	{
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

void delete_process(vector<STARTUPINFO> &si, vector<PROCESS_INFORMATION> &pi, vector<HANDLE> &close, 
	vector<HANDLE> &print)
{
	STARTUPINFO sinf = si.back();
	PROCESS_INFORMATION pinf = pi.back();

	HANDLE c = close.back();
	HANDLE p = print.back();

	ReleaseSemaphore(close.back(), 1, NULL);

	si.pop_back();
	pi.pop_back();
	close.pop_back();
	print.pop_back();

	WaitForSingleObject(pinf.hProcess, INFINITE);

	CloseHandle(c);
	CloseHandle(p);

	CloseHandle(pinf.hThread);
	CloseHandle(pinf.hProcess);

	return;
}

bool create_process(bool flag, char *path, vector<STARTUPINFO> &si, vector<PROCESS_INFORMATION> &pi, vector<HANDLE> &close,
	vector<HANDLE> &print, HANDLE &write)
{
	STARTUPINFO sinfo;
	PROCESS_INFORMATION pinfo;
	HANDLE print_semaphore, close_semaphore;

	HANDLE reset = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "Reset");

	ZeroMemory(&sinfo, sizeof(sinfo));
	sinfo.cb = sizeof(sinfo);
	ZeroMemory(&pinfo, sizeof(pinfo));

	string str_id;
	stringstream buffer;

	buffer << rand();
	buffer >> str_id;

	close_semaphore = CreateSemaphore(NULL, 0, 1, (char*)("Close " + str_id).data());
	print_semaphore = CreateSemaphore(NULL, 0, 1, (char*)("Print " + str_id).data());

	if (!CreateProcess(path,					// No module name (use command line)
		(char*)((string)path + " " + str_id).data(),
		NULL,					// Process handle not inheritable
		NULL,					// Thread handle not inheritable
		FALSE,					// Set handle inheritance to FALSE
		0,		// lol
		NULL,					// Use parent's environment block
		NULL,					// Use parent's starting directory 
		&sinfo,					// Pointer to STARTUPINFO structure
		&pinfo)					// Pointer to PROCESS_INFORMATION structure
		)	{
		printf_s("CreateProcess failed. \n");

		getchar();
		return false;
	}

	if (flag) {
		while (WaitForSingleObject(reset, 1) == WAIT_TIMEOUT)	{
			if (WaitForSingleObject(write, 1) == WAIT_OBJECT_0) {
				print_from_buff();
			}
		};
	}

	ReleaseSemaphore(print_semaphore, 1, NULL);

	while (WaitForSingleObject(reset, 1) == WAIT_TIMEOUT);

	si.push_back(sinfo);
	pi.push_back(pinfo);
	close.push_back(close_semaphore);
	print.push_back(print_semaphore);

	return true;
}

void process_enter(bool flag1, char *path, vector<STARTUPINFO> &si, vector<PROCESS_INFORMATION> &pi, vector<HANDLE> &close,
	vector<HANDLE> &print, HANDLE &write)
{
	char symbol;
	symbol = _getch();

	switch (symbol) {
	case 'a':
		if (!create_process(flag1, path, si, pi, close, print, write)) {
			return;
		};
		add = 1;
		break;

	case '-':
		if (si.empty()) {
			break;
		}
		delete_process(si, pi, close, print);
		break;

	case 'q':
		while (!si.empty()) {
			delete_process(si, pi, close, print);
		}
		flag = 1;
		return;
	}
}