#include "editor_mainwindow.h"
#include "shared_functions.h"

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

const LONG DIALOG_EX_STYLES = WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT;
const LONG DIALOG_WS_STYLES = WS_POPUPWINDOW | WS_DLGFRAME;

//-----------------------------------------------------------------------------
// Interface Functions
//-----------------------------------------------------------------------------

void MainWindowFrame::onGameObjectsChanged() {
    entityView->updateObjectList(gameWorldController->getGameMap()->getGameObjects());
}

//-----------------------------------------------------------------------------
// Shared Functions for Dialogs
//-----------------------------------------------------------------------------

///----------------------------------------------------------------------------
/// canCreateDialog
///----------------------------------------------------------------------------

bool MainWindowFrame::canCreateDialog(const int& whichDialogType) const {

    if (activeWindowHandle != GetHwnd()) {
        return false;
    }

    switch (whichDialogType) {
        case EditorDialogTypes::AlterObject: return editObjectDialog ? false : true;
    }

    return false;
}

///----------------------------------------------------------------------------
/// onDialogEnd
///----------------------------------------------------------------------------

void MainWindowFrame::onDialogEnd(const int& whichDialogType) {

    switch (whichDialogType) {

        case EditorDialogTypes::AlterObject:
            assert(editObjectDialog != NULL);
            delete editObjectDialog;
            editObjectDialog = NULL;
            break;

    }

    activeWindowHandle = GetHwnd();
}

///----------------------------------------------------------------------------
/// makeDialogModal - Make the given dialog modal
/// @param a reference to a dialog that inherits from EditDialogBase that is
/// meant to be modal.
///----------------------------------------------------------------------------

void MainWindowFrame::makeDialogModal(EditDialogBase& dialog, const CString& caption) {

    activeWindowHandle = dialog.GetHwnd();
    dialog.SetExStyle(dialog.GetExStyle() | WS_EX_DLGMODALFRAME);
    dialog.setDefaultDialogTitle(caption);

    dialog.goModal();
    // TODO: Make centerWindow use a handle rather than a CWND.
    centerWindowOnCurrentMonitor(MonitorFromWindow(GetHwnd(), 0), reinterpret_cast<CWnd&>(dialog));
    dialog.ShowWindow(SW_SHOW);
}

//-----------------------------------------------------------------------------
// Dialog Related Interface Functions
//-----------------------------------------------------------------------------

///----------------------------------------------------------------------------
/// onAlterObject
///----------------------------------------------------------------------------

bool MainWindowFrame::startEditObjectDialog(GameObject::Builder& objectBuilder, const bool editingObject) {

    editObjectDialog = new (std::nothrow) EditObjectDialog(this, gameWorldController->getGameMap(), GetHwnd(), editingObject);

    if (!editObjectDialog) {
        return false;
    }

    editObjectDialog->Create(GetHwnd(), DIALOG_EX_STYLES,
                             DIALOG_WS_STYLES);

    if (!editObjectDialog->IsWindow()) {
        return false;
    }

    CString caption;
    LanguageMapper& langMap = LanguageMapper::getInstance();

    if (!editingObject) {
        caption = LM_toUTF8("CreateObjectTitle", langMap);
    } else {
        caption = LM_toUTF8("EditObjectTitle", langMap);
        caption.Format(caption, AtoW(objectBuilder.base.description[GameObjectDescriptions::Name].c_str(), CP_UTF8).c_str());
    }

    editObjectDialog->setObjectToEdit(objectBuilder.build());

    makeDialogModal(*editObjectDialog, caption);

    return true;

}

///----------------------------------------------------------------------------
/// finishAlterObjectDialog
///----------------------------------------------------------------------------

void MainWindowFrame::finishedAlterObjectDialog() {

    assert(editObjectDialog != NULL);

    if (editObjectDialog->hasSavedChanges()) {

        if (editObjectDialog->isEditingObject()) {
            gameWorldController->tryReplaceObject(editObjectDialog->getAlteredObject());
        }
        else {
            gameWorldController->tryAddObject(editObjectDialog->getAlteredObject());
        }
    }

    onDialogEnd(EditorDialogTypes::AlterObject);

}