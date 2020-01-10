#pragma comment (lib, "SetupAPI.lib")
#pragma comment (lib, "Kernel32.lib")

#include <iostream>
#include <iomanip>
#include <Windows.h>
#include <WinIoCtl.h>
#include <ntddscsi.h>
#include <conio.h>

using namespace std;

#define bThousand 1024
#define Hundred 100
#define BYTE_SIZE 8

string busType[] = { "UNKNOWN", "SCSI", "ATAPI", "ATA", "ONE_TREE_NINE_FOUR", "SSA", "FIBRE", "USB", "RAID", "ISCSI", "SAS", "SATA", "SD", "MMC" };

void getDeviceInfo(HANDLE diskHandle, STORAGE_PROPERTY_QUERY storageProtertyQuery) {

	STORAGE_DEVICE_DESCRIPTOR* deviceDescriptor = (STORAGE_DEVICE_DESCRIPTOR*)calloc(bThousand, 1);

	deviceDescriptor->Size = bThousand;

	if (!DeviceIoControl(diskHandle,
		IOCTL_STORAGE_QUERY_PROPERTY,				//Отправляем запрос на возврат свойств устройства. 
		&storageProtertyQuery,						//Указатель на буфер данных
		sizeof(storageProtertyQuery),				//Размер входного буфера
		deviceDescriptor,							//Указатель на выходной буфер
		bThousand,									//размер выходного буфера
		NULL,										
		0)) {
		printf("%d", GetLastError());
		CloseHandle(diskHandle);
		exit(-1);
	}

	cout << "Model" << endl << (char*)(deviceDescriptor)+deviceDescriptor->ProductIdOffset << endl << endl;
	cout << "Firmware" << endl << (char*)(deviceDescriptor)+deviceDescriptor->ProductRevisionOffset << endl << endl;
	cout << "Bus type" << endl << busType[deviceDescriptor->BusType].c_str() << endl << endl;
	cout << "Serial number" << endl << (char*)(deviceDescriptor)+deviceDescriptor->SerialNumberOffset << endl << endl;
	cout << "Vendor" << endl << "Seagate Technoloy LLC" << endl << endl;
}

void getMemoryInfo() {
	string path;
	_ULARGE_INTEGER diskSpace;
	_ULARGE_INTEGER freeSpace;

	diskSpace.QuadPart = 0;
	freeSpace.QuadPart = 0;

	_ULARGE_INTEGER totalDiskSpace;
	_ULARGE_INTEGER totalFreeSpace;

	totalDiskSpace.QuadPart = 0;
	totalFreeSpace.QuadPart = 0;

	//Получаем битовую маску, представляющую имеющиеся в настоящие время дисковые накопители. 
	unsigned long int logicalDrivesCount = GetLogicalDrives();

	//Анализ полученной битовой маски(бит 0 - диск А, бит 1 - диск B). 
	for (char var = 'A'; var < 'Z'; var++) {
		if ((logicalDrivesCount >> var - 65) & 1 && var != 'F') {
			path = var;
			path.append(":\\"); 
			GetDiskFreeSpaceEx(path.c_str(), 0, &diskSpace, &freeSpace);
			diskSpace.QuadPart = diskSpace.QuadPart / (bThousand * bThousand);
			freeSpace.QuadPart = freeSpace.QuadPart / (bThousand * bThousand);

			//Определяем тип диска(3 - жесткий диск) 
			if (GetDriveType(path.c_str()) == 3) {
				totalDiskSpace.QuadPart += diskSpace.QuadPart;
				totalFreeSpace.QuadPart += freeSpace.QuadPart;
			}
		}
	}
	 
	cout << endl << "Total space[Gb] : " << totalDiskSpace.QuadPart * 1.0 / 1024 << endl;
	cout << "Free space[Gb]  : " << totalFreeSpace.QuadPart * 1.0 / 1024 << endl;
	cout << "Busy space[Gb]  : " << (totalDiskSpace.QuadPart - totalFreeSpace.QuadPart) * 1.0 / 1024 << endl;
	cout << "Busy space[%]   : " << 100.0 - (double)totalFreeSpace.QuadPart / (double)totalDiskSpace.QuadPart * Hundred << endl << endl;
}

void getAtaPioDmaSupportStandarts(HANDLE diskHandle) {

	UCHAR identifyDataBuffer[512 + sizeof(ATA_PASS_THROUGH_EX)] = { 0 };

	ATA_PASS_THROUGH_EX &PTE = *(ATA_PASS_THROUGH_EX *)identifyDataBuffer;	//Структура для отправки АТА команды устройству 
	PTE.Length = sizeof(PTE);
	PTE.TimeOutValue = 10;									//Размер структуры 
	PTE.DataTransferLength = 512;							//Размер буфера для данных 
	PTE.DataBufferOffset = sizeof(ATA_PASS_THROUGH_EX);		//Смещение в байтах от начала структуры до буфера данных 
	PTE.AtaFlags = ATA_FLAGS_DATA_IN;						//Флаг, говорящий о чтении байтов из устройства 

	IDEREGS *ideRegs = (IDEREGS *)PTE.CurrentTaskFile;
	ideRegs->bCommandReg = 0xEC;

	//Производим запрос устройству 
	if (!DeviceIoControl(diskHandle,
		IOCTL_ATA_PASS_THROUGH,								//посылаем структуру с командами типа ATA_PASS_THROUGH_EX
		&PTE, 
		sizeof(identifyDataBuffer), 
		&PTE, 
		sizeof(identifyDataBuffer), 
		NULL,
		NULL)) {
		cout << GetLastError() << std::endl;
		return;
	}
	//Получаем указатель на массив полученных данных 
	WORD *data = (WORD *)(identifyDataBuffer + sizeof(ATA_PASS_THROUGH_EX));	
	short ataSupportByte = data[80];
	int i = 2 * BYTE_SIZE;
	int bitArray[2 * BYTE_SIZE];
	
	//Вывод поддерживаемых режимов DMA 
	unsigned short dmaSupportedBytes = data[63];
	int i2 = 2 * BYTE_SIZE;
	//Превращаем байты с информацией о поддержке DMA в массив бит
	while (i2--) {
		bitArray[i2] = dmaSupportedBytes & 32768 ? 1 : 0;
		dmaSupportedBytes = dmaSupportedBytes << 1;
	}

	//Анализируем полученный массив бит. 
	cout << "DMA Support: ";
	for (int i = 0; i < 8; i++) {
		if (bitArray[i] == 1) {
			cout << "DMA" << i;
			if (i != 2) cout << ", ";
		}
	}
	cout << endl;

	unsigned short pioSupportedBytes = data[64];
	int i3 = 2 * BYTE_SIZE;
	//Превращаем байты с информацией о поддержке PIO в массив бит 
	while (i3--) {
		bitArray[i3] = pioSupportedBytes & 32768 ? 1 : 0;
		pioSupportedBytes = pioSupportedBytes << 1;
	}

	//Анализируем полученный массив бит. 
	cout << "PIO Support: ";
	for (int i = 0; i < 2; i++) {
		if (bitArray[i] == 1) {
			cout << "PIO" << i + 3;
			if (i != 1) cout << ", ";
		}
	}
	cout << endl;
}

void init(HANDLE& diskHandle) {
	//Открытие файла с информацией о диске 
	diskHandle = CreateFile("//./PhysicalDrive0", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
	if (diskHandle == INVALID_HANDLE_VALUE) {
		cout << "Administrator need";
		_getch();
		return;
	}
}

int main()
{
	STORAGE_PROPERTY_QUERY storagePropertyQuery;				//Структура с информацией об запросе 
	storagePropertyQuery.QueryType = PropertyStandardQuery;		//Запрос драйвера, чтобы он вернул дескриптор устройства. 
	storagePropertyQuery.PropertyId = StorageDeviceProperty;	//Флаг, гооврящий мы хотим получить дескриптор устройства. 
	HANDLE diskHandle;

	init(diskHandle);
	getDeviceInfo(diskHandle, storagePropertyQuery);
	getMemoryInfo();
	getAtaPioDmaSupportStandarts(diskHandle);
	_getch();
	return 0;
}
