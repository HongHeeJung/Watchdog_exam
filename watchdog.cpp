#pragma comment(lib, "winmm.lib")

#include <Windows.h>
#include <iostream>
#include <conio.h>

using namespace std;

#define WATCHDOGTIMER 100 // 100ms 마다 수행
#define WAKE_UP_SIG 0
#define DOG_REPORT_SIG 1
static int g_n;
CRITICAL_SECTION m_cs;

void SetFlag(int flag)
{
	EnterCriticalSection(&m_cs);
	g_n = flag;
	LeaveCriticalSection(&m_cs);
};

int GetFlag()
{
	return g_n;
};

void CreateNewPacket()
{
	SetFlag(DOG_REPORT_SIG);

	cout << "Create a new packet." << '\n';
}

// Thread 1 function
UINT ThreadOne(LPVOID IParam)
{
	for (;;) {
		cout << "=========== Thread 1 ==========" << '\n';
		CreateNewPacket();
		Sleep(WATCHDOGTIMER + 10000);
		cout << "============ Done ============" << '\n';
	}
	// return the thread
	return 0;
}

// Thread 2 function
// ...

void CALLBACK WatchDog(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	if (GetFlag() == WAKE_UP_SIG) { // Watchdog timer
		EnterCriticalSection(&m_cs);
		cout << "Flag = 0" << '\n';
		cout << "Watchdog" << '\n';
		// RESET
		//CreateNewPacket();
		LeaveCriticalSection(&m_cs);
	}
	else {
		EnterCriticalSection(&m_cs);
		cout << "Flag = 1" << '\n';
		cout << "Running..." << '\n';
		LeaveCriticalSection(&m_cs);
	}

	// Clear Flag
	SetFlag(DOG_REPORT_SIG);
}

int main() {
	// Create the array of Handle.
	HANDLE hThrd;

	// Thread IDs
	DWORD IDThread1;
	DWORD IDThread2;

	// Initialize the critical section
	InitializeCriticalSection(&m_cs);

	// Create threads using Create Thread function with NULL Security.
	hThrd = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadOne, (LPVOID)NULL, 0, &IDThread1);
	EnterCriticalSection(&m_cs);
	cout << "=========== Main ============" << '\n';
	LeaveCriticalSection(&m_cs);

	// Set Watchdog timer
	timeSetEvent(WATCHDOGTIMER, 1, (LPTIMECALLBACK)WatchDog, 0, TIME_PERIODIC | TIME_CALLBACK_FUNCTION);

	// Wait for the main thread
	WaitForSingleObject(hThrd, INFINITE);

	// Delete critical Section
	DeleteCriticalSection(&m_cs);

	return 0;
}
