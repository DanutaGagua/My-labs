#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <ncurses.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <chrono>
#include <ctime>
#include <string.h>

using namespace std;

int main()
{
  int st;

int number;

	cout << "Enter number raws: ";

	cin >> number;

	char **data;

	data = (char**)malloc(number*sizeof(char*));

	//data[0] = '\0';

	cin.ignore(1);

	for (int i = 0; i < number; i++)
	{
		char str[200];

		cin.getline(str, 200);
		//cin.ignore(1);

		data[i] = (char*)malloc(200*sizeof(char));

		strcpy(data[i], str); 
	}

  pid_t pid = fork();
  switch (pid) {
    case -1:
      cout << "Error\n";
      break;
    case 0:

      execv("./a", data);
       
      break;

    default:

	//execv("./a", data);
	auto currentTime = chrono::system_clock::now();  
		auto currentTimeT = chrono::system_clock::to_time_t(currentTime); 
		cout << ctime(&currentTimeT) <<'\r';

	while (!(waitpid(pid, &st, WNOHANG) > 0))  
	{

		auto currentTime = chrono::system_clock::now();  
		auto currentTimeT = chrono::system_clock::to_time_t(currentTime); 
		cout << ctime(&currentTimeT) <<'\r';

	}
      break;
  }

  return 0;
}
