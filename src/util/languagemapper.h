#ifndef __LANGUAGEMAPPER_H__
#define __LANGUAGEMAPPER_H__

#include <map>
#include <string>
#include <fstream>
#include "../compat/std_extras_compat.h"
#include "../thirdparty/simpleson/json.h"

///----------------------------------------------------------------------------
/// LanguageConstants - Used by the translation system of the editor.
///----------------------------------------------------------------------------

namespace LanguageConstants {

    // Main Program Window

    int const FileMenuItem                  = 100;
    int const NewMenuItem                   = 101;
    int const OpenMenuItem                  = 102;
    int const SaveMenuItem                  = 103;
    int const SaveAsMenuItem                = 104;
    int const ExitMenuItem                  = 105;

    int const ObjectAddButton               = 201;
    int const ObjectEditButton              = 202;
    int const ObjectPlaceButton             = 203;
    int const ObjectDeleteButton            = 204;

    int const CharacterAddButton            = 205;
    int const CharacterEditButton           = 206;
    int const CharacterPlaceButton          = 207;
    int const CharacterDeleteButton         = 208;

    int const ObjectGroupboxCaption         = 209;
    int const CharacterGroupboxCaption      = 210;

    // Window Captions

    int const MainWindowCaption             = 301;
    int const AddObjectDialogCaption        = 302;
    int const EditObjectDialogCaption       = 303;
    int const AddCharacterDialogCaption     = 304;
    int const EditCharacterDialogCaption    = 305;

    // Edit Object Dialog
    // Tab Labels

    int const DescriptionTab                = 400;
    int const QualitiesTab                  = 401;
    int const EffectsTab                    = 402;
    int const LocationsTab                  = 403;

    // Descriptions Tab TODO: new names to distinguish
    // which of these are for objects, and which are for
    // characters

    int const DescriptionsGroupLabel        = 404;
    int const NameLabel                     = 405;
    int const SightLabel                    = 406;
    int const UseLabel                      = 407;
    int const LastUseLabel                  = 408;
    int const IconLabel                     = 409;
    int const SoundLabel                    = 410;
    int const BrowseButton                  = 411;

    // Qualities Tab

    int const FlagsGroupLabel               = 412;
    int const MasterKeyFlag                 = 413;
    int const InvisibleFlag                 = 414;
    int const ClimbFlag                     = 415;
    int const ProtectFlag                   = 416;
    int const TorchFlag                     = 417;
    int const WornFlag                      = 418;
    int const FixedFlag                     = 419;
    int const MoneyFlag                     = 420;

    int const PropertiesGroupLabel          = 421;
    int const MonetaryLabel                 = 422;
    int const UsesLabel                     = 423;
    int const ObjectHeldLabel               = 424;

    // Effects Tab

    int const EffectsOnAttribGroup          = 425;
    int const ConsumpativeLabel             = 426;
    int const ReversedLabel                 = 427;

    int const ObjectAttributesGroup         = 428;
    int const ObjectBaseAmountLabel         = 429;
    int const ObjectRandomAmountLabel       = 430;
    int const PositiveSignLabel             = 431;
    int const NegativeSignLabel             = 432;

    int const ObjectEnergyLabel             = 433;
    int const ObjectSkillLabel              = 434;
    int const ObjectWillpowerLabel          = 435;
    int const ObjectLuckLabel               = 436;
    int const ObjectTorchlifeLabel          = 437;

    int const MakesPlayersSightLabel        = 438;
    int const SightNoEffectOption           = 439;
    int const SightPlayerBlindOption        = 440;
    int const SightPlayerNormalOption       = 441;
    int const SightPlayerInfraredOption     = 442;

    int const MakesPlayersHearingLabel      = 443;
    int const HearingNoEffectOption         = 444;
    int const HearingPlayerDeafOption       = 445;
    int const HearingPlayerNormalOption     = 446;
    int const HearingPlayerSonicOption      = 447;

    // Locations Tab

    int const LocationGroupLabel            = 448;
    int const OnGroundAtLabel               = 449;
    int const PlayerStartsWithLabel         = 450;
    int const CharacterHasItLabel           = 451;
    int const UnlocksDoorAtLabel            = 452;

    // Combo Box Values
    int const NoObjectSelected              = 453;
    int const NoCharacterSelected           = 454;

    // Edit Character Dialog
    // Tab Labels

    int const CharDescriptonsTab            = 500;
    int const CharQualitiesTab              = 501;
    int const CharAttributesTab             = 502;
    int const CharMiscTab                   = 503;

    // Descriptions

    int const CharDescriptionsGroup         = 504;
    int const CharNameLabel                 = 505;
    int const CharOnSightLabel              = 506;
    int const CharOnFightLabel              = 507;
    int const CharOnDeathLabel              = 508;
    int const CharIconLabel                 = 509;
    int const CharSoundLabel                = 510;
    int const CharBrowseButtonCaption       = 511;

    // Flags

    int const CharFlagsGroup                = 512;
    int const CharEnterDark                 = 513;
    int const CharEnterHazard               = 514;
    int const CharUseJumppad                = 515;
    int const CharBypassGates               = 516;
    int const CharBypassDoors               = 517;
    int const CharClimbBarriers             = 518;
    int const CharWanders                   = 519;
    int const CharFollowPlayer              = 520;

    // Properties

    int const CharPropertiesGroup           = 521;
    int const CharMoneyLabel                = 522;
    int const CharTypeLabel                 = 523;
    int const CharMissionaryType            = 524;
    int const CharTraderType                = 525;
    int const CharFighterType               = 526;

    // Attributes

    int const CharAttributesGroup           = 527;
    int const CharAttribAmountLabel         = 528;
    int const CharAttribEnergy              = 529;
    int const CharAttribSkill               = 530;
    int const CharAttribWillpower           = 531;
    int const CharAttribLuck                = 532;

    int const CharSightLabel                = 533;
    int const CharSightNormal               = 534;
    int const CharSightBlind                = 535;
    int const CharSightInfrared             = 536;

    int const CharLocationGroup             = 537;
    int const CharInventoryGroup            = 538;

    // Generic Labels/Captions
    
    int const XCoordLabel                   = 801;
    int const YCoordLabel                   = 802;
    int const GenericOKButtonCaption        = 803;
    int const GenericCancelButtonCaption    = 804;
    int const GenericApplyButtonCaption     = 805;

    // Error Messages

    int const TileGetError                  = 10001;
    int const TileGetErrorCaption           = 10002;
    int const FileReadError                 = 10003;
    int const FileReadErrorCaption          = 10004;

    int const IntegerOutOfRange             = 10005;
    int const IntegerOutOfRangeTitle        = 10006;

    // Error for when a string is not found.

    int const ErrStringNotFound             = 32767;

}

///----------------------------------------------------------------------------
/// LanguageMapper 
///----------------------------------------------------------------------------

class LanguageMapper {

	public:

        static LanguageMapper& getInstance() {
            static LanguageMapper mapper;
            return mapper;
        }

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

#endif // __LANGUAGEMAPPER_H__
