#include <fstream>
#include "languagemapper.h"
#include "../compat/stdint_compat.h"
#include "../compat/std_extras_compat.h"
#include "../thirdparty/simpleson/json.h"
#include "../default_language.h"

LanguageMapper::LanguageMapper() : mapperLoaded(false) {
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

