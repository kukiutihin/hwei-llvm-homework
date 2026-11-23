#!/usr/bin/env bash
set -euo pipefail

SKIBC="$1"
TEST_FILE="$2"
EXPECTED_FILE="${TEST_FILE%.skib67}_expected.txt"

EXPECTED=$(<"$EXPECTED_FILE")
ACTUAL=$("$SKIBC" "$TEST_FILE")

if [[ "$ACTUAL" != "$EXPECTED" ]]; then
  echo "FAIL: $TEST_FILE"
  echo "expected:"
  printf "%s\n" "$EXPECTED"
  echo ""
  echo "got:"
  printf "%s\n" "$ACTUAL"
  exit 1
fi
