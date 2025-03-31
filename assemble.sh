#!/bin/bash
set -e

INSTR_FILE="input.s"
TMP_DIR="tmp"
TMP_ASM="$TMP_DIR/input.s"
OBJ="$TMP_DIR/output.o"
EXE="$TMP_DIR/output"

# Create tmp directory if it doesn't exist
mkdir -p "$TMP_DIR"

# Create full .s file with boilerplate
cat > "$TMP_ASM" <<EOF
.text
.global _start
_start:
EOF

# Append user instructions safely
while IFS= read -r line || [[ -n "$line" ]]; do
    echo "$line" >> "$TMP_ASM"
done < "$INSTR_FILE"

# Assemble and link
as -o "$OBJ" "$TMP_ASM"
ld -o "$EXE" "$OBJ"

# Dump little-endian bytes with disassembly
objdump -d "$OBJ" \
  | grep -E '^[[:space:]]*[0-9a-f]+:' \
  | while read -r line; do
    # Extract instruction bytes in little-endian order
    hex=$(echo "$line" \
      | sed -E 's/^[[:space:]]*[0-9a-f]+:\s*([0-9a-f]{2})\s*([0-9a-f]{2})\s*([0-9a-f]{2})\s*([0-9a-f]{2}).*/0x\4, 0x\3, 0x\2, 0x\1/')
    
    # Extract disassembled instruction
    disasm=$(echo "$line" \
      | sed -E 's/^[[:space:]]*[0-9a-f]+:.*?[0-9a-f]{2}([[:space:]]+[0-9a-f]{2}){3}[[:space:]]*//')
    
    printf "%-32s // %s\n" "$hex" "$disasm"
done
