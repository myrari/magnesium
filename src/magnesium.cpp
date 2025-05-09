#include "magnesium.h"
#include "config.h"

#include <RE/Skyrim.h>
#include <httplib.h>
#include <queue>
#include <thread>

using namespace std;
using namespace Magnesium;
using namespace SKSE;
using namespace httplib;

static std::queue<Message> messageQueue;

std::string messageTypeToString(MessageType messageType) {
    switch (messageType) {
    case ConsoleCommand:
        return "ConsoleCommand";
    case DisplayMessage:
        return "DisplayMessage";
    default:
        return "Unknown";
    }
}

Message::Message(MessageType type, string data) : type(type), data(data) {}

string Message::toString() {
    // MESSAGE SERIALIZATION SCHEMA:
    // ${messageType};{data}
    // ; is delimiter
    return std::format("{};{}", messageTypeToString(this->type), this->data);
}

Message Message::fromJson() {
    return Message::Display("Deserialize not yet implemented!");
}

Message Message::Command(std::string command) {
    return Message(MessageType::ConsoleCommand, command);
}

Message Message::Display(std::string message) {
    return Message(MessageType::DisplayMessage, message);
}

void Magnesium::PushMessage(Message message) {
    log::trace("Pushing message: {}", message.toString());
    messageQueue.push(message);
}

string Magnesium::PopMessage(RE::StaticFunctionTag*) {
    if (messageQueue.empty()) {
        log::trace("Message queue empty");
        return "";
    }
    auto message = messageQueue.front();
    messageQueue.pop();
    auto msgStr = message.toString();
    log::trace("Sent message {}", msgStr);
    return msgStr;
}

const char* main_page = R"(
<form id="formElem">
    <label for="msg">Message:</label>
    <input type="text" id="msg" name="msg"><br><br>
    <input type="submit">
</form>
<script>
    formElem.onsubmit = async (e) => {
        e.preventDefault();
        await fetch('/display', {
            method: 'POST',
            body: new FormData(formElem)
        });
    };
</script>
)";

void RunServer(int port) {
    log::debug("Server worker thread running");

    Server svr;

    if (!svr.is_valid()) {
        log::error("Error creating HTTP server!");
        return;
    }

    svr.set_error_handler([](const Request& req, Response& res) {
        const char* fmt =
            "<p>Error Status: <span style='color:red;'>%d</span></p>";
        char buf[BUFSIZ];
        snprintf(buf, sizeof(buf), fmt, res.status);
        res.set_content(buf, "text/html");
        log::error("HTTP error when requesting: {} - {}", req.path, res.status);
    });

    svr.Get("/", [](const Request&, Response& res) {
        log::debug("HTTP request to /");
        res.set_content(main_page, "text/html");
    });

    svr.Post("/display", [](const Request& req, Response& res) {
        auto msg = req.get_file_value("msg");
        log::debug("Received display message: {}", msg.content);
        PushMessage(Message::Display(msg.content));

        res.set_content("done", "text/plain");
    });

    log::debug("Launching server");

    svr.listen("localhost", port);
}

void Magnesium::StartServer() {
    auto port = Config::GetSingleton().port;

    std::thread srv_t(RunServer, port);

    // TODO: we are never actually stopping/deallocating this thread, so this is
    // a (small) memory leak
    srv_t.detach();

    return;
}
