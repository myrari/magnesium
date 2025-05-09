#pragma once

namespace Magnesium {

struct LogLevelMap : public std::map<std::string, spdlog::level::level_enum> {
    LogLevelMap() {
        this->operator[]("trace") = spdlog::level::trace;
        this->operator[]("debug") = spdlog::level::debug;
        this->operator[]("info") = spdlog::level::info;
        this->operator[]("warn") = spdlog::level::warn;
        this->operator[]("err") = spdlog::level::err;
        this->operator[]("critical") = spdlog::level::critical;
        this->operator[]("off") = spdlog::level::off;
    }
};

class Config {
  public:
    spdlog::level::level_enum log_level;

    int port;

    [[nodiscard]] static const Config& GetSingleton();

    static const Config& Default() noexcept {
        return Config(spdlog::level::info, 8080);
    }

  private:
    Config(spdlog::level::level_enum level, int port) : log_level(level), port(port) {};
};

} // namespace Magnesium