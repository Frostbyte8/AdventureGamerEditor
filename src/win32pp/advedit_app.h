#ifndef __ADVEDIT_APP_H__
#define __ADVEDIT_APP_H__

#include <wxx_appcore.h>
#include "editor_mainwindow.h"

class AdvEditApp : public CWinApp {
	
	public:

		AdvEditApp() {}
		virtual ~AdvEditApp() {}
		virtual BOOL InitInstance() {
			mainWindowFrame.Create();
			return TRUE;
		}

	private:
		MainWindowFrame mainWindowFrame;
};

#endif // __ADVEDIT_APP_H__