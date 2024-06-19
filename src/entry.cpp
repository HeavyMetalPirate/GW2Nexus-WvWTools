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
// Settings
void LoadSettings();
void StoreSettings();


/* globals */
AddonDefinition AddonDef	= {};
HMODULE hSelf				= nullptr;
AddonAPI* APIDefs			= nullptr;
NexusLinkData* NexusLink	= nullptr;
Mumble::Data* MumbleLink	= nullptr;
gw2api::wvw::Match* match	= nullptr;

bool unloading = false;

arcdps::StateChange stateChange = arcdps::StateChange::Unknown;

/* services */
Renderer renderer;
AutoPipsCalculator autoPipsCalculator;
WorldInventory worldInventory;
WvWMatchService matchService;

/* settings */
Settings settings = {
	"",		// Begin Date
	"",		// End Date
	0,		// alliance Id
	0,		// current rank
	0,		// current pips progressed
	false,	// has commitment

	false,	// renderAutoPipsResult
	{50,50},// autoPipsPosition
	300,		// autoPipsAlign
	0,			// autoPipsWidth
	false,	// renderKillDeathRatio
	{50,50},// killDeathPosition
	false,	// renderKDSameLine

	"per TicK: @p, done: @d of 1450, tickets: @t of 365, remaining: @r",	// autoPips Format
	"Done for the week, see you at reset!"									// autoPips done message
};

/* temporary settings */
bool renderPipsCalculator = false;
bool showServerSelection = false;

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
	AddonDef.Version.Minor = 1;
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

	initializeAlliances();
	LoadSettings();
	
	// start Match Service auto load
	matchService.startThread();

	// SimpleShortcut
	APIDefs->AddSimpleShortcut(ADDON_NAME, AddonSimpleShortcut);

	// Events
	APIDefs->SubscribeEvent("EV_ARCDPS_COMBATEVENT_LOCAL_RAW", HandleArcEventLocal);
	APIDefs->SubscribeEvent("EV_MUMBLE_IDENTITY_UPDATED", HandleIdentityChanged);

	// Add an options window and a regular render callback
	APIDefs->RegisterRender(ERenderType_PreRender, AddonPreRender);
	APIDefs->RegisterRender(ERenderType_Render, AddonRender);
	APIDefs->RegisterRender(ERenderType_PostRender, AddonPostRender);
	APIDefs->RegisterRender(ERenderType_OptionsRender, AddonOptions);

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

	/* let's clean up after ourselves */
	APIDefs->RemoveSimpleShortcut(ADDON_NAME);

	APIDefs->UnsubscribeEvent("EV_ARCDPS_COMBATEVENT_LOCAL_RAW", HandleArcEventLocal);
	APIDefs->UnsubscribeEvent("EV_MUMBLE_IDENTITY_UPDATED", HandleIdentityChanged);

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
	// TODO render current auto pips calculator settings and option to overwrite

	gw2api::worlds::alliance* currentAlliance = worldInventory.getAlliance(settings.allianceId);

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
							settings.allianceId = w->id;
							matchService.loadMatchData();
							showServerSelection = false;
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
							settings.allianceId = w->id;
							matchService.loadMatchData();
							showServerSelection = false;
						}

						i++;
					}
				}
				ImGui::EndTable();
			}
		}
	}

	ImGui::Separator();
	ImGui::Text("Functions");
	ImGui::Checkbox("Show K/D of current match", &settings.renderKillDeathRatio);
	ImGui::SameLine();
	ImGui::Checkbox("Horizontal mode", &settings.renderKDSameLine);
	if (ImGui::BeginTable("##KDProps", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::InputInt("Position (X)", &settings.killDeathPosition.x);
		ImGui::TableSetColumnIndex(1);
		ImGui::InputInt("Position (Y)", &settings.killDeathPosition.y);


		ImGui::EndTable();
	}


	ImGui::Checkbox("Show automated calculation result", &settings.renderAutoPipsResult);
	char bufferAutoPipsFormat[256];
	strncpy_s(bufferAutoPipsFormat, settings.autoPipsDisplayFormat.c_str(), sizeof(bufferAutoPipsFormat));
	if (ImGui::InputText("AutoCalc Display Format", bufferAutoPipsFormat, sizeof(bufferAutoPipsFormat))) {
		settings.autoPipsDisplayFormat = bufferAutoPipsFormat;
	}
	ImGui::TextWrapped("Placeholders: @p (Pips per tick), @d (Pips done total), @t (Tickets done total), @r (time remaining until first diamond completion)");

	char bufferAutoPipsDone[256];
	strncpy_s(bufferAutoPipsDone, settings.autoPipsDoneText.c_str(), sizeof(bufferAutoPipsDone));
	if (ImGui::InputText("AutoCalc 'Done' (=0 time left) text", bufferAutoPipsDone, sizeof(bufferAutoPipsDone))) {
		settings.autoPipsDoneText = bufferAutoPipsDone;
	}
	if (ImGui::BeginTable("##AutoPipsProps", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::InputInt("Position (X)", &settings.autoPipsPosition.x);
		ImGui::TableSetColumnIndex(1);
		ImGui::InputInt("Position (Y)", &settings.autoPipsPosition.y);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::InputInt("Width", &settings.autoPipsWidth);
		ImGui::TableSetColumnIndex(1);
		static const char* textAlignComboItems[3];
		textAlignComboItems[0] = "Center";
		textAlignComboItems[1] = "Left";
		textAlignComboItems[2] = "Right";
		if (ImGui::Combo("Alignment", &settings.autoPipsAlignment, textAlignComboItems, IM_ARRAYSIZE(textAlignComboItems))) {

		}

		ImGui::EndTable();
	}


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
			ImGui::TextUnformatted(rankname.c_str()); // TODO translate to 0=wood/1=bronze/2=silver/etc.

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
			ImGui::TextUnformatted("Commander:");
			ImGui::TableSetColumnIndex(1);
			ImGui::TextUnformatted(autoPipsCalculator.commander ? "Yes" : "No");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::TextUnformatted("Public Commander:");
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
		ImGui::TextWrapped("- Personal Rank will be updated as soon as you level up");
		ImGui::TextWrapped("- Chests done will be updated everytime you finish another skirmish reward track step");
		ImGui::TextWrapped("- (Public) Commander will be updated if you tag up/down");
		ImGui::TextWrapped("- Commitment will be updated if a new matchup is detected (compared to settings) and at least 4 chests (=100 pips) were finished at that point");
		ImGui::Text("In case you want to override commitment, use this button:");
		ImGui::SameLine();
		if (ImGui::Button("override")) {
			settings.hasCommitment = !settings.hasCommitment;
			autoPipsCalculator.commitment = settings.hasCommitment;
		}
	}
}

void AddonSimpleShortcut() {
	if (ImGui::Checkbox("Pips Calculator", &renderPipsCalculator)) {

	}
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
		
		arcdps::RewardType reward = arcdps::RewardTypeFromInt(rewardType);

		switch (reward) {
			// rewardId = current rank
		case arcdps::RewardType::WvWRankUp: autoPipsCalculator.setPersonalRank(rewardId); settings.currentRank = rewardId; break;
			// rewardId = Pips to current chest (i.e. finishing last chest of diamond = 1450)
		case arcdps::RewardType::WvWSkirmishProgress: autoPipsCalculator.setPipsProgress(rewardId); settings.pipsProgressed = rewardId;  break;
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

			// set autopips calc values
			autoPipsCalculator.commitment = settings.hasCommitment;
			autoPipsCalculator.setPersonalRank(settings.currentRank);
			autoPipsCalculator.setPipsProgress(settings.pipsProgressed);
		}
	}
	else {
		// Create new settings!
		StoreSettings();
	}
}

void StoreSettings() {

	if (match != nullptr) {
		// set begin + end date
		settings.matchBegin = match->start_time;
		settings.matchEnd = match->end_time;
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
	autoPipsCalculator.commander = identity->IsCommander;
	autoPipsCalculator.publicCommander = identity->IsCommander;
}