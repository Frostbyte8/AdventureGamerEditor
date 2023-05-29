#ifndef __SHARED_FUNCTIONS_H__
#define __SHARED_FUNCTIONS_H__

// TODO: Maybe Helper functions would be a better name for this? Maybe shared_win32pp_functions.h
// TODO: EOD isn't needed as all windows could benefit from these functions, not just the dialog.

#include <wxx_wincore.h>
#include <wxx_stdcontrols.h>
#include "../util/languagemapper.h"
#include "../util/win32pp_extras.h"

//=============================================================================
// Functions that rely on Langauge Mapper.
//=============================================================================

///----------------------------------------------------------------------------
/// LM_toUTF8 - Converts a Language Mapper string to a CString.
///----------------------------------------------------------------------------

inline CString LM_toUTF8(const std::string& ID, const LanguageMapper& langMap) {
    // TODO: Update string
    CString retVal = AtoW(langMap.get("FileMenu").c_str(), CP_UTF8);
    return retVal;
}

///----------------------------------------------------------------------------
/// LM_SetWindowText - Set the caption of the given control to the given 
/// languagemap string.
/// @param reference to a control.
/// @param reference to a CString to be filled with the new caption string.
/// @param constant reference to the instance of the Language mapper.
/// @param ID of the string to be used.
///----------------------------------------------------------------------------

inline void EOD_SetWindowText(const std::string& ID, CWnd& widget, CString& caption, const LanguageMapper& langMap) {
    // TODO: Update String
    caption = AtoW(langMap.get("FileMenu").c_str(), CP_UTF8);
    widget.SetWindowTextW(caption);
}

///----------------------------------------------------------------------------
/// LM_SetWindowText - Set the caption of the given control to the given 
/// std::string
/// @param reference to a control.
/// @param reference to a CString to be filled with the new caption string.
/// @param constant reference to the std::string containing the caption.
///----------------------------------------------------------------------------

inline void EOD_SetWindowText(const std::string& str, CWnd& widget, CString& caption) {
    caption = AtoW(str.c_str(), CP_UTF8);
    widget.SetWindowTextW(caption);
}

///----------------------------------------------------------------------------
/// LM_AddString - Adds a string from the language mapper to the combobox given
/// @param reference to a CCombobox.
/// @param reference to a CString to be filled with the new string.
/// @param constant reference to the instance of the Langauge Mapper.
/// @param ID of the string to be used.
///----------------------------------------------------------------------------

inline void EOD_AddString(const std::string& ID, CComboBox& widget, CString& caption, const LanguageMapper& langMap) {
    // TODO: Update String
    caption = AtoW(langMap.get("FileMenu").c_str(), CP_UTF8);
    widget.AddString(caption);
}

//=============================================================================
// Helper Functions
//=============================================================================

inline void centerWindowOnCurrentMonitor(const HMONITOR& currentMonitor, CWnd& window) {
    
    RECT rc = window.GetClientRect();
    
    MONITORINFOEX monitorInfo;
    monitorInfo.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfo(currentMonitor, &monitorInfo);

    AdjustWindowRectEx(&rc, window.GetStyle(), FALSE, window.GetExStyle());

    // Calculate Where the window is positioned, then offset it to where the monitor is.

    const CPoint windowPos((abs(monitorInfo.rcWork.right - monitorInfo.rcWork.left) / 2) - ((rc.right + abs(rc.left)) / 2) + monitorInfo.rcMonitor.left,
                           (abs(monitorInfo.rcWork.bottom - monitorInfo.rcWork.top) / 2) - ((rc.bottom + abs(rc.top)) / 2) + monitorInfo.rcMonitor.top);

    window.SetWindowPos(0, windowPos.x, windowPos.y, 0, 0, SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOSIZE | SWP_NOZORDER | SWP_NOREPOSITION);

}

// TODO: This shouldn't be inline, but removing the inline keyword prevents this from compiling, do this at some point.

inline void processValidatorError(std::string& errorMessage, std::string& errorTitle, const InputValidator* validator) {

    LanguageMapper& langMap = LanguageMapper::getInstance();
    const int errorCode = validator->getErrorCode();

    if(validator->getType() == validatorTypes::Integer) {
        const IntegerValidator* intValidator = reinterpret_cast<const IntegerValidator*>(validator);
            
        if(errorCode == errorCodes::OutOfRange) {
            
            // TODO: Update String
            errorMessage = langMap.get("FileMenu");

            // TODO: make this a loop

            size_t pos = errorMessage.find("%d", 0);
            if(pos != std::string::npos) {
                errorMessage.erase(pos, 2);
                std::string intVar = std::to_string(intValidator->getMinValue());
                errorMessage.insert(pos, intVar);
            }

            pos = errorMessage.find("%d", 0);
            if(pos != std::string::npos) {
                errorMessage.erase(pos, 2);
                std::string intVar = std::to_string(intValidator->getMaxValue());
                errorMessage.insert(pos, intVar);
            }

        }
        else if(errorCode == errorCodes::InvalidData) {
            // TODO: Update String
            errorMessage = langMap.get("FileMenu");
            errorTitle = "TODO: Make a title";
        }
    }
    else if(validator->getType() == validatorTypes::String) {
        
        const StringValidator* strValidator = reinterpret_cast<const StringValidator*>(validator);

        if(errorCode == errorCodes::TooManyChars) {
            errorMessage = "String has too many characters. The maximum amount of characters is %d.";

            size_t pos = errorMessage.find("%d", 0);
            if(pos != std::string::npos) {
                errorMessage.erase(pos, 2);
                std::string maxChars = std::to_string(strValidator->getMaxChars());
                errorMessage.insert(pos, maxChars);
            }

        }
        else if(errorCode == errorCodes::NotEnoughChars) {
            
            errorMessage = "String does not have enough chars. The minimum amount of characters is %d.";
            size_t pos = errorMessage.find("%d", 0);

            if(pos != std::string::npos) {
                errorMessage.erase(pos, 2);
                std::string minChars = std::to_string(strValidator->getMinChars());
                errorMessage.insert(pos, minChars);
            }

        }
        else if(errorCode == errorCodes::EndsWith) {
            
            errorMessage = "String must end in one of the following: ";
            
            const std::vector<std::string>& extenVec = strValidator->getEndsWith();
            
            const size_t numExten = extenVec.size();
            
            for(size_t i = 0; i < numExten; ++i) {
                
                errorMessage += extenVec[i].c_str();

                if(i != numExten - 1) {
                    errorMessage += ", ";
                }
                else {
                    errorMessage += ".";
                }
            }
        }

    }

}

#endif // __SHARED_FUNCTIONS_H__
