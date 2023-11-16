#ifndef __EDIT_TILEDESCRIPTION_DIALOG_H__
#define __EDIT_TILEDESCRIPTION_DIALOG_H__

#include <wxx_wincore.h>
#include <wxx_stdcontrols.h>
#include "../../util/win32pp_extras.h"
#include "../../win32/window_metrics.h"
#include "../editdialog_base.h"

class EditTileDescriptionDialog : public EditDialogBase {

    public:

        // Constructors
        EditTileDescriptionDialog(MainWindowInterface* inMainWindow, HWND inParentHandle);

        // Accessors
        const std::string getTileName() const;
        const std::string getTileDescription() const;

        // Mutators
        void setTileDescription(const std::string& inName, const std::string& inDescription);

    protected:

        virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);
        virtual bool trySaveData();

    private:

        void moveControls();

        CStatic         lblTileName;
        CAnsiEdit       txtTileName;
        CStatic         lblTileDescription;
        CAnsiEdit       txtTileDescription;

        std::string     tileName;
        std::string     tileDescription;
        CString         dialogCaption;

};

#endif // __EDIT_TILEDESCRIPTION_DIALOG_H__ 
