#include "gameworld_controller.h"
#include "../compat/std_extras_compat.h"
#include "../thirdparty/simpleson/json.h"
#include <fstream>

bool GameWorldController::LoadLanguageFile(const std::string& filePath, const std::string& fileName) {

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
			languageMapper.addLangString(key, result.get(*it));
		}
    }

    ifs.close();

    return true;
}
