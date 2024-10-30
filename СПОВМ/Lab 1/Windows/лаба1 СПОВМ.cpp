//#define _CRT_SECURE_NO_WARNINGS

//#include "pch.h"
#include <iostream>
#include <stdlib.h>
#include <Windows.h>
#include <string.h>

using namespace std;

int main()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	int number;

	cout << "Enter number raws: ";

	cin >> number;

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
	//cin.ignore(1);

	string str = "Archive_system.exe ";
	str += data;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	if (!CreateProcess(NULL,					// No module name (use command line)
		(char *)str.data(),
		NULL,					// Process handle not inheritable
		NULL,					// Thread handle not inheritable
		FALSE,					// Set handle inheritance to FALSE
		0,		// lol
		NULL,					// Use parent's environment block
		NULL,					// Use parent's starting directory 
		&si,					// Pointer to STARTUPINFO structure
		&pi)					// Pointer to PROCESS_INFORMATION structure
		)
	{
		cout << "CreateProcess failed.\n";
		return -1;
	}

	SYSTEMTIME lt;

	while (WaitForSingleObject(pi.hProcess, 100))
	{
		GetLocalTime(&lt);
		cout << lt.wHour << "." << lt.wMinute << "." << lt.wSecond << "\r";
	}
	
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	
	return 0;
}


