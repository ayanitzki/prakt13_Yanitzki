#include <Windows.h>
#include <iostream>
#include <cmath>
using namespace std;

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "rus");

	if (argc < 3)
	{
		cout << "Недостаточное кол-во передаваемых аргументов" << endl;
		return -1;
	}
	int start = stoi(argv[1]);
	int end = stoi(argv[2]);
	int countSimple = 0;

	DWORD startTimer = GetTickCount();
	for (int i = start; i <= end; i++)
	{
		bool isSimple = true;
		if (i < 2)
		{
			isSimple = false;
		}
		for (int j = 2; j <= sqrt(i); j++)
		{
			if (i % j == 0)
			{
				isSimple = false;
				break;
			}
		}
		if (isSimple == true)
		{
			countSimple++;
		}
	}
	DWORD endTimer = GetTickCount();
	DWORD diffTime = endTimer - startTimer;

	cout << "Процесс PID: " << GetCurrentProcessId() << endl;
	return diffTime;
}
