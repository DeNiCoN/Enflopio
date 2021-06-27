#!/usr/bin/env bash

cd "$(dirname "$0")"
cmake -B Release -S .. -DCMAKE_BUILD_TYPE=Release "$@"
