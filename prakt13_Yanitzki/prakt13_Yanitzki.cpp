#include <Windows.h>
#include <iostream>
#include <string>
#include <conio.h>
using namespace std;

struct Process
{
    wstring name;
    DWORD id;
    DWORD priorityClass;
    HANDLE hProcess;
    HANDLE hThread;
    bool isFinished;
    int place;
    double time;
};

const int maxProcess = 5;
Process processList[maxProcess];
int processCount = 0;
int finishedCount = 0;

DWORD priorities[5] = { IDLE_PRIORITY_CLASS, BELOW_NORMAL_PRIORITY_CLASS, NORMAL_PRIORITY_CLASS, ABOVE_NORMAL_PRIORITY_CLASS, HIGH_PRIORITY_CLASS };

void StartProcess(wstring appName, wstring args, DWORD priority)
{
    STARTUPINFO si;
    PROCESS_INFORMATION piApp;

    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    ZeroMemory(&piApp, sizeof(PROCESS_INFORMATION));

    wstring argsLine = appName + L" " + args;
    wchar_t* cmd = new wchar_t[argsLine.length() + 1];
    wcscpy_s(cmd, argsLine.length() + 1, argsLine.c_str());

    if (!CreateProcess(NULL, cmd, NULL, NULL, FALSE, CREATE_SUSPENDED | CREATE_NEW_CONSOLE, NULL, NULL, &si, &piApp))
    {
        cout << "Ошибка создания процесса. Код: " << GetLastError() << endl;
        delete[] cmd;
        exit(1);
    }

    if (!SetPriorityClass(piApp.hProcess, priority))
    {
        cout << "Ошибка установки приоритета. Код: " << GetLastError() << endl;
    }


    processList[processCount].name = appName;
    processList[processCount].id = piApp.dwProcessId;
    processList[processCount].hProcess = piApp.hProcess;
    processList[processCount].hThread = piApp.hThread;
    processList[processCount].priorityClass = priority;
    processList[processCount].isFinished = false;
    processList[processCount].place = 0;
    processList[processCount].time = 0.0;
    processCount++;

    delete[] cmd;
}
void ShowList()
{
    cout << "=== ТЕКУЩЕЕ СОСТОЯНИЕ ГОНКИ ===" << endl;

    for (int i = 0; i < processCount; i++)
    {
        if (!processList[i].isFinished)
        {
            if (WaitForSingleObject(processList[i].hProcess, 0) == WAIT_OBJECT_0)
            {
                processList[i].isFinished = true;
                finishedCount++;

                DWORD exitCode = 0;
                if (GetExitCodeProcess(processList[i].hProcess, &exitCode))
                {
                    processList[i].time = exitCode / 1000.0;
                }
            }
        }

        cout << "[" << i + 1 << "] PID: " << processList[i].id;
        cout << " | Статус: " << (processList[i].isFinished ? "Завершен" : "Выполняется");

        if (processList[i].priorityClass == IDLE_PRIORITY_CLASS)
        {
            cout << " | Приоритет: IDLE";
        }
        else if (processList[i].priorityClass == BELOW_NORMAL_PRIORITY_CLASS)
        { 
            cout << " | Приоритет: BELOW_NORMAL";
        }
        else if (processList[i].priorityClass == NORMAL_PRIORITY_CLASS)
        {
            cout << " | Приоритет: NORMAL";
        }
        else if (processList[i].priorityClass == ABOVE_NORMAL_PRIORITY_CLASS)
        {
            cout << " | Приоритет: ABOVE_NORMAL";
        }
        else if (processList[i].priorityClass == HIGH_PRIORITY_CLASS)
        {
            cout << " | Приоритет: HIGH";
        }

        if (processList[i].isFinished)
            cout << " | Время: " << processList[i].time << " сек." << endl;
        else
            cout << " | Время: вычисляется" << endl;
    }
}
void ChangePriority()
{
    system("cls");
    ShowList();
    cout << endl << "Введите номер участника для смены приоритета (1-" << processCount << "): ";
    int index;
    cin >> index;
    index--;

    if (index < 0 || index >= processCount || processList[index].isFinished)
    {
        cout << "Неверный номер участника или он уже завершил гонку" << endl;
        Sleep(1000);
        return;
    }

    cout << "Выберите новый класс приоритета:" << endl;
    cout << "1 - IDLE_PRIORITY_CLASS" << endl;
    cout << "2 - BELOW_NORMAL_PRIORITY_CLASS" << endl;
    cout << "3 - NORMAL_PRIORITY_CLASS" << endl;
    cout << "4 - ABOVE_NORMAL_PRIORITY_CLASS" << endl;
    cout << "5 - HIGH_PRIORITY_CLASS" << endl;
    cout << "Выбор: ";

    int pchoice;
    cin >> pchoice;

    if (pchoice < 1 || pchoice > 5)
    {
        cout << "Неверный выбор приоритета" << endl;
        Sleep(1000);
        return;
    }

    DWORD newPriority = priorities[pchoice - 1];
    if (SetPriorityClass(processList[index].hProcess, newPriority))
    {
        processList[index].priorityClass = newPriority;
        cout << "Приоритет успешно изменен!" << endl;
    }
    else
    {
        cout << "Ошибка изменения приоритета. Код ошибки: " << GetLastError() << endl;
    }
    Sleep(1000);
}
int main()
{
    setlocale(LC_ALL, "rus");

    int count = 0;
    while (count < 2 || count > 5)
    {
        cout << "Введите количество участников гонки (от 2 до 5): ";
        cin >> count;
    }

    long long rangeStart, rangeEnd;
    cout << "Введите начальное и конечное значение диапазона (например, 2 15000000): ";
    cin >> rangeStart >> rangeEnd;

    wstring args = to_wstring(rangeStart) + L" " + to_wstring(rangeEnd);

    for (int i = 0; i < count; i++)
    {
        cout << endl << "=== НАСТРОЙКА УЧАСТНИКА #" << i + 1 << " ===" << endl;
        cout << "1 - IDLE_PRIORITY_CLASS" << endl;
        cout << "2 - BELOW_NORMAL_PRIORITY_CLASS" << endl;
        cout << "3 - NORMAL_PRIORITY_CLASS" << endl;
        cout << "4 - ABOVE_NORMAL_PRIORITY_CLASS" << endl;
        cout << "5 - HIGH_PRIORITY_CLASS" << endl;
        cout << "Выберите приоритет: ";

        int pchoice;
        cin >> pchoice;
        if (pchoice < 1 || pchoice > 5) pchoice = 3;

        StartProcess(L"prakt13_SimpleNum.exe", args, priorities[pchoice - 1]);
    }

    cout << endl << "Все участники готовы. Нажмите любую клавишу для старта гонки..." << endl;
    _getch();

    for (int i = 0; i < processCount; i++)
    {
        ResumeThread(processList[i].hThread);
    }

    while (finishedCount < processCount)
    {
        system("cls");
        ShowList();
        cout << endl << "Нажмите, чтобы изменить приоритет участника во время гонки..." << endl;

        if (_kbhit())
        {
            char key = _getch();
            if (key == '1')
            {
                ChangePriority();
            }
        }
        Sleep(1000);
    }

    for (int i = 0; i < processCount - 1; i++)
    {
        for (int j = 0; j < processCount - i - 1; j++)
        {
            if (processList[j].time > processList[j + 1].time)
            {
                Process temp = processList[j];
                processList[j] = processList[j + 1];
                processList[j + 1] = temp;
            }
        }
    }

    for (int i = 0; i < processCount; i++)
    {
        processList[i].place = i + 1;
    }

    system("cls");
    cout << "<<< ИТОГОВАЯ ТАБЛИЦА РЕЗУЛЬТАТОВ >>>" << endl;
    for (int i = 0; i < processCount; i++)
    {
        cout << "Место #" << processList[i].place
            << " | PID: " << processList[i].id;

        if (processList[i].priorityClass == IDLE_PRIORITY_CLASS) cout << " | Приоритет: IDLE";
        else if (processList[i].priorityClass == BELOW_NORMAL_PRIORITY_CLASS) cout << " | Приоритет: BELOW_NORMAL";
        else if (processList[i].priorityClass == NORMAL_PRIORITY_CLASS) cout << " | Приоритет: NORMAL";
        else if (processList[i].priorityClass == ABOVE_NORMAL_PRIORITY_CLASS) cout << " | Приоритет: ABOVE_NORMAL";
        else if (processList[i].priorityClass == HIGH_PRIORITY_CLASS) cout << " | Приоритет: HIGH";

        cout << " | Время: " << processList[i].time << " сек." << endl;

        CloseHandle(processList[i].hThread);
        CloseHandle(processList[i].hProcess);
    }

    return 0;
}
