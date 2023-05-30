#ifndef __LANGUAGEMAPPER_H__
#define __LANGUAGEMAPPER_H__

#include <map>
#include <string>
#include <fstream>
#include "../compat/stdint_compat.h"
#include "../compat/std_extras_compat.h"
#include "../thirdparty/simpleson/json.h"

///----------------------------------------------------------------------------
/// LanguageMapper 
///----------------------------------------------------------------------------

class LanguageMapper {

	public:

        static LanguageMapper& getInstance() {
            static LanguageMapper mapper;
            return mapper;
        }

        /*
		std::string get(const unsigned int& key) const {
			
			std::string outString;
			
			std::map<unsigned int, std::string>::const_iterator it;
			
			it = languageMap.find(key);
			
			if(it != languageMap.end()) {
				outString = it->second;
			}
            else {

                it = languageMap.find(LanguageConstants::ErrStringNotFound);

                if(it != languageMap.end()) {
                    outString = it->second;
                }
                else {
                    outString = "Err: String not found, and Language File is missing strings!";
                }
            }

			return outString;
		}
        */

        std::string get(const std::string& key) const {
			
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

	private:

        bool add(const std::string& key, const std::string& value) {
			languageMap[keyToHash(key)] = value;
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

                std::string key;

		        for(it = keyList.begin(); it != keyList.end(); ++it) {
			        add(*it, result.get(*it));
		        }

                return true;
            }

            return false;

        }

        static uint32_t keyToHash(const std::string& key) {
            uint32_t hashValue = 0;
            const size_t stringLength = key.length();
            
            // TODO: We can do 4 of these at a time
            for(size_t i = 0; i < stringLength; ++i) {
                hashValue = (hashValue * 31) + key[i];
            }

            return hashValue;
        }

        LanguageMapper() {
            LoadLanguageFile("","");
        }

        LanguageMapper(const LanguageMapper&) {};
        void operator=(const LanguageMapper&) {};
		std::map<uint32_t, std::string> languageMap;
};

#endif // __LANGUAGEMAPPER_H__
