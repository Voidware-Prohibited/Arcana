#!/bin/bash

# Set title (supported by some Linux terminal emulators)
echo -ne "\033]0;Build Plugin\007"

PLUGIN_NAME="Arcana"
SOURCE_BUILD_VERSION="5.8"
INSTALLED_BUILD_VERSION="5.8"
ENGINE_DIRECTORY=""

# 1. Check if ENGINE_DIRECTORY is already set as an environment variable
if [ -n "$UE_ROOT" ]; then
    ENGINE_DIRECTORY="$UE_ROOT"
fi

# 2. If not found, check common Linux source / installed paths
# (Adjust these paths if you install UE elsewhere, e.g., /opt/UnrealEngine)
if [ -z "$ENGINE_DIRECTORY" ]; then
    POSSIBLE_PATHS=(
        "$HOME/UnrealEngine-$SOURCE_BUILD_VERSION"
        "$HOME/Games/Heroic/UnrealEngine-$INSTALLED_BUILD_VERSION"
        "/opt/UnrealEngine-$INSTALLED_BUILD_VERSION"
    )

    for PATH_CHECK in "${POSSIBLE_PATHS[@]}"; do
        if [ -d "$PATH_CHECK" ]; then
            ENGINE_DIRECTORY="$PATH_CHECK"
            break
        fi
     Cabe
fi

# 3. Handle error if the engine directory cannot be found
if [ -z "$ENGINE_DIRECTORY" ]; then
    clear
    echo "Can't find a path to the engine!"
    echo "Please set the UE_ROOT environment variable or update this script."
    echo "Example: export UE_ROOT='/path/to/UnrealEngine'"
    echo ""
    read -p "Press [Enter] to exit..."
    exit 0
fi

# Get the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Define Linux-specific paths
AUTOMATION_TOOL_PATH="$ENGINE_DIRECTORY/Engine/Build/BatchFiles/RunUAT.sh"
PLUGIN_PATH="$SCRIPT_DIR/$PLUGIN_NAME.uplugin"
OUTPUT_PATH="$SCRIPT_DIR/Build"

echo "Automation Tool Path: $AUTOMATION_TOOL_PATH"
echo ""

# Ensure the automation tool is executable and run it
if [ -f "$AUTOMATION_TOOL_PATH" ]; then
    chmod +x "$AUTOMATION_TOOL_PATH"
    "$AUTOMATION_TOOL_PATH" BuildPlugin -Plugin="$PLUGIN_PATH" -Package="$OUTPUT_PATH" -Rocket -TargetPlatforms=Linux
else
    echo "Error: RunUAT.sh not found at $AUTOMATION_TOOL_PATH"
fi

echo ""
read -p "Press [Enter] to exit..."
exit 0
