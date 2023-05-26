#ifndef __EDITDIALOG_BASE_H__
#define __EDITDIALOG_BASE_H__

#include <wxx_wincore.h>
#include "../model/gamemap.h"
#include "../win32/window_metrics.h"
#include "../interface/mainwindow_interface.h"
#include "../interface/dialogbase_interface.h"

class EditDialogBase : public CWnd, public DialogBaseInterface {
    
    public:

        bool goModal();
        bool tryClose();
        void endModal();
        void madeChange();
        void changesSaved();
        const bool hasSavedChanges() const { return areSavedChanges; }

        virtual void moveControls() { return; }

        virtual int askYesNoQuestion(const std::string& inQuestion, const std::string& inTitle, bool allowCancel);
        virtual void displayErrorMessage(const std::string& inMessage, const std::string& inTitle);

    protected:

        EditDialogBase(MainWindowInterface* inMainWindow, HWND inParentWindow) : 
                       mainWindow(inMainWindow), parentWindow(inParentWindow), optionChosen(IDCLOSE), changeMade(false), areSavedChanges(false) {}

        static bool CALLBACK SetProperFont(HWND child, LPARAM font) {
            ::SendMessage(child, WM_SETFONT, font, true);
            return true;
        }

        virtual void PreCreate(CREATESTRUCT& cs);
        
        MainWindowInterface*        mainWindow;
        WindowMetrics               windowMetrics;
        HWND                        parentWindow;

        bool                        changeMade;
        bool                        areSavedChanges;
        int                         optionChosen;

    private:

};

#endif // __EDITDIALOG_BASE_H__
