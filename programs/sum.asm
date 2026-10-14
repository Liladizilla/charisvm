# Labels are case-insensitive. This program computes 7 + 5 and outputs 12.
start:
    LDI R0, 7
    LDI R1, 5
    ADD R0, R1
    OUT R0
    HLT
