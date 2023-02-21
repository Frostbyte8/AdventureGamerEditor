#ifndef __LANGUAGE_CONTROLLER_H__
#define __LANGUAGE_CONTROLLER_H__

#include "../util/languagemapper.h" // TODO: move to model

class LanguageController {

    public:
        bool LoadLanguageFile(const std::string& filePath, const std::string& fileName);

        // TODO: return a copy of this
        LanguageMapper& getLanguageMapper() { return languageMapper; }
        LanguageMapper languageMapper;
};

#endif // __LANGUAGE_CONTROLLER_H__
