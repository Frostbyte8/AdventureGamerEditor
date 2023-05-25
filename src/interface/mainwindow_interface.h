#ifndef __MAINWINDOW_INTERFACE_H__
#define __MAINWINDOW_INTERFACE_H__

#include "shared_interface_definitions.h"
#include <string>

class MainWindowInterface {

    public:

        ///--------------------------------------------------------------------
        /// Sent when the program wants to asks a Yes/No question
        /// @param The question to ask
        /// @param Title of the message box
        /// @param if true, the user should have a way of canceling the operation
        /// @return an Integer indicating what the user selected. See MainWindowInterfaceResponses
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
        /// Sent when window is being told that the user wants to add, edit
        /// place or delete a character
        /// @param an integer specifying the type of action to take.
        ///--------------------------------------------------------------------
        virtual void onAlterCharacter(const int& alterType, const size_t& index) = 0;

        ///--------------------------------------------------------------------
        /// Sent when window is being told that the user wants to add, edit
        /// place or delete an object
        /// @param an integer specifying the type of action to take.
        ///--------------------------------------------------------------------
        virtual void onAlterObject(const int& alterType, const size_t& index) = 0;

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
        /// Sent when the Edit Character Dialog Window is finished
        ///--------------------------------------------------------------------
        virtual void finishedEditCharacterDialog(const int& alterType, const bool& wasCanceled) = 0;

        ///--------------------------------------------------------------------
        /// Sent when the Edit Object Dialog Window is finished
        ///--------------------------------------------------------------------
        virtual void finishedEditObjectDialog(const int& alterType, const bool& wasCanceled, const bool& pressedApply) = 0;

        ///--------------------------------------------------------------------
        /// Sent when the Edit Story Dialog Window is finished
        ///--------------------------------------------------------------------
        virtual void finishedEditStoryDialog(const bool& wasCanceled, const bool& pressedApply) = 0;

        ///--------------------------------------------------------------------
        /// Sent when the Edit World Info Dialog is finished
        ///--------------------------------------------------------------------
        virtual void finishedEditWorldInfoDialog(const bool& wasCanceled) = 0;

        ///--------------------------------------------------------------------
        /// Sent when window is being told that the edit tile description
        /// dialog is closed.
        ///--------------------------------------------------------------------
        virtual void finishedEditTileDescriptionDialog(const bool& wasCanceled, const bool& pressedApply) = 0;

};

#endif // __MAINWINDOW_INTERFACE_H__
