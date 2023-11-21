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

///----------------------------------------------------------------------------
/// onGameObjectsChanged
///----------------------------------------------------------------------------

void MainWindowFrame::onGameObjectsChanged() {
    entityView->updateObjectList(gameWorldController->getGameMap()->getGameObjects());
}

///----------------------------------------------------------------------------
/// onGameCharactersChanged
///----------------------------------------------------------------------------

void MainWindowFrame::onGameCharactersChanged() {
    entityView->updateCharacterList(gameWorldController->getGameMap()->getGameCharacters());
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
        case EditorDialogTypes::AlterCharacter: return editCharacterDialog ? false : true;
        case EditorDialogTypes::EditTileDescription: return editTileDescriptionDialog ? false : true;
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

        case EditorDialogTypes::AlterCharacter:
            assert(editCharacterDialog != NULL);
            delete editCharacterDialog;
            editCharacterDialog = NULL;
            break;

        case EditorDialogTypes::EditTileDescription:
            assert(editTileDescriptionDialog != NULL);
            delete editTileDescriptionDialog;
            editTileDescriptionDialog = NULL;
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
/// startEditObjectDialog
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
/// finishedAlterObjectDialog
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

///----------------------------------------------------------------------------
/// startEditCharacterDialog
///----------------------------------------------------------------------------

bool MainWindowFrame::startEditCharacterDialog(GameCharacter::Builder& characterBuilder, const bool editingCharacter) {

    editCharacterDialog = new (std::nothrow) EditCharacterDialog(this, gameWorldController->getGameMap(), GetHwnd(), editingCharacter);

    if (!editCharacterDialog) {
        return false;
    }

    editCharacterDialog->Create(GetHwnd(), DIALOG_EX_STYLES,
                             DIALOG_WS_STYLES);

    if (!editCharacterDialog->IsWindow()) {
        return false;
    }

    CString caption;
    LanguageMapper& langMap = LanguageMapper::getInstance();

    if (!editingCharacter) {
        caption = LM_toUTF8("CreateCharacterTitle", langMap);
    }
    else {
        caption = LM_toUTF8("EditCharacterTitle", langMap);
        caption.Format(caption, AtoW(characterBuilder.base.description[GameObjectDescriptions::Name].c_str(), CP_UTF8).c_str());
    }

    editCharacterDialog->setCharacterToEdit(characterBuilder.build());

    makeDialogModal(*editCharacterDialog, caption);

    return true;

}

///----------------------------------------------------------------------------
/// finishedAlterCharacterDialog
///----------------------------------------------------------------------------

void MainWindowFrame::finishedAlterCharacterDialog() {

    assert(editCharacterDialog != NULL);

    if (editCharacterDialog->hasSavedChanges()) {

        if (editCharacterDialog->isEditingCharacter()) {
            gameWorldController->tryReplaceCharacter(editCharacterDialog->getAlteredCharacter());
        }
        else {
            gameWorldController->tryAddCharacter(editCharacterDialog->getAlteredCharacter());
        }
    }

    onDialogEnd(EditorDialogTypes::AlterCharacter);

}

///----------------------------------------------------------------------------
/// startEditTileDescriptionDialog
///----------------------------------------------------------------------------

bool MainWindowFrame::startEditTileDescriptionDialog(const std::string& name, const std::string& description) {

    editTileDescriptionDialog = new (std::nothrow) EditTileDescriptionDialog(this, GetHwnd());

    if (!editTileDescriptionDialog) {
        return false;
    }

    editTileDescriptionDialog->Create(GetHwnd(), DIALOG_EX_STYLES,
                                DIALOG_WS_STYLES);

    if (!editTileDescriptionDialog->IsWindow()) {
        return false;
    }

    CString caption;
    LanguageMapper& langMap = LanguageMapper::getInstance();

    caption = LM_toUTF8("CreateCharacterTitle", langMap);

    editTileDescriptionDialog->setTileDescription(name, description);

    makeDialogModal(*editTileDescriptionDialog, caption);

    return true;
}

///----------------------------------------------------------------------------
/// finishedEditTileDescriptionDialog
///----------------------------------------------------------------------------

void MainWindowFrame::finishedEditTileDescriptionDialog() {

    assert(editTileDescriptionDialog != NULL);

    if (editTileDescriptionDialog->hasSavedChanges()) {
        gameWorldController->tryUpdateTileDescription(editTileDescriptionDialog->getTileName(),
                                                      editTileDescriptionDialog->getTileDescription());
    }

    onDialogEnd(EditorDialogTypes::EditTileDescription);

}