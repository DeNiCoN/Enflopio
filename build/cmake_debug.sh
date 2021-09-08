#!/usr/bin/env bash

cd "$(dirname "$0")" || exit
cmake -B Debug -S .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug "$@"
cp Debug/compile_commands.json ..
