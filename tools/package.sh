#!/bin/bash

# mkdir -p '${workspaceFolder}/contrib/Distribution/Release/SKSE/Plugins' && find '${workspaceFolder}/contrib/Distribution/Release/' -maxdepth 1 -name '*.dll' -exec cp {} '${workspaceFolder}/contrib/Distribution/Release/SKSE/Plugins/' \\;

TMP_DIR="tmp_pkg_$1_$(date +%s)"

mkdir "$TMP_DIR"

# ESP file
cp "$1.esp" "$TMP_DIR/"

# scripts
mkdir "$TMP_DIR/Scripts"
find "./contrib/Distribution/PapyrusRelease/" -name "*.pex" -exec cp {} "$TMP_DIR/Scripts/" \;

# DLL
mkdir -p "$TMP_DIR/SKSE/Plugins"
find "./contrib/Distribution/PluginRelease/" -name "*.dll" -exec cp {} "$TMP_DIR/SKSE/Plugins/" \;

# config
mkdir -p "$TMP_DIR/SKSE/Plugins"
find "./contrib/Distribution/Config/" -name "*.yaml" -exec cp {} "$TMP_DIR/SKSE/Plugins/" \;

(cd "$TMP_DIR" && 7z a -tzip "../contrib/Distribution/$1.zip" "./*")

rm -rf "$TMP_DIR"
