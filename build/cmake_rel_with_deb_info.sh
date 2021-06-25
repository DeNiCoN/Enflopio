#!/usr/bin/env bash

cd "$(dirname "$0")"
cmake -B RelWithDebInfo -S .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo "$@"
cp RelWithDebInfo/compile_commands.json ..
