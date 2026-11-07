#!/bin/bash

PLUGIN_NAME="Arcana"

# Check for the standalone argument
RUN_MODE="AutomationCommandlet"
if [[ "$1" == "-standalone" || "$1" == "--standalone" ]]; then
    RUN_MODE="game"
fi

# Get the directory of the current script (Plugins/PluginName/Scripts)
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE}" )" && pwd )"

# Navigate up to the project root
PROJECT_ROOT="$( cd "$SCRIPT_DIR/../../.." && pwd )"

# Find the .uproject file
UPROJECT_PATH=$(find "$PROJECT_ROOT" -maxdepth 1 -name "*.uproject" | head -n 1)

if [ -z "$UPROJECT_PATH" ]; then
    echo "[ERROR] No .uproject file found in $PROJECT_ROOT"
    exit 1
fi

# Attempt to find the Engine path
if [ -z "$UNREAL_ENGINE_PATH" ]; then
    ENGINE_PATH="/opt/UnrealEngine" 
else
    ENGINE_PATH="$UNREAL_ENGINE_PATH"
fi

RUNUAT_PATH="$ENGINE_PATH/Engine/Build/BatchFiles/RunUAT.sh"

if [ ! -f "$RUNUAT_PATH" ]; then
    echo "[ERROR] RunUAT.sh not found at $RUNUAT_PATH"
    echo "Please set the \$UNREAL_ENGINE_PATH environment variable."
    exit 1
fi

echo "Project Found: $UPROJECT_PATH"
echo "Engine Found:  $ENGINE_PATH"
echo "Run Mode:      $RUN_MODE"
echo "Running Automated Tests..."

# Execute the tests
bash "$RUNUAT_PATH" RunUnreal -project="$UPROJECT_PATH" -scriptargs="-RunTest=$PLUGIN_NAME -NullRHI -NoSound -$RUN_MODE"

