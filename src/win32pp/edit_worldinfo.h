#ifndef __EDIT_WORLDINFO_H__
#define __EDIT_WORLDINFO_H__

#include <wxx_wincore.h>
#include <wxx_stdcontrols.h>
#include "../util/win32pp_extras.h"
#include "../win32/window_metrics.h"

class EditWorldInfoDialog {

    public:

        void setWorldInfo();
        void calculatePageWidth(const WindowMetrics& wndMetrics); 

    protected:

        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);

    private:

        void moveControls(const WindowMetrics& wndMetrics);

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
