#pragma once

#include "..\macros.h"

#include "..\Menu\Menu.h"
#include "..\Menu\Routine.h"

#include "..\Natives\natives2.h"
#include "..\Util\ExePath.h"
#include "..\Scripting\GTAped.h"
#include "..\Scripting\Game.h"
#include "..\Util\StringManip.h"

#include <string>
#include <array>
#include <vector>
#include <set>
#include <simpleini\SimpleIni.h>

typedef unsigned short uint16_t;

namespace sub
{
	namespace Speech
	{
		struct SpeechNameS { std::string title, label; };
		extern std::vector<SpeechNameS> vSpeechNames;
		struct VoiceNameS { std::string title, label; };
		extern std::vector<VoiceNameS> vVoiceNames;
		struct AmbientSpeechDataS { std::string title, voiceName, speechName, paramName; };
		extern const std::vector<AmbientSpeechDataS> vSpeechData;
		struct SpeechParamS { std::string title, label; };
		extern const std::array<SpeechParamS, 37> vSpeechParams;

		struct AmbientVoice_t
		{
			std::string voiceName;
			std::vector<std::string> speechNames;
		};
		extern std::vector<AmbientVoice_t> vVoiceData;
		extern AmbientVoice_t* _currVoiceInfo;
		extern uint16_t _currSpeechParamIndex;

		bool PopulateVoiceData();
		void VoiceChangerMenu();
		void AmbientSpeechPlayerMenu();
		void Sub_AmbientSpeechPlayer_InVoice();

	}

}




