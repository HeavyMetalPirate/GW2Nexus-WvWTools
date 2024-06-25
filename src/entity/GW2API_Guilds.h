#ifndef GW2API_GUILDS_H
#define GW2API_GUILDS_H

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace gw2api {
    namespace guilds {
        struct Guild {
            std::string id;
            std::string name;
            std::string tag;
        };
        inline void to_json(json& j, const Guild& g) {
            j = json{
                {"id", g.id},
                {"name", g.name},
                {"tag", g.tag}
            };
        }
        inline void from_json(const json& j, Guild& g) {
            j.at("id").get_to(g.id);
            j.at("name").get_to(g.name);
            j.at("tag").get_to(g.tag);
        }
    }
}

#endif