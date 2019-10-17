#include <iostream>
#include <iomanip>
#include <Windows.h>
#include <WinIoCtl.h>
#include <ntddscsi.h>
#include <conio.h>
#include <powrprof.h>

#pragma comment (lib, "powrprof.lib")

using namespace std;

int main() 
{
	SYSTEM_POWER_STATUS battery_status = { 0 };

	while (!(_kbhit()))
	{
		cout << "Type key to exit." << endl;

		if (GetSystemPowerStatus(&battery_status))
		{
			cout << setw(30) << left << "AC Line Status ";

			if (battery_status.ACLineStatus)
			{
				cout << "Online" << endl;
			}
			else
			{
				cout << "Offline" << endl;
			}

			cout << setw(30) << left << "Battery Life Percent " << (int)battery_status.BatteryLifePercent << endl;

			cout << setw(30) << left << "Battery Saver ";

			if (battery_status.SystemStatusFlag)
			{
				cout << "On" << endl;
			}
			else
			{
				cout << "Off" << endl;
			}

			system("powercfg -l");
		}

		Sleep(1000);

		system("CLS");
	}	

	char buf = _getch(); 

	cout << "Select number of need item:" << endl;
	cout << "1: Sleep mode" << endl;
	cout << "2: Hybernation mode" << endl;
	cout << "3: Exit" << endl;

	int flag;

	cin >> flag;

	switch (flag)
	{
	case 1: 
		SetSuspendState(0, 0, 0);
		break;
	case 2:
		SetSuspendState(1, 0, 0);
		break;
	default:
		break;
	}

	system("pause");
	return 0;
}