#ifndef __MAINWINDOW_INTERFACE_H__
#define __MAINWINDOW_INTERFACE_H__

#include "shared_interface_definitions.h"
#include "../model/gameobject.h"
#include <string>

namespace EditorDialogTypes {
    int const AlterObject           = 0;
    int const AlterCharacter        = 1;
    int const EditStory             = 2;
    int const EditTileDescription   = 3;
    int const EditWorldInfo         = 4;
    int const ResizeWorld           = 5;
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
        ///--------------------------------------------------------------------
        virtual bool startEditObjectDialog(GameObject::Builder& objectBuilder, const bool editingObject) = 0;

        ///--------------------------------------------------------------------
        /// Sent when the Edit Object Dialog Window is finished
        ///--------------------------------------------------------------------
        virtual void finishedAlterObjectDialog() = 0;

        ///--------------------------------------------------------------------
        /// Sent to notify the main window that the object list has been
        /// changed
        ///--------------------------------------------------------------------
        virtual void onGameObjectsChanged() = 0;

        ///--------------------------------------------------------------------
        /// Sent when the Edit Character Dialog Window is finished
        ///--------------------------------------------------------------------
        virtual void finishedEditCharacterDialog() = 0;

        ///--------------------------------------------------------------------
        /// Sent when window is being told that the user wants to add, edit
        /// place or delete a character
        /// @param an integer specifying the type of action to take.
        ///--------------------------------------------------------------------
        virtual void onAlterCharacter(const int& alterType, const size_t& index) = 0;

        ///--------------------------------------------------------------------
        /// Sent when window is being told that the user wants to edit the 
        /// Game World's story
        ///--------------------------------------------------------------------
        virtual void onEditStory() = 0;

        ///--------------------------------------------------------------------
        /// Sent when window is being told that the user wants to edit a tile's 
        /// name or description.
        ///--------------------------------------------------------------------
        virtual void onEditTileDescription() = 0;

        ///--------------------------------------------------------------------
        /// Sent when window is being told that the user wants to edit the 
        /// Game World's information
        ///--------------------------------------------------------------------
        virtual void onEditWorldInfo() = 0;

        ///--------------------------------------------------------------------
        /// Sent when window is being told that the user wants to resize the
        /// Game World
        ///--------------------------------------------------------------------
        virtual void onResizeWorld() = 0;

        ///--------------------------------------------------------------------
        /// Sent when the user is trying to select a new tile
        /// @return true if the selection was valid, false if it was not
        ///--------------------------------------------------------------------
        virtual bool onSelectedTileChanged(const int& row, const int& col) = 0;

        ///--------------------------------------------------------------------
        /// Sent when a tile is changed
        ///--------------------------------------------------------------------
        virtual void onTileUpdated(const int& index) = 0;

        ///--------------------------------------------------------------------
        /// Sent when the Edit Story Dialog Window is finished
        ///--------------------------------------------------------------------
        virtual void finishedEditStoryDialog() = 0;

        ///--------------------------------------------------------------------
        /// Sent when the Edit World Info Dialog is finished
        ///--------------------------------------------------------------------
        virtual void finishedEditWorldInfoDialog() = 0;

        ///--------------------------------------------------------------------
        /// Sent when window is being told that the edit tile description
        /// dialog is finished
        ///--------------------------------------------------------------------
        virtual void finishedEditTileDescriptionDialog() = 0;

        ///--------------------------------------------------------------------
        /// Sent when window is being told that the resize world dialog is
        /// finished
        ///--------------------------------------------------------------------
        virtual void finishedResizeWorldDialog() = 0;

        ///--------------------------------------------------------------------
        /// Sent when the window needs to spawn the Save Dialog
        /// @param path to file
        /// @param file name
        /// @return an Integer indicating what the user selected. See GenericInterfaceResponses
        /// for more information
        ///--------------------------------------------------------------------
        virtual int onSaveFileDialog(std::string& filePath, std::string& fileName) = 0;

};

#endif // __MAINWINDOW_INTERFACE_H__
