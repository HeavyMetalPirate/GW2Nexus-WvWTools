#ifndef CONSTANTS_H
#define CONSTANTS_H

static const char* ADDON_NAME = "WvWTools";

static const std::string genericAccount = "Generic";
static const std::string baseUrl = "https://api.guildwars2.com";

static const char* ICON_NOTIFICATION = "ICON_NOTIFICATION";

static const std::vector<std::pair<std::string, int>>  tierMap = {
	{"Wood", 4},
	{"Bronze", 4},
	{"Silver", 5},
	{"Gold", 5},
	{"Platinum", 5},
	{"Mithril", 6},
	{"Diamond", 6}
};

static std::map<int, int> pipsPerChestTier = {
	{1, 25},
	{2, 30},
	{3, 35},
	{4, 40},
	{5, 45},
	{6, 50},
	{7, 55}
};
static std::map<int, int> ticketsPerChestTier = {
	{1, 3},
	{2, 5},
	{3, 7},
	{4, 9},
	{5, 11},
	{6, 13},
	{7, 14}
};

#endif
