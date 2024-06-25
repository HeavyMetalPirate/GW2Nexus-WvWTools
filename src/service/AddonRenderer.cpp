#include "AddonRenderer.h"

/* render protos */
void pipsCalculator();
void matchExplorer();
void autoPipsResult();
void killDeathRatio();
void victoryPoints();
void drawSkirmishCard(gw2api::wvw::Skirmish* skirmish);
std::string convertMinutesToHoursAndMinutes(int totalMinutes);

std::string mapFilterText;
bool showCamps = true;
bool showKeeps = true;
bool showTowers = true;
bool showCastles = true;
bool showSpawns = false;
bool showMercenaries = true;
bool showRuins = true;
bool showOwnedByRed = true;
bool showOwnedByGreen = true;
bool showOwnedByBlue = true;

PipsCalculator calculator = PipsCalculator();
PipsResult result = calculator.calculate();
PipsResult autoResult = autoPipsCalculator.calculate();

ImVec4 colorRed = ImVec4(1, 0, 0, 1);
ImVec4 colorGreen = ImVec4(0, 1, 0, 1);
ImVec4 colorBlue = ImVec4(0, 0.5, 1, 1);

void Renderer::preRender() {
	if(iconNotification == nullptr)
		iconNotification = APIDefs->GetTexture(ICON_NOTIFICATION);
}
void Renderer::render() {
	pipsCalculator();
	autoPipsResult();
	killDeathRatio();
	victoryPoints();
	matchExplorer();
}
void Renderer::postRender() {
	// TODO impl
}

void Renderer::unload() {
	// TODO Impl
}

//========================================================
void victoryPoints() {
	if (!settings.victoryPoints.render) return;
	if (!isInWvW()) return;
	if (match == nullptr) return;
	if (!NexusLink->IsGameplay) return;
	if (MumbleLink->Context.IsMapOpen) return;

	int redTeamId, greenTeamId, blueTeamId;
	redTeamId = match->all_worlds["red"][0];
	greenTeamId = match->all_worlds["green"][0];
	blueTeamId = match->all_worlds["blue"][0];

	std::string teamNameRed, teamNameBlue, teamNameGreen;
	switch (settings.victoryPoints.teamnameMode) {
	case 0:
		teamNameRed = worldInventory.getAlliance(redTeamId)->name + ": ";
		teamNameBlue = worldInventory.getAlliance(blueTeamId)->name + ": ";
		teamNameGreen = worldInventory.getAlliance(greenTeamId)->name + ": ";
		break;
	case 1:
		teamNameRed = "Red: ";
		teamNameBlue = "Blue: ";
		teamNameGreen = "Green: ";
		break;
	case 2:
	default:
		teamNameRed = "";
		teamNameBlue = "";
		teamNameGreen = "";
	}

	std::string textRed = teamNameRed + std::to_string(match->victory_points.red);
	std::string textBlue = teamNameBlue + std::to_string(match->victory_points.blue);
	std::string textGreen = teamNameGreen + std::to_string(match->victory_points.green);

	ImVec2 sizeRed = ImGui::CalcTextSize(textRed.c_str());
	ImVec2 sizeBlue = ImGui::CalcTextSize(textBlue.c_str());
	ImVec2 sizeGreen = ImGui::CalcTextSize(textGreen.c_str());

	ImVec2 textSize;
	if (sizeRed.x > sizeBlue.x && sizeRed.x > sizeGreen.x) {
		textSize = sizeRed;
	}
	else if (sizeBlue.x > sizeRed.x && sizeBlue.x > sizeGreen.x) {
		textSize = sizeBlue;
	}
	else {
		textSize = sizeGreen;
	}

	ImVec2 widgetPos = ImVec2(static_cast<float>(settings.victoryPoints.position.x), static_cast<float>(settings.victoryPoints.position.y));
	ImVec2 widgetSize = ImVec2(static_cast<float>(settings.victoryPoints.width), settings.victoryPoints.sameLine ? (ImGui::GetTextLineHeightWithSpacing()) : (3 * ImGui::GetTextLineHeightWithSpacing()));
		//settings.victoryPoints.sameLine ? (textSize.y + 2 * ImGui::GetTextLineHeightWithSpacing()) : (textSize.y + 4 * ImGui::GetTextLineHeightWithSpacing()));
	if (widgetSize.x <= 0) {
		if (settings.victoryPoints.sameLine) {
			widgetSize.x = sizeBlue.x + sizeRed.x + sizeGreen.x + (2* ImGui::CalcTextSize(" ").x);
		}
		else {
			widgetSize.x = textSize.x;
		}
	}
	if (widgetPos.x <= 0) {
		widgetPos.x = 0;
	}
	if (widgetPos.y <= 0) {
		widgetPos.y = 0;
	}
	ImGui::SetNextWindowPos(widgetPos);
	ImGui::SetNextWindowSize(widgetSize);

	if (ImGui::Begin("Victory Points", nullptr, ImGuiWindowFlags_AlwaysAutoResize
												| ImGuiWindowFlags_NoTitleBar
												| ImGuiWindowFlags_NoInputs
												| ImGuiWindowFlags_NoScrollbar)) {

		if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
		{
			ImGui::BeginTooltip();
			ImGui::Text("Victory Points");
			ImGui::EndTooltip();
		}

		// TODO set to lineheight
		//ImGui::SetCursorPosY(0.0f);
		
		if (settings.victoryPoints.sameLine) {
			
			switch (settings.victoryPoints.alignment) {
			case 0: // center
				ImGui::SetCursorPosX((widgetSize.x - sizeRed.x - sizeGreen.x - sizeBlue.x - (2 * ImGui::GetStyle().ItemInnerSpacing.x)) / 2);
				ImGui::TextColored(colorRed, textRed.c_str());
				ImGui::SameLine();
				ImGui::TextColored(colorGreen, textGreen.c_str());
				ImGui::SameLine();
				ImGui::TextColored(colorBlue, textBlue.c_str());
				break;
			case 1: // left
				ImGui::TextColored(colorRed, textRed.c_str());
				ImGui::SameLine();
				ImGui::TextColored(colorGreen, textGreen.c_str());
				ImGui::SameLine();
				ImGui::TextColored(colorBlue, textBlue.c_str());
				break;
			case 2: // right
				ImGui::SetCursorPosX((widgetSize.x - sizeRed.x - sizeGreen.x - sizeBlue.x - (2 * ImGui::GetStyle().ItemInnerSpacing.x)));
				ImGui::TextColored(colorRed, textRed.c_str());
				ImGui::SameLine();
				ImGui::TextColored(colorGreen, textGreen.c_str());
				ImGui::SameLine();
				ImGui::TextColored(colorBlue, textBlue.c_str());
				break;
			}
		}
		else {
			switch (settings.victoryPoints.alignment) {
			case 0: // center
				ImGui::SetCursorPosX((widgetSize.x - sizeRed.x) / 2);
				ImGui::TextColored(colorRed, textRed.c_str());
				ImGui::SetCursorPosX((widgetSize.x - sizeGreen.x) / 2);
				ImGui::TextColored(colorGreen, textGreen.c_str());
				ImGui::SetCursorPosX((widgetSize.x - sizeBlue.x) / 2);
				ImGui::TextColored(colorBlue, textBlue.c_str());
				break;
			case 1: // left
				ImGui::TextColored(colorRed, textRed.c_str());
				ImGui::TextColored(colorGreen, textGreen.c_str());
				ImGui::TextColored(colorBlue, textBlue.c_str());
				break;
			case 2: // right
				ImGui::SetCursorPosX(widgetSize.x - sizeRed.x - 5);
				ImGui::TextColored(colorRed, textRed.c_str());
				ImGui::SetCursorPosX(widgetSize.x - sizeGreen.x - 5);
				ImGui::TextColored(colorGreen, textGreen.c_str());
				ImGui::SetCursorPosX(widgetSize.x - sizeBlue.x - 5);
				ImGui::TextColored(colorBlue, textBlue.c_str());

				break;
			}
		}
		ImGui::End();
	}
}

void matchExplorer() {
	if (!renderMatchExplorer) return;
	if (match == nullptr) {
		if (ImGui::Begin("Match Explorer", &renderMatchExplorer, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "No match data found!");
			ImGui::End();
		}
		return;
	}
	AccountSettings as = accountName.empty() ? settings.accountSettings[genericAccount] : settings.accountSettings[accountName];
	int redTeamId, greenTeamId, blueTeamId;
	redTeamId = match->all_worlds["red"][0];
	greenTeamId = match->all_worlds["green"][0];
	blueTeamId = match->all_worlds["blue"][0];

	// Current skirmish data
	auto currentSkirmish = match->skirmishes[match->skirmishes.size() - 1];
	std::chrono::time_point matchStart = parse_date(match->start_time);
	auto duration = std::chrono::hours((currentSkirmish.id - 1) * 2);
	auto skirmishStart = matchStart + duration;
	auto skirmishStartLocal = getUTCinLocalTimePoint(skirmishStart);
	auto skirmishEnd = skirmishStart + std::chrono::hours(2);
	auto skirmishEndLocal = getUTCinLocalTimePoint(skirmishEnd);

	auto now = std::chrono::system_clock::now();

	if (ImGui::Begin("Match Explorer", &renderMatchExplorer, ImGuiWindowFlags_NoCollapse)) {
		ImGui::Text("Tier ");
		ImGui::SameLine();
		ImGui::Text((match->id.substr(2)).c_str());
		ImGui::SameLine();
		std::string lastUpdateText = "Last updated: " + format_date_output(lastUpdate);
		ImVec2 lastUpdateSize = ImGui::CalcTextSize(lastUpdateText.c_str());
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - lastUpdateSize.x - 15);
		ImGui::Text(lastUpdateText.c_str());

		ImGui::Text("Duration:");
		ImGui::SameLine();
		ImGui::Text(getUTCinLocalDate(match->start_time).c_str());
		ImGui::SameLine();
		ImGui::Text(" to ");
		ImGui::SameLine();
		ImGui::Text(getUTCinLocalDate(match->end_time).c_str());
		ImGui::Separator();

		

		if (now > skirmishEndLocal) {
			if (iconNotification != nullptr) {
				ImTextureID myTextureID = (ImTextureID)iconNotification->Resource;
				ImVec2 textureSize = ImVec2(iconNotification->Width, iconNotification->Height);
				ImGui::Image(myTextureID, textureSize);
				ImGui::SameLine();
			}
			ImGui::TextColored(colorRed, "Skirmish data in GW2 API outdated! Values might not show current state.");
		}

		if(ImGui::BeginTable("##ExplorerTeams", 4)) {

			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 0.8f);
			ImGui::TableSetupColumn("Red", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Green", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Blue", ImGuiTableColumnFlags_WidthStretch);

			ImGui::TableNextRow();
			ImGui::TableNextColumn();			
			ImGui::Text("Name");
			ImGui::TableNextColumn();
			ImGui::TextColored(colorRed, worldInventory.getAlliance(redTeamId)->name.c_str());
			ImGui::TableNextColumn();
			ImGui::TextColored(colorGreen, worldInventory.getAlliance(greenTeamId)->name.c_str());
			ImGui::TableNextColumn();
			ImGui::TextColored(colorBlue, worldInventory.getAlliance(blueTeamId)->name.c_str());

			ImGui::Separator();
			ImGui::TableNextRow();
			ImGui::TableNextColumn(); 
			ImGui::Text("Victory Points");
			ImGui::TableNextColumn();
			ImGui::Text(std::to_string(match->victory_points.red).c_str());
			ImGui::TableNextColumn();
			ImGui::Text(std::to_string(match->victory_points.green).c_str());
			ImGui::TableNextColumn();
			ImGui::Text(std::to_string(match->victory_points.blue).c_str());

			ImGui::Separator();
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text("Kills");
			ImGui::TableNextColumn();
			ImGui::Text(std::to_string(match->kills.red).c_str());
			ImGui::TableNextColumn();
			ImGui::Text(std::to_string(match->kills.green).c_str());
			ImGui::TableNextColumn();
			ImGui::Text(std::to_string(match->kills.blue).c_str());

			ImGui::Separator();
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text("Deaths");
			ImGui::TableNextColumn();
			ImGui::Text(std::to_string(match->deaths.red).c_str());
			ImGui::TableNextColumn();
			ImGui::Text(std::to_string(match->deaths.green).c_str());
			ImGui::TableNextColumn();
			ImGui::Text(std::to_string(match->deaths.blue).c_str());

			int redKills, redDeaths;
			int blueKills, blueDeaths;
			int greenKills, greenDeaths;

			redKills = match->kills.red;
			redDeaths = match->deaths.red;
			blueKills = match->kills.blue;
			blueDeaths = match->deaths.blue;
			greenKills = match->kills.green;
			greenDeaths = match->deaths.green;

			float redKD;
			if (redDeaths > 0) { redKD = static_cast<float>(redKills) / static_cast<float>(redDeaths); }
			else { redKD = static_cast<float>(redKills); }
			float blueKD;
			if (blueDeaths > 0) { blueKD = static_cast<float>(blueKills) / static_cast<float>(blueDeaths); }
			else { blueKD = static_cast<float>(blueKills); }
			float greenKD;
			if (greenDeaths > 0) { greenKD = static_cast<float>(greenKills) / static_cast<float>(greenDeaths); }
			else { greenKD = static_cast<float>(greenKills); }

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text("K/D Ratio");
			ImGui::TableNextColumn();
			ImGui::Text(std::format("{:.2f}", redKD).c_str());
			ImGui::TableNextColumn();
			ImGui::Text(std::format("{:.2f}", greenKD).c_str());
			ImGui::TableNextColumn();
			ImGui::Text(std::format("{:.2f}", blueKD).c_str());

			ImGui::Separator();
			
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text("Current Skirmish:");
			ImGui::TableNextColumn();
			ImGui::Text("Begin");
			ImGui::TableNextColumn();
			ImGui::Text("End");
			ImGui::TableNextColumn();

			drawSkirmishCard(&currentSkirmish);

			ImGui::EndTable();
		}

		if (ImGui::CollapsingHeader("Skirmish History")) {
			if (ImGui::BeginTable("##ExplorerSkirmishes", 4)) {
				ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 0.8f);
				ImGui::TableSetupColumn(worldInventory.getAlliance(redTeamId)->name.c_str(), ImGuiTableColumnFlags_WidthStretch, 1.0f);
				ImGui::TableSetupColumn(worldInventory.getAlliance(greenTeamId)->name.c_str(), ImGuiTableColumnFlags_WidthStretch, 1.0f);
				ImGui::TableSetupColumn(worldInventory.getAlliance(blueTeamId)->name.c_str(), ImGuiTableColumnFlags_WidthStretch, 1.0f);
				//ImGui::TableHeadersRow();

				for (auto skirmish : match->skirmishes) {
					if (skirmish.id == currentSkirmish.id) continue;

					drawSkirmishCard(&skirmish);
				}
				ImGui::EndTable();
			}
		}
		if (ImGui::CollapsingHeader("Map Informations")) {

			// define filters
			char bufferMapFilterText[256];
			strncpy_s(bufferMapFilterText, mapFilterText.c_str(), sizeof(bufferMapFilterText));
			if (ImGui::InputText("Filter Objectives", bufferMapFilterText, sizeof(bufferMapFilterText))) {
				mapFilterText = bufferMapFilterText;
			}
			
			ImGui::Checkbox("Show Spawns", &showSpawns);
			ImGui::SameLine();
			ImGui::Checkbox("Show Camps", &showCamps);
			ImGui::SameLine();
			ImGui::Checkbox("Show Towers", &showTowers);
			ImGui::SameLine();
			ImGui::Checkbox("Show Keeps", &showKeeps);
			ImGui::Checkbox("Show Castles", &showCastles);
			ImGui::SameLine();
			ImGui::Checkbox("Show Ruins", &showRuins);
			ImGui::SameLine();
			ImGui::Checkbox("Show Mercenaries", &showMercenaries);

			ImGui::Text("Show owned by");
			ImGui::SameLine();
			ImGui::Checkbox(worldInventory.getAlliance(redTeamId)->name.c_str(), &showOwnedByRed);
			ImGui::SameLine();
			ImGui::Checkbox(worldInventory.getAlliance(greenTeamId)->name.c_str(), &showOwnedByGreen);
			ImGui::SameLine();
			ImGui::Checkbox(worldInventory.getAlliance(blueTeamId)->name.c_str(), &showOwnedByBlue);

			for (auto map : match->maps) {
				std::string mapname;
				if (map.type == "Center") mapname = "Eternal Battlegrounds";
				else if (map.type == "RedHome") mapname = worldInventory.getAlliance(redTeamId)->name + " Desert Borderlands";
				else if (map.type == "BlueHome") mapname = worldInventory.getAlliance(blueTeamId)->name + " Alpine Borderlands";
				else if (map.type == "GreenHome") mapname = worldInventory.getAlliance(greenTeamId)->name + " Alpine Borderlands";

				if (ImGui::CollapsingHeader(mapname.c_str())) { 
					if (ImGui::BeginTable("##ExplorerMapStats", 4)) {

						ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 0.8f);
						ImGui::TableSetupColumn(worldInventory.getAlliance(redTeamId)->name.c_str(), ImGuiTableColumnFlags_WidthStretch);
						ImGui::TableSetupColumn(worldInventory.getAlliance(greenTeamId)->name.c_str(), ImGuiTableColumnFlags_WidthStretch);
						ImGui::TableSetupColumn(worldInventory.getAlliance(blueTeamId)->name.c_str(), ImGuiTableColumnFlags_WidthStretch);
						ImGui::TableHeadersRow();

						int redKills, redDeaths;
						int blueKills, blueDeaths;
						int greenKills, greenDeaths;

						redKills = map.kills.red;
						redDeaths = map.deaths.red;
						blueKills = map.kills.blue;
						blueDeaths = map.deaths.blue;
						greenKills = map.kills.green;
						greenDeaths = map.deaths.green;

						float redKD;
						if (redDeaths > 0) { redKD = static_cast<float>(redKills) / static_cast<float>(redDeaths); }
						else { redKD = static_cast<float>(redKills); }
						float blueKD;
						if (blueDeaths > 0) { blueKD = static_cast<float>(blueKills) / static_cast<float>(blueDeaths); }
						else { blueKD = static_cast<float>(blueKills); }
						float greenKD;
						if (greenDeaths > 0) { greenKD = static_cast<float>(greenKills) / static_cast<float>(greenDeaths); }
						else { greenKD = static_cast<float>(greenKills); }

						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						ImGui::Text("Kills");
						ImGui::TableNextColumn();
						ImGui::Text(std::to_string(redKills).c_str());
						ImGui::TableNextColumn();
						ImGui::Text(std::to_string(greenKills).c_str());
						ImGui::TableNextColumn();
						ImGui::Text(std::to_string(blueKills).c_str());

						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						ImGui::Text("Deaths");
						ImGui::TableNextColumn();
						ImGui::Text(std::to_string(redDeaths).c_str());
						ImGui::TableNextColumn();
						ImGui::Text(std::to_string(greenDeaths).c_str());
						ImGui::TableNextColumn();
						ImGui::Text(std::to_string(blueDeaths).c_str());

						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						ImGui::Text("K/D Ratio");
						ImGui::TableNextColumn();
						ImGui::Text(std::format("{:.2f}", redKD).c_str());
						ImGui::TableNextColumn();
						ImGui::Text(std::format("{:.2f}", greenKD).c_str());
						ImGui::TableNextColumn();
						ImGui::Text(std::format("{:.2f}", blueKD).c_str());

						ImGui::EndTable();
					}
					ImGui::Separator();

					for (auto objective : map.objectives) {
						// filters, filters, filters!
						if (!showSpawns && objective.type == "Spawn") continue;
						if (!showCamps && objective.type == "Camp") continue;
						if (!showTowers && objective.type == "Tower") continue;
						if (!showKeeps && objective.type == "Keep") continue;
						if (!showCastles && objective.type == "Castle") continue;
						if (!showRuins && objective.type == "Ruins") continue;
						if (!showMercenaries && objective.type == "Mercenary") continue;
						// Ownership filters						
						if (!showOwnedByRed && objective.owner == "Red") continue;
						if (!showOwnedByGreen && objective.owner == "Green") continue;
						if (!showOwnedByBlue && objective.owner == "Blue") continue;

						gw2api::wvw::Location* objectiveData = worldInventory.getObjective(objective.id);
						
						if (objectiveData == nullptr) {
							if (!mapFilterText.empty()) continue;
							ImGui::TextColored(ImVec4(1, 0, 0, 1), ("!!! Unknown objective id: " + objective.id).c_str());
						}
						else {
							std::string objName = objectiveData->name;
							// check name filter
							if (!mapFilterText.empty() && toLower(objName).find(toLower(mapFilterText)) == std::string::npos) continue;

							if(objective.owner == "Red") ImGui::TextColored(colorRed, objectiveData->name.c_str());
							else if(objective.owner == "Green")  ImGui::TextColored(colorGreen, objectiveData->name.c_str());
							else  ImGui::TextColored(colorBlue, objectiveData->name.c_str());
						}

						ImGui::SameLine();
						std::string text;
						ImVec2 textSize;

						text = "Last flipped: " + getUTCinLocalDate(objective.last_flipped);
						textSize = ImGui::CalcTextSize(text.c_str());
						ImGui::SetCursorPosX(ImGui::GetWindowWidth() - textSize.x - 15);
						ImGui::Text(text.c_str());
								
						if (objective.type == "Camp" || objective.type == "Tower" || objective.type == "Keep" || objective.type == "Castle") {
							// objectives with upgrades and tiers, so display them
							// Tiers: 0 / 20 / 60 / 140 total delivered
							int currentTier;
							if (objective.yaks_delivered < 20) currentTier = 0;
							else if (objective.yaks_delivered < 60) currentTier = 1;
							else if (objective.yaks_delivered < 140) currentTier = 2;
							else currentTier = 3;
							ImGui::Text(("Tier " + std::to_string(currentTier) + " " + objective.type).c_str());

							if (objective.claimed_by.empty()) {
								ImGui::Text("Unclaimed!");
							}
							else {
								std::string guildname = guildInfo.getGuildName(objective.claimed_by);
								ImGui::Text(("Claimed by: " + guildname).c_str());
								ImGui::SameLine();
								ImGui::Text((" at: " + getUTCinLocalDate(objective.claimed_at)).c_str());
							}

							ImGui::Text(("Score per tick: " + std::to_string(objective.points_tick)).c_str());
							ImGui::SameLine();
							ImGui::Text((" | Capture score: " + std::to_string(objective.points_capture)).c_str());

							// only show dolly & upgrade info if in team
							if (((as.allianceId == redTeamId && objective.owner == "Red")
								|| (as.allianceId == greenTeamId && objective.owner == "Green")
								|| (as.allianceId == blueTeamId && objective.owner == "Blue"))) {

								if (currentTier < 3) {
									int currentDelivery;
									int nextTierDelivery;
									if (currentTier == 0) { currentDelivery = objective.yaks_delivered; nextTierDelivery = 20; }
									else if (currentTier == 1) { currentDelivery = objective.yaks_delivered - 20; nextTierDelivery = 40; }
									else if (currentTier == 2) { currentDelivery = objective.yaks_delivered - 60; nextTierDelivery = 80; }
									ImGui::Text(("Dolyaks delivered: " + std::to_string(currentDelivery) + "/" + std::to_string(nextTierDelivery)).c_str());
								}
								ImGui::Text("Upgrades:");
								for (auto upgradeId : objective.guild_upgrades) {
									ImGui::Bullet();
									gw2api::wvw::ObjectiveUpgrade* upgrade = worldInventory.getObjectiveUpgrade(upgradeId);
									if (upgrade == nullptr) {
										ImGui::TextColored(ImVec4(1, 0, 0, 1), ("Unknown upgrade id " + std::to_string(upgradeId)).c_str());
									}
									else {
										ImGui::Text(upgrade->name.c_str());
									}
								}
							}
						}
						else {
							ImGui::Text(objective.type.c_str());
						}
						ImGui::Separator();
					}
				}
			}
		}
		ImGui::End();
	}
}

void killDeathRatio() {
	if (!settings.killDeath.render) return;
	if (!isInWvW()) return;
	if (match == nullptr) return;
	if (!NexusLink->IsGameplay) return;
	if (MumbleLink->Context.IsMapOpen) return;

	int redKills, redDeaths;
	int blueKills, blueDeaths;
	int greenKills, greenDeaths;

	redKills = match->kills.red;
	redDeaths = match->deaths.red;
	blueKills = match->kills.blue;
	blueDeaths = match->deaths.blue;
	greenKills = match->kills.green;
	greenDeaths = match->deaths.green;

	float redKD;
	if (redDeaths > 0) { redKD = static_cast<float>(redKills) / static_cast<float>(redDeaths); }
	else { redKD = static_cast<float>(redKills); }
	float blueKD;
	if (blueDeaths > 0) { blueKD = static_cast<float>(blueKills) / static_cast<float>(blueDeaths); }
	else { blueKD = static_cast<float>(blueKills); }
	float greenKD;
	if (greenDeaths > 0) { greenKD = static_cast<float>(greenKills) / static_cast<float>(greenDeaths); }
	else { greenKD = static_cast<float>(greenKills); }
	
	int redTeamId, greenTeamId, blueTeamId;
	redTeamId = match->all_worlds["red"][0];
	greenTeamId = match->all_worlds["green"][0];
	blueTeamId = match->all_worlds["blue"][0];
	std::string teamNameRed, teamNameBlue, teamNameGreen;
	switch (settings.killDeath.teamnameMode) {
	case 0:
		teamNameRed = worldInventory.getAlliance(redTeamId)->name + ": ";
		teamNameBlue = worldInventory.getAlliance(blueTeamId)->name + ": ";
		teamNameGreen = worldInventory.getAlliance(greenTeamId)->name + ": ";
		break;
	case 1:
		teamNameRed = "Red: ";
		teamNameBlue = "Blue: ";
		teamNameGreen = "Green: ";
		break;
	case 2:
	default:
		teamNameRed = "";
		teamNameBlue = "";
		teamNameGreen = "";
	}

	std::string textRed = teamNameRed + std::format("{:.2f}", redKD);
	std::string textBlue = teamNameBlue + std::format("{:.2f}", blueKD);
	std::string textGreen = teamNameGreen + std::format("{:.2f}", greenKD);
	
	ImVec2 sizeRed = ImGui::CalcTextSize(textRed.c_str());
	ImVec2 sizeBlue = ImGui::CalcTextSize(textBlue.c_str());
	ImVec2 sizeGreen = ImGui::CalcTextSize(textGreen.c_str());

	ImVec2 textSize;
	if (sizeRed.x > sizeBlue.x && sizeRed.x > sizeGreen.x) {
		textSize = sizeRed;
	}
	else if (sizeBlue.x > sizeRed.x && sizeBlue.x > sizeGreen.x) {
		textSize = sizeBlue;
	}
	else {
		textSize = sizeGreen;
	}

	ImVec2 widgetPos = ImVec2(static_cast<float>(settings.killDeath.position.x), static_cast<float>(settings.killDeath.position.y));
	ImVec2 widgetSize = ImVec2(static_cast<float>(settings.killDeath.width), 
		settings.killDeath.sameLine ? (ImGui::GetTextLineHeightWithSpacing()) : (3 * ImGui::GetTextLineHeightWithSpacing()));
	if (widgetSize.x <= 0) {
		if (settings.killDeath.sameLine) {
			widgetSize.x = sizeBlue.x + sizeRed.x + sizeGreen.x + (2 * ImGui::CalcTextSize(" ").x) + 10;
		}
		else {
			widgetSize.x = textSize.x + 10;
		}
	}
	if (widgetPos.x <= 0) {
		widgetPos.x = 0;
	}
	if (widgetPos.y <= 0) {
		widgetPos.y = 0;
	}
	ImGui::SetNextWindowPos(widgetPos);
	ImGui::SetNextWindowSize(widgetSize);

	if (ImGui::Begin("K/D Tracker", nullptr, ImGuiWindowFlags_AlwaysAutoResize
											| ImGuiWindowFlags_NoTitleBar
											| ImGuiWindowFlags_NoInputs 
											| ImGuiWindowFlags_NoScrollbar)) {

		if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
		{
			ImGui::BeginTooltip();
			ImGui::Text("K/D Tracker");
			ImGui::EndTooltip();
		}
		// TODO set it to line height GetTextLineHeightWithSpacing
		// ImGui::SetCursorPosY(0.0f);

		if (settings.killDeath.sameLine) {
			switch (settings.killDeath.alignment) {
			case 0: // center
				ImGui::SetCursorPosX((widgetSize.x - sizeRed.x - sizeGreen.x - sizeBlue.x - (2 * ImGui::GetStyle().ItemInnerSpacing.x)) / 2);
				ImGui::TextColored(colorRed, textRed.c_str());
				ImGui::SameLine();
				ImGui::TextColored(colorGreen, textGreen.c_str());
				ImGui::SameLine();
				ImGui::TextColored(colorBlue, textBlue.c_str());
				break;
			case 1: // left
				ImGui::TextColored(colorRed, textRed.c_str());
				ImGui::SameLine();
				ImGui::TextColored(colorGreen, textGreen.c_str());
				ImGui::SameLine();
				ImGui::TextColored(colorBlue, textBlue.c_str());
				break;
			case 2: // right
				ImGui::SetCursorPosX((widgetSize.x - sizeRed.x - sizeGreen.x - sizeBlue.x - (2 * ImGui::GetStyle().ItemInnerSpacing.x)));
				ImGui::TextColored(colorRed, textRed.c_str());
				ImGui::SameLine();
				ImGui::TextColored(colorGreen, textGreen.c_str());
				ImGui::SameLine();
				ImGui::TextColored(colorBlue, textBlue.c_str());
				break;
			}
		}
		else {
			switch (settings.killDeath.alignment) {
			case 0: // center
				ImGui::SetCursorPosX((widgetSize.x - sizeRed.x) / 2);
				ImGui::TextColored(colorRed, textRed.c_str());
				ImGui::SetCursorPosX((widgetSize.x - sizeGreen.x) / 2);
				ImGui::TextColored(colorGreen, textGreen.c_str());
				ImGui::SetCursorPosX((widgetSize.x - sizeBlue.x) / 2);
				ImGui::TextColored(colorBlue, textBlue.c_str());
				break;
			case 1: // left
				ImGui::TextColored(colorRed, textRed.c_str());
				ImGui::TextColored(colorGreen, textGreen.c_str());
				ImGui::TextColored(colorBlue, textBlue.c_str());
				break;
			case 2: // right
				ImGui::SetCursorPosX(widgetSize.x - sizeRed.x - 5);
				ImGui::TextColored(colorRed, textRed.c_str());
				ImGui::SetCursorPosX(widgetSize.x - sizeGreen.x - 5);
				ImGui::TextColored(colorGreen, textGreen.c_str());
				ImGui::SetCursorPosX(widgetSize.x - sizeBlue.x - 5);
				ImGui::TextColored(colorBlue, textBlue.c_str());

				break;
			}
		}
		ImGui::End();
	}
}

void autoPipsResult() {
	if (!settings.autoPips.render) return;
	if (!isInWvW()) return;
	if (!NexusLink->IsGameplay) return;
	if (MumbleLink->Context.IsMapOpen) return;

	std::string text;

	autoResult = autoPipsCalculator.calculate();
	if (autoResult.timeRemainingMinutes > 0) {
		text = settings.autoPipsDisplayFormat;
		if (text.empty()) {
			text = "tick: @t, done: @d/1450, tickets: @t/365, rem.: @r";
		}
	}
	else {
		text = settings.autoPipsDoneText;
		if (text.empty()) {
			text = "Diamond Tier completed.";
		}
	}

	// 	//"per TicK: @p, done: @d of 1450, tickets: @t of 365, remaining: @r"
	replaceAll(text, "@p", std::to_string(autoResult.pipsPerTick));
	replaceAll(text, "@d", std::to_string(autoResult.finishedPips));
	replaceAll(text, "@t", std::to_string(autoResult.ticketsEarned));
	replaceAll(text, "@r", convertMinutesToHoursAndMinutes(autoResult.timeRemainingMinutes));

	ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
	ImVec2 widgetPos = ImVec2(static_cast<float>(settings.autoPips.position.x), static_cast<float>(settings.autoPips.position.y));
	ImVec2 widgetSize = ImVec2(static_cast<float>(settings.autoPips.width), textSize.y);
	if (widgetSize.x <= 0) {
		widgetSize.x = 300;
	}
	if (widgetPos.x <= 0) {
		widgetPos.x = 0;
	}
	if (widgetPos.y <= 0) {
		widgetPos.y = 0;
	}
	ImGui::SetNextWindowPos(widgetPos);
	ImGui::SetNextWindowSize(widgetSize);

	if (ImGui::Begin("AutoPipsResult", nullptr,ImGuiWindowFlags_AlwaysAutoResize 
												| ImGuiWindowFlags_NoTitleBar
												| ImGuiWindowFlags_NoInputs
												| ImGuiWindowFlags_NoScrollbar)) {

		float textX;
		switch (settings.autoPips.alignment) {
			case 0: textX = (widgetSize.x - textSize.x) / 2.0f; break;
			case 1: textX = 1; break; // extra padding to the left
			case 2: textX = widgetSize.x - textSize.x - 1; break; // -1 = extra padding to the right
			default: textX = (widgetSize.x - textSize.x) / 2.0f;
		}

		ImGui::SetCursorPosX(textX);
		ImGui::SetCursorPosY(0.0f);
		ImGui::Text(text.c_str());
		ImGui::End();
	}
}

void pipsCalculator() {
	if (!renderPipsCalculator) return;

	if (ImGui::Begin("PipsCalculator", &renderPipsCalculator, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Separator();
		ImGui::Text("Current Reward Chest");
		ImGui::BeginGroup();

		if (ImGui::BeginTable("RewardTable", 2)) {
			// Loop through the map and populate the table rows
			int radioButtonCount = 0;

			for (const auto& [key, value] : tierMap) {
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s", key.c_str());
				ImGui::TableSetColumnIndex(1);
				
				for (int i = 0; i < value; i++) {
					ImGui::PushID(radioButtonCount);
					if(ImGui::RadioButton("", &calculator.chestsDone, radioButtonCount)) {
						result = calculator.calculate();
					}
					ImGui::PopID();
					radioButtonCount++;
					ImGui::SameLine();
				}
			}
			ImGui::EndTable();
		}
		ImGui::EndGroup();

		ImGui::Separator();
		ImGui::Text("World placement");
		ImGui::BeginGroup();
		if(ImGui::RadioButton("First place", &calculator.rank, 3)) {
			result = calculator.calculate();
		}
		if(ImGui::RadioButton("Second place", &calculator.rank, 2)) {
			result = calculator.calculate();
		}
		if(ImGui::RadioButton("Third place", &calculator.rank, 1)) {
			result = calculator.calculate();
		}
		ImGui::EndGroup();

		ImGui::Separator();
		ImGui::Text("Personal Rank");
		ImGui::BeginGroup();
		if(ImGui::RadioButton("Wood (1-149)", &calculator.pRank1, 0)) {
			result = calculator.calculate();
		}
		if(ImGui::RadioButton("Bronze (150-619)", &calculator.pRank1, 1)) {
			result = calculator.calculate();
		}
		if(ImGui::RadioButton("Silver (620-1394)", &calculator.pRank1, 2)) {
			result = calculator.calculate();
		}
		if(ImGui::RadioButton("Gold (1395-2544)", &calculator.pRank1, 3)) {
			result = calculator.calculate();
		}
		if(ImGui::RadioButton("Platinum (2545-4094)", &calculator.pRank1, 4)) {
			result = calculator.calculate();
		}
		if(ImGui::RadioButton("Mithril (4095-6444)", &calculator.pRank1, 5)) {
			result = calculator.calculate();
		}
		if(ImGui::RadioButton("Diamond (6445-9999)", &calculator.pRank1, 6)) {
			result = calculator.calculate();
		}
		if(ImGui::RadioButton("God of WvW (10000)", &calculator.pRank1, 7)) {
			result = calculator.calculate();
		}
		ImGui::EndGroup();

		ImGui::Separator();
		ImGui::Text("Other bonuses");
		if (ImGui::Checkbox("Commitment", &calculator.commitment)) {
			result = calculator.calculate();
		}
		if(ImGui::Checkbox("Commander", &calculator.commander)) {
			result = calculator.calculate();
		}
		if(ImGui::Checkbox("Public Commander", &calculator.publicCommander)) {
			result = calculator.calculate();
		}
		
		ImGui::Separator();
		ImGui::Text("Result");
		ImGui::Text("Pips Per Tick:");
		ImGui::SameLine();
		ImGui::Text(std::to_string(result.pipsPerTick).c_str());

		ImGui::Text("Pips done:");
		ImGui::SameLine();
		ImGui::Text((std::to_string(result.finishedPips) + " of 1450").c_str());

		ImGui::Text("Tickets earned:");
		ImGui::SameLine();
		ImGui::Text((std::to_string(result.ticketsEarned) + " of 365").c_str());

		ImGui::Text("Time remaining:");
		ImGui::SameLine();
		ImGui::Text(convertMinutesToHoursAndMinutes(result.timeRemainingMinutes).c_str());

		ImGui::End();
	}
}

std::string convertMinutesToHoursAndMinutes(int totalMinutes) {
	int hours = totalMinutes / 60;
	int minutes = totalMinutes % 60;

	return std::to_string(hours) + " hours " + std::to_string(minutes) + " minutes";
}

void drawSkirmishCard(gw2api::wvw::Skirmish* skirmish) {
	// Calculate Victory Points
	std::map<std::string, int> teamScores = {
		{"Red", skirmish->scores.red},
		{"Blue", skirmish->scores.blue},
		{"Green", skirmish->scores.green}
	};
	std::vector<std::pair<std::string, int>> sortedColorValues(teamScores.begin(), teamScores.end());
	std::sort(sortedColorValues.begin(), sortedColorValues.end(), [](const auto& a, const auto& b) {
		return a.second > b.second;
		});
	teamScores[sortedColorValues[0].first] = 5;
	teamScores[sortedColorValues[1].first] = 4;
	teamScores[sortedColorValues[2].first] = 3;

	std::chrono::time_point matchStart = parse_date(match->start_time);
	auto duration = std::chrono::hours((skirmish->id - 1) * 2);
	auto skirmishStart = matchStart + duration;
	auto skirmishStartLocal = getUTCinLocalTimePoint(skirmishStart);
	auto skirmishEnd = skirmishStart + std::chrono::hours(2);
	auto skirmishEndLocal = getUTCinLocalTimePoint(skirmishEnd);

	ImGui::TableNextRow();
	ImGui::TableNextColumn();
	ThickSeparator(3.0f, 1.0f);
	ImGui::Text(std::to_string(skirmish->id).c_str());
	ImGui::TableNextColumn();
	ThickSeparator(3.0f, 0);
	ImGui::Text(format_date_output(skirmishStartLocal).c_str());
	ImGui::TableNextColumn();
	ThickSeparator(3.0f, 0);
	ImGui::Text(format_date_output(skirmishEndLocal).c_str());
	ImGui::TableNextColumn();
	ThickSeparator(3.0f, 0);

	ImGui::TableNextRow();
	ImGui::Separator();
	ImGui::TableNextColumn();
	ImGui::Text("Total");
	ImGui::TableNextColumn();
	ImGui::Text(std::to_string(skirmish->scores.red).c_str());
	ImGui::TableNextColumn();
	ImGui::Text(std::to_string(skirmish->scores.green).c_str());
	ImGui::TableNextColumn();
	ImGui::Text(std::to_string(skirmish->scores.blue).c_str());

	ImGui::Separator();
	ImGui::TableNextRow();
	ImGui::TableNextColumn();
	ImGui::Text("Victory Points");
	ImGui::TableNextColumn();
	ImGui::Text(std::to_string(teamScores["Red"]).c_str());
	ImGui::TableNextColumn();
	ImGui::Text(std::to_string(teamScores["Green"]).c_str());
	ImGui::TableNextColumn();
	ImGui::Text(std::to_string(teamScores["Blue"]).c_str());

	for (auto mapScore : skirmish->map_scores) {
		ImGui::Separator();
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::Text(mapScore.type.c_str());
		ImGui::TableNextColumn();
		ImGui::Text(std::to_string(mapScore.scores.red).c_str());
		ImGui::TableNextColumn();
		ImGui::Text(std::to_string(mapScore.scores.green).c_str());
		ImGui::TableNextColumn();
		ImGui::Text(std::to_string(mapScore.scores.blue).c_str());
	}
}