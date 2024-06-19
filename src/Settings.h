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

struct Settings {
    std::string matchBegin;
    std::string matchEnd;
    int allianceId;
    int currentRank;
    int pipsProgressed;
    bool hasCommitment;

    // display settings
    bool renderAutoPipsResult;
    Position autoPipsPosition;
    int autoPipsWidth;
    int autoPipsAlignment;
    bool renderKillDeathRatio;
    Position killDeathPosition;
    bool renderKDSameLine;

    std::string autoPipsDisplayFormat;
    std::string autoPipsDoneText;

};


// Function to convert the struct to JSON
inline void to_json(json& j, const Settings& settings) {
    j = json{
        {"matchBegin", settings.matchBegin},
        {"matchEnd", settings.matchEnd},
        {"allianceId", settings.allianceId},
        {"currentRank", settings.currentRank},
        {"pipsProgressed", settings.pipsProgressed},
        {"hasCommitment", settings.hasCommitment},
        {"renderAutoPipsResult", settings.renderAutoPipsResult},
        {"autoPipsPosition", settings.autoPipsPosition},
        {"autoPipsAlignment", settings.autoPipsAlignment},
        {"autoPipsWidth", settings.autoPipsWidth},
        {"renderKillDeathRatio", settings.renderKillDeathRatio},
        {"killDeathPosition", settings.killDeathPosition},
        {"renderKDSameLine", settings.renderKDSameLine},
        {"autoPipsDisplayFormat", settings.autoPipsDisplayFormat},
        {"autoPipsDoneText", settings.autoPipsDoneText}
    };
}

// Function to populate the struct from JSON
inline void from_json(const json& j, Settings& s) {
    j.at("matchBegin").get_to(s.matchBegin);
    j.at("matchEnd").get_to(s.matchEnd);
    j.at("allianceId").get_to(s.allianceId);
    j.at("currentRank").get_to(s.currentRank);
    j.at("pipsProgressed").get_to(s.pipsProgressed);
    j.at("hasCommitment").get_to(s.hasCommitment);

    if(j.contains("renderAutoPipsResult"))
        j.at("renderAutoPipsResult").get_to(s.renderAutoPipsResult);
    if (j.contains("autoPipsPosition"))
        j.at("autoPipsPosition").get_to(s.autoPipsPosition);
    if (j.contains("renderKillDeathRatio"))
        j.at("renderKillDeathRatio").get_to(s.renderKillDeathRatio);
    if (j.contains("killDeathPosition"))
        j.at("killDeathPosition").get_to(s.killDeathPosition);
    if (j.contains("renderKDSameLine"))
        j.at("renderKDSameLine").get_to(s.renderKDSameLine);
    if (j.contains("autoPipsDisplayFormat"))
        j.at("autoPipsDisplayFormat").get_to(s.autoPipsDisplayFormat);
    if (j.contains("autoPipsDoneText"))
        j.at("autoPipsDoneText").get_to(s.autoPipsDoneText);
    if (j.contains("autoPipsWidth"))
        j.at("autoPipsWidth").get_to(s.autoPipsWidth);
    if (j.contains("autoPipsAlignment"))
        j.at("autoPipsAlignment").get_to(s.autoPipsAlignment);
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

#endif