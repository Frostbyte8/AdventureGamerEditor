#ifndef __LANGUAGEMAPPER_H__
#define __LANGUAGEMAPPER_H__

#include <map>
#include <string>
#include <fstream>
#include "../compat/std_extras_compat.h"
#include "../thirdparty/simpleson/json.h"

// TODO: Deal with runtime errors

class LanguageMapper {

	public:

        static LanguageMapper& getInstance() {
            static LanguageMapper mapper;
            return mapper;
        }

		std::string get(const unsigned int& key) {
			
			std::string outString = "Err: String not Found";
			
			std::map<unsigned int, std::string>::iterator it;
			
			it = languageMap.find(key);
			
			if(it != languageMap.end()) {
				outString = it->second;
			}

			return outString;
		}

	private:

		bool add(const unsigned int& key, const std::string& value) {
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
			        add(key, result.get(*it));
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

///----------------------------------------------------------------------------
/// LanguageConstants - Used by the translation system of the editor.
///----------------------------------------------------------------------------

namespace LanguageConstants {

    int const FileMenuItem      = 100;
    int const NewMenuItem       = 101;
    int const OpenMenuItem      = 102;
    int const SaveMenuItem      = 103;
    int const SaveAsMenuItem    = 104;
    int const ExitMenuItem      = 105;

    int const ObjectAddButton       = 201;
    int const ObjectEditButton      = 202;
    int const ObjectPlaceButton     = 203;
    int const ObjectDeleteButton    = 204;

    int const CharacterAddButton    = 205;
    int const CharacterEditButton   = 206;
    int const CharacterPlaceButton  = 207;
    int const CharacterDeleteButton = 208;

    int const ObjectGroupboxCaption     = 209;
    int const CharacterGroupboxCaption  = 210;

    int const MainWindowCaption         = 301;

    // Error Messages

    int const TileGetError              = 10001;
    int const TileGetErrorCaption       = 10002;
    int const FileReadError             = 10003;
    int const FileReadErrorCaption      = 10004;

}

#endif // __LANGUAGEMAPPER_H__