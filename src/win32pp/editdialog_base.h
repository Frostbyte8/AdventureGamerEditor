#ifndef __EDITDIALOG_BASE_H__
#define __EDITDIALOG_BASE_H__

#include <wxx_wincore.h>
#include "../model/gamemap.h"
#include "../win32/window_metrics.h"
#include "../interface/mainwindow_interface.h"
#include "../interface/dialogbase_interface.h"

namespace DefControlIDs {
    const WORD IDAPPLY = 1001;
}

class EditDialogBase : public CWnd, public DialogBaseInterface {
    
    public:

       
        // Accessors        
        const bool hasSavedChanges() const { return areSavedChanges; }
        const WindowMetrics& getWindowMetrics() const { return windowMetrics; }
        const int getOptionChosen() const { return optionChosen; }

        // Mutators

        void setDefaultDialogTitle(const CString& inTitle);

        // Public Functions
        bool goModal();         // Dialog is now modal
        void endModal(void (MainWindowInterface::*finishFunction)() = NULL);        // Dialog is no longer modal
 
        void dialogButtonPressed(const int& which); // Ok/Cancel/Apply were clicked
        void changesSaved();    // Indicate changes were saved
        bool tryClose();        // Attempt to close the window
        bool trySave();         // Attempt to Save Data
        void madeChange();      // Indicate a change was made

        // Interface functions
       
        virtual int askYesNoQuestion(const std::string& inQuestion, const std::string& inTitle, bool allowCancel);
        virtual void displayErrorMessage(const std::string& inMessage, const std::string& inTitle);
               

    protected:

        EditDialogBase(MainWindowInterface* inMainWindow, HWND inParentWindow);

        // Win32 Functions
        virtual void PreCreate(CREATESTRUCT& cs);

        // Pure virtual functions
        virtual void notifyChangeMade();
        virtual void notifyChangesSaved();
        virtual void moveControls() = 0;
        virtual bool trySaveData() = 0;

        static bool CALLBACK SetProperFont(HWND child, LPARAM font) {
            ::SendMessage(child, WM_SETFONT, font, true);
            return true;
        }

        MainWindowInterface*        mainWindow;
        WindowMetrics               windowMetrics;
        HWND                        parentWindow;

        CString                     originalWindowTitle;
        bool                        changeMade;
        bool                        areSavedChanges;
        int                         optionChosen;
        bool                        dialogReady;

    private:

};

#endif // __EDITDIALOG_BASE_H__
