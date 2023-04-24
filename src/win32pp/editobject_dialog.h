#ifndef __EDITOBJECT_DIALOG_H__
#define __EDITOBJECT_DIALOG_H__

#include <wxx_wincore.h>
#include <wxx_stdcontrols.h>
#include <wxx_imagelist.h> // Needed for wxx_tab
#include <wxx_tab.h>
#include "editobject_tabviews.h"
#include "../win32/window_metrics.h"
#include "../interface/mainwindow_interface.h"

// TODO: Window Metrics needs its own instance since if it moves to a differnt monitor
// the metrics may be differnt.

// TODO: Minimum Size and Maximum Size

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
        EditObjectDescriptionsTab* descriptionsTab;

        CButton                 dialogButtons[3];
        CTab                    tabControl;
        CSize                   contentSize;

};



#endif // __EDITOBJECT_DIALOG_H__
