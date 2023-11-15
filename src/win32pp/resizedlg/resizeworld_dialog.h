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
        const int getNewWidth() const { return worldWidth; }
        const int getNewHeight() const { return worldHeight; }

        // Mutators
        void setWorldDimensions(const int& currentWidth, const int& currentHeight);

    protected:

        virtual void OnClose();
        virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);
        virtual BOOL PreTranslateMessage(MSG& msg);
        void moveControls();

        virtual void notifyChangeMade();
        virtual void notifyChangesSaved();
        virtual bool trySaveData();

        InputValidator* validateFields();

    private:

        LONG caclculateWindowWidth();
        //void insertData();

        CStatic         lblDimenions[2];
        CEdit           txtDimensions[2];
        CButton         btnDialog[3];

        IntegerValidator    dimensionValidator[2];

        int worldWidth;
        int worldHeight;
        

};

#endif // __RESIZEWORLD_DIALOG_H__