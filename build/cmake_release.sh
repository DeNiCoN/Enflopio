#!/usr/bin/env bash

cd "$(dirname "$0")"
cmake -B Release -S .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Release "$@"
cp RelWithDebInfo/compile_commands.json ..
