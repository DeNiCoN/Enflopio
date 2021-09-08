#!/usr/bin/env bash

cd "$(dirname "$0")" || exit
emcmake cmake -B Emscripten -S .. -DCMAKE_BUILD_TYPE=Release "$@"
