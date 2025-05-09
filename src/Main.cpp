#include "config.h"
#include "magnesium.h"
#include "papyrus.h"

#include <stddef.h>

using namespace RE::BSScript;
using namespace Magnesium;
using namespace SKSE;
using namespace SKSE::log;
using namespace SKSE::stl;

namespace {
void InitializeLogging() {
    const auto LOG_LEVEL = Config::GetSingleton().log_level;

    auto path = log_directory();
    if (!path) {
        report_and_fail("Unable to lookup SKSE logs directory.");
    }
    *path /= PluginDeclaration::GetSingleton()->GetName();
    *path += L".log";

    std::shared_ptr<spdlog::logger> log;
    if (IsDebuggerPresent()) {
        log = std::make_shared<spdlog::logger>(
            "Global", std::make_shared<spdlog::sinks::msvc_sink_mt>());
    } else {
        log = std::make_shared<spdlog::logger>(
            "Global", std::make_shared<spdlog::sinks::basic_file_sink_mt>(
                          path->string(), true));
    }

    spdlog::set_default_logger(std::move(log));
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [%t] [%s:%#] %v");
    spdlog::set_level(LOG_LEVEL);
    spdlog::flush_on(LOG_LEVEL);
}

void InitializePapyrus() {
    log::trace("Initializing Papyrus bindings...");
    if (GetPapyrusInterface()->Register(Magnesium::RegisterFunctions)) {
        log::debug("Papyrus functions bound.");
    } else {
        stl::report_and_fail("Failure to register Papyrus bindings.");
    }
}

void InitializeMessaging() {
    if (!GetMessagingInterface()->RegisterListener(
            [](MessagingInterface::Message* message) {
                switch (message->type) {
                case MessagingInterface::kPostLoad:

                    log::info("Starting HTTP server");
                    StartServer();

                    break;

                case MessagingInterface::kDataLoaded:

                    // set default fetch interval
                    PushMessage(Message::SetInterval(Config::GetSingleton().interval));
                    break;
                }
            })) {
        stl::report_and_fail("Unable to register message listener.");
    }
}
} // namespace

SKSEPluginLoad(const LoadInterface* skse) {
    InitializeLogging();

    auto* plugin = PluginDeclaration::GetSingleton();
    log::info("{} is loading...", plugin->GetName());

    Init(skse);
    InitializeMessaging();
    InitializePapyrus();

    log::info("{} has finished loading", plugin->GetName());
    return true;
}
