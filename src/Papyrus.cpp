#include "papyrus.h"
#include "magnesium.h"

constexpr std::string_view PapyrusClass = "Magnesium";

bool Magnesium::RegisterFunctions(RE::BSScript::IVirtualMachine* vm) {
    vm->RegisterFunction("PopMessage", PapyrusClass, PopMessage);

    return true;
}