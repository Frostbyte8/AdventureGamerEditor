#ifndef __EDIT_WORLDINFO_H__
#define __EDIT_WORLDINFO_H__

#include <wxx_wincore.h>
#include <wxx_stdcontrols.h>
#include "../util/win32pp_extras.h"
#include "../win32/window_metrics.h"
#include "editdialog_base.h"

class EditWorldInfoDialog : public EditDialogBase {

    
    public:
        EditWorldInfoDialog(MainWindowInterface* inMainWindow, const GameMap* inGameMap, HWND inParentHandle);

        void setWorldInfo();
        void calculatePageWidth(); 

    protected:

        virtual void OnClose(); 
        virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);

    private:

        void moveControls();

        CButton         grpWorldInfo;
        CStatic         lblProperties[2]; // Name and Money
        CAnsiEdit       txtProperties[2];
        CStatic         lblAttributes[5];
        CEdit           txtAttributes[5];
        CSpinButton     spnAttributes[5];
        CButton         btnDialog[3];

        LONG            pageWidth;

        // TODO: Player Start X/Y, 
};

#endif //__EDIT_WORLDINFO_H__
