#ifndef __ABOUT_DIALOG_H__
#define __ABOUT_DIALOG_H__

#include <wxx_wincore.h>
#include <wxx_stdcontrols.h>
#include "../../util/win32pp_extras.h"
#include "../../win32/window_metrics.h"
#include "../editdialog_base.h"

class AboutDialog : public EditDialogBase {

    public:
        
        // Constructor
        AboutDialog(MainWindowInterface* inMainWindow, HWND inParentHandle);

    protected:

        virtual int OnCommand(WPARAM wParam, LPARAM lParam);
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);

        virtual bool trySaveData() { return true;  };

    private:

        void moveControls();

        CStatic         lblProgramName;
        CStatic         lblProgramVersion;
        CStatic         lblProgramCopyright;
        CStatic         lblProgramIcon;

        CEdit           txtCredits;
        CButton         btnOkay;

        HICON           programIcon;

        CString         dialogCaption;
};

#endif // __ABOUT_DIALOG_H__