#!/bin/bash

# test_kernel.sh - Script para testar o kernel em QEMU
# Nota: Requer qemu-system-i386 instalado

set -e

echo "=== HubbleOS Kernel Test ==="
echo ""

# Verificar se qemu está disponível
if ! command -v qemu-system-i386 &> /dev/null; then
    echo "❌ qemu-system-i386 não encontrado"
    echo "Instale com: apt-get install qemu-system-x86"
    exit 1
fi

# Verificar se kernel.bin existe
if [ ! -f "kernel.bin" ]; then
    echo "❌ kernel.bin não encontrado"
    echo "Execute: make"
    exit 1
fi

echo "✅ Verificações passaram"
echo ""
echo "Iniciando kernel em QEMU..."
echo "(Para sair: pressione Ctrl+A X)"
echo ""

# Rodaro kernel em QEMU (modo usuário, sem rede por padrão)
qemu-system-i386 \
    -kernel kernel.bin \
    -m 256 \
    -append "console=ttyS0" \
    -nographic \
    -monitor telnet::55555,server,nowait

echo ""
echo "Teste concluído"
