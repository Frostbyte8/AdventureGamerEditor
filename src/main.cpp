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
		
        try {
            LanguageMapper::getInstance().tryLoadLangauge("", "");
        }
        catch(json::parsing_error) {
            MessageBox(NULL, L"Could not parse lang_en.json.", L"Parsing error", MB_ICONERROR);
        }

        if(!LanguageMapper::getInstance().isLoaded()) {
            const int retVal = MessageBox(NULL,
                                         L"The language mapper was unable to load lang_en.json. Continue without Language support?",
                                         L"Could not load language",
                                         MB_ICONERROR | MB_YESNO);
            if(retVal == IDNO) {
                return 0;
            }
        }
        
        int debugRetVal = DebugRun();
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG); 
		//_CrtDumpMemoryLeaks();
		return debugRetVal;
	}
#else
	int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
		AdvEditApp advEditApp;
        return advEditApp.Run();
	}
#endif // _DEBUG


#endif // __WIN32_PP__