#ifndef __MAINWINDOW_INTERFACE_H__
#define __MAINWINDOW_INTERFACE_H__

#include <string>

namespace MainWindowInterfaceResponses {
    const int No        = 0;
    const int Yes       = 1;
    const int Cancel    = 2;
}

class MainWindowInterface {

    public:
        virtual int AskYesNoQuestion(const std::string& question, const std::string& title, bool allowCancel) = 0;
        virtual void DisplayErrorMessage(const std::string& message, const std::string& title) = 0;
};

#endif // __MAINWINDOW_INTERFACE_H__
