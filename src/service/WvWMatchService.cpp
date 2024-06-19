#include "WvWMatchService.h"

void WvWMatchService::loadMatchData() {
	if (settings.allianceId == 0) return; // no allianceId selected
	
	try {
		std::string url = baseUrl + "/v2/wvw/matches?world=" + std::to_string(settings.allianceId);
		std::string matchResponse = HTTPClient::GetRequest(url);
		if (matchResponse == "") {
			APIDefs->Log(ELogLevel_CRITICAL, ADDON_NAME, "Empty Response from WvW API. Certain functionality might not be fully available.");
			return;
		}
		json matchJson = json::parse(matchResponse);
		gw2api::wvw::Match m = matchJson;

		delete match; // does this even help? idk and SO is snob land
		match = new gw2api::wvw::Match(m);

		setAutoPipsCalculatorValues();
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
		
	});
	matchThread.detach();
}

void WvWMatchService::stopThread() {
	unloading = true;
	if (matchThread.joinable()) {
		matchThread.join();
	}
}

void WvWMatchService::setAutoPipsCalculatorValues() {
	if (match == nullptr) return;
	std::chrono::system_clock::time_point oldMatchEnd, currentMatchBegin;

	if (!settings.matchEnd.empty())
		oldMatchEnd = parse_date(settings.matchEnd);
	else
		oldMatchEnd = parse_date("1970-01-01T02:00:00Z");

	currentMatchBegin = parse_date(match->start_time);

	if (oldMatchEnd <= currentMatchBegin) {
		// we have a new match weee!
		// check if old match was last week or before
		auto duration = currentMatchBegin - oldMatchEnd;
		auto days = std::chrono::duration_cast<std::chrono::hours>(duration).count() / 24;

		if (settings.pipsProgressed >= 100 && days <= 7) {
			settings.hasCommitment = true;
			autoPipsCalculator.commitment = true;
		}
		// set to new match stuff
		settings.pipsProgressed = 0;
		settings.matchBegin = match->start_time;
		settings.matchEnd = match->end_time;

		// reset also the auto pips calculator stuff
		autoPipsCalculator.setPipsProgress(0);
	}

	std::string userTeam;

	for (const auto& pair : match->all_worlds) {
		if (contains_value(pair.second, settings.allianceId)) {
			userTeam = pair.first;
		}
	}

	if (userTeam.empty()) {
		APIDefs->Log(ELogLevel_CRITICAL, ADDON_NAME, ("Could not find team name for user allianceId " + std::to_string(settings.allianceId)).c_str());
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