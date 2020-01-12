#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>  
#include <windows.h>
#include <iostream> 
#include <string>
#include <conio.h>

using namespace std;
using namespace cv; 

void makePhoto();
void makeVideo();
void writeInfo();

int main()
{
	VideoCapture cap(0);

	system("CLS"); 

	cout << "HP Webcam" << endl;
	cout << setw(15) << left << "Interface " << "USB" << endl;
	cout << setw(15) << left << "Resolution ";
	cout << cap.get(CAP_PROP_FRAME_WIDTH) << "*" << cap.get(CAP_PROP_FRAME_HEIGHT) << endl << endl;

	writeInfo();

	cap.~VideoCapture();

	while (true)
	{
		if (_kbhit())
		{
			switch (_getch())
			{
				case 'e': return 0; break;
				case 'p': makePhoto(); break;
				case 'v': makeVideo(); break;
				case 'h': 
					::ShowWindow(::GetConsoleWindow(), SW_HIDE);
					makeVideo(); 
					::ShowWindow(::GetConsoleWindow(), SW_SHOW);
					break;
			}

			writeInfo();
		}
	}
}

void makePhoto()
{
	static int number = 0;
	number++;
	
	VideoCapture cap(0);
	system("CLS"); 

	if (cap.isOpened())
	{
		Mat img;
		cap >> img;
		if (!img.empty())
		{
			char fileName[1000];
			sprintf_s(fileName, "E:\\IaPD %d.jpg", number);
			imwrite(fileName, img);
		}
	}

	system("CLS");
	cout << "Photo was made." << endl << endl;

	cap.~VideoCapture();
}

void makeVideo()
{
	static int number = 0;
	number++;

	char fileName[1000];
	sprintf_s(fileName, "E:\\IaPD %d.avi", number);
	
	VideoCapture cap(0);

	system("CLS"); 

	double dWidth = cap.get(CAP_PROP_FRAME_WIDTH);
	double dHeight = cap.get(CAP_PROP_FRAME_HEIGHT);

	VideoWriter video(fileName, VideoWriter::fourcc('M', 'J', 'P', 'G'), 25, Size(dWidth, dHeight));
	while (!_kbhit())
	{
		Mat frame;
		cap >> frame;
		if (frame.empty())
			break;
		video.write(frame);
	}
	cap.release();
	video.release();

	system("CLS");
	cout << "Video was made." << endl << endl;

	cap.~VideoCapture();
}

void writeInfo()
{
	cout << "Enter key:" << endl;
	cout << "e         - to exit" << endl;
	cout << "p         - to make photo" << endl;
	cout << "v         - to make video" << endl;
	cout << "h         - to make hidden video" << endl;
	cout << "else keys - to stop writing of video" << endl;
}