#!/usr/bin/env bash

cd "$(dirname "$0")"
cmake -B Debug -S .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug "$@"
cp RelWithDebInfo/compile_commands.json ..
