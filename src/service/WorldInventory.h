#ifndef WORLD_INVENTORY_H
#define WORLD_INVENTORY_H

#include <map>
#include "../entity/GW2API_Worlds.h"
#include "../entity/GW2API_WvW.h"

class WorldInventory {
public:
	WorldInventory();

	void addAlliance(gw2api::worlds::alliance* alliance);
	gw2api::worlds::alliance* getAlliance(int id);
	std::vector < gw2api::worlds::alliance*> getAllAlliances();

	void addObjectives(std::vector<gw2api::wvw::Location> objectives, gw2api::wvw::Match* match);
	gw2api::wvw::Location* getObjective(std::string objectiveId);

	void addObjectiveUpgrades(std::vector<gw2api::wvw::ObjectiveUpgrade> upgrades);
	gw2api::wvw::ObjectiveUpgrade* getObjectiveUpgrade(int id);
	
	std::string getGuildName(std::string guildId);

private:
	std::map<int, gw2api::worlds::alliance*> loadedAlliances = std::map<int, gw2api::worlds::alliance*>();
	std::map<std::string, gw2api::wvw::Location*> loadedObjectives = std::map<std::string, gw2api::wvw::Location*>();
	std::map<int, gw2api::wvw::ObjectiveUpgrade*> loadedObjectiveUpgrades = std::map<int, gw2api::wvw::ObjectiveUpgrade*>();
};

#endif /* WORLD_INVENTORY_H */
