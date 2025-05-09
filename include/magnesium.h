#pragma once

#include <RE/Skyrim.h>

namespace Magnesium {

enum MessageType {
    ConsoleCommand,
    DisplayMessage,
    SetInterval,
};

class Message {
  public:
    MessageType type;
    std::string data;

    std::string toString();

    static Message Command(std::string);
    static Message Display(std::string);
    static Message SetInterval(float);

  private:
    Message(MessageType, std::string);
};

void PushMessage(Message);
std::string PopMessage(RE::StaticFunctionTag*);

void StartServer();

} // namespace Magnesium