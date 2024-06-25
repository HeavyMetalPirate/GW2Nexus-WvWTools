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

void WorldInventory::addObjectives(std::vector<gw2api::wvw::Location> objectives, gw2api::wvw::Match* match) {
	for (auto objective : objectives) {

		// Sanitize objectives
		int redTeamId, greenTeamId, blueTeamId;
		redTeamId = match->all_worlds["red"][0];
		greenTeamId = match->all_worlds["green"][0];
		blueTeamId = match->all_worlds["blue"][0];
		
		// sectors RBL blue: 1311; sectors red: 1343; sectors green: 1350
		// sectors BBL blue: 976, 980; sectors red: 977?; sectors green: 974? << verify red and green because I got blue again... T_T
		// sectors GBL blue: 1000; sectors red: 997; sectors green: 992, 993
		// sectors EBG blue: 834, 836 ; sectors red: 843, 845; sectors green: 848, 850

		if (objective.sector_id == 834 || objective.sector_id == 836 || objective.sector_id == 1311 || objective.sector_id == 976 || objective.sector_id == 980 || objective.sector_id == 1000) {
			objective.name = this->getAlliance(blueTeamId)->name + " " + objective.name;
		}
		else if (objective.sector_id == 1350 || objective.sector_id == 974 || objective.sector_id == 992 || objective.sector_id == 993 || objective.sector_id == 848 || objective.sector_id == 850) {
			objective.name = this->getAlliance(greenTeamId)->name + " " + objective.name;
		}
		else if (objective.sector_id == 1343 || objective.sector_id == 977 || objective.sector_id == 997 || objective.sector_id == 843 || objective.sector_id == 845) {
			objective.name = this->getAlliance(redTeamId)->name + " " + objective.name;
		}
		
		gw2api::wvw::Location* obj = new gw2api::wvw::Location(objective);
		loadedObjectives.emplace(obj->id, obj);
	}
}

gw2api::wvw::Location* WorldInventory::getObjective(std::string id) {
	if (this->loadedObjectives.count(id))
		return this->loadedObjectives[id];
	else
		return nullptr;
}

void WorldInventory::addObjectiveUpgrades(std::vector<gw2api::wvw::ObjectiveUpgrade> upgrades) {
	for (auto objective : upgrades) {
		gw2api::wvw::ObjectiveUpgrade* obj = new gw2api::wvw::ObjectiveUpgrade(objective);
		loadedObjectiveUpgrades.emplace(obj->id, obj);
	}
}

gw2api::wvw::ObjectiveUpgrade* WorldInventory::getObjectiveUpgrade(int id) {
	if (this->loadedObjectiveUpgrades.count(id))
		return this->loadedObjectiveUpgrades[id];
	else
		return nullptr;
}