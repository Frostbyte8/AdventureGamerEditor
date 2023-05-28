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
        
        // Public Functions
        bool goModal();         // Dialog is now modal
        void endModal(void (MainWindowInterface::*finishFunction)() = NULL);        // Dialog is no longer modal
        
        void dialogButtonPressed(const int& which); 

        void changesSaved();    // Indicate changes were saved
        bool tryClose();        // Attempt to close the window
        bool trySave();         // Attempt to Save Data
        void madeChange();      // Indicate a change was made

        // Interface functions
       
        virtual int askYesNoQuestion(const std::string& inQuestion, const std::string& inTitle, bool allowCancel);
        virtual void displayErrorMessage(const std::string& inMessage, const std::string& inTitle);
        
        // Pure virtual functions
        virtual void moveControls() { return; } // TODO: Make PVF 
        virtual bool trySaveData() { return true; } // TODO: Make PVF
        

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
