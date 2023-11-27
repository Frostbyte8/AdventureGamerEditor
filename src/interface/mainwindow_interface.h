#ifndef __MAINWINDOW_INTERFACE_H__
#define __MAINWINDOW_INTERFACE_H__

#include "shared_interface_definitions.h"
#include "../model/gameobject.h"
#include "../model/gamecharacter.h"
#include "../model/gameinfo.h"
#include <string>

namespace EditorDialogTypes {
    const int AlterObject           = 0;
    const int AlterCharacter        = 1;
    const int EditStoryAndSummary   = 2;
    const int EditTileDescription   = 3;
    const int EditWorldInfo         = 4;
    const int ResizeWorld           = 5;
    const int SaveDialog            = 6; // Win32++ doesn't need this, but another view might.
    const int LoadDialog            = 7; // Win32++ doesn't need this, but another view might.
}

namespace EditorTileUpdateFlags {
    const int Description       = 0x01;
    const int Type              = 0x02;
    const int Both              = 0x03;
}

namespace EditorConnectionUpdateFlags {
    const int Jumps             = 0x01;
    const int Switches          = 0x02;
    const int Both              = 0x03;
}

class MainWindowInterface {

    public:

        ///--------------------------------------------------------------------
        /// Sent when the program wants to asks a Yes/No question
        /// @param The question to ask
        /// @param Title of the message box
        /// @param if true, the user should have a way of canceling the operation
        /// @return an Integer indicating what the user selected. See GenericInterfaceResponses
        /// for more information
        ///--------------------------------------------------------------------
        virtual int askYesNoQuestion(const std::string& question, const std::string& title,
                                     bool allowCancel) = 0;

        ///--------------------------------------------------------------------
        /// Sent when the program wants to display an error message
        /// @param The error message
        /// @param Title of the message box, if necessary.
        ///--------------------------------------------------------------------
        virtual void displayErrorMessage(const std::string& message, const std::string& title) = 0;

        ///--------------------------------------------------------------------
        /// Sent when the program wants to display a standard message
        /// @param The message
        /// @param Title of the message box, if necessary.
        /// @param A GenericInterfaceMessageTypes
        ///--------------------------------------------------------------------
        virtual void displayMessage(const std::string& message, const std::string& title, const int& type) = 0;

        ///--------------------------------------------------------------------
        /// Sent when the window is being asked if it can create a dialog of a
        /// specific type.
        /// @param Which dialog to try and create
        /// @returns true if it can create the dialog, false if it cannot
        /// create it or an invalid dialog type is given.
        ///--------------------------------------------------------------------
        virtual bool canCreateDialog(const int& whichDialogType) const = 0;

        ///--------------------------------------------------------------------
        /// Sent when a dialog has been closed
        /// @param Which dialog was closed
        ///--------------------------------------------------------------------
        virtual void onDialogEnd(const int& whichDialogType) = 0;
       
        ///--------------------------------------------------------------------
        /// Sent when window is being told that the user wants to add or edit
        /// an object. Place and Delete do not have dialog boxes, at least
        /// not at this time with Win32.
        /// @param A GameObject::Builder containing the information about the
        /// object we are editing. This is ignored if an object is being added.
        /// @param a true if editing an existing object, false if adding a new
        /// object
        /// @return true if the dialog was created, false if it was not.
        ///--------------------------------------------------------------------
        virtual bool startEditObjectDialog(GameObject::Builder& objectBuilder, const bool editingObject) = 0;

        ///--------------------------------------------------------------------
        /// Sent when the Edit Object Dialog Window is finished
        ///--------------------------------------------------------------------
        virtual void finishedAlterObjectDialog() = 0;

        ///--------------------------------------------------------------------
        /// Sent when window is being told that the user wants to add or edit
        /// a character. Place and Delete do not have dialog boxes, at least
        /// not at this time with Win32.
        /// @param A GameCharacter::Builder containing the information about
        /// the character we are editing. This is ignored if an character is
        /// being added.
        /// @param a true if editing an existing character, false if adding a
        /// character
        /// @return true if the dialog was created, false if it was not.
        ///--------------------------------------------------------------------
        virtual bool startEditCharacterDialog(GameCharacter::Builder& objectCharacter, const bool editingCharacter) = 0;

        ///--------------------------------------------------------------------
        /// Sent when the Edit Character Dialog Window is finished
        ///--------------------------------------------------------------------
        virtual void finishedAlterCharacterDialog() = 0;

        ///--------------------------------------------------------------------
        /// Sent to notify the main window that the characters and objects
        /// list may have changed
        /// @param the object list has changed in some relevant way
        /// @param the objects here list needs to be updated.
        /// @param the character list has changed in some relevant way
        /// @param the characters here list needs to be updated.        
        ///--------------------------------------------------------------------
        virtual void onEntitiesChanged(const bool& objectsListChanged, const bool& objectsHereChanged,
                                       const bool& charsListChanged, const bool& charsHereChanged) = 0;
                     
        ///--------------------------------------------------------------------
        /// Sent when window is being told that the user wants to edit the
        /// name/description of a tile.
        /// @param String containing the current name of the tile.
        /// @param String containing the current description of the tile.
        /// @return true if the dialog was created, false if it was not.
        ///--------------------------------------------------------------------
        virtual bool startEditTileDescriptionDialog(const std::string& name, const std::string& description) = 0;

        ///--------------------------------------------------------------------
        /// Sent when window is being told that the edit tile description
        /// dialog is finished
        ///--------------------------------------------------------------------
        virtual void finishedEditTileDescriptionDialog() = 0;

        ///--------------------------------------------------------------------
        /// Sent when the tile the user is editing has changed
        ///--------------------------------------------------------------------
        virtual void onSelectedTileChanged() = 0;

        ///--------------------------------------------------------------------
        /// Sent to notify the window that an individual tile has been updated.
        /// @param an integer specifying the index of the tile being updated
        /// @param an integer containing flags of how the tile is being updated
        ///--------------------------------------------------------------------
        virtual void onTileUpdated(const int& index, const int& tileUpdateFlags) = 0;

        ///--------------------------------------------------------------------
        /// Sent when the user selected a new tile to draw with
        ///--------------------------------------------------------------------
        virtual void onDrawingTileChanged() = 0;

        ///--------------------------------------------------------------------
        /// Sent when window is being told that the user wants to edit the 
        /// Game World's story/summary
        /// @param a string containing the game's story
        /// @param a string containing the game's summary
        ///--------------------------------------------------------------------
        virtual bool startEditStoryAndSummaryDialog(const std::string& story, const std::string& summary) = 0;

        ///--------------------------------------------------------------------
        /// Sent when the Edit Story Dialog Window is finished
        ///--------------------------------------------------------------------
        virtual void finishedEditStoryAndSummaryDialog() = 0;

        ///--------------------------------------------------------------------
        /// Sent when the Story and/or Summary were updated.
        ///--------------------------------------------------------------------
        virtual void onStoryAndSummaryUpdated() = 0;

        ///--------------------------------------------------------------------
        /// Sent when window is being told that the user wants to edit the 
        /// Game World's properties
        /// @param A reference to the current world's properties (GameInfo)
        /// @return true if the dialog was created successfully, false if it
        /// was not
        ///--------------------------------------------------------------------
        virtual bool startEditWorldInfoDialog(const GameInfo& gameInfo) = 0;

        ///--------------------------------------------------------------------
        /// Sent when the Edit World Info Dialog is finished
        ///--------------------------------------------------------------------
        virtual void finishedEditWorldInfoDialog() = 0;

        ///--------------------------------------------------------------------
        /// Sent when the World's info was updated
        ///--------------------------------------------------------------------
        virtual void onWorldInfoUpdated() = 0;

        ///--------------------------------------------------------------------
        /// Sent when window is being told that the user wants to edit the 
        /// Game World's dimensions
        /// @param A reference to the number of rows the game world currently
        /// has.
        /// @param a reference to the number of cols the game world currently
        /// has
        /// @return true if the dialog was created successfully, false if it
        /// was not
        ///--------------------------------------------------------------------
        virtual bool startResizeWorldDialog(const int& numRows, const int& numCols) = 0;

        ///--------------------------------------------------------------------
        /// Sent when window is being told that the resize world dialog is
        /// finished
        ///--------------------------------------------------------------------
        virtual void finishedResizeWorldDialog() = 0;

        ///--------------------------------------------------------------------
        /// Sent when the dimensions of the game world have changed
        ///--------------------------------------------------------------------
        virtual void onWorldResized() = 0;

        ///--------------------------------------------------------------------
        /// Sent when the State of game World (New/Close/Open) has changed
        /// and certain operations may need to be enabled/disabled
        ///--------------------------------------------------------------------
        virtual void onWorldStateChanged() = 0;

        ///--------------------------------------------------------------------
        /// Sent when window is being told that the user wants to save the
        /// game, but it needs file path info.
        /// @return true if the dialog was created, false if it was not.
        ///--------------------------------------------------------------------
        virtual bool startSaveDialog() = 0;

        ///--------------------------------------------------------------------
        /// Sent when window is being told that the save dialog has finished.
        ///--------------------------------------------------------------------
        virtual void finishedSaveDialog() = 0;

        ///--------------------------------------------------------------------
        /// Sent when window when changes were saved, and you might want to
        /// indicate that.
        ///--------------------------------------------------------------------
        virtual void onChangesSaved() = 0;

        ///--------------------------------------------------------------------
        /// Sent when window is being told that the user wants to load the
        /// game, but it needs file path info.
        /// @return true if the dialog was created, false if it was not.
        ///--------------------------------------------------------------------
        virtual bool startLoadDialog() = 0;

        ///--------------------------------------------------------------------
        /// Sent when window is being told that the load dialog has finished.
        ///--------------------------------------------------------------------
        virtual void finishedLoadDialog() = 0;


};

#endif // __MAINWINDOW_INTERFACE_H__
