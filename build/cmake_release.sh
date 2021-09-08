#!/usr/bin/env bash

cd "$(dirname "$0")" || exit
cmake -B Release -S .. -DCMAKE_BUILD_TYPE=Release "$@"
