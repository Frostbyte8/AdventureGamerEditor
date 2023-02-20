#define __WIN32_PP__ // Until another UI is made, we'll use this.
#ifdef __WIN32_PP__

#include "win32pp/advedit_app.h"

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	AdvEditApp advEditApp;
    return advEditApp.Run();
}


#endif // __WIN32_PP__