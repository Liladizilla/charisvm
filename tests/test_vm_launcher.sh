#!/bin/sh
set -eu

SCRIPT=./scripts/charisvm-qemu
"$SCRIPT" --help > build/vm-help.txt
grep -q 'Launch a real x86-64 guest using QEMU' build/vm-help.txt

if "$SCRIPT" --memory invalid > /dev/null 2>&1; then
    echo "FAIL: launcher accepted a non-numeric memory size" >&2
    exit 1
fi
if "$SCRIPT" --cpus 0 > /dev/null 2>&1; then
    echo "FAIL: launcher accepted zero vCPUs" >&2
    exit 1
fi
if "$SCRIPT" --unknown > /dev/null 2>&1; then
    echo "FAIL: launcher accepted an unknown option" >&2
    exit 1
fi

echo "All QEMU launcher argument tests passed."
