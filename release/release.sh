#!/bin/bash
ROOT_DIR=$(git rev-parse --show-toplevel)
RELEASE_DIR="$ROOT_DIR"/release

cd "$ROOT_DIR"/include/argparser || exit

quom argparser.hpp "$RELEASE_DIR"/argparser.hpp
