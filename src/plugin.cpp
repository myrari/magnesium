// Copyright (c) 2024 Myra Roberts
// This file is part of Magnesium.
//
// Magnesium is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation, either version 2 of the License, or (at your option) any later
// version.
//
// Magnesium is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.

#include <queue>
#include <sstream>
#include <vector>

#include <afxwin.h>

#include "../lib/mongoose.h"

#include "logger.h"

#define PORT "8000"

// from https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
std::vector<std::string> splitString (const std::string &s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss (s);
    std::string item;

    while (getline (ss, item, delim)) {
        result.push_back (item);
    }

    return result;
}

namespace Magnesium {
    enum MessageType {
        ConsoleCommand,
        DisplayMessage,
    };

    class Message {
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

    public:
        MessageType type;
        std::string data;

        Message(MessageType type, std::string data): type(type), data(data) {};

        static Message Command(std::string command) {
            return Message(MessageType::ConsoleCommand, command);
        }

        static Message Display(std::string message) {
            return Message(MessageType::DisplayMessage, message);
        }

        std::string toString() {
            // MESSAGE SERIALIZATION SCHEMA:
            // ${messageType};{data}
            // ; is delimiter
            return std::format("{};{}", messageTypeToString(this->type), this->data);
        }

        static Message fromString(std::string string) {
            char delimiter = ';';
            std::vector<std::string> components = splitString(string, delimiter);
            std::string type = components[0];
            std::string data = components[1];

            if (type == "DisplayMessage") {
                return Message::Display(data);
            } else if (type == "ConsoleCommand") {
                return Message::Command(data);
            } else {
                logger::error("Recieved a message with unknown return type {}, replacing with blank display", type);
                return Message::Display("");
            }
        }
    };

    static std::queue<Message> messageQueue;

    // weird workaround bc Windows API defines a macro for "GetMessage"
    #undef GetMessage
    std::string GetMessage(RE::StaticFunctionTag*) {
        if (messageQueue.empty()) {
            logger::trace("Message requested, but queue was empty");
            return "";
        }
        Message message = messageQueue.front();
        messageQueue.pop();
        logger::debug("Message requested, popped {}", message.toString());
        return message.toString();
    }

    bool PapyrusFunctions(RE::BSScript::IVirtualMachine* vm) {
        vm->RegisterFunction("GetMessage", "CustomPapyrusFunctions", GetMessage);
        logger::info("Papyrus functions registered");
        return true;
    }

    static void HttpEvtHandler(struct mg_connection* c, int evt, void* evt_data) {
        if (evt == MG_EV_HTTP_MSG) {
            struct mg_http_message* message = (struct mg_http_message*) evt_data;
            const std::string messageString = message->body.buf;

            mg_http_reply(c, 200, "", "");

            logger::info("Command {} received, adding to queue", messageString);

            messageQueue.push(Message::fromString(messageString));
        }
    }

    int InitHttpServer() {
        std::string address = std::string("http://127.0.0.1:") + std::string(PORT);

        struct mg_mgr mgr;
        mg_mgr_init(&mgr);
        mg_http_listen(&mgr, address.c_str(), HttpEvtHandler, NULL);

        logger::info("Starting HTTP server at {}", address);

        for (;;) {
            mg_mgr_poll(&mgr, 1000);
        }

        return 0;
    }

    UINT __cdecl WorkerThread(LPVOID pParam) {
        InitHttpServer();

        return 0;
    }
}

SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    SKSE::Init(skse);
    SetupLog();

    spdlog::set_level(spdlog::level::info);

    // register GetCommand function to Papyrus
    SKSE::GetPapyrusInterface()->Register(Magnesium::PapyrusFunctions);

    // launch MFC worker thread to run HTTP server
    AfxBeginThread(Magnesium::WorkerThread, 0);

    return true;
}
