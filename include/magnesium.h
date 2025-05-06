#pragma once

#include <RE/Skyrim.h>

namespace Magnesium {

enum MessageType {
    ConsoleCommand,
    DisplayMessage,
};

class Message {
  public:
    MessageType type;
    std::string data;

    Message(MessageType, std::string);

    std::string toString();

    static Message fromJson();

    static Message Command(std::string);
    static Message Display(std::string);
};

std::string NextMessage(RE::StaticFunctionTag*);

} // namespace Magnesium