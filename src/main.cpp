#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif // _DEBUG

#define __WIN32_PP__ // Until another UI is made, we'll use this.
#ifdef __WIN32_PP__

#include "win32pp/advedit_app.h"


#ifdef _DEBUG
	int DebugRun() {
		AdvEditApp advEditApp;
		return advEditApp.Run();
	}

	int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
		int retval = DebugRun();
		_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG); 
		_CrtDumpMemoryLeaks();
		return retval;
	}
#else
	int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
		AdvEditApp advEditApp;
		return advEditApp.Run()
	}
#endif // _DEBUG


#endif // __WIN32_PP__