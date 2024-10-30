#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

int main(int argc, char *argv[])
{	
	if (argc < 2)
	{
		cout << "Not enough arguments." << endl;

		return -2;
	}
	
	fstream file;

	file.open("input.txt", ios::in);

	if (!file)
	{
		cout << "File doesn't exist." << endl;

		return -1;
	}

	int raws = 0;
	vector <string> data;

	while (!file.eof())
	{
		char str[1000];

		file.getline(str, 1000);

		data.push_back(string(str));

		raws++;
	}
	
	for (int j = 0; j < argc; j++)
	{
		int flag = 1;

		for (vector<string>::iterator i = data.begin(); i != data.end(); i++)
		{
			if ((*i).find(argv[j]) != string::npos)
			{
				cout << flag << " " << (*i) << endl;
			}
			
			flag++;
		}
	}

	file.close();

getchar();

	//system("pause");

	return 0;
}
