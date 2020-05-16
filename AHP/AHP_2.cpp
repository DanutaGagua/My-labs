#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <iostream>
#include <chrono>
#include <malloc.h>

#define OFFSET 1048576
#define BLOCK 1048576 / sizeof(int)
#define MIN_K 2
#define MAX_K 19

using namespace std;

void setIndexes(int* array, int fragment_size, int offset,
	int fragments_count)
{
	int last_fragment = fragments_count - 1;
	int last_element = fragment_size - 1;

	for (int i = 0; i < fragment_size; i++) {
		for (int j = 0; j < fragments_count; j++) {
			array[j * offset + i] = (j + 1) * offset + i;
		}
		array[last_fragment * offset + i] = i + 1;
	}
	array[last_fragment * offset + last_element] = 0;
}

void set_indexes(int* array, int fragment_size, int n)
{
	int element_k = 64 / sizeof(int);
	int block = BLOCK;
	int set_k = fragment_size / element_k;

	for (int i = 0; i < set_k; i++)
		{
			for (int j = 0; j < n; j++)
			{
				for (int k = 0; k < element_k; k++)
				{
					array[j * block + i * element_k + k] = j * block + i * element_k + k + 1;
				}
				array[j * block + (i + 1) * element_k - 1] = (j + 1) * block + i * element_k;
			}
			array[(n - 1) * block + i * element_k + element_k - 1] = (i + 1) * element_k;
		}
	array[(n - 1) * block + fragment_size - 1] = 0;
}

void set_index(int* array, int fragment_size, int n)
{
	int element_k = 64 / sizeof(int);
	int block = BLOCK;
	int set_k = fragment_size / element_k;

	for (int i = 0; i < set_k; i++)
	{
		for (int j = 0; j < n; j++)
		{
			array[j * block + i * element_k] = (j+1) * block + i * element_k;
		}
		array[(n - 1) * block + i * element_k] = (i + 1) * element_k;
	}
	array[(n - 1) * block + (set_k - 1)*element_k] = 0;
}

void work(int kol)
{
	std::chrono::high_resolution_clock clock;

	int integers_count = OFFSET / sizeof(int);
	int offset_int = OFFSET / sizeof(int);
	int element_k = 64 / sizeof(int);
	int block = OFFSET / sizeof(int);
	int set_k = block / element_k;

	int* array = (int*)_aligned_malloc(19*BLOCK*4, 64);
	int index = 0;
	for (int n = MIN_K; n <= MAX_K; n++)
	{
		switch (kol) 
		{
			case 0: setIndexes(array, BLOCK / n, BLOCK, n); break;
			case 1: set_indexes(array, BLOCK / n, n); break;
			case 2: set_index(array, BLOCK / n, n); break;
		}

		auto start = clock.now();

		index = 0;

		for (int i = 0; i < 200000000; i++) {
			index = array[index];
		}
		
		auto end = clock.now();
		 
		cout << n << ": " << (end - start).count() << endl;
	}
	array[index] = 78;
	_aligned_free(array);
}

int main()
{
	for (int i = 0; i < 3; i++)
	{
		work(i);
	}

	system("pause");
	return 0;
}
