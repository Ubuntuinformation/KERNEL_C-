#!/bin/bash

# build_packages.sh - Gera stub de pacotes .efh para HubbleOS
# Nota: Isto é um exemplo simplificado. Em produção, seriam necessários
# cross-compilers e scripts de build mais sofisticados.

set -e

PACKAGES_DIR="packages"
mkdir -p "$PACKAGES_DIR"

echo "=== Gerando pacotes .efh para HubbleOS ==="

# Função para criar um stub de pacote .efh
create_efh_stub() {
    local name=$1
    local author=$2
    local size=${3:-1024}
    local file="$PACKAGES_DIR/$name.efh"
    
    echo "Criando $file (stub)..."
    
    # Header EFH mínimo
    # Magic: EFH\0
    # Version: 1
    # Entry offset: 256 (header + metadata)
    # Code size: $size
    
    # Usamos printf para gerar bytes
    printf "EFH\x00" > "$file"
    printf "\x01\x00\x00\x00" >> "$file"  # version = 1
    printf "\x00\x01\x00\x00" >> "$file"  # entry_offset = 256
    printf "\x00\x04\x00\x00" >> "$file"  # code_size = 1024
    printf "\x00\x00\x00\x00" >> "$file"  # data_offset = 0
    printf "\x00\x00\x00\x00" >> "$file"  # data_size = 0
    
    # Name (32 bytes)
    printf "%-32s" "$name" | head -c 32 >> "$file"
    
    # Author (32 bytes)
    printf "%-32s" "$author" | head -c 32 >> "$file"
    
    # Description (64 bytes)
    printf "%-64s" "Stub for $name" | head -c 64 >> "$file"
    
    # Pad to 256 bytes (header + metadata)
    current_size=$(stat -f%z "$file" 2>/dev/null || stat -c%s "$file" 2>/dev/null)
    pad=$((256 - current_size))
    if [ $pad -gt 0 ]; then
        dd if=/dev/zero bs=1 count=$pad >> "$file" 2>/dev/null
    fi
    
    # Código dummy (return 0, representado por NOP + RET)
    # x86 32-bit: 0x90 = NOP, 0xC3 = RET
    dd if=/dev/zero bs=1 count=$size >> "$file" 2>/dev/null
    
    ls -lh "$file"
}

# Criar stubs de pacotes
create_efh_stub "git" "Ubuntuinformation" 5242880
create_efh_stub "curl" "Ubuntuinformation" 2097152
create_efh_stub "gcc" "Ubuntuinformation" 52428800
create_efh_stub "bash" "Ubuntuinformation" 1048576
create_efh_stub "make" "Ubuntuinformation" 1048576

echo ""
echo "=== Pacotes gerados ==="
ls -lh $PACKAGES_DIR/

echo ""
echo "Nota: Estes são stubs. Para pacotes reais, seria necessário:"
echo "1. Compilar cada ferramenta com cross-compiler (i686-elf-gcc)"
echo "2. Linkeditar para ELF 32-bit relocável"
echo "3. Empacotar no formato .efh com headers corretos"
echo "4. Incluir libs C necessárias (libc.a, etc.)"
