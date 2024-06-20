#ifndef GLOBALS_H
#define GLOBALS_H

#ifndef NOMINMAX
#define NOMINMAX
#endif // !NOMINMAX

#ifndef UNICODE
#define UNICODE
#endif // !UNICODE

#ifndef STRICT
#define STRICT
#endif // !STRICT

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <chrono>
#include <numeric>
#include <filesystem>
#include <fstream>

#include "imgui/imgui.h"
#include "nexus/Nexus.h"
#include "mumble/Mumble.h"

#include "Constants.h"
#include "Settings.h"

#include "entity/GW2API_WvW.h"
#include "entity/GW2API_Worlds.h"

#include "ArcDPS.h"
#include "service/PipsCalculator.h"
#include "service/HttpClient.h"
#include "service/WorldInventory.h"

extern AddonDefinition AddonDef;
extern HMODULE hSelf;
extern AddonAPI* APIDefs;
extern Mumble::Data* MumbleLink;
extern NexusLinkData* NexusLink;

extern AutoPipsCalculator autoPipsCalculator;
extern WorldInventory worldInventory;

extern gw2api::wvw::Match* match;

extern Settings settings;

extern std::string accountName;
extern bool unloading;

/* Utility */
namespace fs = std::filesystem;
using json = nlohmann::json;

inline bool isInWvW() {
	Mumble::EMapType mapType = MumbleLink->Context.MapType;

	switch (mapType) {
		// A WvW Map: yeeeee
		case Mumble::EMapType::WvW_RedBorderlands: return true;
		case Mumble::EMapType::WvW_GreenBorderlands: return true;
		case Mumble::EMapType::WvW_BlueBorderlands: return true;
		case Mumble::EMapType::WvW_EdgeOfTheMists: return true;
		case Mumble::EMapType::WvW_EternalBattlegrounds: return true;
		case Mumble::EMapType::WvW_Lounge: return true;
		case Mumble::EMapType::WvW_ObsidianSanctum: return true;
		case Mumble::EMapType::WVW_FortunesVale: return true;
		// Not a WvW map: boooo
		default: return false;
	}
}

inline void replaceAll(std::string& str, const std::string& from, const std::string& to) {
	size_t startPos = 0;
	while ((startPos = str.find(from, startPos)) != std::string::npos) {
		str.replace(startPos, from.length(), to);
		startPos += to.length(); // Move past the replaced substring
	}
}
inline std::string getAddonFolder() {
	std::string pathFolder = APIDefs->GetAddonDirectory(ADDON_NAME);
	// Create folder if not exist
	if (!fs::exists(pathFolder)) {
		try {
			fs::create_directory(pathFolder);
		}
		catch (const std::exception& e) {
			std::string message = "Could not create addon directory: ";
			message.append(pathFolder);
			APIDefs->Log(ELogLevel::ELogLevel_CRITICAL, ADDON_NAME, message.c_str());

			// Suppress the warning for the unused variable 'e'
#pragma warning(suppress: 4101)
			e;
		}
	}
	return pathFolder;
}
inline bool contains_value(const std::vector<int>& vec, int value) {
	return std::find(vec.begin(), vec.end(), value) != vec.end();
}
#endif // GLOBALS_H