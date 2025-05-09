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
    if (!GetMessagingInterface()->RegisterListener([](MessagingInterface::
                                                          Message* message) {
            switch (message->type) {
            // Skyrim lifecycle events.
            case MessagingInterface::
                kPostLoad: // Called after all plugins have finished running
                           // SKSEPlugin_Load. It is now safe to do
                           // multithreaded operations, or operations against
                           // other plugins.
                log::info("Starting HTTP server");
                StartServer();

                break;
            case MessagingInterface::kPostPostLoad: // Called after all
                                                    // kPostLoad message
                                                    // handlers have run.
            case MessagingInterface::kInputLoaded:  // Called when all game data
                                                    // has been found.
                break;
            case MessagingInterface::kDataLoaded: // All ESM/ESL/ESP plugins
                                                  // have loaded, main menu is
                                                  // now active.
                // It is now safe to access form data.

                PushMessage(Message::Display("test message!"));
                break;

            // Skyrim game events.
            case MessagingInterface::kNewGame: // Player starts a new game from
                                               // main menu.
            case MessagingInterface::kPreLoadGame: // Player selected a game to
                                                   // load, but it hasn't loaded
                                                   // yet. Data will be the name
                                                   // of the loaded save.
            case MessagingInterface::kPostLoadGame:

                break; // Player's selected save game has finished
                       // loading. Data will be a boolean indicating
                       // whether the load was successful.
            case MessagingInterface::kSaveGame: // The player has saved a game.
                                                // Data will be the save name.
            case MessagingInterface::kDeleteGame: // The player deleted a saved
                                                  // game from within the load
                                                  // menu.
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
