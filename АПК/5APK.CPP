#include <dos.h>
#include <conio.h>
#include <stdio.h>

void interrupt (*int4A) (...);
void interrupt new_int4A (...)
{
	float kd = 1193180 / 400 ;

		outp(0x43, 0xb6);

		outp(0x42, kd%256);
		kd /= 256;
		outp(0x42,kd);

		outp(0x61, (inp(0x61))|0x03);
		delay(200);
		outp(0x61, (inp(0x61))&0xfc);
		delay(2);	

	printf("Bip!\n");
	int4A();
}

void print(int a);

int  get_data(int a);

void get_time();

void set_time();

void set_clock();

int main()
{
	clrscr();

	get_time();

	set_time();

	set_clock();

	return 0;
}

void get_time()
{
	print(4);
	printf(":");

	print(2);
	printf(":");

	print(0);
	printf("\n");
}

void set_time()
{
	
	int state_1;
 	 //Check update flag
 	 do {
		outp(0x70, 10);
		state_1 = inp(0x71);
 	 } while (state_1 >= 128);

	int result;	

	result = get_data(11);
	outp(0x70, 11);
	result |= 128; 
	outp(0x71,  result);	

	printf("Enter hours: ");
	scanf("%d", &result);
	
	outp(0x70, 4);
	outp(0x71, ((result / 10) << 4) + (result % 10));

	printf("Enter minutes: ");
	scanf("%d", &result);
	
	outp(0x70, 2);
	outp(0x71, ((result / 10) << 4) + (result % 10));

	printf("Enter seconds: ");
	scanf("%d", &result);
	
	outp(0x70, 0);
	outp(0x71, ((result / 10) << 4) + (result % 10));

	delay(2000);

	get_time();

	delay(2000);

	char state_str[9];
	int state = get_data(11);

	for(int j = 7; j >= 0; j--)
	{
		state_str[j] = state % 2 + '0';
		state /= 2;
	}
	state_str[8] = '\0';

	printf("%s\n", state_str);

	result = get_data(11);
	outp(0x70, 11);
	result &= 127; 
	outp(0x71,  result);
}

void print(int a)
{
	int result;

	result = get_data(a);

	printf("%d", (result >> 4) * 10 + (result & 0xF));
}

int  get_data(int a)
{
	outp(0x70, a);
	return inp(0x71);
}

void set_clock()
{
	int result;

	_disable();

	int4A = getvect(0x4A);
	
	setvect(0x4A, new_int4A);

	result = get_data(11);
	outp(0x70, 11);
	result |= 0x22; 
	outp(0x71,  result);

	result = get_data(4);
	outp(0x70, 5);
	outp(0x71, result);

	result = get_data(2);
	outp(0x70, 3);
	outp(0x71, result);

	result = get_data(0);
	outp(0x70, 1);
	outp(0x71, result + 8);
	
	char state_str[9];
	int state = get_data(11);

	for(int j = 7; j >= 0; j--)
	{
		state_str[j] = state % 2 + '0';
		state /= 2;
	}
	state_str[8] = '\0';

	printf("%s\n", state_str);

	_enable();

	print(5);
	printf(":");

	print(3);
	printf(":");

	print(1);
	printf("\n");

	delay(10000);

	_disable();

	setvect(0x4A, int4A);

	_enable();	
}