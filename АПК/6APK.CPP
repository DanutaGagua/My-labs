#include <dos.h>
#include <conio.h>
#include <stdio.h>

int flag = 0;

void interrupt (*int9) (...);

void interrupt new_int9 (...)
{
	int result; 

	result = inp(0x60);

	printf("%x   ", result);
 
	int9(); 

} 

void wait_to_ready(); 
void light();

int main()
{
	clrscr();

	_disable(); 
	int9 = getvect(0x09); 
	setvect(0x09, new_int9);  
	_enable();  

	light();

	while(!flag);

	_disable(); 
	setvect(0x09, int9); 
	_enable();	

	return 0;
} 

void wait_to_ready()
{
	int result;

	do
	{
		result = inp(0x64);
		result &= 0x02;
	}
	while (result != 0x00);

	return; 
}

void light()
{ 
	wait_to_ready();

	outp(0x60, 0xed);

	wait_to_ready();

	outp(0x60, 0xff);

	delay(2000);

	wait_to_ready();

	outp(0x60, 0x00);
	
	return;
}