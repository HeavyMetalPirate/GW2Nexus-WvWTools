#ifndef GW2API_WVW_H
#define GW2API_WVW_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

// Helpers for date parsing
inline std::chrono::system_clock::time_point parse_date(const std::string& date_str) {
    std::tm tm = {};
    std::istringstream ss(date_str);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}
inline std::string format_date(const std::chrono::system_clock::time_point& time_point) {
    std::time_t time_t = std::chrono::system_clock::to_time_t(time_point);
    std::tm tm;
    gmtime_s(&tm, &time_t);
    std::ostringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    return ss.str();
}

namespace gw2api {
    namespace wvw {
        struct Scores {
            int red;
            int blue;
            int green;
        };

        struct Worlds {
            int red;
            int blue;
            int green;
        };

        struct Deaths {
            int red;
            int blue;
            int green;
        };

        struct Kills {
            int red;
            int blue;
            int green;
        };

        struct VictoryPoints {
            int red;
            int blue;
            int green;
        };

        struct MapScores {
            std::string type;
            Scores scores;
        };

        struct Skirmish {
            int id;
            Scores scores;
            std::vector<MapScores> map_scores;
        };

        struct Objective {
            std::string id;
            std::string type;
            std::string owner;
            std::string last_flipped;
            std::string claimed_by;
            std::string claimed_at;
            int points_tick;
            int points_capture;
            int yaks_delivered;
            std::vector<int> guild_upgrades;
        };

        struct Map {
            int id;
            std::string type;
            Scores scores;
            std::vector<Objective> objectives;
            Deaths deaths;
            Kills kills;
        };

        struct Match {
            std::string id;
            std::string start_time;
            std::string end_time;
            Scores scores;
            Worlds worlds;
            std::map<std::string, std::vector<int>> all_worlds;
            Deaths deaths;
            Kills kills;
            VictoryPoints victory_points;
            std::vector<Skirmish> skirmishes;
            std::vector<Map> maps;
        };
    }
}

namespace nlohmann {
    template <>
    struct adl_serializer<gw2api::wvw::Scores> {
        static void to_json(json& j, const gw2api::wvw::Scores& s) {
            j = json{ {"red", s.red}, {"blue", s.blue}, {"green", s.green} };
        }

        static void from_json(const json& j, gw2api::wvw::Scores& s) {
            j.at("red").get_to(s.red);
            j.at("blue").get_to(s.blue);
            j.at("green").get_to(s.green);
        }
    };

    template <>
    struct adl_serializer<gw2api::wvw::Worlds> {
        static void to_json(json& j, const gw2api::wvw::Worlds& w) {
            j = json{ {"red", w.red}, {"blue", w.blue}, {"green", w.green} };
        }

        static void from_json(const json& j, gw2api::wvw::Worlds& w) {
            j.at("red").get_to(w.red);
            j.at("blue").get_to(w.blue);
            j.at("green").get_to(w.green);
        }
    };

    template <>
    struct adl_serializer<gw2api::wvw::Deaths> {
        static void to_json(json& j, const gw2api::wvw::Deaths& d) {
            j = json{ {"red", d.red}, {"blue", d.blue}, {"green", d.green} };
        }

        static void from_json(const json& j, gw2api::wvw::Deaths& d) {
            j.at("red").get_to(d.red);
            j.at("blue").get_to(d.blue);
            j.at("green").get_to(d.green);
        }
    };

    template <>
    struct adl_serializer<gw2api::wvw::Kills> {
        static void to_json(json& j, const gw2api::wvw::Kills& k) {
            j = json{ {"red", k.red}, {"blue", k.blue}, {"green", k.green} };
        }

        static void from_json(const json& j, gw2api::wvw::Kills& k) {
            j.at("red").get_to(k.red);
            j.at("blue").get_to(k.blue);
            j.at("green").get_to(k.green);
        }
    };

    template <>
    struct adl_serializer<gw2api::wvw::VictoryPoints> {
        static void to_json(json& j, const gw2api::wvw::VictoryPoints& v) {
            j = json{ {"red", v.red}, {"blue", v.blue}, {"green", v.green} };
        }

        static void from_json(const json& j, gw2api::wvw::VictoryPoints& v) {
            j.at("red").get_to(v.red);
            j.at("blue").get_to(v.blue);
            j.at("green").get_to(v.green);
        }
    };

    template <>
    struct adl_serializer<gw2api::wvw::MapScores> {
        static void to_json(json& j, const gw2api::wvw::MapScores& m) {
            j = json{ {"type", m.type}, {"scores", m.scores} };
        }

        static void from_json(const json& j, gw2api::wvw::MapScores& m) {
            j.at("type").get_to(m.type);
            j.at("scores").get_to(m.scores);
        }
    };

    template <>
    struct adl_serializer<gw2api::wvw::Skirmish> {
        static void to_json(json& j, const gw2api::wvw::Skirmish& s) {
            j = json{ {"id", s.id}, {"scores", s.scores}, {"map_scores", s.map_scores} };
        }

        static void from_json(const json& j, gw2api::wvw::Skirmish& s) {
            j.at("id").get_to(s.id);
            j.at("scores").get_to(s.scores);
            j.at("map_scores").get_to(s.map_scores);
        }
    };

    template <>
    struct adl_serializer<gw2api::wvw::Objective> {
        static void to_json(json& j, const gw2api::wvw::Objective& o) {
            j = json{
                {"id", o.id}, {"type", o.type}, {"owner", o.owner}, {"last_flipped", o.last_flipped},
                {"claimed_by", o.claimed_by}, {"claimed_at", o.claimed_at}, {"points_tick", o.points_tick},
                {"points_capture", o.points_capture}, {"yaks_delivered", o.yaks_delivered}, {"guild_upgrades", o.guild_upgrades}
            };
        }

        static void from_json(const json& j, gw2api::wvw::Objective& o) {
            j.at("id").get_to(o.id);
            j.at("type").get_to(o.type);
            j.at("owner").get_to(o.owner);
            if (j.contains("last_flipped") && !j.at("last_flipped").is_null())
                j.at("last_flipped").get_to(o.last_flipped);
            if(j.contains("claimed_by") && !j.at("claimed_by").is_null())
                j.at("claimed_by").get_to(o.claimed_by);
            if (j.contains("claimed_at") && !j.at("claimed_at").is_null())
                j.at("claimed_at").get_to(o.claimed_at);
            j.at("points_tick").get_to(o.points_tick);
            j.at("points_capture").get_to(o.points_capture);
            if (j.contains("yaks_delivered"))
                j.at("yaks_delivered").get_to(o.yaks_delivered);
            if (j.contains("guild_upgrades"))
                j.at("guild_upgrades").get_to(o.guild_upgrades);
        }
    };

    template <>
    struct adl_serializer<gw2api::wvw::Map> {
        static void to_json(json& j, const gw2api::wvw::Map& m) {
            j = json{
                {"id", m.id}, {"type", m.type}, {"scores", m.scores},
                {"objectives", m.objectives}, {"deaths", m.deaths}, {"kills", m.kills}
            };
        }

        static void from_json(const json& j, gw2api::wvw::Map& m) {
            j.at("id").get_to(m.id);
            j.at("type").get_to(m.type);
            j.at("scores").get_to(m.scores);
            j.at("objectives").get_to(m.objectives);
            j.at("deaths").get_to(m.deaths);
            j.at("kills").get_to(m.kills);
        }
    };

    template <>
    struct adl_serializer<gw2api::wvw::Match> {
        static void to_json(json& j, const gw2api::wvw::Match& m) {
            j = json{
                {"id", m.id}, {"start_time", m.start_time}, {"end_time", m.end_time}, {"scores", m.scores},
                {"worlds", m.worlds}, {"all_worlds", m.all_worlds}, {"deaths", m.deaths}, {"kills", m.kills},
                {"victory_points", m.victory_points}, {"skirmishes", m.skirmishes}, {"maps", m.maps}
            };
        }

        static void from_json(const json& j, gw2api::wvw::Match& m) {
            j.at("id").get_to(m.id);
            j.at("start_time").get_to(m.start_time);
            j.at("end_time").get_to(m.end_time);
            j.at("scores").get_to(m.scores);
            j.at("worlds").get_to(m.worlds);
            j.at("all_worlds").get_to(m.all_worlds);
            j.at("deaths").get_to(m.deaths);
            j.at("kills").get_to(m.kills);
            j.at("victory_points").get_to(m.victory_points);
            j.at("skirmishes").get_to(m.skirmishes);
            j.at("maps").get_to(m.maps);
        }
    };
}
#endif