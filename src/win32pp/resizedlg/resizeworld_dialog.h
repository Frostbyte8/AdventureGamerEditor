#ifndef __RESIZEWORLD_DIALOG_H__
#define __RESIZEWORLD_DIALOG_H__

#include <wxx_wincore.h>
#include <wxx_stdcontrols.h>
#include "../../util/win32pp_extras.h"
#include "../../util/inputvalidator.h"
#include "../../win32/window_metrics.h"
#include "../editdialog_base.h"

class ResizeWorldDialog : public EditDialogBase {

    public:

        // Constructors
        ResizeWorldDialog(MainWindowInterface* inMainWindow, HWND inParentHandle);

        // Accessors
        const int getNewCols() const { return worldWidth; }
        const int getNewRows() const { return worldHeight; }

        // Mutators
        void setWorldDimensions(const int& currentWidth, const int& currentHeight);

    protected:

        virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);
        void moveControls();

        virtual bool trySaveData();

        InputValidator* validateFields();

    private:

        LONG calculateWindowWidth();

        CStatic             lblDimenions[2];
        CEdit               txtDimensions[2];

        IntegerValidator    dimensionValidator[2];

        int worldWidth;
        int worldHeight;
        

};

#endif // __RESIZEWORLD_DIALOG_H__