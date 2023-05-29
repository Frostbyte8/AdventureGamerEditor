#ifndef __EDIT_WORLDINFO_H__
#define __EDIT_WORLDINFO_H__

#include <wxx_wincore.h>
#include <wxx_stdcontrols.h>
#include "../util/win32pp_extras.h"
#include "../win32/window_metrics.h"
#include "editdialog_base.h"

class EditWorldInfoDialog : public EditDialogBase {

    public:

        // Constructors
        EditWorldInfoDialog(MainWindowInterface* inMainWindow, HWND inParentHandle);

        // Accessors
        GameInfo getGameInfo();

        // Mutators
        void setWorldInfo(const GameInfo& gameInfo);
        
    protected:

        virtual void OnClose(); 
        virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);
        void moveControls();

        virtual void notifyChangeMade();
        virtual void notifyChangesSaved();
        virtual bool trySaveData();

        //virtual LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam);

    private:

        LONG caclculateWindowWidth();
        void insertData();
      
        CButton         grpWorldInfo;
        CStatic         lblProperties[2]; // Name and Money
        CAnsiEdit       txtProperties[2];
        CStatic         lblAttributes[5];
        CEdit           txtAttributes[5];
        CSpinButton     spnAttributes[5];
        CEdit           txtRandomAttributes[5];
        CSpinButton     spnRandomAttributes[5];
        CButton         btnDialog[3];
        
        // TODO: At some point, gameInfo needs to be passed in
        
        const GameMap*          gameMap;
        GameInfo                newGameInfo;

};

#endif //__EDIT_WORLDINFO_H__