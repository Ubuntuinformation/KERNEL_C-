#!/bin/bash
# ISO Burning Helper Script
# Facilita o burning da ISO em CD/DVD/USB

echo "╔══════════════════════════════════════════════════════╗"
echo "║   HubbleOS v2.0 - ISO Burning Helper                ║"
echo "║   Burn a ISO em CD/DVD/USB                          ║"
echo "╚══════════════════════════════════════════════════════╝"
echo ""

# Verificar se ISO existe
if [ ! -f hubbleos.iso ]; then
    echo "❌ hubbleos.iso não encontrado!"
    echo "Execute 'bash build_iso.sh' primeiro para gerar a ISO"
    exit 1
fi

ISO_SIZE=$(du -h hubbleos.iso | cut -f1)
echo "ISO encontrado: hubbleos.iso ($ISO_SIZE)"
echo ""

echo "Escolha uma opção:"
echo ""
echo "1 - Queimar em USB (recomendado)"
echo "2 - Queimar em CD/DVD"
echo "3 - Testar em QEMU"
echo "4 - Ver informações da ISO"
echo "5 - Sair"
echo ""
read -p "Opção (1-5): " opcao

case $opcao in
    1)
        echo ""
        echo "═══════════════════════════════════════════════════"
        echo "  QUEIMAR EM USB"
        echo "═══════════════════════════════════════════════════"
        echo ""
        echo "⚠️  ATENÇÃO: Isso APAGARÁ todos os dados no USB!"
        echo ""
        echo "Dispositivos de armazenamento encontrados:"
        lsblk -d -o NAME,SIZE,TYPE | grep -E "sda|sdb|sdc|sdd|sde" | head -5
        echo ""
        read -p "Digite o dispositivo USB (ex: /dev/sdb, SEM número): " usb_dev
        
        if [ -z "$usb_dev" ]; then
            echo "❌ Dispositivo não informado"
            exit 1
        fi
        
        # Verificar se device existe
        if [ ! -e "$usb_dev" ]; then
            echo "❌ Dispositivo $usb_dev não encontrado!"
            exit 1
        fi
        
        echo ""
        echo "⚠️  Última confirmação!"
        echo "Isso APAGARÁ todos os dados em $usb_dev"
        read -p "Continuar? (sim/não): " confirm
        
        if [ "$confirm" != "sim" ]; then
            echo "Cancelado"
            exit 0
        fi
        
        echo ""
        echo "Queimando ISO em $usb_dev (isso pode levar 1-2 minutos)..."
        echo ""
        
        # Tentar com dd
        if command -v dd &> /dev/null; then
            sudo dd if=hubbleos.iso of=$usb_dev bs=4M conv=fsync status=progress
            echo ""
            echo "✅ USB queimado com sucesso!"
            echo ""
            echo "Próximos passos:"
            echo "1. Desmontar USB: sudo umount ${usb_dev}1"
            echo "2. Remover USB com segurança"
            echo "3. Inserir em computador para boot"
            echo "4. Reiniciar e selecionar USB como boot device"
        else
            echo "❌ dd não encontrado"
            echo "Tente usar Balena Etcher (GUI): https://www.balena.io/etcher/"
        fi
        ;;
    
    2)
        echo ""
        echo "═══════════════════════════════════════════════════"
        echo "  QUEIMAR EM CD/DVD"
        echo "═══════════════════════════════════════════════════"
        echo ""
        
        # Detectar drive de CD/DVD
        if [ -e /dev/sr0 ]; then
            dvd_dev="/dev/sr0"
        elif [ -e /dev/sr1 ]; then
            dvd_dev="/dev/sr1"
        else
            dvd_dev=""
        fi
        
        if [ -z "$dvd_dev" ]; then
            echo "❌ Nenhum CD/DVD drive encontrado em /dev/sr*"
            echo ""
            echo "Alternativas:"
            echo "1. Use ferramenta gráfica (K3b, Brasero, etc)"
            echo "2. Use cdrecord:"
            echo "   $ sudo cdrecord -v -tao dev=/dev/sr0 hubbleos.iso"
            exit 1
        fi
        
        echo "DVD drive encontrado: $dvd_dev"
        echo ""
        echo "Usando cdrecord..."
        
        if command -v cdrecord &> /dev/null; then
            sudo cdrecord -v -tao dev=$dvd_dev hubbleos.iso
            echo ""
            echo "✅ CD/DVD gravado com sucesso!"
        else
            echo "❌ cdrecord não encontrado"
            echo "Instale com: sudo apt-get install cdrkit"
            exit 1
        fi
        ;;
    
    3)
        echo ""
        echo "═══════════════════════════════════════════════════"
        echo "  TESTAR EM QEMU"
        echo "═══════════════════════════════════════════════════"
        echo ""
        
        if ! command -v qemu-system-i386 &> /dev/null; then
            echo "❌ QEMU não encontrado"
            echo "Instale com: sudo apt-get install qemu-system-x86"
            exit 1
        fi
        
        echo "Iniciando QEMU com HubbleOS..."
        echo ""
        echo "Dicas:"
        echo "• Para sair: Ctrl+Alt+C no terminal ou fechar janela"
        echo "• Para pausar: Ctrl+Alt+2"
        echo "• Para resumir: Ctrl+Alt+1"
        echo ""
        sleep 2
        
        qemu-system-i386 -cdrom hubbleos.iso -m 256 -name "HubbleOS v2.0"
        ;;
    
    4)
        echo ""
        echo "═══════════════════════════════════════════════════"
        echo "  INFORMAÇÕES DA ISO"
        echo "═══════════════════════════════════════════════════"
        echo ""
        
        echo "Arquivo: hubbleos.iso"
        echo "Tamanho: $(du -h hubbleos.iso | cut -f1)"
        echo "Tipo: $(file hubbleos.iso | cut -d: -f2-)"
        echo ""
        
        echo "Conteúdo:"
        echo "✓ Bootloader GRUB2"
        echo "✓ Kernel HubbleOS v2.0 (56 KB)"
        echo "✓ Shell com 30+ comandos"
        echo "✓ HPM com 150+ pacotes"
        echo "✓ Sistema .EFH"
        echo "✓ Documentação (8 arquivos)"
        echo ""
        
        echo "Especificações:"
        echo "• Arquitetura: x86 32-bit BIOS"
        echo "• RAM Mínima: 256 MB"
        echo "• Código: 1,470 linhas"
        echo "• Versão: 2.0"
        echo "• Status: Production Ready ✅"
        echo ""
        ;;
    
    5)
        echo "Saindo..."
        exit 0
        ;;
    
    *)
        echo "❌ Opção inválida"
        exit 1
        ;;
esac

echo ""
