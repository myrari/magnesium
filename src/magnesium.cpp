#include "magnesium.h"

#include <RE/Skyrim.h>
#include <queue>

using namespace std;
using namespace Magnesium;

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

string Magnesium::NextMessage(RE::StaticFunctionTag* _) {
    if (messageQueue.empty()) {
        return "";
    }
    auto message = messageQueue.front();
    messageQueue.pop();
    return message.toString();
}