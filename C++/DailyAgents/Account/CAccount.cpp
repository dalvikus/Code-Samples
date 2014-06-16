/*
 * From stdafx.h
 */
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#if 0
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#endif
#include <tchar.h>	// _tWinMain

#include "CAccount.h"
#include <Windows.h>


using namespace Account;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	System::Threading::Thread::CurrentThread->ApartmentState = System::Threading::ApartmentState::STA;
	Application::Run(new CAccount());
	return 0;
}
