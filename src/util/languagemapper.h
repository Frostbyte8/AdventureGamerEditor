#ifndef __LANGUAGEMAPPER_H__
#define __LANGUAGEMAPPER_H__

#include <map>
#include <vector>
#include <string>
#include "../compat/stdint_compat.h"

///----------------------------------------------------------------------------
/// LanguageMapper 
///----------------------------------------------------------------------------

struct LanguagePack {
    std::string fileName;
    std::string displayName;
};

namespace LanguagePackConstants {
    const int UseDefaultLanguage = -1;
}

class LanguageMapper {

	public:

        static LanguageMapper& getInstance() {
            static LanguageMapper mapper;
            return mapper;
        }

        std::string get(const std::string& key) const;

        const std::vector<LanguagePack>& getLanguagePacks() const { return languagePacks; }

        const std::string& getPackName(const size_t ID) const {return languagePacks[ID].fileName; }
		
		bool tryLoadDefaultLanguage();

        bool tryLoadLangauge(const std::string& filePath, const std::string& fileName);

        const bool isLoaded() const;

        void updatePackList(const std::vector<LanguagePack>& packs);

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
        std::vector<LanguagePack> languagePacks;
        bool mapperLoaded;
        int defaultLangNumStrings;
        int loadedPackID;
};

void findLanguagePacks(std::vector<LanguagePack>& outPacks);

#endif // __LANGUAGEMAPPER_H__
