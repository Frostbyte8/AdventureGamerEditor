#ifndef __DIALOGBASE_INTERFACE_H__
#define __DIALOGBASE_INTERFACE_H__

#include "shared_interface_definitions.h"
#include <string>

class DialogBaseInterface {

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
        virtual void displayErrorMessage(const std::string& inMessage, const std::string& inTitle) = 0;
  
        ///--------------------------------------------------------------------
        /// Sent when the data of the dialog needs to be saved.
        /// @return true if the save was successful, false if it was not.
        ///--------------------------------------------------------------------
        virtual bool saveData() = 0;
};

#endif // __DIALOGBASE_INTERFACE_H__