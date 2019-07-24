#include <iostream>
using namespace std;

float find_max(float array[]);
float delete_array_max(float array[], int index, float max);

int main()
{
	float array[10];

	cout << "Enter 10 double numbers\n";

	for (int i = 0; i < 10; i++)
	{
		char buf[1000];

		char flag = 0;

		do
		{
			cout << "Enter " << i << " element\n";

			flag = 0;
			
			cin >> array[i];

			if (cin.fail())
			{
				cout << "Error. Enter number\n ";
				cin.clear();
				cin.getline(buf, 1000);
				cin.ignore(1);

				flag = 1;
			}
		} while (flag);
	}

	float max = find_max(array);
	 
	cout << "Max element " << max << endl;

	for (int i = 0; i < 10; i++)
	{
		cout << delete_array_max(array, i, max) << " ";
	}

	return 0;
}

float find_max(float array[])
{
	float max = array[0];
	
	_asm
	{

		finit

		mov ecx, 9

		mov edi, array

		add edi, 4

		find_:

			fld [edi]

			fld max

			fcomp

			fstsw ax

			fwait

			and ah, 01000101b

			jle if_greater

			fstp max
			jmp next_for

			if_greater:

				ffree st(0)

			next_for :

				dec ecx
				add edi, 4
				cmp ecx, 0

		jne find_
	}

	return max;
}

float delete_array_max(float array[], int index, float max)
{
	float value;

	_asm
	{
		finit

		mov edi, array

		mov eax, index
		mov edx, 4

		mul edx

		add edi, eax

		fld [edi]

		fld max

		fdiv

		fstp value
	}

	return value;
}