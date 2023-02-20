#ifndef __LANGUAGEMAPPER_H__
#define __LANGUAGEMAPPER_H__

#include <map>
#include <string>

// TODO: Load JSON file with default definitions
// Locking the mapper to prevent adding invalid key

class LanguageMapper {

	public:

		std::string getLangString(const unsigned int& key) {
			
			std::string outString = "Err: String not Found";
			
			std::map<unsigned int, std::string>::iterator it;
			
			it = languageMap.find(key);
			
			if(it != languageMap.end()) {
				outString = it->second;
			}

			return outString;
		}

		bool addLangString(const unsigned int& key, const std::string& value) {
			languageMap[key] = value;
			return true;
		}

	private:
		std::map<unsigned int, std::string> languageMap;
};

#endif // __LANGUAGEMAPPER_H__