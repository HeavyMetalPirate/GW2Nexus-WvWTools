#ifndef WVW_MATCH_SERVICE_H
#define WVW_MATCH_SERVICE_H

#include <thread>

#include "../Globals.h"
#include "HttpClient.h"

class WvWMatchService {
public:
	void loadObjectives();
	void loadObjectiveUpgrades();
	void loadMatchData();
	void startThread();
	void stopThread();
private:
	bool unloading = false;
	std::thread matchThread;

	void setAutoPipsCalculatorValues();
};

#endif