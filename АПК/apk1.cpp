#include <iostream>
#include <math.h>
#include <time.h>
#include <Windows.h>

using namespace std;

void calculate_on_C(int x);
void calculate_on_ASM(int x);

int main()
{ 
	int a, b, d, start, end;
	
	cout << "Enter a, b, d." << endl;
	
	cin >> a >> b >> d;

	SYSTEMTIME lt;

	GetLocalTime(&lt); 
	start = lt.wSecond * 1000 + lt.wMilliseconds;

	for (int i = a; i <= b; i += d)
	{
		calculate_on_C(i);
	} 

	GetLocalTime(&lt); 
	end = lt.wSecond * 1000 + lt.wMilliseconds;

	cout << end - start << endl;

	GetLocalTime(&lt);
	start = lt.wSecond * 1000 + lt.wMilliseconds;

	for (int i = a; i <= b; i += d)
	{
		calculate_on_ASM(i);
	}

	GetLocalTime(&lt);
	end = lt.wSecond * 1000 + lt.wMilliseconds;

	cout << end - start << endl;

	system("pause");
}

void calculate_on_C(int x)
{  
	double result;
	
	if (x != 1)
	{
		result = (x*x + x) * 1.0 / abs(x - 1);
	}
}

void calculate_on_ASM(int x)
{
	double result = x;

	if (x == 1) return;

	_asm
	{
		finit
		
		fld result

		fmul st(0), st(0);  

		fld result

		fadd    

		fld result

		fld1

		fsub

		fdiv 

		fstp result
	} 
}
