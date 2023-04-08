#ifndef __MAINWINDOW_INTERFACE_H__
#define __MAINWINDOW_INTERFACE_H__

#include <string>

namespace MainWindowInterfaceResponses {
    int const No        = 0;
    int const Yes       = 1;
    int const Cancel    = 2;
}

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
        virtual int AskYesNoQuestion(const std::string& question, const std::string& title,
                                     bool allowCancel) = 0;

        ///--------------------------------------------------------------------
        /// Sent when the program wants to display an error message
        /// @param The error message
        /// @param Title of the message box, if necessary.
        ///--------------------------------------------------------------------
        virtual void DisplayErrorMessage(const std::string& message, const std::string& title) = 0;
};

#endif // __MAINWINDOW_INTERFACE_H__
