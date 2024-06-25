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
    std::time_t time_utc = _mkgmtime(&tm);
    return std::chrono::system_clock::from_time_t(time_utc);
}
inline std::string format_date(const std::chrono::system_clock::time_point& time_point) {
    std::time_t time_t = std::chrono::system_clock::to_time_t(time_point);
    std::tm tm;
    localtime_s(&tm, &time_t);
    std::ostringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S%z");
    return ss.str();
}
inline std::string format_date_output(const std::chrono::system_clock::time_point& time_point) {
    std::time_t time_t = std::chrono::system_clock::to_time_t(time_point);
    std::tm tm;
    localtime_s(&tm, &time_t);
    std::ostringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}
inline std::chrono::system_clock::time_point getUTCinLocalTimePoint(const std::chrono::system_clock::time_point utc) {
    std::time_t time_t_utc = std::chrono::system_clock::to_time_t(utc);
    std::tm local_tm;
    localtime_s(&local_tm, &time_t_utc);

    std::time_t time_t_local = std::mktime(&local_tm);
    return std::chrono::system_clock::from_time_t(time_t_local);
}
inline std::string getUTCinLocalDate(const std::string& date_str) {
    std::chrono::system_clock::time_point utc = parse_date(date_str);
    std::chrono::system_clock::time_point local = getUTCinLocalTimePoint(utc);
    return format_date_output(local);
}

namespace gw2api {
    namespace wvw {
        struct ObjectiveUpgrade {
            int id;
            std::string name;
            std::string description;
        };
        inline void to_json(nlohmann::json& j, const ObjectiveUpgrade& i) {
            j = nlohmann::json{
                {"id", i.id},
                {"name", i.name},
                {"description", i.description}
            };
        }
        inline void from_json(const nlohmann::json& j, ObjectiveUpgrade& i) {
            j.at("id").get_to(i.id);
            j.at("name").get_to(i.name);
            j.at("description").get_to(i.description);
        }

        struct Location {
            std::string id;
            std::string name;
            int sector_id;
            std::string type;
            std::string map_type;
            int map_id;
            std::optional<int> upgrade_id;
            std::optional<std::vector<double>> coord;
            std::vector<double> label_coord;
            std::optional<std::string> marker;
            std::string chat_link;
        };
        inline void to_json(nlohmann::json& j, const Location& l) {
            j = nlohmann::json{
                {"id", l.id},
                {"name", l.name},
                {"sector_id", l.sector_id},
                {"type", l.type},
                {"map_type", l.map_type},
                {"map_id", l.map_id},
                {"label_coord", l.label_coord},
                {"chat_link", l.chat_link}
            };

            if (l.upgrade_id) {
                j["upgrade_id"] = *l.upgrade_id;
            }

            if (l.coord) {
                j["coord"] = *l.coord;
            }

            if (l.marker) {
                j["marker"] = *l.marker;
            }
        }

        inline void from_json(const nlohmann::json& j, Location& l) {
            j.at("id").get_to(l.id);
            j.at("name").get_to(l.name);
            j.at("sector_id").get_to(l.sector_id);
            j.at("type").get_to(l.type);
            j.at("map_type").get_to(l.map_type);
            j.at("map_id").get_to(l.map_id);
            j.at("chat_link").get_to(l.chat_link);

            if (j.contains("label_coord")) {
                j.at("label_coord").get_to(l.label_coord);
            }

            if (j.contains("upgrade_id")) {
                l.upgrade_id = j.at("upgrade_id").get<int>();
            }
            else {
                l.upgrade_id = std::nullopt;
            }

            if (j.contains("coord")) {
                l.coord = j.at("coord").get<std::vector<double>>();
            }
            else {
                l.coord = std::nullopt;
            }

            if (j.contains("marker")) {
                l.marker = j.at("marker").get<std::string>();
            }
            else {
                l.marker = std::nullopt;
            }
        }

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