#include "GuildInfoService.h"

void GuildInfoService::loadGuildFromApi(std::string id) {
	APIDefs->Log(ELogLevel_INFO, ADDON_NAME, ("Loading info for guild id " + id).c_str());
	try {
		std::string url = baseUrl + "/v2/guild/" + id;
		std::string response = HTTPClient::GetRequest(url);
		if (response == "") {
			APIDefs->Log(ELogLevel_CRITICAL, ADDON_NAME, "Empty Response from Guild API. Certain functionality might not be fully available.");
			return;
		}
		json guildJson = json::parse(response);
		gw2api::guilds::Guild guild = guildJson;

		guildnames[id] = guild.name;
	}
	catch (const std::exception& e) {
		APIDefs->Log(ELogLevel_CRITICAL, ADDON_NAME, e.what());
	}
	catch (...) {
		APIDefs->Log(ELogLevel_CRITICAL, ADDON_NAME, "Could not load guild info. Certain functionality might not be fully available.");
	}
}

void GuildInfoService::startLoaderThread() {
	loaderThread = std::thread([&] {
		while (true) {
			if (unloading) break;
			Sleep(50);
			if (requestedIds.size() == 0) continue;

			std::vector<std::string>::iterator iter;
			for (auto id: requestedIds) {
				if (id.second == true) continue;
				loadGuildFromApi(id.first);
				requestedIds[id.first] = true;
				if (unloading) break;
			}
		}
		APIDefs->Log(ELogLevel_INFO, ADDON_NAME, "Guild Info Thread ended successfully.");
	});
	loaderThread.detach();
}

void GuildInfoService::stopLoaderThread() {
	this->unloading = true;
	if (loaderThread.joinable()) {
		loaderThread.join();
	}
	APIDefs->Log(ELogLevel_INFO, ADDON_NAME, "Guild Info Thread stopped.");
}

std::string GuildInfoService::getGuildName(std::string guildId) {
	if (this->guildnames.count(guildId)) {
		return this->guildnames[guildId];
	}
	this->guildnames.emplace(guildId, guildId); // place temp to only request once
	this->requestedIds.emplace(guildId, false); // place request
	return guildId;
}