///----------------------------------------------------------------------------------------------------
/// Copyright (c) Raidcore.GG - All rights reserved.
///
/// This code is licensed under the MIT license.
/// You should have received a copy of the license along with this source file.
/// You may obtain a copy of the license at: https://opensource.org/license/MIT
/// 
/// Name         :  entry.cpp
/// Description  :  Simple example of a Nexus addon implementation.
///----------------------------------------------------------------------------------------------------
/// 
#include "service/AddonRenderer.h"
#include "service/WvWMatchService.h"
#include "service/AddonInitalize.h"

#include "Globals.h"

/* proto */
// Loading
void AddonLoad(AddonAPI* aApi);
void AddonUnload();
// Rendering
void AddonPreRender();
void AddonRender();
void AddonPostRender();
void AddonOptions();
// Shortcuts and Keybinds
void AddonSimpleShortcut();
// Events => TODO MumbleIdentityChange for Tagged Up/Down iirc
void HandleArcEventLocal(void* eventArgs);
void HandleIdentityChanged(void* eventArgs);
void HandleAccountName(void* eventArgs);
void HandleSelfJoin(void* eventArgs);
void HandleSquadJoin(void* eventArgs);
void HandleSquadLeave(void* eventArgs);
// Settings
void LoadSettings();
void StoreSettings();
void renderWidgetPropertiesTable(const char* id, WidgetSettings* widgetSettings, bool hasMultiLine, bool hasTeamNames);


/* globals */
AddonDefinition AddonDef	= {};
HMODULE hSelf				= nullptr;
AddonAPI* APIDefs			= nullptr;
NexusLinkData* NexusLink	= nullptr;
Mumble::Data* MumbleLink	= nullptr;
gw2api::wvw::Match* match	= nullptr;
Texture* iconNotification   = nullptr;

bool unloading = false;
std::string accountName = "";

arcdps::StateChange stateChange = arcdps::StateChange::Unknown;

/* services */
Renderer renderer;
AutoPipsCalculator autoPipsCalculator;
WorldInventory worldInventory;
WvWMatchService matchService;
GuildInfoService guildInfo;

/* settings */
Settings settings = {};

/* temporary settings */
bool renderPipsCalculator = false;
bool renderMatchExplorer = false;
bool showServerSelection = false;
bool showOverrideAutoCalc = false;

///----------------------------------------------------------------------------------------------------
/// DllMain:
/// 	Main entry point for DLL.
/// 	We are not interested in this, all we get is our own HMODULE in case we need it.
///----------------------------------------------------------------------------------------------------
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH: hSelf = hModule; break;
		case DLL_PROCESS_DETACH: break;
		case DLL_THREAD_ATTACH: break;
		case DLL_THREAD_DETACH: break;
	}
	return TRUE;
}

///----------------------------------------------------------------------------------------------------
/// GetAddonDef:
/// 	Export needed to give Nexus information about the addon.
///----------------------------------------------------------------------------------------------------
extern "C" __declspec(dllexport) AddonDefinition* GetAddonDef()
{
	AddonDef.Signature = -9865498         ; // set to random unused negative integer
	AddonDef.APIVersion = NEXUS_API_VERSION;
	AddonDef.Name = "WvW Toolbox";	
	AddonDef.Version.Major = 0;
	AddonDef.Version.Minor = 5;
	AddonDef.Version.Build = 0;
	AddonDef.Version.Revision = 0;
	AddonDef.Author = "HeavyMetalPirate.2695";
	AddonDef.Description = "Tools to enhance your World vs. World experience.";
	AddonDef.Load = AddonLoad;
	AddonDef.Unload = AddonUnload;
	AddonDef.Flags = EAddonFlags_None;

	/* not necessary if hosted on Raidcore, but shown anyway for the example also useful as a backup resource */
	AddonDef.Provider = EUpdateProvider_GitHub;
	AddonDef.UpdateLink = "https://github.com/HeavyMetalPirate/GW2Nexus-WvWTools";

	return &AddonDef;
}

///----------------------------------------------------------------------------------------------------
/// AddonLoad:
/// 	Load function for the addon, will receive a pointer to the API.
/// 	(You probably want to store it.)
///----------------------------------------------------------------------------------------------------
void AddonLoad(AddonAPI* aApi)
{
	APIDefs = aApi; // store the api somewhere easily accessible

	ImGui::SetCurrentContext((ImGuiContext*)APIDefs->ImguiContext); // cast to ImGuiContext*
	ImGui::SetAllocatorFunctions((void* (*)(size_t, void*))APIDefs->ImguiMalloc, (void(*)(void*, void*))APIDefs->ImguiFree); // on imgui 1.80+

	NexusLink = (NexusLinkData*)APIDefs->GetResource("DL_NEXUS_LINK");
	MumbleLink = (Mumble::Data*)APIDefs->GetResource("DL_MUMBLE_LINK");

	unpackResources();

	autoPipsCalculator = AutoPipsCalculator();
	worldInventory = WorldInventory();
	renderer = Renderer();
	matchService = WvWMatchService();
	guildInfo = GuildInfoService();
	guildInfo.setAPIDefs(APIDefs);

	initializeAlliances();
	LoadSettings();
	
	// start Match Service auto load
	matchService.startThread();
	guildInfo.startLoaderThread();

	// SimpleShortcut
	APIDefs->AddSimpleShortcut(ADDON_NAME, AddonSimpleShortcut);

	// Events
	APIDefs->SubscribeEvent("EV_ARCDPS_COMBATEVENT_LOCAL_RAW", HandleArcEventLocal);
	APIDefs->SubscribeEvent("EV_MUMBLE_IDENTITY_UPDATED", HandleIdentityChanged);
	APIDefs->SubscribeEvent("EV_ACCOUNT_NAME", HandleAccountName);
	APIDefs->SubscribeEvent("EV_ARCDPS_SELF_JOIN", HandleSelfJoin);
	APIDefs->SubscribeEvent("EV_ARCDPS_SQUAD_JOIN", HandleSquadJoin);
	APIDefs->SubscribeEvent("EV_ARCDPS_SQUAD_LEAVE", HandleSquadLeave);

	// Add an options window and a regular render callback
	APIDefs->RegisterRender(ERenderType_PreRender, AddonPreRender);
	APIDefs->RegisterRender(ERenderType_Render, AddonRender);
	APIDefs->RegisterRender(ERenderType_PostRender, AddonPostRender);
	APIDefs->RegisterRender(ERenderType_OptionsRender, AddonOptions);

	APIDefs->RaiseEventNotification("EV_REQUEST_ACCOUNT_NAME"); // Request account name at load
	APIDefs->RaiseEventNotification("EV_REPLAY_ARCDPS_SQUAD_JOIN"); // Request all squad joins in case player is in a squad at load time

	APIDefs->Log(ELogLevel_DEBUG, ADDON_NAME, "<c=#00ff00>WvWToolbox</c> was loaded.");
}

///----------------------------------------------------------------------------------------------------
/// AddonUnload:
/// 	Everything you registered in AddonLoad, you should "undo" here.
///----------------------------------------------------------------------------------------------------
void AddonUnload()
{
	bool unloading = true;
	
	// Stop match service thread
	matchService.stopThread();
	guildInfo.stopLoaderThread();

	/* let's clean up after ourselves */
	APIDefs->RemoveSimpleShortcut(ADDON_NAME);

	APIDefs->UnsubscribeEvent("EV_ARCDPS_COMBATEVENT_LOCAL_RAW", HandleArcEventLocal);
	APIDefs->UnsubscribeEvent("EV_MUMBLE_IDENTITY_UPDATED", HandleIdentityChanged);
	APIDefs->UnsubscribeEvent("EV_ACCOUNT_NAME", HandleAccountName);
	APIDefs->UnsubscribeEvent("EV_ARCDPS_SELF_JOIN", HandleSelfJoin);
	APIDefs->UnsubscribeEvent("EV_ARCDPS_SQUAD_JOIN", HandleSquadJoin);
	APIDefs->UnsubscribeEvent("EV_ARCDPS_SQUAD_LEAVE", HandleSquadLeave);

	APIDefs->DeregisterRender(AddonPreRender);
	APIDefs->DeregisterRender(AddonRender);
	APIDefs->DeregisterRender(AddonPostRender);
	APIDefs->DeregisterRender(AddonOptions);

	StoreSettings();

	APIDefs->Log(ELogLevel_DEBUG, ADDON_NAME, "<c=#ff0000>Signing off</c>, it was an honor commander.");
}

///----------------------------------------------------------------------------------------------------
/// AddonRender:
/// 	Called every frame. Safe to render any ImGui.
/// 	You can control visibility on loading screens with NexusLink->IsGameplay.
///----------------------------------------------------------------------------------------------------
void AddonRender()
{
	renderer.render();
}

///----------------------------------------------------------------------------------------------------
/// AddonOptions:
/// 	Basically an ImGui callback that doesn't need its own Begin/End calls.
///----------------------------------------------------------------------------------------------------
void AddonOptions()
{
	AccountSettings* accountSettings;
	if (settings.accountSettings.count(accountName) > 0) {
		accountSettings = &settings.accountSettings[accountName];
	}
	else {
		accountSettings = &settings.accountSettings[genericAccount];
	}

	ImGui::Text(("Current account name: " + accountName).c_str());

	gw2api::worlds::alliance* currentAlliance = worldInventory.getAlliance(accountSettings->allianceId);
	if (currentAlliance == nullptr) {
		ImGui::Text("No WvW alliance selected as home.");
	}
	else {
		ImGui::Text(("Current alliance selection: " + currentAlliance->name).c_str());
	}
	
	if (ImGui::Button("Alliance Selection")) {
		showServerSelection = !showServerSelection;
	}

	if (showServerSelection) {
		if (ImGui::CollapsingHeader("US Alliances")) {
			const int columns = 6; // Number of columns in the grid
			if (ImGui::BeginTable("US_Servers_Table", columns)) {
				int i = 0;
				for (auto w : worldInventory.getAllAlliances()) {
					if ((w->id - 10000) / 1000 == 1) {

						if (i % columns == 0) {
							ImGui::TableNextRow();
						}
						ImGui::TableNextColumn();

						if (ImGui::Button(w->name.c_str())) {
							accountSettings->allianceId = w->id;
							matchService.loadMatchData();
							showServerSelection = false;
							StoreSettings();
						}

						i++;
					}
				}
				ImGui::EndTable();
			}
		}
		if (ImGui::CollapsingHeader("EU Alliances")) {
			const int columns = 6; // Number of columns in the grid
			if (ImGui::BeginTable("EU_Servers_Table", columns)) {
				int i = 0;
				for (auto w : worldInventory.getAllAlliances()) {
					if ((w->id - 10000) / 1000 == 2) {

						if (i % columns == 0) {
							ImGui::TableNextRow();
						}
						ImGui::TableNextColumn();

						if (ImGui::Button(w->name.c_str())) {
							accountSettings->allianceId = w->id;
							matchService.loadMatchData();
							showServerSelection = false;
							StoreSettings();
						}

						i++;
					}
				}
				ImGui::EndTable();
			}
		}
	}

	ThickSeparator(3.0f, 1.0f);

	ImGui::Text("Functions");
	ImGui::Checkbox("Show K/D of current match", &settings.killDeath.render);
	renderWidgetPropertiesTable("##KillDeathProps", &settings.killDeath, true, true);

	ImGui::Separator();
	ImGui::Checkbox("Show Victory Points of current match", &settings.victoryPoints.render);
	renderWidgetPropertiesTable("##VictoryPointsProps", &settings.victoryPoints, true, true);

	ImGui::Separator();
	ImGui::Checkbox("Show Skirmish score", &settings.skirmishScore.render);
	renderWidgetPropertiesTable("##SkirmishScoreProps", &settings.skirmishScore, true, true);

	ImGui::Checkbox("Hide stale data warnings on widgets", &settings.hideStaleDataWarnings);

	ImGui::Separator();
	ImGui::Checkbox("Show automated calculation result", &settings.autoPips.render);
	char bufferAutoPipsFormat[256];
	strncpy_s(bufferAutoPipsFormat, settings.autoPipsDisplayFormat.c_str(), sizeof(bufferAutoPipsFormat));
	if (ImGui::InputText("AutoCalc Display Format", bufferAutoPipsFormat, sizeof(bufferAutoPipsFormat))) {
		settings.autoPipsDisplayFormat = bufferAutoPipsFormat;
		StoreSettings();
	}
	ImGui::TextWrapped("Placeholders: @p (Pips per tick), @d (Pips done total), @t (Tickets done total), @r (time remaining until first diamond completion)");

	char bufferAutoPipsDone[256];
	strncpy_s(bufferAutoPipsDone, settings.autoPipsDoneText.c_str(), sizeof(bufferAutoPipsDone));
	if (ImGui::InputText("AutoCalc 'Done' (=0 time left) text", bufferAutoPipsDone, sizeof(bufferAutoPipsDone))) {
		settings.autoPipsDoneText = bufferAutoPipsDone;
		StoreSettings();
	}
	renderWidgetPropertiesTable("##AutoPipsProps", &settings.autoPips, false, false);

	if (ImGui::CollapsingHeader("AutoPipsCalculator explained")) {
		ImGui::TextWrapped("The AutoPipsCalculator is an approximation to the time remaining based on your gameplay and performance of your alliance.");
		ImGui::TextWrapped("There will be inaccuracies, but overall it should give you a good indication of how long you have left until your first diamond skirmish track completion.");
		ImGui::TextColored(ImVec4(0, 255, 0, 1), "The AutoPipsCalculator requires ArcDPS to be installed and loaded!");
		ImGui::TextColored(ImVec4(0, 255, 0, 1), "Otherwise pips and personal rank progress cannot be updated!");
		ImGui::TextWrapped("(Note: there is no way to determine whether a tag is public or not so it is assumed public all the time!)");
		ImGui::Separator();
		ImGui::Text("AutoPipsCalculator bases calculation off these values:");
		if (ImGui::BeginTable("##AutoPipsCalculator", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
			ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableHeadersRow();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::TextUnformatted("Personal Rank:");
			ImGui::TableSetColumnIndex(1);
			std::string rankname;
			switch (autoPipsCalculator.pRank1) {
			case 0: rankname = "Wood"; break;
			case 1: rankname = "Bronze"; break;
			case 2: rankname = "Silver"; break;
			case 3: rankname = "Gold"; break;
			case 4: rankname = "Platinum"; break;
			case 5: rankname = "Mithril"; break;
			case 6: rankname = "Diamond"; break;
			case 7: rankname = "Legend"; break;
			default: rankname = "unknown";
			}
			ImGui::TextUnformatted(rankname.c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::TextUnformatted("Server Rank:");
			ImGui::TableSetColumnIndex(1);
			ImGui::TextUnformatted(autoPipsCalculator.rank == 1 ? "3" : autoPipsCalculator.rank == 2 ? "2" : "1");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::TextUnformatted("Commitment:");
			ImGui::TableSetColumnIndex(1);
			ImGui::TextUnformatted(autoPipsCalculator.commitment ? "Yes" : "No");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::TextUnformatted("Commander Status:");
			ImGui::TableSetColumnIndex(1);
			ImGui::TextUnformatted(autoPipsCalculator.taggedUp ? "Yes" : "No");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::TextUnformatted("Squad Size:");
			ImGui::TableSetColumnIndex(1);
			ImGui::TextUnformatted(std::to_string(autoPipsCalculator.squadSize).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::TextUnformatted("Commander Bonus:");
			ImGui::TableSetColumnIndex(1);
			ImGui::TextUnformatted(autoPipsCalculator.commander ? "Yes" : "No");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::TextUnformatted("Public Commander Bonus:");
			ImGui::TableSetColumnIndex(1);
			ImGui::TextUnformatted(autoPipsCalculator.publicCommander ? "Yes" : "No");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::TextUnformatted("Chests done:");
			ImGui::TableSetColumnIndex(1);
			ImGui::TextUnformatted(std::to_string(autoPipsCalculator.chestsDone).c_str());

			ImGui::EndTable();
		}
		ImGui::TextWrapped("These values will update if you play World vs. World in the following:");
		ImGui::TextWrapped("- Server Rank will be updated by calls to the REST API using the alliance provided above");
		ImGui::TextWrapped("- Personal Rank will be updated every time you receive a level up reward bouncy chest");
		ImGui::TextWrapped("- Chests done will be updated everytime you receive a skirmish reward track step bouncy chest");
		ImGui::TextWrapped("- (Public) Commander will be updated if you tag up/down");
		ImGui::TextWrapped("- Commitment will be updated if a new matchup is detected (compared to settings) and at least 4 chests (=100 pips) were finished at that point");
		ImGui::Text("In case you want to override values, use this button:");
		ImGui::SameLine();
		if (ImGui::Button("Override Settings")) {
			showOverrideAutoCalc = true;
		}

		if (showOverrideAutoCalc) {
			if (ImGui::Button("Save")) {
				showOverrideAutoCalc = false;
				StoreSettings();
			}

			if (ImGui::Button("Change Commitment")) {
				accountSettings->hasCommitment = !accountSettings->hasCommitment;
				autoPipsCalculator.commitment = accountSettings->hasCommitment;
			}
			if (ImGui::InputInt("WvW Rank", &accountSettings->currentRank)) {
				autoPipsCalculator.setPersonalRank(accountSettings->currentRank);
			}
			if (ImGui::InputInt("Pips done", &accountSettings->pipsProgressed)) {
				autoPipsCalculator.setPipsProgress(accountSettings->pipsProgressed);
			}
			ImGui::TextWrapped("(Use the manual calculator to figure out pips for your current completion; 1450 Pips = diamond skirmish track completion)");
		}
	}
}

void renderWidgetPropertiesTable(const char* id, WidgetSettings* widgetSettings, bool hasMultiLine, bool hasTeamNames) {
	static const char* textAlignComboItems[4];
	textAlignComboItems[0] = "Center";
	textAlignComboItems[1] = "Left";
	textAlignComboItems[2] = "Right";
	textAlignComboItems[3] = "Block";

	static const char* teamNameComboItems[3];
	teamNameComboItems[0] = "Alliance";
	teamNameComboItems[1] = "Color";
	teamNameComboItems[2] = "None";
	
	if (ImGui::BeginTable(id, 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::InputInt("Position (X)", &widgetSettings->position.x);
		ImGui::TableSetColumnIndex(1);
		ImGui::InputInt("Position (Y)", &widgetSettings->position.y);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::InputInt("Width (0 = auto)", &widgetSettings->width);
		ImGui::TableSetColumnIndex(1);
		if (ImGui::Combo("Alignment", &widgetSettings->alignment, textAlignComboItems, IM_ARRAYSIZE(textAlignComboItems))) {
			StoreSettings();
		}

		if (hasMultiLine || hasTeamNames) {
			ImGui::TableNextRow();
			if (hasTeamNames) {
				ImGui::TableNextColumn();
				if (ImGui::Combo(id, &widgetSettings->teamnameMode, teamNameComboItems, IM_ARRAYSIZE(teamNameComboItems))) {
					StoreSettings();
				}
			}
			if (hasMultiLine) {
				ImGui::TableNextColumn();
				ImGui::Checkbox("Horizontal", &widgetSettings->sameLine);
			}
		}
		ImGui::EndTable();
	}
}

void AddonSimpleShortcut() {
	ImGui::Checkbox("Pips Calculator", &renderPipsCalculator);	
	ImGui::Checkbox("Match Explorer", &renderMatchExplorer);
}

void AddonPreRender() {
	renderer.preRender();
}
void AddonPostRender() {
	renderer.postRender();
}

void HandleArcEventLocal(void* eventArgs) {
	// only work this while in WvW I guess
	
	arcdps::EvCombatData* data = (arcdps::EvCombatData*)eventArgs;
	if (data == nullptr) return;
	if (data->ev == nullptr) return;
	if (data->src == nullptr) return;
	stateChange = arcdps::StateChangeFromInt(data->ev->is_statechange);

	// tiny chest thingy
	if (stateChange == arcdps::StateChange::Reward) {
		// dst_agent: reward id
		// value: reward type
		long rewardId = data->ev->dst_agent;
		int rewardType = data->ev->value;
#ifndef NDEBUG
		APIDefs->Log(ELogLevel_INFO, ADDON_NAME, ("Reward Id: " + std::to_string(rewardId) + ", Reward Type: " + std::to_string(rewardType)).c_str());
#endif // !NDEBUG
		
		AccountSettings* accountSettings;
		if (settings.accountSettings.count(accountName) > 0) {
			accountSettings = &settings.accountSettings[accountName];
		}
		else {
			accountSettings = &settings.accountSettings[genericAccount];
		}

		arcdps::RewardType reward = arcdps::RewardTypeFromInt(rewardType);

		switch (reward) {
			// rewardId = current rank
		case arcdps::RewardType::WvWRankUp: autoPipsCalculator.setPersonalRank(rewardId); accountSettings->currentRank = rewardId; StoreSettings(); break;
			// rewardId = Pips to current chest (i.e. finishing last chest of diamond = 1450)
		case arcdps::RewardType::WvWSkirmishProgress: autoPipsCalculator.setPipsProgress(rewardId); accountSettings->pipsProgressed = rewardId; StoreSettings(); break;
			// rewardId = id of the reward track, i.e. 376 = GoB track
		case arcdps::RewardType::WvWTrackProgress: break; 
		default: break;// No op
		}
	}
}

void LoadSettings() {
	// Get addon directory
	std::string pathData = getAddonFolder() + "/settings.json";
	if (fs::exists(pathData)) {
		std::ifstream dataFile(pathData);

		if (dataFile.is_open()) {
			json jsonData;
			dataFile >> jsonData;
			dataFile.close();
			// parse settings, yay
			settings = jsonData;

			// if accountSettings are empty, initialize them
			if (settings.accountSettings.size() == 0) {
				// try to restore legacy settings
				std::string matchBegin = "1970-01-01T18:00:00Z";
				std::string matchEnd = "1970-01-01T18:00:00Z";
				int allianceId = 0;
				int pipsDone = 0;
				int rank = 0;
				bool commitment = false;

				if (jsonData.contains("matchBegin")) {
					matchBegin = jsonData["matchBegin"].get<std::string>();
				}
				if (jsonData.contains("matchEnd")) {
					matchEnd = jsonData["matchEnd"].get<std::string>();
				}
				if (jsonData.contains("allianceId")) {
					allianceId = jsonData["allianceId"].get<int>();
				}
				if (jsonData.contains("currentRank")) {
					rank = jsonData["currentRank"].get<int>();
				}
				if (jsonData.contains("pipsProgressed")) {
					pipsDone = jsonData["pipsProgressed"].get<int>();
				}
				if (jsonData.contains("hasCommitment")) {

					commitment = jsonData["hasCommitment"].get<bool>();
				}
				AccountSettings as = {
					matchBegin,
					matchEnd,
					allianceId,
					rank,
					pipsDone,
					commitment
				};
				settings.accountSettings.emplace(genericAccount, as);
			}

			if (accountName.empty()) {
				autoPipsCalculator.setPersonalRank(settings.accountSettings[genericAccount].currentRank);
				autoPipsCalculator.setPipsProgress(settings.accountSettings[genericAccount].pipsProgressed);
				autoPipsCalculator.commitment = settings.accountSettings[genericAccount].hasCommitment;
			}
			else {
				autoPipsCalculator.setPersonalRank(settings.accountSettings[accountName].currentRank);
				autoPipsCalculator.setPipsProgress(settings.accountSettings[accountName].pipsProgressed);
				autoPipsCalculator.commitment = settings.accountSettings[accountName].hasCommitment;
			}

			// Migrate widget settings to new structure
			// Existence of any of the legacy settings indicates old format so translate it
			if (jsonData.contains("autoPipsPosition")) {
				WidgetSettings autoPips = {
					jsonData["renderAutoPipsResult"].get<bool>(),
					{jsonData["autoPipsPosition"]["x"].get<int>(), jsonData["autoPipsPosition"]["y"].get<int>()},
					false,
					jsonData["autoPipsWidth"].get<int>(),
					0,
					0
				};
				settings.autoPips = autoPips;
			}
			if (jsonData.contains("killDeathPosition")) {
				WidgetSettings killDeath = {
					jsonData["renderKillDeathRatio"].get<bool>(),
					{jsonData["killDeathPosition"]["x"].get<int>(), jsonData["killDeathPosition"]["y"].get<int>()},
					jsonData["renderKDSameLine"].get<bool>(),
					300,
					0,
					0
				};
				settings.killDeath = killDeath;
			}

			StoreSettings();
		}
	}
	else {
		// Create new settings!
		settings = {
			{
				{ genericAccount, {
						"1970-01-01T18:00:00Z",
						"1970-01-01T18:00:00Z",
						0,
						0,
						0,
						false
					}
				}
			},
			false,
			{false, {50,50}, 0, 300, 0, 0},
			{false, {50,50}, 0, 300, 0, 0},
			{false, {50,50}, 0, 300, 0, 0},
			{false, {50,50}, 0, 300, 0, 0},
			"per TicK: @p, done: @d of 1450, tickets: @t of 365, remaining: @r",
			"Done for the week, see you at reset!"
		};
		StoreSettings();
	}
}

void StoreSettings() {

	if (match != nullptr) {
		// set begin + end date
		if (!accountName.empty() && settings.accountSettings.contains(accountName)) {
			settings.accountSettings[accountName].matchBegin = match->start_time;
			settings.accountSettings[accountName].matchEnd = match->end_time;
		}
		else {
			settings.accountSettings[genericAccount].matchBegin = match->start_time;
			settings.accountSettings[genericAccount].matchEnd = match->end_time;
		}
	}

	json j = settings;

	std::string pathData = getAddonFolder() + "/settings.json";
	std::ofstream outputFile(pathData);
	if (outputFile.is_open()) {
		outputFile << j.dump(4) << std::endl;
		outputFile.close();
	}
	else {
		APIDefs->Log(ELogLevel_WARNING, ADDON_NAME, "Could not store default settings.json - configuration might get lost between loads.");
	}
}

void HandleIdentityChanged(void* anEventArgs) {
	Mumble::Identity* identity = (Mumble::Identity*)anEventArgs;
	if (identity == nullptr) return;
	autoPipsCalculator.taggedUp = identity->IsCommander;
}

void HandleAccountName(void* eventArgs) {
	if (!accountName.empty()) return; // we already got the name, ignore this

	const char* name = (const char*)eventArgs;
	APIDefs->Log(ELogLevel_INFO, ADDON_NAME, ("Received Account Name: " + std::string(name)).c_str());
	
	accountName = std::string(name);
	if(!accountName.empty())
		accountName = accountName.substr(1);

	if (settings.accountSettings.count(accountName) == 0) {
		// clone generic settings
		AccountSettings as = AccountSettings(settings.accountSettings[genericAccount]);
		settings.accountSettings.emplace(accountName, as);
	}

	autoPipsCalculator.setPersonalRank(settings.accountSettings[accountName].currentRank);
	autoPipsCalculator.setPipsProgress(settings.accountSettings[accountName].pipsProgressed);
	autoPipsCalculator.commitment = settings.accountSettings[accountName].hasCommitment;

	StoreSettings();
}

void HandleSelfJoin(void* eventArgs) {
	if (!accountName.empty()) return; // we already got the name, ignore this

	EvAgentUpdate* ev = (EvAgentUpdate*)eventArgs;
	const char* name = ev->account;
	APIDefs->Log(ELogLevel_INFO, ADDON_NAME, ("Received Account Name: " + std::string(name)).c_str());

	accountName = std::string(name);
	if (!accountName.empty())
		accountName = accountName.substr(1);

	if (settings.accountSettings.count(accountName) == 0) {
		// clone generic settings
		AccountSettings as = AccountSettings(settings.accountSettings[genericAccount]);
		settings.accountSettings.emplace(accountName, as);
	}

	autoPipsCalculator.setPersonalRank(settings.accountSettings[accountName].currentRank);
	autoPipsCalculator.setPipsProgress(settings.accountSettings[accountName].pipsProgressed);
	autoPipsCalculator.commitment = settings.accountSettings[accountName].hasCommitment;

	StoreSettings();
}

void HandleSquadJoin(void* eventArgs) {
	EvAgentUpdate* ev = (EvAgentUpdate*)eventArgs;
	autoPipsCalculator.addPlayerToSquad(std::string(ev->account));
}
void HandleSquadLeave(void* eventArgs) {
	EvAgentUpdate* ev = (EvAgentUpdate*)eventArgs;
	autoPipsCalculator.removePlayerFromSquad(std::string(ev->account));
}