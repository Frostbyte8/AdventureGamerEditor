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

		std::string get(const unsigned int& key) const {
			
			std::string outString = "Err: String not Found";
			
			std::map<unsigned int, std::string>::const_iterator it;
			
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

    // Edit Object Dialog

    // Tab Labels
    int const DescriptionTab            = 400;
    int const QualitiesTab              = 401;
    int const EffectsTab                = 402;
    int const LocationsTab              = 403;

    // Descriptions Tab
    int const DescriptionsGroupLabel    = 404;
    int const NameLabel                 = 405;
    int const SightLabel                = 406;
    int const UseLabel                  = 407;
    int const LastUseLabel              = 408;
    int const IconLabel                 = 409;
    int const SoundLabel                = 410;
    int const BrowseButton              = 411;

    // Qualities Tab

    int const FlagsGroupLabel           = 412;
    int const MasterKeyFlag             = 413;
    int const InvisibleFlag             = 414;
    int const ClimbFlag                 = 415;
    int const ProtectFlag               = 416;
    int const TorchFlag                 = 417;
    int const WornFlag                  = 418;
    int const FixedFlag                 = 419;
    int const MoneyFlag                 = 420;

    int const PropertiesGroupLabel      = 421;
    int const MonetaryLabel             = 422;
    int const UsesLabel                 = 423;
    int const ObjectHeldLabel           = 424;

    // Effects Tab

    int const EffectsOnAttribGroup      = 425;
    int const ConsumpativeLabel         = 426;
    int const ReversedLabel             = 427;

    int const ObjectAttributesGroup     = 428;
    int const ObjectBaseAmountLabel     = 429;
    int const ObjectRandomAmountLabel   = 430;
    int const PositiveSignLabel         = 431;
    int const NegativeSignLabel         = 432;

    int const ObjectEnergyLabel         = 433;
    int const ObjectSkillLabel          = 434;
    int const ObjectWillpowerLabel      = 435;
    int const ObjectLuckLabel           = 436;
    int const ObjectTorchlifeLabel      = 437;

    int const MakesPlayersSightLabel    = 438;
    int const SightNoEffectOption       = 439;
    int const SightPlayerBlindOption    = 440;
    int const SightPlayerNormalOption   = 441;
    int const SightPlayerInfraredOption = 442;

    int const MakesPlayersHearingLabel  = 443;
    int const HearingNoEffectOption     = 444;
    int const HearingPlayerDeafOption   = 445;
    int const HearingPlayerNormalOption = 446;
    int const HearingPlayerSonicOption  = 447;



    // Error Messages

    int const TileGetError              = 10001;
    int const TileGetErrorCaption       = 10002;
    int const FileReadError             = 10003;
    int const FileReadErrorCaption      = 10004;

}

#endif // __LANGUAGEMAPPER_H__
