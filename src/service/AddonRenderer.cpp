#include "AddonRenderer.h"

/* render protos */
void pipsCalculator();
void autoPipsResult();
void killDeathRatio();
std::string convertMinutesToHoursAndMinutes(int totalMinutes);

PipsCalculator calculator = PipsCalculator();
PipsResult result = calculator.calculate();
PipsResult autoResult = autoPipsCalculator.calculate();

ImVec4 colorRed = ImVec4(1, 0, 0, 1);
ImVec4 colorGreen = ImVec4(0, 1, 0, 1);
ImVec4 colorBlue = ImVec4(0, 0.5, 1, 1);

void Renderer::preRender() {
	// TODO impl
}
void Renderer::render() {
	pipsCalculator();
	autoPipsResult();
	killDeathRatio();
}
void Renderer::postRender() {
	// TODO impl
}

void Renderer::unload() {
	// TODO Impl
}

//========================================================

void killDeathRatio() {
	if (!settings.renderKillDeathRatio) return;
	if (!isInWvW()) return;
	if (match == nullptr) return;
	if (!NexusLink->IsGameplay) return;
	if (MumbleLink->Context.IsMapOpen) return;

	int redKills, redDeaths;
	int blueKills, blueDeaths;
	int greenKills, greenDeaths;

	redKills = match->kills.red;
	redDeaths = match->deaths.red;
	float redKD;
	if (redDeaths > 0) { redKD = static_cast<float>(redKills) / static_cast<float>(redDeaths); }
	else { redKD = redKills; }

	blueKills = match->kills.blue;
	blueDeaths = match->deaths.blue;
	float blueKD;
	if (blueDeaths > 0) { blueKD = static_cast<float>(blueKills) / static_cast<float>(blueDeaths); }
	else { blueKD = blueKills; }


	greenKills = match->kills.green;
	greenDeaths = match->deaths.green;
	float greenKD;
	if (greenDeaths > 0) { greenKD = static_cast<float>(greenKills) / static_cast<float>(greenDeaths); }
	else { greenKD = greenKills; }

	ImVec2 maxSize = ImGui::CalcTextSize("Green:");
	
	ImVec2 widgetPos = ImVec2(settings.killDeathPosition.x, settings.killDeathPosition.y);
	if (widgetPos.x <= 0) {
		widgetPos.x = 0;
	}
	if (widgetPos.y <= 0) {
		widgetPos.y = 0;
	}

	ImGui::SetNextWindowPos(widgetPos);

	if (ImGui::Begin("K/D Tracker", nullptr, ImGuiWindowFlags_AlwaysAutoResize
												| ImGuiWindowFlags_NoTitleBar
												| ImGuiWindowFlags_NoInputs )) {

		if (settings.renderKDSameLine) {
			ImGui::TextColored(colorRed, ("Red: " + std::format("{:.2f}", redKD)).c_str());
			ImGui::SameLine();
			ImGui::TextColored(colorGreen, ("Green: " + std::format("{:.2f}", greenKD)).c_str());
			ImGui::SameLine();
			ImGui::TextColored(colorBlue, ("Blue: " + std::format("{:.2f}", blueKD)).c_str());
		}
		else {
			ImGui::TextColored(colorRed, "Red:");
			ImGui::SameLine();
			ImGui::SetCursorPosX(maxSize.x + 10);
			ImGui::TextColored(colorRed, std::format("{:.2f}", redKD).c_str());

			ImGui::TextColored(colorGreen, "Green:");
			ImGui::SameLine();
			ImGui::SetCursorPosX(maxSize.x + 10);
			ImGui::TextColored(colorGreen, std::format("{:.2f}", greenKD).c_str());

			ImGui::TextColored(colorBlue, "Blue:");
			ImGui::SameLine();
			ImGui::SetCursorPosX(maxSize.x + 10);
			ImGui::TextColored(colorBlue, std::format("{:.2f}", blueKD).c_str());
		}
		ImGui::End();
	}
}

void autoPipsResult() {
	if (!settings.renderAutoPipsResult) return;
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
	ImVec2 widgetPos = ImVec2(settings.autoPipsPosition.x, settings.autoPipsPosition.y);
	ImVec2 widgetSize = ImVec2(settings.autoPipsWidth, textSize.y);
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

	if (ImGui::Begin("AutoPipsResult", nullptr, ImGuiWindowFlags_AlwaysAutoResize 
												| ImGuiWindowFlags_NoTitleBar
												| ImGuiWindowFlags_NoInputs)) {

		float textX;
		switch (settings.autoPipsAlignment) {
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