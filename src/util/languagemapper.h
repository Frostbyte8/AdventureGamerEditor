#ifndef __LANGUAGEMAPPER_H__
#define __LANGUAGEMAPPER_H__

#include <map>
#include <string>
#include "../compat/stdint_compat.h"

///----------------------------------------------------------------------------
/// LanguageMapper 
///----------------------------------------------------------------------------

class LanguageMapper {

	public:

        static LanguageMapper& getInstance() {
            static LanguageMapper mapper;
            return mapper;
        }

        std::string get(const std::string& key) const;
		
		bool tryLoadDefaultLanguage();

        bool tryLoadLangauge(const std::string& filePath, const std::string& fileName);

        const bool isLoaded() const;

	private:

        bool add(const std::string& key, const std::string& value);

        bool LoadLanguageFile(const std::string& data);

        static uint32_t keyToHash(const std::string& key) {

            uint32_t hashValue = 0;
            const size_t stringLength = key.length();
            size_t i = 0;
            
            for(i = 0; i + 3 < stringLength; i+=4) {
                hashValue = (hashValue * 31) + (key[i] | (key[i+1] << 8) | 
                                               (key[i+2] << 16) | (key[i+3] << 24));
            }

            const size_t remainder = stringLength - i;

            if(remainder) {
                if(remainder == 3) {
                    hashValue = (hashValue * 31) + (key[i] | (key[i+1] << 8) | (key[i+2] << 16));
                }
                else if(remainder == 2) {
                    hashValue = (hashValue * 31) + (key[i] | (key[i+1] << 8));
                }
                else {
                    hashValue = (hashValue * 31) + key[i];
                }
            }

            return hashValue;
        }

        LanguageMapper();
        LanguageMapper(const LanguageMapper&) {};
        void operator=(const LanguageMapper&) {};
        
		std::map<uint32_t, std::string> languageMap;
        bool mapperLoaded;
};

#endif // __LANGUAGEMAPPER_H__
