#ifndef __EDIT_TILEDESCRIPTION_DIALOG_H__
#define __EDIT_TILEDESCRIPTION_DIALOG_H__

#include <wxx_wincore.h>
#include <wxx_stdcontrols.h>
#include "../util/win32pp_extras.h"
#include "../win32/window_metrics.h"
#include "editdialog_base.h"

class EditTileDescriptionDialog : public EditDialogBase {

    public:

        EditTileDescriptionDialog(MainWindowInterface* inMainWindow, HWND inParentHandle);

        const std::string getTileName() const { return tileName; }
        const std::string getTileDescription() const { return tileDescription; }

        void setTileDescription(const std::string& inName, const std::string& inDescription);

    protected:

        virtual void OnClose(); 
        virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);

    private:

        void moveControls();

        CStatic         lblTileName;
        CAnsiEdit       txtTileName;
        CStatic         lblTileDescription;
        CAnsiEdit       txtTileDescription;
        CButton         btnDialogButtons[3];


        std::string     tileName;
        std::string     tileDescription;

        CString         dialogCaption;

        int optionChosen;
};

#endif // __EDIT_TILEDESCRIPTION_DIALOG_H__ 
