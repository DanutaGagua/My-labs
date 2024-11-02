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

int flag = 0;

void delete_process(vector<STARTUPINFO> &si, vector<PROCESS_INFORMATION> &pi, vector<HANDLE> &close, vector<HANDLE> &print);
bool create_process(char *path, vector<STARTUPINFO> &si, vector<PROCESS_INFORMATION> &pi, vector<HANDLE> &close, vector<HANDLE> &print, HANDLE server);

void man(char *program, char *id);
void parent(char *path);
void archive(int argc, char *argv[]);

void process_enter(char *path, vector<STARTUPINFO> &si, vector<PROCESS_INFORMATION> &pi, vector<HANDLE> &close, vector<HANDLE> &print, HANDLE server);

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
	HANDLE reset = CreateEvent(NULL, FALSE, FALSE, "Reset");
	HANDLE server = CreateEvent(NULL, TRUE, FALSE, "Server");

	srand(time(0));

	while (1)	{
		if (_kbhit())	{
			process_enter(path, si, pi, close, print, server);
		}

		if (flag) {
			CloseHandle(server);
			CloseHandle(reset);

			return;
		}

		system("CLS");

		for (int i = 0; i < print.size(); i++)		{
			SetEvent(print[i]);
			
			while (WaitForSingleObject(reset, 1) == WAIT_TIMEOUT)			{
				if (_kbhit())	{
					process_enter(path, si, pi, close, print, server);

					if (flag)	{
						CloseHandle(server);
						CloseHandle(reset);

						return;
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

void man(char *program, char *id) {
	string str_id = "Child " + (string)id;
	HANDLE child = CreateEvent(NULL, FALSE, FALSE, (char*)str_id.data());
	str_id = "Print " + (string)id;
	HANDLE print = OpenEvent(EVENT_ALL_ACCESS, FALSE, (char*)str_id.data());
	str_id = "Close " + (string)id;
	HANDLE close = OpenEvent(EVENT_ALL_ACCESS, FALSE, (char*)str_id.data());

	HANDLE reset = OpenEvent(EVENT_ALL_ACCESS, FALSE, "Reset");
	HANDLE server = OpenEvent(EVENT_ALL_ACCESS, TRUE, "Server");

	STARTUPINFO  si;
	PROCESS_INFORMATION  pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	while (WaitForSingleObject(print, 1) == WAIT_TIMEOUT);

	SetEvent(server);

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

	for (int i = 0; i < number; i++)
	{
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

		CloseHandle(print);
		CloseHandle(close);
		CloseHandle(child);
		CloseHandle(reset);
		CloseHandle(server);
	}

	ResetEvent(print);
	ResetEvent(server);
	SetEvent(reset);

	char buf[255];
	sprintf_s(buf, "Dark Alive Zombie %s\n\0", id);

	while (1)	{
		char str[2];

		if (WaitForSingleObject(close, 1) == WAIT_OBJECT_0)		{
			CloseHandle(print);
			CloseHandle(close);
			CloseHandle(reset);
			CloseHandle(server);

			SetEvent(child);

			WaitForSingleObject(pi.hProcess, INFINITE);

			CloseHandle(child);
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);

			return;
		}

		if (WaitForSingleObject(print, 1) == WAIT_OBJECT_0) {
			SetEvent(server);
			for (int i = 0; i < strlen(buf); i++)
			{
				if (WaitForSingleObject(close, 1) == WAIT_OBJECT_0)
				{
					CloseHandle(print);
					CloseHandle(close);
					CloseHandle(reset);
					CloseHandle(server);

					SetEvent(child);

					WaitForSingleObject(pi.hProcess, INFINITE);

					CloseHandle(child);
					CloseHandle(pi.hThread);
					CloseHandle(pi.hProcess);

					return;
				}
				printf("%c", buf[i]);
				Sleep(50);
			}
			ResetEvent(print);
			ResetEvent(server);
			SetEvent(reset);
		}
	}
}

void archive(int argc, char *argv[]) {
	string str_id = "Child " + (string)argv[1];
	HANDLE child = OpenEvent(EVENT_ALL_ACCESS, FALSE, (char*)str_id.data());

	if (argc < 4){
		cout << endl << "Not enough arguments." << endl;

		while (1) {
			if (WaitForSingleObject(child, 1) == WAIT_OBJECT_0) {
				break;
			}

			if (_kbhit()) {
				break;
			}
		}

		CloseHandle(child);

		return;
	}

	fstream file;

	file.open("input.txt", ios::in);

	if (!file)	{
		cout << "File doesn't exist.\n";

		while (1) {
			if (WaitForSingleObject(child, 1) == WAIT_OBJECT_0) {
				break;
			}

			if (_kbhit()) {
				break;
			}
		}

		CloseHandle(child);

		return;
	}

	int raws = 0;
	vector <string> data;

	while (!file.eof())
	{
		char str[1000];

		file.getline(str, 1000);

		data.push_back(string(str));

		//cout << str << endl;

		raws++;
	}

	for (int j = 1; j < argc; j++)
	{
		int flag = 1;

		for (vector<string>::iterator i = data.begin(); i != data.end(); i++)
		{
			//cout << *i << endl;

			if ((*i).find(argv[j]) != string::npos)
			{
				cout << flag << " " << (*i) << endl;
			}

			flag++;
		}
	}

	file.close();

	while (1) {
		if (WaitForSingleObject(child, 1) == WAIT_OBJECT_0) {
			break;
		}

		if (_kbhit()) {
			break;
		}
	}

	CloseHandle(child);
	
	return;
}

void delete_process(vector<STARTUPINFO> &si, vector<PROCESS_INFORMATION> &pi, vector<HANDLE> &close, vector<HANDLE> &print)
{
	STARTUPINFO sinf = si.back();
	PROCESS_INFORMATION pinf = pi.back();

	HANDLE c = close.back();
	HANDLE p = print.back();

	SetEvent(c);

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

bool create_process(char *path, vector<STARTUPINFO> &si, vector<PROCESS_INFORMATION> &pi, vector<HANDLE> &close, vector<HANDLE> &print, HANDLE server)
{
	STARTUPINFO sinfo;
	PROCESS_INFORMATION pinfo;
	HANDLE close_event, print_event;

	HANDLE reset = OpenEvent(EVENT_ALL_ACCESS, FALSE, "Reset");

	ZeroMemory(&sinfo, sizeof(sinfo));
	sinfo.cb = sizeof(sinfo);
	ZeroMemory(&pinfo, sizeof(pinfo));

	string str, str_id;
	int id;

	stringstream buffer;

	id = rand();

	buffer << id;
	buffer >> str_id;

	str = "Close " + str_id;
	close_event = CreateEvent(NULL, FALSE, FALSE, (char*)str.data());

	str = "Print " + str_id;
	print_event = CreateEvent(NULL, TRUE, FALSE, (char*)str.data());

	str = (string)path + " " + str_id;

	if (!CreateProcess(path,					// No module name (use command line)
		(char*)str.data(),
		NULL,					// Process handle not inheritable
		NULL,					// Thread handle not inheritable
		FALSE,					// Set handle inheritance to FALSE
		0,		// lol
		NULL,					// Use parent's environment block
		NULL,					// Use parent's starting directory 
		&sinfo,					// Pointer to STARTUPINFO structure
		&pinfo)					// Pointer to PROCESS_INFORMATION structure
		)
	{
		printf_s("CreateProcess failed. \n");

		getchar();

		return false;
	}

	while (WaitForSingleObject(server, 1) == WAIT_OBJECT_0);

	SetEvent(print_event);

	while (WaitForSingleObject(reset, 1) == WAIT_TIMEOUT);

	si.push_back(sinfo);
	pi.push_back(pinfo);
	close.push_back(close_event);
	print.push_back(print_event);

	return true;
}

void process_enter(char *path, vector<STARTUPINFO> &si, vector<PROCESS_INFORMATION> &pi, vector<HANDLE> &close, vector<HANDLE> &print, HANDLE server)
{
	char symbol;
	symbol = _getch();

	switch (symbol) {
	case 'a':
		if (!create_process(path, si, pi, close, print, server)) {
			return;
		};
		//cout << "Created" << endl;

		break;

	case '-':
		if (si.empty()) {
			break;
		}

		delete_process(si, pi, close, print);

		//cout << "Deleted" << endl;

		break;

	case 'q':
		while (!si.empty()) {
			delete_process(si, pi, close, print);
		}

		flag = 1;

		return;
	}
}