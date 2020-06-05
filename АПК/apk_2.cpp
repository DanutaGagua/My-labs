#include <iostream>
#include <math.h>
#include <time.h>
#include <Windows.h>
#include <time.h>

using namespace std;

void copy_on_C(short *a_1, short *a_2, unsigned int size);
void copy_on_ASM(short *a_1, short *a_2, unsigned int size);
void copy_on_vector_ASM(short *a_1, short *a_2, unsigned int size);

bool is_equal(short* a, short* b, unsigned int size);

int main()
{
	unsigned int n, m, start, end, size;
	srand(time(NULL));

	cout << "Enter n, m." << endl;

	cin >> n >> m;
	size = n * m;

	short *a_1 = (short*)calloc(n*m, sizeof(short));
	short *a_2 = (short*)calloc(n*m, sizeof(short));
	short *a_3 = (short*)calloc(n*m, sizeof(short));
	short *a_4 = (short*)calloc(n*m, sizeof(short));

	for (unsigned int i = 0; i < n*m; i++)
	{
		a_1[i] = rand();
	}

	SYSTEMTIME lt;

	GetLocalTime(&lt);
	start = lt.wSecond * 1000 + lt.wMilliseconds;
	
	copy_on_C(a_1, a_2, size);

	GetLocalTime(&lt);
	end = lt.wSecond * 1000 + lt.wMilliseconds;

	cout << is_equal(a_1, a_2, size) << " " << end - start << endl;

	GetLocalTime(&lt);
	start = lt.wSecond * 1000 + lt.wMilliseconds;

	copy_on_ASM(a_1, a_3, size);

	GetLocalTime(&lt);
	end = lt.wSecond * 1000 + lt.wMilliseconds;

	cout << is_equal(a_1, a_3, size) << " " << end - start << endl;

	GetLocalTime(&lt);
	start = lt.wSecond * 1000 + lt.wMilliseconds;

	copy_on_vector_ASM(a_1, a_4, size);

	GetLocalTime(&lt);
	end = lt.wSecond * 1000 + lt.wMilliseconds;

	cout << is_equal(a_1, a_4, size) << " " << end - start << endl;

	free(a_1);
	free(a_2);
	free(a_3);
	free(a_4);

	system("pause");
}

void copy_on_C(short *a_1, short *a_2, unsigned int size)
{
	for (unsigned int i = 0; i <= size; i++)
	{
		a_2[i] = a_1[i];
	}
}

void copy_on_ASM(short *a_1, short *a_2, unsigned int size)
{
	unsigned int s = size;
	
	_asm
	{
		mov edi, a_1
		mov esi, a_2

		mov ecx, dword ptr s

		cycle :

			mov edx, [edi]
			mov [esi], edx

			add edi, 2
			add esi, 2

			dec ecx
			cmp ecx, 0

			jne cycle
	}
}

void copy_on_vector_ASM(short *a_1, short *a_2, unsigned int size)
{
	unsigned int s = size / 8;

	_asm
	{
		mov edi, a_1
		mov esi, a_2

		mov ecx, dword ptr s

		cycle:

			movw mm0, [edi]
			movw [esi], mm0

			add edi, 8
			add esi, 8

			dec ecx
			cmp ecx, 0

		jne cycle

		emms
	}
}

bool is_equal(short* a, short* b, unsigned int size)
{
	for (unsigned int i = 0; i < size; i++)
	{
		if (abs(a[i] != b[i]))
		{
			return false;
		}
	}

	return true;
}