#ifndef __EDITDIALOG_BASE_H__
#define __EDITDIALOG_BASE_H__

#include <wxx_wincore.h>
#include "../model/gamemap.h"
#include "../win32/window_metrics.h"
#include "../interface/mainwindow_interface.h"

class EditDialogBase : public CWnd, public DialogBaseInterface {
    
    public:

        bool GoModal();
        bool tryClose();

        void madeChange();
        //void changeReset();

        virtual int askYesNoQuestion(const std::string& question, const std::string& title, bool allowCancel);

    protected:

        EditDialogBase(MainWindowInterface* inMainWindow, HWND inParentWindow) : 
                       mainWindow(inMainWindow), parentWindow(inParentWindow), changeMade(false) {}

        static bool CALLBACK SetProperFont(HWND child, LPARAM font) {
            ::SendMessage(child, WM_SETFONT, font, true);
            return true;
        }

        virtual void PreCreate(CREATESTRUCT& cs);
        
        WindowMetrics               windowMetrics;
        MainWindowInterface*        mainWindow;
        HWND                        parentWindow;
        bool                        changeMade;
        int                         optionChosen;

    private:

};

#endif // __EDITDIALOG_BASE_H__
