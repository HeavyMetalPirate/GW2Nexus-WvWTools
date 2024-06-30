#include "WvWMatchService.h"

bool objectivesLoaded = false;
bool upgradesLoaded = false;
bool possibleStaleData = false;

std::chrono::time_point<std::chrono::system_clock> lastUpdate = std::chrono::system_clock::now();

void WvWMatchService::loadObjectives() {
	if (match == nullptr) return; // we need the match data for sanitizing objective names
	try {
		std::string url = baseUrl + "/v2/wvw/objectives?ids=all";
		std::string matchResponse = HTTPClient::GetRequest(url);
		if (matchResponse == "") {
			APIDefs->Log(ELogLevel_CRITICAL, ADDON_NAME, "Empty Response from WvW API. Certain functionality might not be fully available.");
			return;
		}
		json matchJson = json::parse(matchResponse);
		std::vector<gw2api::wvw::Location> objectives = matchJson;
		worldInventory.addObjectives(objectives, match);
		APIDefs->Log(ELogLevel_INFO, ADDON_NAME, "Loaded objectives information.");
		objectivesLoaded = true;
	}
	catch (const std::exception& e) {
		APIDefs->Log(ELogLevel_CRITICAL, ADDON_NAME, e.what());
	}
	catch (...) {
		APIDefs->Log(ELogLevel_CRITICAL, ADDON_NAME, "Could not load WvW match data. Certain functionality might not be fully available.");
	}
}

void WvWMatchService::loadObjectiveUpgrades() {
	try {
		std::string url = baseUrl + "/v2/guild/upgrades?ids=all";
		std::string matchResponse = HTTPClient::GetRequest(url);
		if (matchResponse == "") {
			APIDefs->Log(ELogLevel_CRITICAL, ADDON_NAME, "Empty Response from WvW API. Certain functionality might not be fully available.");
			return;
		}
		json matchJson = json::parse(matchResponse);
		std::vector<gw2api::wvw::ObjectiveUpgrade> upgrades = matchJson;
		worldInventory.addObjectiveUpgrades(upgrades);
		APIDefs->Log(ELogLevel_INFO, ADDON_NAME, "Loaded objective upgrade information.");
		upgradesLoaded = true;
	}
	catch (const std::exception& e) {
		APIDefs->Log(ELogLevel_CRITICAL, ADDON_NAME, e.what());
	}
	catch (...) {
		APIDefs->Log(ELogLevel_CRITICAL, ADDON_NAME, "Could not load WvW match data. Certain functionality might not be fully available.");
	}
}

void WvWMatchService::loadMatchData() {
	int allianceId = 0;

	if (settings.accountSettings.count(accountName) > 0) {
		allianceId = settings.accountSettings[accountName].allianceId;
	}
	else {
		allianceId = settings.accountSettings[genericAccount].allianceId;
	}

	if (allianceId == 0) return; // no allianceId selected
	
	try {
		gw2api::wvw::Kills kills;
		if (match == nullptr) {
			kills = {0,0,0};
		}
		else {
			kills = gw2api::wvw::Kills(match->kills);
		}
		
		std::string url = baseUrl + "/v2/wvw/matches?world=" + std::to_string(allianceId);
		std::string matchResponse = HTTPClient::GetRequest(url);
		if (matchResponse == "") {
			APIDefs->Log(ELogLevel_CRITICAL, ADDON_NAME, "Empty Response from WvW API. Certain functionality might not be fully available.");
			return;
		}
		json matchJson = json::parse(matchResponse);
		gw2api::wvw::Match m = matchJson;

		if (kills.red == m.kills.red && kills.blue == m.kills.blue && kills.green == m.kills.green) {
			possibleStaleData = true;
		}
		else {
			possibleStaleData = false;
		}

		delete match; // does this even help? idk and SO is snob land
		match = new gw2api::wvw::Match(m);

		setAutoPipsCalculatorValues();
		lastUpdate = std::chrono::system_clock::now();
	}
	catch (const std::exception& e) {
		APIDefs->Log(ELogLevel_CRITICAL, ADDON_NAME, e.what());
	}
	catch (...) {
		APIDefs->Log(ELogLevel_CRITICAL, ADDON_NAME, "Could not load WvW match data. Certain functionality might not be fully available.");
	}
}

void WvWMatchService::startThread() {
	matchThread = std::thread([&] {
		while (true) {


			// load current match data
#ifndef NDEBUG
			APIDefs->Log(ELogLevel_TRACE, ADDON_NAME, "Querying Match Data...");
#endif // !NDEBUG

			loadMatchData();

			// optional data loads
			if (!objectivesLoaded)
				loadObjectives();
			if (!upgradesLoaded)
				loadObjectiveUpgrades();

#ifndef NDEBUG
			APIDefs->Log(ELogLevel_TRACE, ADDON_NAME, "Querying done, sleeping...");
#endif // !NDEBUG
			// sleep for a minute total, checking every ms if we are unloading
			for (int i = 0; i < 60000; i++) {
				Sleep(1);
				if (unloading) break;
			}
#ifndef NDEBUG
			APIDefs->Log(ELogLevel_TRACE, ADDON_NAME, "Sleeping done, checking unload condition...");
#endif // !NDEBUG
			if (unloading) break;
#ifndef NDEBUG
			APIDefs->Log(ELogLevel_TRACE, ADDON_NAME, "Continuing...");
#endif // !NDEBUG
		}		
		APIDefs->Log(ELogLevel_INFO, ADDON_NAME, "Match Info Thread ended successfully.");
	});
	matchThread.detach();
}

void WvWMatchService::stopThread() {
	unloading = true;
	if (matchThread.joinable()) {
		matchThread.join();
	}
	APIDefs->Log(ELogLevel_INFO, ADDON_NAME, "Match Info Thread stopped.");
}

void WvWMatchService::setAutoPipsCalculatorValues() {
	if (match == nullptr) return;
	std::chrono::system_clock::time_point oldMatchEnd, currentMatchBegin;

	AccountSettings* accountSettings;
	if (settings.accountSettings.count(accountName) > 0) {
		accountSettings = &settings.accountSettings[accountName];
	}
	else {
		accountSettings = &settings.accountSettings[genericAccount];
	}

	if (!accountSettings->matchEnd.empty())
		oldMatchEnd = parse_date(accountSettings->matchEnd);
	else
		oldMatchEnd = parse_date("1970-01-01T02:00:00Z");

	currentMatchBegin = parse_date(match->start_time);

	if (oldMatchEnd <= currentMatchBegin) {
		// we have a new match weee!
		// check if old match was last week or before
		auto duration = currentMatchBegin - oldMatchEnd;
		auto days = std::chrono::duration_cast<std::chrono::hours>(duration).count() / 24;

		if (accountSettings->pipsProgressed >= 100 && days <= 7) {
			accountSettings->hasCommitment = true;
			autoPipsCalculator.commitment = true;
		}
		// set to new match stuff
		accountSettings->pipsProgressed = 0;
		accountSettings->matchBegin = match->start_time;
		accountSettings->matchEnd = match->end_time;

		// reset also the auto pips calculator stuff
		autoPipsCalculator.setPipsProgress(0);
	}

	std::string userTeam;

	for (const auto& pair : match->all_worlds) {
		if (contains_value(pair.second, accountSettings->allianceId)) {
			userTeam = pair.first;
		}
	}

	if (userTeam.empty()) {
		APIDefs->Log(ELogLevel_CRITICAL, ADDON_NAME, ("Could not find team name for user allianceId " + std::to_string(accountSettings->allianceId)).c_str());
		return;
	}
	// get the score of the last skirmish
	gw2api::wvw::Skirmish skirmish = match->skirmishes[match->skirmishes.size() - 1];

	std::vector<std::pair<std::string, int>> score_list = {
			{"red", skirmish.scores.red},
			{"blue", skirmish.scores.blue},
			{"green", skirmish.scores.green}
	};

	// Sort in descending order based on the scores
	std::sort(score_list.begin(), score_list.end(), [](const auto& a, const auto& b) {
		return a.second > b.second;
	});
	int rank = 0;
	for (int i = 0; i < score_list.size(); ++i) {
		if (score_list[i].first == userTeam) {
			rank = i + 1;
		}
	}
	autoPipsCalculator.setServerRank(rank);
}