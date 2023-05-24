#ifndef __EDITSTORY_DIALOG_H__
#define __EDITSTORY_DIALOG_H__

#include <wxx_wincore.h>
#include <wxx_stdcontrols.h>
#include "../util/win32pp_extras.h"
#include "../win32/window_metrics.h"
#include "editdialog_base.h"

class EditStoryDialog : public EditDialogBase {

    public:
        EditStoryDialog(MainWindowInterface* inMainWindow, const GameMap* inGameMap, HWND inParentHandle);
        void setStory(const std::string& inStoryText);

    protected:
        virtual void OnClose(); 
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);

    private:

        CAnsiEdit       txtStory;
        CButton         dialogButtons[3];

        int             optionChosen;
};

#endif // __EDITSTORY_DIALOG_H__
