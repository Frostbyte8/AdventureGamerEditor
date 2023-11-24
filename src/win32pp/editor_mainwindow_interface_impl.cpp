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
/// askYesNoQuestion
///----------------------------------------------------------------------------

int MainWindowFrame::askYesNoQuestion(const std::string& inQuestion, const std::string& inTitle,
                                      bool allowCancel) {

    const CString question      = AtoW(inQuestion.c_str(), CP_UTF8);
    const CString title         = AtoW(inTitle.c_str(), CP_UTF8);
    const UINT messageBoxFlags  = MB_ICONQUESTION | (allowCancel ? MB_YESNOCANCEL : MB_YESNO);

    const int retVal = MessageBox(question, title, messageBoxFlags);

    if (retVal == IDYES) {
        return GenericInterfaceResponses::Yes;
    }

    return GenericInterfaceResponses::No;
}

///----------------------------------------------------------------------------
/// displayErrorMessage
///----------------------------------------------------------------------------

void MainWindowFrame::displayErrorMessage(const std::string& inMessage,
                                          const std::string& inTitle) {

    const CString message       = AtoW(inMessage.c_str(), CP_UTF8);
    const CString title         = AtoW(inTitle.c_str(), CP_UTF8);
    const UINT messageBoxFlags  = MB_ICONERROR;

    MessageBox(message, title, messageBoxFlags);

}

///----------------------------------------------------------------------------
/// onEntitiesChanged
///----------------------------------------------------------------------------

void MainWindowFrame::onEntitiesChanged(const bool& objectsListChanged, const bool& objectsHereChanged,
                                        const bool& charsListChanged, const bool& charsHereChanged) {

    if (objectsListChanged) {
        entityView->updateObjectList(gameWorldController->getGameMap()->getGameObjects());
    }

    if (charsListChanged) {
        entityView->updateCharacterList(gameWorldController->getGameMap()->getGameCharacters());
    }

    if (objectsHereChanged || charsHereChanged) {
        updateHereLists(objectsHereChanged, charsHereChanged);
    }

    updateTitleBar(true);
}

///----------------------------------------------------------------------------
/// onSelectedTileChanged
///----------------------------------------------------------------------------

void MainWindowFrame::onSelectedTileChanged() {

    const GameMap* gameMap = gameWorldController->getGameMap();
    const int& selectedRow = gameWorldController->getSelectedRow();
    const int& selectedCol = gameWorldController->getSelectedCol();

    updateHereLists(true, true, gameMap, &selectedRow, &selectedCol);
    updateStatusbar(gameWorldController->getSelectedTileIndex());

    // Finally, we need to tell the map that it too needs to update.

    reinterpret_cast<GameMapPanel&>(gameMapDocker->GetView()).onNewTileSelected();

}

///----------------------------------------------------------------------------
/// onTileUpdated
///----------------------------------------------------------------------------

void MainWindowFrame::onTileUpdated(const int& index, const int& tileUpdateFlags) {

    // TODO: Status bar and Feature Menu only need to be updated if this tile
    // is the selected tile.

    if(tileUpdateFlags & EditorTileUpdateFlags::Description) {
        updateStatusbar(index);
    }

    if(tileUpdateFlags & EditorTileUpdateFlags::Type) {
        updateFeatureMenu(index);
    }

}

///----------------------------------------------------------------------------
/// onDrawingTileChanged
///----------------------------------------------------------------------------

void MainWindowFrame::onDrawingTileChanged() {
    reinterpret_cast<RoadPalettePanel&>(roadSelectorDocker->GetView()).onDrawingTileChanged();
}

///----------------------------------------------------------------------------
/// onStoryAndSummaryUpdated
///----------------------------------------------------------------------------

void MainWindowFrame::onStoryAndSummaryUpdated() {
    updateTitleBar(true);
}

///----------------------------------------------------------------------------
/// onWorldInfoUpdated
///----------------------------------------------------------------------------

void MainWindowFrame::onWorldInfoUpdated() {
    updateTitleBar(false);
}

///----------------------------------------------------------------------------
/// onWorldResized
///----------------------------------------------------------------------------

void MainWindowFrame::onWorldResized() {
    reinterpret_cast<GameMapPanel&>(gameMapDocker->GetView()).onMapSizeChanged();
    updateTitleBar(true);
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
        case EditorDialogTypes::EditStoryAndSummary: return editStoryDialog ? false : true;
        case EditorDialogTypes::EditWorldInfo: return editWorldInfoDialog ? false : true;
        case EditorDialogTypes::ResizeWorld: return resizeWorldDialog ? false : true;
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

        case EditorDialogTypes::EditStoryAndSummary:
            assert(editStoryDialog != NULL);
            delete editStoryDialog;
            editStoryDialog = NULL;
            break;

        case EditorDialogTypes::EditWorldInfo:
            assert(editWorldInfoDialog != NULL);
            delete editWorldInfoDialog;
            editWorldInfoDialog = NULL;
            break;

        case EditorDialogTypes::ResizeWorld:
            assert(resizeWorldDialog != NULL);
            delete resizeWorldDialog;
            resizeWorldDialog = NULL;
            break;

    }

    if(!IsWindowEnabled()) {
        MessageBox(L"Debug: Had to force the window to re-enable itself. Was this intended?", L"Debug Message", MB_OK);
        EnableWindow();
        SetFocus();
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

    caption = LM_toUTF8("EditTileDescTitle", langMap);

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

///-----------------------------------------------------------------------------
/// startEditStoryAndSummaryDialog
///-----------------------------------------------------------------------------

bool MainWindowFrame::startEditStoryAndSummaryDialog(const std::string& story, const std::string& summary) {

    editStoryDialog = new (std::nothrow) EditStoryDialog(this, GetHwnd());

    if (!editStoryDialog) {
        return false;
    }

    editStoryDialog->Create(GetHwnd(), DIALOG_EX_STYLES,
                            DIALOG_WS_STYLES);

    if (!editStoryDialog->IsWindow()) {
        return false;
    }

    CString caption;
    LanguageMapper& langMap = LanguageMapper::getInstance();

    caption = LM_toUTF8("EditTileDescTitle", langMap);

    editStoryDialog->setStoryAndSummary(story, summary);

    makeDialogModal(*editStoryDialog, caption);

    return true;

}

///----------------------------------------------------------------------------
/// finishedEditStoryAndSummaryDialog
///----------------------------------------------------------------------------

void MainWindowFrame::finishedEditStoryAndSummaryDialog() {

    assert(editStoryDialog != NULL);

    if (editStoryDialog->hasSavedChanges()) {
        gameWorldController->tryUpdateStoryAndSummary(editStoryDialog->getStory(),
                                                      editStoryDialog->getSummary());
    }

    onDialogEnd(EditorDialogTypes::EditStoryAndSummary);

}

///-----------------------------------------------------------------------------
/// startEditWorldInfoDialog
///-----------------------------------------------------------------------------

bool MainWindowFrame::startEditWorldInfoDialog(const GameInfo& gameInfo) {

    editWorldInfoDialog = new (std::nothrow) EditWorldInfoDialog(this, GetHwnd());

    if (!editWorldInfoDialog) {
        return false;
    }

    editWorldInfoDialog->Create(GetHwnd(), DIALOG_EX_STYLES,
                                DIALOG_WS_STYLES);

    if (!editWorldInfoDialog->IsWindow()) {
        return false;
    }

    CString caption;
    LanguageMapper& langMap = LanguageMapper::getInstance();

    caption = LM_toUTF8("EditWorldInfoTitle", langMap);

    editWorldInfoDialog->setWorldInfo(gameInfo);

    makeDialogModal(*editWorldInfoDialog, caption);

    return true;

}

///----------------------------------------------------------------------------
/// finishedEditStoryAndSummaryDialog
///----------------------------------------------------------------------------

void MainWindowFrame::finishedEditWorldInfoDialog() {

    assert(editWorldInfoDialog != NULL);

    if (editWorldInfoDialog->hasSavedChanges()) {
        gameWorldController->tryUpdateWorldInfo(editWorldInfoDialog->getGameInfo());
                                               
    }

    onDialogEnd(EditorDialogTypes::EditWorldInfo);

}

///-----------------------------------------------------------------------------
/// startResizeWorldDialog
///-----------------------------------------------------------------------------

bool MainWindowFrame::startResizeWorldDialog(const int& numRows, const int& numCols) {

    resizeWorldDialog = new (std::nothrow) ResizeWorldDialog(this, GetHwnd());

    if (!resizeWorldDialog) {
        return false;
    }

    resizeWorldDialog->Create(GetHwnd(), DIALOG_EX_STYLES,
                                DIALOG_WS_STYLES);

    if (!resizeWorldDialog->IsWindow()) {
        return false;
    }

    CString caption;
    LanguageMapper& langMap = LanguageMapper::getInstance();

    caption = LM_toUTF8("ResizeWorldTitle", langMap);

    resizeWorldDialog->setWorldDimensions(numCols, numRows);

    makeDialogModal(*resizeWorldDialog, caption);

    return true;

}

///-----------------------------------------------------------------------------
/// finishedResizeWorldDialog
///-----------------------------------------------------------------------------

void MainWindowFrame::finishedResizeWorldDialog() {

    assert(resizeWorldDialog != NULL);

    if (resizeWorldDialog->hasSavedChanges()) {
        gameWorldController->tryResizeWorld(resizeWorldDialog->getNewRows(), resizeWorldDialog->getNewCols());
    }

    onDialogEnd(EditorDialogTypes::ResizeWorld);

}