#ifndef SETTINGS_H
#define SETTINGS_H

#include <nlohmann/json.hpp>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <sstream>

using namespace std::chrono;
using json = nlohmann::json;

struct Position {
    int x;
    int y;
};
inline void to_json(json& j, const Position& position) {
    j = json{
        {"x", position.x},
        {"y", position.y}
    };
}
inline void from_json(const json& j, Position& position) {
    j.at("x").get_to(position.x);
    j.at("y").get_to(position.y);
}

struct AccountSettings {
    std::string matchBegin;
    std::string matchEnd;
    int allianceId;
    int currentRank;
    int pipsProgressed;
    bool hasCommitment;
};
inline void to_json(json& j, const AccountSettings& settings) {
    j = json{
        {"matchBegin", settings.matchBegin},
        {"matchEnd", settings.matchEnd},
        {"allianceId", settings.allianceId},
        {"currentRank", settings.currentRank},
        {"pipsProgressed", settings.pipsProgressed},
        {"hasCommitment", settings.hasCommitment}
    };
}
inline void from_json(const json& j, AccountSettings& s) {
    j.at("matchBegin").get_to(s.matchBegin);
    j.at("matchEnd").get_to(s.matchEnd);
    j.at("allianceId").get_to(s.allianceId);
    j.at("currentRank").get_to(s.currentRank);
    j.at("pipsProgressed").get_to(s.pipsProgressed);
    j.at("hasCommitment").get_to(s.hasCommitment);
}

struct WidgetSettings {
    bool render;
    Position position;
    bool sameLine;
    int width;
    int alignment;
    int teamnameMode;
};
inline void to_json(json& j, const WidgetSettings& settings) {
    j = json{
        {"render", settings.render},
        {"sameLine", settings.sameLine},
        {"position", settings.position},
        {"width", settings.width},
        {"alignment", settings.alignment},
        {"teamnameMode", settings.teamnameMode}
    };
}
inline void from_json(const json& j, WidgetSettings& settings) {
    j.at("render").get_to(settings.render);
    j.at("sameLine").get_to(settings.sameLine);
    j.at("position").get_to(settings.position);
    j.at("width").get_to(settings.width);
    j.at("alignment").get_to(settings.alignment);
    j.at("teamnameMode").get_to(settings.teamnameMode);
}

struct Settings {
    std::map<std::string, AccountSettings> accountSettings;

    WidgetSettings autoPips;
    WidgetSettings killDeath;
    WidgetSettings victoryPoints;
    WidgetSettings skirmishScore;

    // specific settings per widget
    std::string autoPipsDisplayFormat;
    std::string autoPipsDoneText;
};


// Function to convert the struct to JSON
inline void to_json(json& j, const Settings& settings) {
    j = json{
        {"accountSettings", settings.accountSettings},
        {"autoPipsWidget", settings.autoPips},
        {"killDeathWidget", settings.killDeath},
        {"victoryPointsWidget", settings.victoryPoints},
        {"skirmishScoreWidget", settings.skirmishScore},
        {"autoPipsDisplayFormat", settings.autoPipsDisplayFormat},
        {"autoPipsDoneText", settings.autoPipsDoneText}
    };
}

// Function to populate the struct from JSON
inline void from_json(const json& j, Settings& s) {
    if(j.contains("accountSettings"))
        j.at("accountSettings").get_to(s.accountSettings);

    if (j.contains("autoPipsWidget"))
        j.at("autoPipsWidget").get_to(s.autoPips);
    if (j.contains("killDeathWidget"))
        j.at("killDeathWidget").get_to(s.killDeath);
    if (j.contains("victoryPointsWidget"))
        j.at("victoryPointsWidget").get_to(s.victoryPoints);
    if (j.contains("skirmishScoreWidget"))
        j.at("skirmishScoreWidget").get_to(s.skirmishScore);

    if (j.contains("autoPipsDisplayFormat"))
        j.at("autoPipsDisplayFormat").get_to(s.autoPipsDisplayFormat);
    if (j.contains("autoPipsDoneText"))
        j.at("autoPipsDoneText").get_to(s.autoPipsDoneText);
}

inline system_clock::time_point string_to_time_point(const std::string& s) {
    std::tm tm = {};
    std::stringstream ss(s);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    return system_clock::from_time_t(std::mktime(&tm));
}
inline bool compare_times(std::string one, std::string other) {
    auto begin = string_to_time_point(one);
    auto end = string_to_time_point(other);
    return begin < end;
}

// temporary settings - no storage
extern bool renderPipsCalculator;
extern bool renderMatchExplorer;

#endif