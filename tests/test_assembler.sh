#!/bin/sh
set -eu

ASM=./build/charisasm
VM=./build/charisvm
DEBUG=./build/charisvm-debug
TMP=build/assembler-test
mkdir -p "$TMP"
trap 'rm -rf "$TMP"' EXIT HUP INT TERM

cat > "$TMP/sum.asm" <<'EOF'
# Mixed-case label declaration and reference must work.
start:
    LDI R0, 7
    LDI R1, 5
    ADD R0, R1
    OUT R0
    HLT
EOF
"$ASM" "$TMP/sum.asm" "$TMP/sum.hex" > "$TMP/assemble.out"
grep -q 'Assembled 12 byte(s)' "$TMP/assemble.out"
"$VM" "$TMP/sum.hex" > "$TMP/run.out"
grep -q 'R0 = 12 (0x0C)' "$TMP/run.out"
grep -q 'OUT = 12 (0x0C)' "$TMP/run.out"
"$DEBUG" "$TMP/sum.hex" > "$TMP/debug.out"
grep -q 'R0=0C R1=05' "$TMP/debug.out"
grep -q 'Debug session finished: 5 instruction(s).' "$TMP/debug.out"
grep -Fq 'RAM[000..015]:' "$TMP/debug.out"

cat > "$TMP/jump.asm" <<'EOF'
    LDI R0, 0
    JZ done
    LDI R1, 99
done:
    HLT
EOF
"$ASM" "$TMP/jump.asm" "$TMP/jump.hex" > /dev/null
"$VM" "$TMP/jump.hex" > "$TMP/jump.out"
grep -q 'R1 = 0 (0x00)' "$TMP/jump.out"

cat > "$TMP/invalid.asm" <<'EOF'
    LDI R9, 1
    HLT
EOF
if "$ASM" "$TMP/invalid.asm" "$TMP/invalid.hex" > /dev/null 2>&1; then
    echo "FAIL: assembler accepted an invalid register" >&2
    exit 1
fi
if [ -e "$TMP/invalid.hex" ]; then
    echo "FAIL: assembler left output behind after an error" >&2
    exit 1
fi

cat > "$TMP/missing-label.asm" <<'EOF'
    JMP nowhere
    HLT
EOF
if "$ASM" "$TMP/missing-label.asm" "$TMP/missing-label.hex" > /dev/null 2>&1; then
    echo "FAIL: assembler accepted an undefined label" >&2
    exit 1
fi

echo "All assembler integration tests passed."
