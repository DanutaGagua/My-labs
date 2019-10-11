#pragma comment (lib, "SetupAPI.lib")

#include <Windows.h> 
#include <iostream> 
#include <SetupAPI.h>
#include <locale.h>
#include <SDKDDKVer.h>
#include <tchar.h>
#include <stdio.h>
#include <iomanip> 
#include <string> 

using namespace std;

int main()
{
	cout << "PCI devices list" << endl;
	setlocale(LC_ALL, "Russian"); 

	HDEVINFO DeviceInfoSet;
	SP_DEVINFO_DATA DeviceInfoData;

	DeviceInfoSet = SetupDiGetClassDevs(
		NULL,
		(PCSTR)("PCI"),
		NULL,
		DIGCF_ALLCLASSES | DIGCF_PRESENT);

	ZeroMemory(&DeviceInfoData, sizeof(SP_DEVINFO_DATA));
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	DWORD DeviceIndex = 0;

	while (SetupDiEnumDeviceInfo(
		DeviceInfoSet,
		DeviceIndex,
		&DeviceInfoData))
	{
		DeviceIndex++; 
		
		DWORD Size = 1000;
		TCHAR DeviceInstanceId[1000];

		if (SetupDiGetDeviceInstanceId(
			DeviceInfoSet,
			&DeviceInfoData,
			DeviceInstanceId,
			sizeof(DeviceInstanceId),
			&Size))
		{
			TCHAR DeviceInstanceName[1000]; 
			
			if (SetupDiGetDeviceRegistryProperty(
				 DeviceInfoSet,
				 &DeviceInfoData,
				 SPDRP_DEVICEDESC,
				 NULL,
				 (PBYTE)DeviceInstanceName,
				 sizeof(DeviceInstanceName),
				 &Size)) 
			{  
				string DeviceName = string(DeviceInstanceName);
				
				string DeviceID = string(DeviceInstanceId);
				string VenID = DeviceID.substr(8, 4);
				string DevID = DeviceID.substr(17, 4);

				cout << setw(50) << left << DeviceName << " VendorID " << VenID << "     DevicedID " << DevID << endl;
			}
			else
			{
				cout << "Error" << endl;
			}
		}
		else
		{
			cout << "Error" << endl;
		} 
	}

	if (DeviceInfoSet)
	{
		SetupDiDestroyDeviceInfoList(DeviceInfoSet);
	}

	system("pause");

	return 0;
}
