#ifndef PIPS_CALCULATOR_H
#define PIPS_CALCULATOR_H

#include "../Constants.h"

struct PipsResult {
	int pipsPerTick;
	int finishedPips;
	int ticketsEarned;
	int timeRemainingMinutes;
};

class PipsCalculator {
public: 
	// Rewards chest
	int chestsDone = 0;

	// World rank
	int rank = 3;

	// Personal rank
	int pRank1 = 0;

	// bonus
	bool commitment = false;
	bool commander = false;
	bool publicCommander = false;

	// calculation
	PipsResult calculate() {

		int pips = rank + (pRank1 + 4) + (commitment ? 1 : 0) + (commander ? 1 : 0) + (publicCommander ? 3 : 0);

		int pipsDone = 0;
		int ticketsDone = 0;
		if (chestsDone <= 4) {
			// Wood Tier: multiply selectedCurrent by pipsPerChestTier[1]
			pipsDone = chestsDone * pipsPerChestTier[1];
			ticketsDone = chestsDone * ticketsPerChestTier[1];
			if (chestsDone == 4) ticketsDone += 5; // bonus tickets from final
		}
		else if (chestsDone <= 8) {
			// Bronze Tier: 4*wood plus (selected - 4)*bronze
			pipsDone = 4 * pipsPerChestTier[1];
			pipsDone += (chestsDone - 4) * pipsPerChestTier[2];

			ticketsDone = 4 * ticketsPerChestTier[1] + 5;
			ticketsDone += (chestsDone - 4) * ticketsPerChestTier[2];
			if (chestsDone == 8) ticketsDone += 5; // bonus tickets from final
		} 
		else if (chestsDone <= 13) {
			// Silver Tier: 4* wood + 4* bronze + (chestsdone - 8) * silver
			pipsDone = 4 * pipsPerChestTier[1];
			pipsDone += 4 * pipsPerChestTier[2];
			pipsDone += (chestsDone - 8) * pipsPerChestTier[3];

			ticketsDone = 4 * ticketsPerChestTier[1] + 5;
			ticketsDone += 4 * ticketsPerChestTier[2] + 5;
			ticketsDone += (chestsDone - 8) * ticketsPerChestTier[3];
			if (chestsDone == 13) ticketsDone += 5; // bonus tickets from final
		}
		else if (chestsDone <= 18) {
			// Gold tier
			pipsDone = 4 * pipsPerChestTier[1];
			pipsDone += 4 * pipsPerChestTier[2];
			pipsDone += 5 * pipsPerChestTier[3];
			pipsDone += (chestsDone - 13) * pipsPerChestTier[4];

			ticketsDone = 4 * ticketsPerChestTier[1] + 5;
			ticketsDone += 4 * ticketsPerChestTier[2] + 5;
			ticketsDone += 5 * ticketsPerChestTier[3] + 5;
			ticketsDone += (chestsDone - 13) * ticketsPerChestTier[4];
			if (chestsDone == 18) ticketsDone += 5; // bonus tickets from final
		}
		else if (chestsDone <= 23) {
			// Platinum tier
			pipsDone = 4 * pipsPerChestTier[1];
			pipsDone += 4 * pipsPerChestTier[2];
			pipsDone += 5 * pipsPerChestTier[3];
			pipsDone += 5 * pipsPerChestTier[4];
			pipsDone += (chestsDone - 18) * pipsPerChestTier[5];

			ticketsDone = 4 * ticketsPerChestTier[1] + 5;
			ticketsDone += 4 * ticketsPerChestTier[2] + 5;
			ticketsDone += 5 * ticketsPerChestTier[3] + 5;
			ticketsDone += 5 * ticketsPerChestTier[4] + 5;
			ticketsDone += (chestsDone - 18) * ticketsPerChestTier[5];
			if (chestsDone == 23) ticketsDone += 5; // bonus tickets from final
		}
		else if (chestsDone <= 29) {
			// Mithril tier
			pipsDone = 4 * pipsPerChestTier[1];
			pipsDone += 4 * pipsPerChestTier[2];
			pipsDone += 5 * pipsPerChestTier[3];
			pipsDone += 5 * pipsPerChestTier[4];
			pipsDone += 5 * pipsPerChestTier[5];
			pipsDone += (chestsDone - 23) * pipsPerChestTier[6];

			ticketsDone = 4 * ticketsPerChestTier[1] + 5;
			ticketsDone += 4 * ticketsPerChestTier[2] + 5;
			ticketsDone += 5 * ticketsPerChestTier[3] + 5;
			ticketsDone += 5 * ticketsPerChestTier[4] + 5;
			ticketsDone += 5 * ticketsPerChestTier[5] + 5;
			ticketsDone += (chestsDone - 23) * ticketsPerChestTier[6];
			if (chestsDone == 29) ticketsDone += 5; // bonus tickets from final
		}
		else {
			// Diamond tier
			pipsDone = 4 * pipsPerChestTier[1];
			pipsDone += 4 * pipsPerChestTier[2];
			pipsDone += 5 * pipsPerChestTier[3];
			pipsDone += 5 * pipsPerChestTier[4];
			pipsDone += 5 * pipsPerChestTier[5];
			pipsDone += 6 * pipsPerChestTier[6];
			pipsDone += (chestsDone - 29) * pipsPerChestTier[7];

			ticketsDone = 4 * ticketsPerChestTier[1] + 5;
			ticketsDone += 4 * ticketsPerChestTier[2] + 5;
			ticketsDone += 5 * ticketsPerChestTier[3] + 5;
			ticketsDone += 5 * ticketsPerChestTier[4] + 5;
			ticketsDone += 5 * ticketsPerChestTier[5] + 5;
			ticketsDone += 6 * ticketsPerChestTier[6] + 5;
			ticketsDone += (chestsDone - 29) * ticketsPerChestTier[7];
			if (chestsDone == 35) ticketsDone = 365; // max reward reached
		}

		// minutes left: 5 for the initial tick + pipsMax - pipsDone then divide pipsPerMinute times 5
		int minutesLeft;
		if (pips == 0) {
			minutesLeft = 0;
		}
		else {
			minutesLeft = 5 + (1450 - pipsDone) / pips * 5;
			if (chestsDone > 29) minutesLeft -= 5; // fix diamond chest... idk why it is inaccurate, rounding?
		}

		return { pips, pipsDone , ticketsDone, minutesLeft };
	}
};

class AutoPipsCalculator : public PipsCalculator {
public:
	void setServerRank(int r) {
		// Ranks are reversed: Rank 1 gives 3 extra pips, Rank 3 gives 1.
		switch (r) {
			case 1: rank = 3; break;
			case 2: rank = 2; break;
			case 3: rank = 1; break;
			default: rank = 0; break; // should never happen
		}
	}

	void setPipsProgress(int currentPips) {
		if (currentPips < 25) {
			chestsDone = 0;
			return;
		}
		int chests = 0;
		int pipsDone = 0;
		// Wood Tier: 4 chests
		for (int i = 0; i < 4; i++) {
			chests++;
			pipsDone += pipsPerChestTier[1];
			if (pipsDone >= currentPips) {
				chestsDone = chests;
				return;
			}
		}
		// Bronze Tier: 4 chests
		for (int i = 0; i < 4; i++) {
			chests++;
			pipsDone += pipsPerChestTier[2];
			if (pipsDone >= currentPips) {
				chestsDone = chests;
				return;
			}
		}
		// Silver Tier: 5 chests
		for (int i = 0; i < 5; i++) {
			chests++;
			pipsDone += pipsPerChestTier[3];
			if (pipsDone >= currentPips) {
				chestsDone = chests;
				return;
			}
		}
		// Gold Tier: 5 chests
		for (int i = 0; i < 5; i++) {
			chests++;
			pipsDone += pipsPerChestTier[4];
			if (pipsDone >= currentPips) {
				chestsDone = chests;
				return;
			}
		}
		// Platinum Tier: 5 chests
		for (int i = 0; i < 5; i++) {
			chests++;
			pipsDone += pipsPerChestTier[5];
			if (pipsDone >= currentPips) {
				chestsDone = chests;
				return;
			}
		}
		// Mithril Tier: 6 chests
		for (int i = 0; i < 6; i++) {
			chests++;
			pipsDone += pipsPerChestTier[6];
			if (pipsDone >= currentPips) {
				chestsDone = chests;
				return;
			}
		}
		// Diamond Tier: 6 chests
		for (int i = 0; i < 6; i++) {
			chests++;
			pipsDone += pipsPerChestTier[7];
			if (pipsDone >= currentPips) {
				chestsDone = chests;
				return;
			}
		}
		chestsDone = chests; // default to max in case number is way higher
	}

	void setPersonalRank(int currentRank) {
		if (currentRank <= 149) { pRank1 = 0; return; }
		if (currentRank <= 619) { pRank1 = 1; return; }
		if (currentRank <= 1394) { pRank1 = 2; return; }
		if (currentRank <= 2544) { pRank1 = 3; return; }
		if (currentRank <= 4094) { pRank1 = 4; return; }
		if (currentRank <= 6444) { pRank1 = 5; return; }
		if (currentRank <= 9999) { pRank1 = 6; return; }
		pRank1 = 7;
	}
};

#endif