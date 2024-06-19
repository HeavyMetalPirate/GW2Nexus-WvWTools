#include "WorldInventory.h"

WorldInventory::WorldInventory() {

}

void WorldInventory::addAlliance(gw2api::worlds::alliance* alliance) {
	this->loadedAlliances[alliance->id] = alliance;
}
gw2api::worlds::alliance* WorldInventory::getAlliance(int id) {
	if (this->loadedAlliances.count(id))
		return this->loadedAlliances[id];
	else
		return nullptr;
}
std::vector<gw2api::worlds::alliance*> WorldInventory::getAllAlliances() {
	std::vector < gw2api::worlds::alliance*> alliances = std::vector<gw2api::worlds::alliance*>();

	for (auto w : this->loadedAlliances) {
		alliances.push_back(w.second);
	}

	std::sort(alliances.begin(), alliances.end(), [](const auto& a, const auto& b) {
		return a->name < b->name;
	});

	return alliances;
}