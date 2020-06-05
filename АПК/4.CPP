#include <stdio.h>
#include <conio.h>
#include <dos.h>

void generete_sound();
void write_station();
void divider();

int main()
{
	clrscr();

	generete_sound();
	write_station();
	divider();

	getchar();

	return 0;
}

void generete_sound()
{
	int frequency[9] = {196, 261, 329, 196, 261, 329, 196, 261, 329};

	int time = 400;
	int time_clock = 1193180;

	int kd;

	int  i;
	for (i = 0; i < 9; i++)
	{
		kd = time_clock / frequency[i] ;

		outp(0x43, 0xb6);

		outp(0x42,kd%256);
		kd /= 256;
		outp(0x42,kd);

		outp(0x61, (inp(0x61))|0x03);
		delay(time);
		outp(0x61, (inp(0x61))&0xfc);
		delay(2);
	}
}

void write_station()
{
	int port[3] = {0x40, 0x41, 0x42};
	int control_word[3] = {0xe2, 0xe4, 0xe8};

	unsigned char state;

	unsigned char state_str[9];

	int i;

	for (i = 0; i < 3; i++)
	{
		outp(0x43, control_word[i]);
		state = inp(port[i]);

		for(int j = 7; j >= 0; j--)
		{
			state_str[j] = state % 2 + '0';
			state /= 2;
		}
		state_str[8] = '\0';

		printf("Port %x, channel %d: %s\n", i, i, state_str);
	}
}

void divider()
{
	int temp;

	outp(0x43, 0);
	temp = inp(0x40);
	printf("%x\n", temp);

	outp(0x43, 64);
	temp = inp(0x41);
	printf("%x\n", temp);

	outp(0x43, 128);
	temp = inp(0x42);
	printf("%x\n", temp);
}