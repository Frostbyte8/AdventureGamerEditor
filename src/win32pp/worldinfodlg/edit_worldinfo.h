#ifndef __EDIT_WORLDINFO_H__
#define __EDIT_WORLDINFO_H__

#include <wxx_wincore.h>
#include <wxx_stdcontrols.h>
#include "../../util/win32pp_extras.h"
#include "../../util/inputvalidator.h"
#include "../../win32/window_metrics.h"
#include "../editdialog_base.h"

class EditWorldInfoDialog : public EditDialogBase {

    public:

        // Constructors
        EditWorldInfoDialog(MainWindowInterface* inMainWindow, HWND inParentHandle);

        // Accessors
        GameInfo getGameInfo();

        // Mutators
        void setWorldInfo(const GameInfo& gameInfo);
        
    protected:

        virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);
        void moveControls();

        virtual bool trySaveData();
        
        InputValidator* validateFields();

    private:

        LONG calculateWindowWidth();
      
        CButton         grpWorldInfo;
        CStatic         lblProperties[2]; // Name and Money
        CAnsiEdit       txtProperties[2];
        CStatic         lblAttributes[5];
        CEdit           txtAttributes[5];
        CSpinButton     spnAttributes[5];
        CEdit           txtRandomAttributes[5];
        CSpinButton     spnRandomAttributes[5];
        
        IntegerValidator    baseAttributeValidator[5];
        IntegerValidator    randomAttributeValidator[5];
        
        // TODO: At some point, gameInfo needs to be passed in
        
        const GameMap*          gameMap;
        GameInfo                newGameInfo;

};

#endif //__EDIT_WORLDINFO_H__