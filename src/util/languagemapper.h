#ifndef __LANGUAGEMAPPER_H__
#define __LANGUAGEMAPPER_H__

#include <map>
#include <string>
#include <fstream>
#include "../compat/std_extras_compat.h"
#include "../thirdparty/simpleson/json.h"


// TODO: Load JSON file with default definitions
// Locking the mapper to prevent adding invalid key

class LanguageMapper {

	public:

        static LanguageMapper& getInstance() {
            static LanguageMapper mapper;
            return mapper;
        }

		std::string getLangString(const unsigned int& key) {
			
			std::string outString = "Err: String not Found";
			
			std::map<unsigned int, std::string>::iterator it;
			
			it = languageMap.find(key);
			
			if(it != languageMap.end()) {
				outString = it->second;
			}

			return outString;
		}

	private:

		bool addString(const unsigned int& key, const std::string& value) {
			languageMap[key] = value;
			return true;
		}

        bool LoadLanguageFile(const std::string& filePath, const std::string& fileName) {

	        std::string fileNameTemp = "lang_en.json";
	        std::ifstream ifs;
	        ifs.open(fileNameTemp.c_str(), std::ifstream::in | std::ios::binary);

	        if(ifs) {
		        std::string data;
		        ifs.seekg(0, std::ios::end);
		        data.resize(ifs.tellg());
		        ifs.seekg(0, std::ios::beg);
		        ifs.read(&data[0], data.size());
		        ifs.close();

		        json::jobject result = json::jobject::parse(data);
		        json::key_list_t keyList = result.list_keys();
		        json::key_list_t::iterator it;

		        for(it = keyList.begin(); it != keyList.end(); ++it) {
			        int key = std::stoi(*it);
			        addString(key, result.get(*it));
		        }

                return true;
            }

            return false;

        }

        LanguageMapper() {
            LoadLanguageFile("","");
        }

        LanguageMapper(const LanguageMapper&) {};
        void operator=(const LanguageMapper&) {};
		std::map<unsigned int, std::string> languageMap;
};

#endif // __LANGUAGEMAPPER_H__