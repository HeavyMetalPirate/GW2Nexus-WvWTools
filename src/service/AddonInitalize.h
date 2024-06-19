#ifndef INITIALIZE_SERVICE_H
#define INITIALIZE_SERVICE_H

#include "../Globals.h"
#include "../resource.h"

#include <Windows.h>

namespace fs = std::filesystem;

static int on_extract_entry(const char* filename, void* arg) {
	static int i = 0;
	int n = *(int*)arg;

	APIDefs->Log(ELogLevel::ELogLevel_DEBUG, ADDON_NAME, filename);
	return 0;
}

static void unpackResource(const int resourceName, const std::string& resourceType, const std::string& targetFileName, bool overwrite = true) {

	std::wstring resourceTypeW(resourceType.begin(), resourceType.end());
	HRSRC hResource = FindResource(hSelf, MAKEINTRESOURCE(resourceName), resourceTypeW.c_str());
	if (hResource == NULL) {
		APIDefs->Log(ELogLevel::ELogLevel_CRITICAL, ADDON_NAME, ("Did not find resource: " + targetFileName).c_str());
		return;
	}

	HGLOBAL hLoadedResource = LoadResource(hSelf, hResource);
	if (hLoadedResource == NULL) {
		APIDefs->Log(ELogLevel::ELogLevel_CRITICAL, ADDON_NAME, ("Could not load resource: " + targetFileName).c_str());
		return;
	}

	LPVOID lpResourceData = LockResource(hLoadedResource);
	if (lpResourceData == NULL) {
		APIDefs->Log(ELogLevel::ELogLevel_CRITICAL, ADDON_NAME, ("Could not lock resource: " + targetFileName).c_str());
		return;
	}

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
	std::string outputPath = pathFolder + "/" + targetFileName;

	if (fs::exists(outputPath) && !overwrite) {
		APIDefs->Log(ELogLevel::ELogLevel_INFO, ADDON_NAME, ("Resource already exists, and should not be overwritten: " + targetFileName).c_str());
		return;
	}

	FILE* file = nullptr;
	errno_t err = fopen_s(&file, outputPath.c_str(), "wb");
	if (err != 0 || file == nullptr) {
		APIDefs->Log(ELogLevel::ELogLevel_CRITICAL, ADDON_NAME, ("Error trying to write (fopen_s): " + targetFileName).c_str());
		return;
	}

	size_t resourceSize = SizeofResource(hSelf, hResource);
	fwrite(lpResourceData, 1, resourceSize, file);

	fclose(file);
	APIDefs->Log(ELogLevel::ELogLevel_INFO, ADDON_NAME, (targetFileName + " extracted from module.").c_str());
}


static void unpackResources() {
	unpackResource(IDR_JSON_ALLIANCES, "JSON", "alliances.json");
}


void initializeAlliances() {
	try {
		// Get addon directory
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


		if (unloading) return;
		// Load events from data.json
		std::string pathData = pathFolder + "/alliances.json";
		if (fs::exists(pathData)) {
			std::ifstream dataFile(pathData);

			if (dataFile.is_open()) {
				json jsonData;
				dataFile >> jsonData;
				dataFile.close();

				std::vector<gw2api::worlds::alliance> alliances = jsonData.get<std::vector<gw2api::worlds::alliance>>();

				for (auto alliance : alliances) {
					gw2api::worlds::alliance* a = new gw2api::worlds::alliance(alliance);
					worldInventory.addAlliance(a);
				}
			}
		}
		
	}
	catch (const std::exception& e) {
		APIDefs->Log(ELogLevel_CRITICAL, ADDON_NAME, "Exception in alliance initialization thread.");
		APIDefs->Log(ELogLevel_CRITICAL, ADDON_NAME, e.what());
	}
	catch (...) {
		APIDefs->Log(ELogLevel_CRITICAL, ADDON_NAME, "Unknown exception in alliance initialization thread.");
	}
	APIDefs->Log(ELogLevel::ELogLevel_INFO, ADDON_NAME, "Alliance loading from storage complete.");
}

#endif
