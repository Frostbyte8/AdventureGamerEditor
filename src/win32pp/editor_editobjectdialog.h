#ifndef __EDITOR_EDITOBJECTDIALOG_H__
#define __EDITOR_EDITOBJECTDIALOG_H__

#include <wxx_wincore.h>
#include <wxx_stdcontrols.h>
#include <wxx_imagelist.h>
#include <wxx_tab.h>
#include "../win32/window_metrics.h"
#include "../interface/mainwindow_interface.h"
#include "../util/win32pp_extras.h"

class EditObjectDialog : public CWnd {

    public:
        EditObjectDialog(MainWindowInterface* inMainWindow, WindowMetrics* inWindowMetrics) : mainWindow(inMainWindow), windowMetrics(inWindowMetrics) {
            contentSize.SetSize(32,32);
        }

    protected:
        virtual void PreRegisterClass(WNDCLASS& wc);
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam);
        

    private:

        LRESULT OnSize(WPARAM& wParam, LPARAM& lParam);

        MainWindowInterface*    mainWindow;
        WindowMetrics*          windowMetrics;

        CButton                 dialogButtons[3];
        CTab                    tabControl;
        CSize                   contentSize;

};

class EditObjectTabPage : public CWnd {

    public:
        //virtual void MoveControls(CSize)
        //virtual CSize getContentMaxSizes();
        //virtual int getLongestControl()

        //virtual bool ValidateFields
        //virtual void PopulateFields
};


class EditObjectDescriptionsTab : public CWnd {

    public:
        EditObjectDescriptionsTab(WindowMetrics* inWindowMetrics) : windowMetrics(inWindowMetrics) { }
        virtual int OnCreate(CREATESTRUCT& cs);

    private:

        CButton                 grpDescriptions;
        CButton                 grpSoundIcon;
        CStatic                 lblDescriptions;
        CAnsiEdit               txtDescriptions;

        WindowMetrics*          windowMetrics;
        CSize                   contentSize;
};

#endif // __EDITOR_EDITOBJECTDIALOG_H__
