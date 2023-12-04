#include <fstream>
#include "languagemapper.h"
#include "../util/frost.h"
#include "../compat/stdint_compat.h"
#include "../compat/std_extras_compat.h"
#include "../thirdparty/simpleson/json.h"
#include "../default_language.h"

LanguageMapper::LanguageMapper() : mapperLoaded(false), loadedPackID(LanguagePackConstants::UseDefaultLanguage) {
}

std::string LanguageMapper::get(const std::string& key) const {
	
	std::string outString;

    std::map<uint32_t, std::string>::const_iterator it = languageMap.find(keyToHash(key));
			
	if(it != languageMap.end()) {
		outString = it->second;
	}
    else {
        outString = "Error: String not found.";
    }
    
	return outString;
}
	
bool LanguageMapper::tryLoadDefaultLanguage() {

    std::string* data = getDefaultLanguageForLangMap();
    
    if(data == NULL) {
        return false;
    }

    try {
        if(!LoadLanguageFile(*data)) {
            delete data;
            data = NULL;
            return false;
        }
    }
    catch(json::parsing_error e) {
        delete data;
        data = NULL;
        throw(json::parsing_error(e.what()));
        return false;
    }		    
    
    delete data;
    data = NULL;

    mapperLoaded = true;
    return true;
}

bool LanguageMapper::tryLoadLangauge(const std::string& filePath, const std::string& fileName) {
    
    std::string fileNameTemp = "lang_en.json";
    std::ifstream ifs;
    ifs.open(fileNameTemp.c_str(), std::ifstream::in | std::ios::binary);
    	        
    if(!ifs) {
        return false;
    }
    
    std::string data;
    
    ifs.seekg(0, std::ios::end);
    data.resize(static_cast<size_t>(ifs.tellg()));
    ifs.seekg(0, std::ios::beg);
    ifs.read(&data[0], data.size());
    ifs.close();
    
    if(!LoadLanguageFile(data)) {
        mapperLoaded = false;
        return false;
    }

    mapperLoaded = true;
    return true;
}

const bool LanguageMapper::isLoaded() const {
    return mapperLoaded;
}


bool LanguageMapper::add(const std::string& key, const std::string& value) {
	languageMap[keyToHash(key)] = value;
	return true;
}

bool LanguageMapper::LoadLanguageFile(const std::string& data) {

    json::jobject result = json::jobject::parse(data);
    json::key_list_t keyList = result.list_keys();
    json::key_list_t::iterator it;

    std::string key;

    for(it = keyList.begin(); it != keyList.end(); ++it) {
        add(*it, result.get(*it));
    }

    return true;

}

void LanguageMapper::updatePackList(const std::vector<LanguagePack>& packs) {

    languagePacks.clear();

    const size_t numPacks = packs.size();

    for (size_t i = 0; i < numPacks; ++i) {
        languagePacks.push_back(packs[i]);
    }

}

#ifdef _WIN32

#include <wxx_filefind.h>

void findLanguagePacks(std::vector<LanguagePack>& outPacks) {

    const DWORD bufferLength = GetCurrentDirectory(0, NULL);

#ifdef __WIN9X_COMPAT__
    char* filePath = new char[bufferLength+2];
#else 
    wchar_t* filePath = new wchar_t[bufferLength+2];
#endif 

    const DWORD retVal = GetCurrentDirectory(bufferLength, &filePath[0]);
    LanguageMapper& langMap = LanguageMapper::getInstance();
    CString newPath;

    if(!retVal) {
        delete[] filePath;
        filePath = NULL;
        return;
    }
    else {
        newPath = CString(filePath);
        delete[] filePath;
        filePath = NULL;
    }
    
    // We have to do all this because \\?\ disables the use of "." and "..".
    // But of course MSDN doesn't directly tell you this. While it does say it
    // on the page they link to, it's not under the heading they tell you to
    // read.

    newPath = "\\\\?\\" + newPath + "\\lang_*.json";

    CFileFind fileList;

    if (fileList.FindFirstFile(newPath)) {

        do {
            const CString& fileName = fileList.GetFileName();
            const int firstChar = fileName.Find(_T("_"));
            const int lastChar = fileName.Find(_T(".json"));

            if(firstChar != 0 && lastChar != 0) {
                std::string name = TtoA(fileName.Mid(firstChar + 1, (lastChar - (firstChar + 1))), CP_UTF8);

                LanguagePack lp;
                lp.displayName = name;
                lp.fileName = TtoA(fileName, CP_UTF8);
                outPacks.push_back(lp);

            }
        } while (fileList.FindNextFile());
        
    }

}

#endif