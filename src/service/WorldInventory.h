#ifndef WORLD_INVENTORY_H
#define WORLD_INVENTORY_H

#include <map>
#include "../entity/GW2API_Worlds.h"

class WorldInventory {
public:
	WorldInventory();

	void addAlliance(gw2api::worlds::alliance* alliance);
	gw2api::worlds::alliance* getAlliance(int id);
	std::vector < gw2api::worlds::alliance*> getAllAlliances();

private:
	std::map<int, gw2api::worlds::alliance*> loadedAlliances = std::map<int, gw2api::worlds::alliance*>();
};

#endif /* WORLD_INVENTORY_H */
