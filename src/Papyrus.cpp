#include "papyrus.h"
#include "magnesium.h"

constexpr std::string_view PapyrusClass = "Magnesium";

bool Magnesium::RegisterFunctions(RE::BSScript::IVirtualMachine* vm) {
    vm->RegisterFunction("NextMessage", PapyrusClass, NextMessage);

    return true;
}