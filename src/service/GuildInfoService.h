#ifndef GUILD_INVENTORY_H
#define GUILD_INVENTORY_H

#ifndef NOMINMAX
#define NOMINMAX
#endif // !NOMINMAX

#ifndef UNICODE
#define UNICODE
#endif // !UNICODE

#ifndef STRICT
#define STRICT
#endif // !STRICT

#include <thread>
#include <queue>

#include "../nexus/Nexus.h"

#include "HttpClient.h"
#include "../entity/GW2API_Guilds.h"
#include "../Constants.h"

class GuildInfoService {
public:
	void setAPIDefs(AddonAPI* api) {
		this->APIDefs = api;
	}

	std::string getGuildName(std::string guildId);
	void startLoaderThread();
	void stopLoaderThread();

private:
	AddonAPI* APIDefs = nullptr;
	bool unloading = false;
	std::thread loaderThread;

	std::map<std::string, std::string> guildnames = std::map<std::string, std::string>();
	std::map<std::string, bool> requestedIds = std::map<std::string, bool>();
	void loadGuildFromApi(std::string id);
};

#endif
