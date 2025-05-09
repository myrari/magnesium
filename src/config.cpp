#include "config.h"

#include <yaml-cpp/yaml.h>

using namespace Magnesium;
using namespace SKSE;

const Config& Magnesium::Config::GetSingleton() {
    static Config instance = Config::Default();

    static std::atomic_bool initialized;
    static std::latch latch(1);
    if (!initialized.exchange(true)) {
        auto cfg = YAML::LoadFile("Data/SKSE/Plugins/magnesium.yaml");

        auto log_level = cfg["log_level"].as<std::string>();
        LogLevelMap log_level_map;
        instance.log_level = log_level_map[log_level];

        auto port = cfg["port"].as<int>();
        instance.port = port;

        latch.count_down();
    }
    latch.wait();

    return instance;
}
